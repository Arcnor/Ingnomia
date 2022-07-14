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
#pragma once

#include "bt_node.h"

class BT_NodeBBPrecondition final : public BT_Node
{
public:
	BT_NodeBBPrecondition( std::string name, BT_BlackboardMap& blackboard );
	~BT_NodeBBPrecondition();

	BT_RESULT tick() override;

	[[nodiscard]] json serialize() const override;

private:
	std::string m_key;
	std::string m_expected;

	friend class BT_Node;
	void setInternalData( const std::string& key, const std::string& expected )
	{
		m_key = key;
		m_expected = expected;
	}

	static inline int m_factoryIndex = registerFactoryMethod<BT_NodeBBPrecondition>("BT_NodeBBPrecondition");
};
