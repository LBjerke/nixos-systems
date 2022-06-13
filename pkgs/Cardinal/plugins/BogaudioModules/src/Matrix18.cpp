
#include "Matrix18.hpp"

struct Matrix18Widget : KnobMatrixModuleWidget {
	static constexpr int hp = 6;

	Matrix18Widget(Matrix18* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "Matrix18");
		createScrews();

		// generated by svg_widgets.rb
		auto mix1ParamPosition = Vec(14.7, 32.2);
		auto mix2ParamPosition = Vec(14.7, 69.2);
		auto mix3ParamPosition = Vec(14.7, 106.2);
		auto mix4ParamPosition = Vec(14.7, 143.2);
		auto mix5ParamPosition = Vec(14.7, 180.2);
		auto mix6ParamPosition = Vec(14.7, 217.2);
		auto mix7ParamPosition = Vec(14.7, 254.2);
		auto mix8ParamPosition = Vec(14.7, 291.2);

		auto inInputPosition = Vec(53.5, 321.0);

		auto out1OutputPosition = Vec(53.5, 30.0);
		auto out2OutputPosition = Vec(53.5, 67.0);
		auto out3OutputPosition = Vec(53.5, 104.0);
		auto out4OutputPosition = Vec(53.5, 141.0);
		auto out5OutputPosition = Vec(53.5, 178.0);
		auto out6OutputPosition = Vec(53.5, 215.0);
		auto out7OutputPosition = Vec(53.5, 252.0);
		auto out8OutputPosition = Vec(53.5, 289.0);
		// end generated by svg_widgets.rb

		createKnob(mix1ParamPosition, module, Matrix18::MIX1_PARAM);
		createKnob(mix2ParamPosition, module, Matrix18::MIX2_PARAM);
		createKnob(mix3ParamPosition, module, Matrix18::MIX3_PARAM);
		createKnob(mix4ParamPosition, module, Matrix18::MIX4_PARAM);
		createKnob(mix5ParamPosition, module, Matrix18::MIX5_PARAM);
		createKnob(mix6ParamPosition, module, Matrix18::MIX6_PARAM);
		createKnob(mix7ParamPosition, module, Matrix18::MIX7_PARAM);
		createKnob(mix8ParamPosition, module, Matrix18::MIX8_PARAM);

		addInput(createInput<Port24>(inInputPosition, module, Matrix18::IN_INPUT));

		addOutput(createOutput<Port24>(out1OutputPosition, module, Matrix18::OUT1_OUTPUT));
		addOutput(createOutput<Port24>(out2OutputPosition, module, Matrix18::OUT2_OUTPUT));
		addOutput(createOutput<Port24>(out3OutputPosition, module, Matrix18::OUT3_OUTPUT));
		addOutput(createOutput<Port24>(out4OutputPosition, module, Matrix18::OUT4_OUTPUT));
		addOutput(createOutput<Port24>(out5OutputPosition, module, Matrix18::OUT5_OUTPUT));
		addOutput(createOutput<Port24>(out6OutputPosition, module, Matrix18::OUT6_OUTPUT));
		addOutput(createOutput<Port24>(out7OutputPosition, module, Matrix18::OUT7_OUTPUT));
		addOutput(createOutput<Port24>(out8OutputPosition, module, Matrix18::OUT8_OUTPUT));
	}
};

Model* modelMatrix18 = createModel<Matrix18, Matrix18Widget>("Bogaudio-Matrix18", "MATRIX18", "8-way signal router with attenuverters", "Switch", "Polyphonic");
