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
#include "bt_nodebbprecondition.h"

BT_NodeBBPrecondition::BT_NodeBBPrecondition( std::string name, BT_BlackboardMap& blackboard ) :
	BT_Node( name, blackboard )
{
}

BT_NodeBBPrecondition::~BT_NodeBBPrecondition()
{
}

BT_RESULT BT_NodeBBPrecondition::tick()
{
	bool found = false;
	const auto it = m_blackboard.find(m_key);
	if (it != m_blackboard.end()) {
		const std::string *blackboardValue = std::get_if<std::string>( &it->second );
		found = *blackboardValue == m_expected;
	}

	if ( found || m_expected == "*" )
	{
		if ( m_children.size() > 0 )
		{
			return m_children[0]->tick();
		}
	}
	return BT_RESULT::FAILURE;
}


json BT_NodeBBPrecondition::serialize() const
{
	return BT_Node::serialize( m_factoryIndex );
}