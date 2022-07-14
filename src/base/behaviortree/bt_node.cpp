/*	
	This file is part of Ingnomia https://github.com/rschurade/Ingnomia
    Copyright (C) 2017-2020  Ralph Schurade, Ingnomia Team

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "bt_node.h"

#include <utility>

#include "bt_tree.h"
#include "spdlog/spdlog.h"

BT_Node::BT_Node( std::string name, BT_BlackboardMap& blackboard ) :
	m_name(std::move( name )),
	m_blackboard( blackboard )
{
}

BT_Node::~BT_Node()
{
	for ( const auto& child : m_children )
	{
		delete child;
	}
}

json BT_Node::serialize( int factoryId ) const
{
	std::vector<json> childs;
	for ( const auto& child : m_children )
	{
		childs.push_back( child->serialize() );
	}

	return json {
		{ "_Type", m_typeMap[factoryId] },
		{ "Name", m_name },
		{ "ID", m_index },
		{ "Status", m_status },
		{ "Childs", childs },
	};
}

BT_Node* BT_Node::deserialize( const json& in, const BT_ActionMap& actionMap, BT_BlackboardMap& blackboard )
{
	const auto typeStr = in.value("_Type", "");
	const auto name = in.value( "Name", "" );
	auto result = m_factoryMap[typeStr](name, blackboard);
	result->deserialize( in, actionMap );

	auto vcl  = in.at( "Childs" );
	for ( const auto& child : vcl )
	{
		result->m_children.push_back( deserialize( child, actionMap, blackboard ) );
	}

	return result;
}

void BT_Node::deserialize( const json& in, const BT_ActionMap& actionMap )
{
	if ( m_name != in.value( "Name", "" ) )
	{
		spdlog::debug("error loading behavior tree state - nodes don't match");
	}
	m_index  = in.value( "ID", -1 );
	m_status = in.value( "Status", BT_RESULT::IDLE );
//
//	auto vcl  = in.at( "Childs" );
//	int index = 0;
//	if ( vcl.size() == m_children.size() )
//	{
//
//		for ( auto child : m_children )
//		{
//			child->deserialize( vcl[index++], actionMap );
//		}
//	}
//	else
//	{
//		//tree changed between saving and loading, this will have undetermined results
//		// TODO throw exception or make config option to allow or deny loading this
//		if ( vcl.size() < m_children.size() )
//		{
//			for ( auto vcm : vcl )
//			{
//				m_children[index++]->deserialize( vcm, <#initializer #> );
//			}
//		}
//		else
//		{
//			for ( auto child : m_children )
//			{
//				child->deserialize( vcl[index++], <#initializer #> );
//			}
//		}
//	}
}

BT_Node* BT_Node::addFallback( std::string name )
{
	BT_NodeFallback* bn = new BT_NodeFallback( name, m_blackboard );
	m_children.push_back( bn );

	return bn;
}

BT_Node* BT_Node::addFallbackStar( std::string name )
{
	BT_NodeFallbackStar* bn = new BT_NodeFallbackStar( name, m_blackboard );
	m_children.push_back( bn );

	return bn;
}

BT_Node* BT_Node::addSequence( std::string name )
{
	BT_NodeSequence* bn = new BT_NodeSequence( name, m_blackboard );
	m_children.push_back( bn );

	return bn;
}

BT_Node* BT_Node::addSequenceStar( std::string name )
{
	BT_NodeSequenceStar* bn = new BT_NodeSequenceStar( name, m_blackboard );
	m_children.push_back( bn );

	return bn;
}

BT_Node* BT_Node::addForceSuccess( const std::string& name )
{
	BT_NodeForceSuccess* bn = new BT_NodeForceSuccess( name.empty() ? "ForceSuccess" : name, m_blackboard );
	m_children.push_back( bn );

	return bn;
}

BT_Node* BT_Node::addForceFailure( const std::string& name )
{
	BT_NodeForceFailure* bn = new BT_NodeForceFailure( name.empty() ? "ForceFailure" : name, m_blackboard );
	m_children.push_back( bn );

	return bn;
}

BT_Node* BT_Node::addInverter( std::string name )
{
	BT_NodeInverter* bn = new BT_NodeInverter( name, m_blackboard );
	m_children.push_back( bn );

	return bn;
}

BT_Node* BT_Node::addRepeat( std::string name, int num )
{
	BT_NodeRepeat* bn = new BT_NodeRepeat( name, m_blackboard );
	bn->setInternalData( num );
	m_children.push_back( bn );

	return bn;
}

BT_Node* BT_Node::addRepeatUntilSuccess( std::string name, int num )
{
	BT_NodeRepeatUntilSuccess* bn = new BT_NodeRepeatUntilSuccess( name, m_blackboard );
	bn->setInternalData( num );
	m_children.push_back( bn );

	return bn;
}

void BT_Node::addTree( BT_Node* tree )
{
	m_children.push_back( tree );
}

BT_Node* BT_Node::addConditional( std::string name, std::function<BT_RESULT( bool )> callback )
{
	BT_NodeConditional* bn = new BT_NodeConditional( name, m_blackboard, callback );
	m_children.push_back( bn );

	return bn;
}

BT_Node* BT_Node::addAction( std::string name, std::function<BT_RESULT( bool )> callback )
{
	BT_NodeAction* bn = new BT_NodeAction( name, m_blackboard, callback );
	m_children.push_back( bn );

	return bn;
}

BT_Node* BT_Node::addBBPrecondition( std::string name, std::string key, std::string expected )
{
	BT_NodeBBPrecondition* bn = new BT_NodeBBPrecondition( name, m_blackboard );
	bn->setInternalData( key, expected );
	m_children.push_back( bn );

	return bn;
}

void BT_Node::haltAllChildren()
{
	for ( auto child : m_children )
	{
		child->halt();
	}
}

void BT_Node::halt()
{
	m_index = 0;
	haltAllChildren();
}