
#include "Matrix81.hpp"

struct Matrix81Widget : KnobMatrixModuleWidget {
	static constexpr int hp = 6;

	Matrix81Widget(Matrix81* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "Matrix81");
		createScrews();

		// generated by svg_widgets.rb
		auto mix1ParamPosition = Vec(55.7, 32.2);
		auto mix2ParamPosition = Vec(55.7, 69.2);
		auto mix3ParamPosition = Vec(55.7, 106.2);
		auto mix4ParamPosition = Vec(55.7, 143.2);
		auto mix5ParamPosition = Vec(55.7, 180.2);
		auto mix6ParamPosition = Vec(55.7, 217.2);
		auto mix7ParamPosition = Vec(55.7, 254.2);
		auto mix8ParamPosition = Vec(55.7, 291.2);

		auto in1InputPosition = Vec(12.5, 30.0);
		auto in2InputPosition = Vec(12.5, 67.0);
		auto in3InputPosition = Vec(12.5, 104.0);
		auto in4InputPosition = Vec(12.5, 141.0);
		auto in5InputPosition = Vec(12.5, 178.0);
		auto in6InputPosition = Vec(12.5, 215.0);
		auto in7InputPosition = Vec(12.5, 252.0);
		auto in8InputPosition = Vec(12.5, 289.0);

		auto outOutputPosition = Vec(12.5, 321.0);
		// end generated by svg_widgets.rb

		createKnob(mix1ParamPosition, module, Matrix81::MIX1_PARAM);
		createKnob(mix2ParamPosition, module, Matrix81::MIX2_PARAM);
		createKnob(mix3ParamPosition, module, Matrix81::MIX3_PARAM);
		createKnob(mix4ParamPosition, module, Matrix81::MIX4_PARAM);
		createKnob(mix5ParamPosition, module, Matrix81::MIX5_PARAM);
		createKnob(mix6ParamPosition, module, Matrix81::MIX6_PARAM);
		createKnob(mix7ParamPosition, module, Matrix81::MIX7_PARAM);
		createKnob(mix8ParamPosition, module, Matrix81::MIX8_PARAM);

		addInput(createInput<Port24>(in1InputPosition, module, Matrix81::IN1_INPUT));
		addInput(createInput<Port24>(in2InputPosition, module, Matrix81::IN2_INPUT));
		addInput(createInput<Port24>(in3InputPosition, module, Matrix81::IN3_INPUT));
		addInput(createInput<Port24>(in4InputPosition, module, Matrix81::IN4_INPUT));
		addInput(createInput<Port24>(in5InputPosition, module, Matrix81::IN5_INPUT));
		addInput(createInput<Port24>(in6InputPosition, module, Matrix81::IN6_INPUT));
		addInput(createInput<Port24>(in7InputPosition, module, Matrix81::IN7_INPUT));
		addInput(createInput<Port24>(in8InputPosition, module, Matrix81::IN8_INPUT));

		addOutput(createOutput<Port24>(outOutputPosition, module, Matrix81::OUT_OUTPUT));
	}
};

Model* modelMatrix81 = createModel<Matrix81, Matrix81Widget>("Bogaudio-Matrix81", "MATRIX81", "8-channel mixer", "Mixer", "Polyphonic");
