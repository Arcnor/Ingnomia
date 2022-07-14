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
#include "bt_nodesequencestar.h"

#include "spdlog/spdlog.h"

BT_NodeSequenceStar::BT_NodeSequenceStar( std::string name, BT_BlackboardMap& blackboard ) :
	BT_Node( name, blackboard )
{
}

BT_NodeSequenceStar::~BT_NodeSequenceStar()
{
}

json BT_NodeSequenceStar::serialize() const
{
	auto result = BT_Node::serialize( m_factoryIndex );
	result["RoF"] = m_resetOnFailure;
	return result;
}

void BT_NodeSequenceStar::deserialize( const json& in, const BT_ActionMap& actionMap )
{
	BT_Node::deserialize( in, actionMap );
	m_resetOnFailure = in.value( "RoF", false );
}

BT_RESULT BT_NodeSequenceStar::tick()
{
	m_status = BT_RESULT::RUNNING;

	while ( m_index < m_children.size() )
	{
		BT_RESULT child_status = m_children[m_index]->tick();

		if ( child_status == BT_RESULT::RUNNING )
		{
			// Suspend execution and return RUNNING.
			// At the next tick, index will be the same.
			return BT_RESULT::RUNNING;
		}
		else if ( child_status == BT_RESULT::SUCCESS )
		{
			// continue the while loop
			++m_index;
		}
		else if ( child_status == BT_RESULT::FAILURE )
		{
			// Suspend execution and return FAILURE.
			// At the next tick, index will be the same.
			//if( m_resetOnFailure )
			{
				haltAllChildren();
				m_index = 0;
			}
			return BT_RESULT::FAILURE;
		}
	}
	// all the children returned success. Return SUCCESS too.
	m_index = 0;
	haltAllChildren();
	return BT_RESULT::SUCCESS;
}