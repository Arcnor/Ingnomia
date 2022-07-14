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

class BT_NodeRepeat final : public BT_Node
{
public:
	BT_NodeRepeat( std::string name, BT_BlackboardMap& blackboard );
	~BT_NodeRepeat();

	[[nodiscard]] json serialize() const override;
	void deserialize( const json& in, const BT_ActionMap& actionMap ) override;

	BT_RESULT tick() override;

private:
	int m_num = 0;

	friend class BT_Node;
	void setInternalData( int num )
	{
		m_num = num;
	}

	static inline int m_factoryIndex = registerFactoryMethod<BT_NodeRepeat>("BT_NodeRepeat");
};
