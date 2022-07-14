#include "bt_factory.h"

#include <gtest/gtest.h>

const std::string contentSimple1 = R"(
<root main_tree_to_execute="MainTree">
	<BehaviorTree ID="MainTree">
		<FallbackStar name="AnimalStandard">
			<Action ID="RandomMoveBig"/>
		</FallbackStar>
	</BehaviorTree>
</root>
)";

const std::string contentSimple2 = R"(
<root main_tree_to_execute="ShedTree">
	<BehaviorTree ID="ShedTree">
		<FallbackStar>
			<SequenceStar>
				<Condition ID="IsOnPasture"/>
				<Condition ID="IsDay"/>
				<Condition ID="IsInShed"/>
				<Action ID="LeaveShed"/>
				<Action ID="FindRandomPastureField"/>
				<Action ID="Move"/>
			</SequenceStar>
			<SequenceStar>
				<Condition ID="IsOnPasture"/>
				<Condition ID="IsNight"/>
				<Inverter>
					<Condition ID="IsInShed"/>
				</Inverter>
				<Action ID="FindShed"/>
				<Action ID="Move"/>
				<Action ID="EnterShed"/>
			</SequenceStar>
			<SequenceStar>
				<Condition ID="IsDay"/>
				<Inverter>
					<Condition ID="IsInShed"/>
				</Inverter>
				<Action ID="RandomMove"/>
			</SequenceStar>
		</FallbackStar>
	</BehaviorTree>
</root>
)";

const std::string contentSerialize1 = R"(
<root main_tree_to_execute="ShedTree">
	<BehaviorTree ID="ShedTree">
		<FallbackStar>
			<SequenceStar>
				<Condition ID="IsOnPasture"/>
				<Action ID="FindShed"/>
			</SequenceStar>
		</FallbackStar>
	</BehaviorTree>
</root>
)";


const std::string contentSerialize1Json = R"(
{
	"Childs": [
		{
			"Childs": [
				{
					"Childs": [],
					"ID": 0,
					"Name": "IsOnPasture",
					"Status": 3,
					"_Type": "BT_NodeConditional"
				},
				{
					"Childs": [],
					"ID": 0,
					"Name": "FindShed",
					"Status": 3,
					"_Type": "BT_NodeAction"
				}
			],
			"ID": 0,
			"Name": "",
			"RoF": true,
			"Status": 3,
			"_Type": "BT_NodeSequenceStar"
		}
	],
	"ID": 0,
	"Name": "",
	"Status": 3,
	"_Type": "BT_NodeFallbackStar"
}
)";

pugi::xml_document loadXml(const std::string& content) {
	pugi::xml_document result;
	result.load_buffer_inplace( (void*)content.c_str(), content.size() );
	return result;
}

TEST(SimpleTest1, BasicParsing) {
	BT_ActionMap actions {
		{ "RandomMoveBig", [](bool){ return BT_RESULT::SUCCESS; } }
	};
	BT_BlackboardMap blackBoard;

	const auto xml = loadXml(contentSimple1);
	const auto root = xml.document_element();

	const auto* node = BT_Factory::load(root, actions, blackBoard);
	EXPECT_EQ(node->status(), BT_RESULT::IDLE);
	EXPECT_NE(dynamic_cast<const BT_NodeFallbackStar*>( node ), nullptr);
}

TEST(SimpleTest2, BasicParsing) {
	BT_ActionMap actions {
		{ "IsOnPasture", [](bool){ return BT_RESULT::SUCCESS; } },
		{ "IsDay", [](bool){ return BT_RESULT::SUCCESS; } },
		{ "IsInShed", [](bool){ return BT_RESULT::SUCCESS; } },
		{ "IsNight", [](bool){ return BT_RESULT::SUCCESS; } },

		{ "FindShed", [](bool){ return BT_RESULT::SUCCESS; } },
		{ "EnterShed", [](bool){ return BT_RESULT::SUCCESS; } },
		{ "LeaveShed", [](bool){ return BT_RESULT::SUCCESS; } },
		{ "FindRandomPastureField", [](bool){ return BT_RESULT::SUCCESS; } },
		{ "Move", [](bool){ return BT_RESULT::SUCCESS; } },
		{ "RandomMove", [](bool){ return BT_RESULT::SUCCESS; } },
	};
	BT_BlackboardMap blackBoard;

	const auto xml = loadXml(contentSimple2);
	const auto root = xml.document_element();

	const auto* node = BT_Factory::load(root, actions, blackBoard);
	EXPECT_EQ(node->status(), BT_RESULT::IDLE);
	EXPECT_NE(dynamic_cast<const BT_NodeFallbackStar*>( node ), nullptr);
}

TEST(SimpleTest1Serialize, BasicSerializing) {
	BT_ActionMap actions {
		{ "IsOnPasture", [](bool){ return BT_RESULT::SUCCESS; } },

		{ "FindShed", [](bool){ return BT_RESULT::SUCCESS; } },
	};
	BT_BlackboardMap blackBoard;

	const auto xml = loadXml(contentSerialize1);
	const auto root = xml.document_element();

	const auto* node = BT_Factory::load(root, actions, blackBoard);
	EXPECT_EQ(node->status(), BT_RESULT::IDLE);
	EXPECT_NE(dynamic_cast<const BT_NodeFallbackStar*>( node ), nullptr);

	auto serialized = node->serialize();
	json expected = json::parse(contentSerialize1Json);
	EXPECT_EQ( serialized, expected );

	auto *deserialized = BT_Node::deserialize(serialized, actions, blackBoard);

	// TODO: Check deserialization equality
}