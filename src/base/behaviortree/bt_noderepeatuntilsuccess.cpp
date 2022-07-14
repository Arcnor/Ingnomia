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
#include "bt_noderepeatuntilsuccess.h"

#include "spdlog/spdlog.h"

BT_NodeRepeatUntilSuccess::BT_NodeRepeatUntilSuccess( std::string name, BT_BlackboardMap& blackboard ) :
	BT_Node( name, blackboard )
{
}

BT_NodeRepeatUntilSuccess::~BT_NodeRepeatUntilSuccess()
{
}

json BT_NodeRepeatUntilSuccess::serialize() const
{
	auto result = BT_Node::serialize( m_factoryIndex );
	result["Num"] = m_num;
	return result;
}

void BT_NodeRepeatUntilSuccess::deserialize( const json& in, const BT_ActionMap& actionMap )
{
	BT_Node::deserialize( in, actionMap );
	m_num = in.value( "Num", -1 );
}

BT_RESULT BT_NodeRepeatUntilSuccess::tick()
{
	while ( m_index < m_num )
	{
		if ( m_children.size() > 0 )
		{
			BT_RESULT result = m_children[0]->tick();

			if ( result == BT_RESULT::RUNNING )
			{
				return BT_RESULT::RUNNING;
			}
			else if ( result == BT_RESULT::SUCCESS )
			{
				m_index = 0;
				return BT_RESULT::SUCCESS;
			}
		}
		++m_index;
	}
	m_index = 0;
	return BT_RESULT::FAILURE;
}