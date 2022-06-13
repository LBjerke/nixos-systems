
#include "Matrix88.hpp"

void Matrix88::elementsChanged() {
	// base-class caller holds lock on _elements.
	Input** cvs = NULL;
	Param** mutes = NULL;
	bool* soloByColumns = NULL;
	for (int i = 1, n = std::min(3, (int)_elements.size()); i < n; ++i) {
		auto e = _elements[i];
		assert(e);
		if (e->cvs) {
			cvs = e->cvs;
		}
		if (e->mutes) {
			mutes = e->mutes;
		}
		if (e->soloByColumns) {
			soloByColumns = e->soloByColumns;
		}
	}
	setCVInputs(cvs);
	setMuteParams(mutes);
	setSoloByColumns(soloByColumns);
}

void Matrix88::processAlways(const ProcessArgs& args) {
	if (expanderConnected()) {
		Matrix88ExpanderMessage* te = toExpander();
		te->baseID = _id;
		te->position = 1;
	}
}

struct Matrix88Widget : KnobMatrixModuleWidget {
	static constexpr int hp = 22;

	Matrix88Widget(Matrix88* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "Matrix88");
		createScrews();

		// generated by svg_widgets.rb
		auto mix11ParamPosition = Vec(53.7, 32.2);
		auto mix21ParamPosition = Vec(53.7, 69.2);
		auto mix31ParamPosition = Vec(53.7, 106.2);
		auto mix41ParamPosition = Vec(53.7, 143.2);
		auto mix51ParamPosition = Vec(53.7, 180.2);
		auto mix61ParamPosition = Vec(53.7, 217.2);
		auto mix71ParamPosition = Vec(53.7, 254.2);
		auto mix81ParamPosition = Vec(53.7, 291.2);
		auto mix12ParamPosition = Vec(88.7, 32.2);
		auto mix22ParamPosition = Vec(88.7, 69.2);
		auto mix32ParamPosition = Vec(88.7, 106.2);
		auto mix42ParamPosition = Vec(88.7, 143.2);
		auto mix52ParamPosition = Vec(88.7, 180.2);
		auto mix62ParamPosition = Vec(88.7, 217.2);
		auto mix72ParamPosition = Vec(88.7, 254.2);
		auto mix82ParamPosition = Vec(88.7, 291.2);
		auto mix13ParamPosition = Vec(123.7, 32.2);
		auto mix23ParamPosition = Vec(123.7, 69.2);
		auto mix33ParamPosition = Vec(123.7, 106.2);
		auto mix43ParamPosition = Vec(123.7, 143.2);
		auto mix53ParamPosition = Vec(123.7, 180.2);
		auto mix63ParamPosition = Vec(123.7, 217.2);
		auto mix73ParamPosition = Vec(123.7, 254.2);
		auto mix83ParamPosition = Vec(123.7, 291.2);
		auto mix14ParamPosition = Vec(158.7, 32.2);
		auto mix24ParamPosition = Vec(158.7, 69.2);
		auto mix34ParamPosition = Vec(158.7, 106.2);
		auto mix44ParamPosition = Vec(158.7, 143.2);
		auto mix54ParamPosition = Vec(158.7, 180.2);
		auto mix64ParamPosition = Vec(158.7, 217.2);
		auto mix74ParamPosition = Vec(158.7, 254.2);
		auto mix84ParamPosition = Vec(158.7, 291.2);
		auto mix15ParamPosition = Vec(193.7, 32.2);
		auto mix25ParamPosition = Vec(193.7, 69.2);
		auto mix35ParamPosition = Vec(193.7, 106.2);
		auto mix45ParamPosition = Vec(193.7, 143.2);
		auto mix55ParamPosition = Vec(193.7, 180.2);
		auto mix65ParamPosition = Vec(193.7, 217.2);
		auto mix75ParamPosition = Vec(193.7, 254.2);
		auto mix85ParamPosition = Vec(193.7, 291.2);
		auto mix16ParamPosition = Vec(228.7, 32.2);
		auto mix26ParamPosition = Vec(228.7, 69.2);
		auto mix36ParamPosition = Vec(228.7, 106.2);
		auto mix46ParamPosition = Vec(228.7, 143.2);
		auto mix56ParamPosition = Vec(228.7, 180.2);
		auto mix66ParamPosition = Vec(228.7, 217.2);
		auto mix76ParamPosition = Vec(228.7, 254.2);
		auto mix86ParamPosition = Vec(228.7, 291.2);
		auto mix17ParamPosition = Vec(263.7, 32.2);
		auto mix27ParamPosition = Vec(263.7, 69.2);
		auto mix37ParamPosition = Vec(263.7, 106.2);
		auto mix47ParamPosition = Vec(263.7, 143.2);
		auto mix57ParamPosition = Vec(263.7, 180.2);
		auto mix67ParamPosition = Vec(263.7, 217.2);
		auto mix77ParamPosition = Vec(263.7, 254.2);
		auto mix87ParamPosition = Vec(263.7, 291.2);
		auto mix18ParamPosition = Vec(298.7, 32.2);
		auto mix28ParamPosition = Vec(298.7, 69.2);
		auto mix38ParamPosition = Vec(298.7, 106.2);
		auto mix48ParamPosition = Vec(298.7, 143.2);
		auto mix58ParamPosition = Vec(298.7, 180.2);
		auto mix68ParamPosition = Vec(298.7, 217.2);
		auto mix78ParamPosition = Vec(298.7, 254.2);
		auto mix88ParamPosition = Vec(298.7, 291.2);

		auto in1InputPosition = Vec(10.5, 30.0);
		auto in2InputPosition = Vec(10.5, 67.0);
		auto in3InputPosition = Vec(10.5, 104.0);
		auto in4InputPosition = Vec(10.5, 141.0);
		auto in5InputPosition = Vec(10.5, 178.0);
		auto in6InputPosition = Vec(10.5, 215.0);
		auto in7InputPosition = Vec(10.5, 252.0);
		auto in8InputPosition = Vec(10.5, 289.0);

		auto out1OutputPosition = Vec(51.5, 328.0);
		auto out2OutputPosition = Vec(86.5, 328.0);
		auto out3OutputPosition = Vec(121.5, 328.0);
		auto out4OutputPosition = Vec(156.5, 328.0);
		auto out5OutputPosition = Vec(191.5, 328.0);
		auto out6OutputPosition = Vec(226.5, 328.0);
		auto out7OutputPosition = Vec(261.5, 328.0);
		auto out8OutputPosition = Vec(296.5, 328.0);
		// end generated by svg_widgets.rb

		createKnob(mix11ParamPosition, module, Matrix88::MIX11_PARAM);
		createKnob(mix21ParamPosition, module, Matrix88::MIX21_PARAM);
		createKnob(mix31ParamPosition, module, Matrix88::MIX31_PARAM);
		createKnob(mix41ParamPosition, module, Matrix88::MIX41_PARAM);
		createKnob(mix51ParamPosition, module, Matrix88::MIX51_PARAM);
		createKnob(mix61ParamPosition, module, Matrix88::MIX61_PARAM);
		createKnob(mix71ParamPosition, module, Matrix88::MIX71_PARAM);
		createKnob(mix81ParamPosition, module, Matrix88::MIX81_PARAM);
		createKnob(mix12ParamPosition, module, Matrix88::MIX12_PARAM);
		createKnob(mix22ParamPosition, module, Matrix88::MIX22_PARAM);
		createKnob(mix32ParamPosition, module, Matrix88::MIX32_PARAM);
		createKnob(mix42ParamPosition, module, Matrix88::MIX42_PARAM);
		createKnob(mix52ParamPosition, module, Matrix88::MIX52_PARAM);
		createKnob(mix62ParamPosition, module, Matrix88::MIX62_PARAM);
		createKnob(mix72ParamPosition, module, Matrix88::MIX72_PARAM);
		createKnob(mix82ParamPosition, module, Matrix88::MIX82_PARAM);
		createKnob(mix13ParamPosition, module, Matrix88::MIX13_PARAM);
		createKnob(mix23ParamPosition, module, Matrix88::MIX23_PARAM);
		createKnob(mix33ParamPosition, module, Matrix88::MIX33_PARAM);
		createKnob(mix43ParamPosition, module, Matrix88::MIX43_PARAM);
		createKnob(mix53ParamPosition, module, Matrix88::MIX53_PARAM);
		createKnob(mix63ParamPosition, module, Matrix88::MIX63_PARAM);
		createKnob(mix73ParamPosition, module, Matrix88::MIX73_PARAM);
		createKnob(mix83ParamPosition, module, Matrix88::MIX83_PARAM);
		createKnob(mix14ParamPosition, module, Matrix88::MIX14_PARAM);
		createKnob(mix24ParamPosition, module, Matrix88::MIX24_PARAM);
		createKnob(mix34ParamPosition, module, Matrix88::MIX34_PARAM);
		createKnob(mix44ParamPosition, module, Matrix88::MIX44_PARAM);
		createKnob(mix54ParamPosition, module, Matrix88::MIX54_PARAM);
		createKnob(mix64ParamPosition, module, Matrix88::MIX64_PARAM);
		createKnob(mix74ParamPosition, module, Matrix88::MIX74_PARAM);
		createKnob(mix84ParamPosition, module, Matrix88::MIX84_PARAM);
		createKnob(mix15ParamPosition, module, Matrix88::MIX15_PARAM);
		createKnob(mix25ParamPosition, module, Matrix88::MIX25_PARAM);
		createKnob(mix35ParamPosition, module, Matrix88::MIX35_PARAM);
		createKnob(mix45ParamPosition, module, Matrix88::MIX45_PARAM);
		createKnob(mix55ParamPosition, module, Matrix88::MIX55_PARAM);
		createKnob(mix65ParamPosition, module, Matrix88::MIX65_PARAM);
		createKnob(mix75ParamPosition, module, Matrix88::MIX75_PARAM);
		createKnob(mix85ParamPosition, module, Matrix88::MIX85_PARAM);
		createKnob(mix16ParamPosition, module, Matrix88::MIX16_PARAM);
		createKnob(mix26ParamPosition, module, Matrix88::MIX26_PARAM);
		createKnob(mix36ParamPosition, module, Matrix88::MIX36_PARAM);
		createKnob(mix46ParamPosition, module, Matrix88::MIX46_PARAM);
		createKnob(mix56ParamPosition, module, Matrix88::MIX56_PARAM);
		createKnob(mix66ParamPosition, module, Matrix88::MIX66_PARAM);
		createKnob(mix76ParamPosition, module, Matrix88::MIX76_PARAM);
		createKnob(mix86ParamPosition, module, Matrix88::MIX86_PARAM);
		createKnob(mix17ParamPosition, module, Matrix88::MIX17_PARAM);
		createKnob(mix27ParamPosition, module, Matrix88::MIX27_PARAM);
		createKnob(mix37ParamPosition, module, Matrix88::MIX37_PARAM);
		createKnob(mix47ParamPosition, module, Matrix88::MIX47_PARAM);
		createKnob(mix57ParamPosition, module, Matrix88::MIX57_PARAM);
		createKnob(mix67ParamPosition, module, Matrix88::MIX67_PARAM);
		createKnob(mix77ParamPosition, module, Matrix88::MIX77_PARAM);
		createKnob(mix87ParamPosition, module, Matrix88::MIX87_PARAM);
		createKnob(mix18ParamPosition, module, Matrix88::MIX18_PARAM);
		createKnob(mix28ParamPosition, module, Matrix88::MIX28_PARAM);
		createKnob(mix38ParamPosition, module, Matrix88::MIX38_PARAM);
		createKnob(mix48ParamPosition, module, Matrix88::MIX48_PARAM);
		createKnob(mix58ParamPosition, module, Matrix88::MIX58_PARAM);
		createKnob(mix68ParamPosition, module, Matrix88::MIX68_PARAM);
		createKnob(mix78ParamPosition, module, Matrix88::MIX78_PARAM);
		createKnob(mix88ParamPosition, module, Matrix88::MIX88_PARAM);

		addInput(createInput<Port24>(in1InputPosition, module, Matrix88::IN1_INPUT));
		addInput(createInput<Port24>(in2InputPosition, module, Matrix88::IN2_INPUT));
		addInput(createInput<Port24>(in3InputPosition, module, Matrix88::IN3_INPUT));
		addInput(createInput<Port24>(in4InputPosition, module, Matrix88::IN4_INPUT));
		addInput(createInput<Port24>(in5InputPosition, module, Matrix88::IN5_INPUT));
		addInput(createInput<Port24>(in6InputPosition, module, Matrix88::IN6_INPUT));
		addInput(createInput<Port24>(in7InputPosition, module, Matrix88::IN7_INPUT));
		addInput(createInput<Port24>(in8InputPosition, module, Matrix88::IN8_INPUT));

		addOutput(createOutput<Port24>(out1OutputPosition, module, Matrix88::OUT1_OUTPUT));
		addOutput(createOutput<Port24>(out2OutputPosition, module, Matrix88::OUT2_OUTPUT));
		addOutput(createOutput<Port24>(out3OutputPosition, module, Matrix88::OUT3_OUTPUT));
		addOutput(createOutput<Port24>(out4OutputPosition, module, Matrix88::OUT4_OUTPUT));
		addOutput(createOutput<Port24>(out5OutputPosition, module, Matrix88::OUT5_OUTPUT));
		addOutput(createOutput<Port24>(out6OutputPosition, module, Matrix88::OUT6_OUTPUT));
		addOutput(createOutput<Port24>(out7OutputPosition, module, Matrix88::OUT7_OUTPUT));
		addOutput(createOutput<Port24>(out8OutputPosition, module, Matrix88::OUT8_OUTPUT));
	}
};

Model* modelMatrix88 = bogaudio::createModel<Matrix88, Matrix88Widget>("Bogaudio-Matrix88", "MATRIX88", "8x8 matrix mixer", "Mixer", "Polyphonic");
