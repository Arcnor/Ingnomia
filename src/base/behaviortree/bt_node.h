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

#include <vector>
#include <absl/container/flat_hash_map.h>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

enum class BT_RESULT
{
	FAILURE,
	SUCCESS,
	RUNNING,
	IDLE
};

using BT_ActionMap = absl::flat_hash_map<std::string, std::function<BT_RESULT( bool )>>;
using BT_BlackboardMap = absl::flat_hash_map<std::string, std::variant<std::string, int, json>>;

class BT_Node
{
public:
	BT_Node( std::string name, BT_BlackboardMap& blackboard );
	virtual ~BT_Node();

	[[nodiscard]] virtual json serialize() const = 0;

	static BT_Node* deserialize( const json& in, const BT_ActionMap& actionMap, BT_BlackboardMap& blackboard );

	virtual BT_RESULT tick()
	{
		return BT_RESULT::FAILURE;
	};

	virtual void haltAllChildren();
	virtual void halt();

	BT_RESULT status() const
	{
		return m_status;
	}

	BT_Node* addFallback( std::string name );
	BT_Node* addFallbackStar( std::string name );
	BT_Node* addForceSuccess( const std::string& name );
	BT_Node* addForceFailure( const std::string& name );
	BT_Node* addSequence( std::string name );
	BT_Node* addSequenceStar( std::string name );
	BT_Node* addInverter( std::string name );
	BT_Node* addRepeat( std::string name, int num );
	BT_Node* addRepeatUntilSuccess( std::string name, int num );
	BT_Node* addBBPrecondition( std::string name, std::string key, std::string expected );

	void addTree( BT_Node* tree );

	BT_Node* addConditional( std::string name, std::function<BT_RESULT( bool )> callback );
	BT_Node* addAction( std::string name, std::function<BT_RESULT( bool )> callback );

protected:
	json serialize( int factoryId ) const;
	virtual void deserialize( const json& in, const BT_ActionMap& actionMap );

	const std::string m_name;
	BT_BlackboardMap& m_blackboard;

	std::vector<BT_Node*> m_children;

	BT_RESULT m_status = BT_RESULT::IDLE;

	int m_index = 0;

private:
	static inline absl::flat_hash_map<std::string, std::function<BT_Node*(const std::string&, BT_BlackboardMap&)>> m_factoryMap;
	static inline absl::flat_hash_map<int, std::string> m_typeMap;

protected:
	template<typename T>
	static int registerFactoryMethod(const std::string& typeName) {
		int factoryId = 10 + m_factoryMap.size();

		m_factoryMap[typeName] = [](const std::string& name, BT_BlackboardMap& blackboard) { return new T(name, blackboard); };
		m_typeMap[factoryId] = typeName;

		return factoryId;
	}
};
