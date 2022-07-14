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

class BT_NodeAction final : public BT_Node
{
public:
	BT_NodeAction( std::string name, BT_BlackboardMap& blackboard );
	BT_NodeAction( std::string name, BT_BlackboardMap& blackboard, std::function<BT_RESULT( bool )> callback );
	~BT_NodeAction();

	BT_RESULT tick() override;

	void halt() override;

	BT_Node* addFallback( std::string name )
	{
		return nullptr;
	};
	BT_Node* addFallbackStar( std::string name )
	{
		return nullptr;
	};
	BT_Node* addSequence( std::string name )
	{
		return nullptr;
	};
	BT_Node* addSequenceStar( std::string name )
	{
		return nullptr;
	};
	BT_Node* addInverter( std::string name )
	{
		return nullptr;
	};
	BT_Node* addRepeat( std::string name, int num )
	{
		return nullptr;
	};
	BT_Node* addRepeatUntilSuccess( std::string name, int num )
	{
		return nullptr;
	};

	void addTree( BT_Node* tree ) {};

	BT_Node* addConditional( std::string name )
	{
		return nullptr;
	};
	BT_Node* addAction( std::string name )
	{
		return nullptr;
	};

	[[nodiscard]] json serialize() const override;
	void deserialize( const json& in, const BT_ActionMap& actionMap ) override;

private:
	std::function<BT_RESULT( bool )> m_callback;

	static inline int m_factoryIndex = registerFactoryMethod<BT_NodeAction>("BT_NodeAction");
};
