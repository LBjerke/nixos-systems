
#include "Mix8x.hpp"

void Mix8x::sampleRateChange() {
	float sr = APP->engine->getSampleRate();
	for (int i = 0; i < 8; ++i) {
		_channels[i]->setSampleRate(sr);
	}
	_returnASL.setParams(sr, MixerChannel::levelSlewTimeMS, MixerChannel::maxDecibels - MixerChannel::minDecibels);
	_returnBSL.setParams(sr, MixerChannel::levelSlewTimeMS, MixerChannel::maxDecibels - MixerChannel::minDecibels);
}

void Mix8x::modulate() {
	for (int i = 0; i < 8; ++i) {
		_channels[i]->modulate();
	}
}

void Mix8x::processAll(const ProcessArgs& args) {
	if (!baseConnected()) {
		outputs[SEND_A_OUTPUT].setVoltage(0.0f);
		outputs[SEND_B_OUTPUT].setVoltage(0.0f);
		return;
	}

	Mix8ExpanderMessage* from = fromBase();
	Mix8ExpanderMessage* to = toBase();
	float sendA = 0.0f;
	float sendB = 0.0f;
	bool sendAActive = outputs[SEND_A_OUTPUT].isConnected();
	bool sendBActive = outputs[SEND_B_OUTPUT].isConnected();
	for (int i = 0; i < 8; ++i) {
		if (from->active[i]) {
			_channels[i]->next(from->preFader[i], from->postFader[i], sendAActive, sendBActive);
			to->postEQ[i] = _channels[i]->postEQ;
			sendA += _channels[i]->sendA;
			sendB += _channels[i]->sendB;
		}
		else {
			to->postEQ[i] = from->preFader[i];
		}
	}
	outputs[SEND_A_OUTPUT].setVoltage(_saturatorA.next(sendA));
	outputs[SEND_B_OUTPUT].setVoltage(_saturatorA.next(sendB));

	bool lAActive = inputs[L_A_INPUT].isConnected();
	bool rAActive = inputs[R_A_INPUT].isConnected();
	if (lAActive || rAActive) {
		float levelA = clamp(params[LEVEL_A_PARAM].getValue(), 0.0f, 1.0f);
		if (inputs[LEVEL_A_INPUT].isConnected()) {
			levelA *= clamp(inputs[LEVEL_A_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
		}
		levelA = 1.0f - levelA;
		levelA *= Amplifier::minDecibels;
		_returnAAmp.setLevel(_returnASL.next(levelA));
		if (lAActive) {
			to->returnA[0] = _returnAAmp.next(inputs[L_A_INPUT].getVoltage());
		}
		else {
			to->returnA[0] = 0.0f;
		}
		if (rAActive) {
			to->returnA[1] = _returnAAmp.next(inputs[R_A_INPUT].getVoltage());
		}
		else {
			to->returnA[1] = to->returnA[0];
		}
	}

	bool lBActive = inputs[L_B_INPUT].isConnected();
	bool rBActive = inputs[R_B_INPUT].isConnected();
	if (lBActive || rBActive) {
		float levelB = clamp(params[LEVEL_B_PARAM].getValue(), 0.0f, 1.0f);
		levelB = 1.0f - levelB;
		levelB *= Amplifier::minDecibels;
		_returnBAmp.setLevel(_returnBSL.next(levelB));
		if (lBActive) {
			to->returnB[0] = _returnBAmp.next(inputs[L_B_INPUT].getVoltage());
		}
		else {
			to->returnB[0] = 0.0f;
		}
		if (rBActive) {
			to->returnB[1] = _returnBAmp.next(inputs[R_B_INPUT].getVoltage());
		}
		else {
			to->returnB[1] = to->returnB[0];
		}
	}
}

struct Mix8xWidget : BGModuleWidget {
	static constexpr int hp = 27;

	Mix8xWidget(Mix8x* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "Mix8x");
		createScrews();

		// generated by svg_widgets.rb
		auto low1ParamPosition = Vec(18.5, 43.0);
		auto mid1ParamPosition = Vec(18.5, 89.0);
		auto high1ParamPosition = Vec(18.5, 135.0);
		auto a1ParamPosition = Vec(18.5, 180.0);
		auto preA1ParamPosition = Vec(30.0, 208.0);
		auto b1ParamPosition = Vec(18.5, 236.0);
		auto preB1ParamPosition = Vec(30.0, 264.0);
		auto low2ParamPosition = Vec(62.5, 43.0);
		auto mid2ParamPosition = Vec(62.5, 89.0);
		auto high2ParamPosition = Vec(62.5, 135.0);
		auto a2ParamPosition = Vec(62.5, 180.0);
		auto preA2ParamPosition = Vec(74.0, 208.0);
		auto b2ParamPosition = Vec(62.5, 236.0);
		auto preB2ParamPosition = Vec(74.0, 264.0);
		auto low3ParamPosition = Vec(106.5, 43.0);
		auto mid3ParamPosition = Vec(106.5, 89.0);
		auto high3ParamPosition = Vec(106.5, 135.0);
		auto a3ParamPosition = Vec(106.5, 180.0);
		auto preA3ParamPosition = Vec(118.0, 208.0);
		auto b3ParamPosition = Vec(106.5, 236.0);
		auto preB3ParamPosition = Vec(118.0, 264.0);
		auto low4ParamPosition = Vec(150.5, 43.0);
		auto mid4ParamPosition = Vec(150.5, 89.0);
		auto high4ParamPosition = Vec(150.5, 135.0);
		auto a4ParamPosition = Vec(150.5, 180.0);
		auto preA4ParamPosition = Vec(162.0, 208.0);
		auto b4ParamPosition = Vec(150.5, 236.0);
		auto preB4ParamPosition = Vec(162.0, 264.0);
		auto low5ParamPosition = Vec(194.5, 43.0);
		auto mid5ParamPosition = Vec(194.5, 89.0);
		auto high5ParamPosition = Vec(194.5, 135.0);
		auto a5ParamPosition = Vec(194.5, 180.0);
		auto preA5ParamPosition = Vec(206.0, 208.0);
		auto b5ParamPosition = Vec(194.5, 236.0);
		auto preB5ParamPosition = Vec(206.0, 264.0);
		auto low6ParamPosition = Vec(238.5, 43.0);
		auto mid6ParamPosition = Vec(238.5, 89.0);
		auto high6ParamPosition = Vec(238.5, 135.0);
		auto a6ParamPosition = Vec(238.5, 180.0);
		auto preA6ParamPosition = Vec(250.0, 208.0);
		auto b6ParamPosition = Vec(238.5, 236.0);
		auto preB6ParamPosition = Vec(250.0, 264.0);
		auto low7ParamPosition = Vec(282.5, 43.0);
		auto mid7ParamPosition = Vec(282.5, 89.0);
		auto high7ParamPosition = Vec(282.5, 135.0);
		auto a7ParamPosition = Vec(282.5, 180.0);
		auto preA7ParamPosition = Vec(294.0, 208.0);
		auto b7ParamPosition = Vec(282.5, 236.0);
		auto preB7ParamPosition = Vec(294.0, 264.0);
		auto low8ParamPosition = Vec(326.5, 43.0);
		auto mid8ParamPosition = Vec(326.5, 89.0);
		auto high8ParamPosition = Vec(326.5, 135.0);
		auto a8ParamPosition = Vec(326.5, 180.0);
		auto preA8ParamPosition = Vec(338.0, 208.0);
		auto b8ParamPosition = Vec(326.5, 236.0);
		auto preB8ParamPosition = Vec(338.0, 264.0);
		auto levelAParamPosition = Vec(370.5, 138.0);
		auto levelBParamPosition = Vec(370.5, 328.0);

		auto a1InputPosition = Vec(14.5, 290.0);
		auto b1InputPosition = Vec(14.5, 325.0);
		auto a2InputPosition = Vec(58.5, 290.0);
		auto b2InputPosition = Vec(58.5, 325.0);
		auto a3InputPosition = Vec(102.5, 290.0);
		auto b3InputPosition = Vec(102.5, 325.0);
		auto a4InputPosition = Vec(146.5, 290.0);
		auto b4InputPosition = Vec(146.5, 325.0);
		auto a5InputPosition = Vec(190.5, 290.0);
		auto b5InputPosition = Vec(190.5, 325.0);
		auto a6InputPosition = Vec(234.5, 290.0);
		auto b6InputPosition = Vec(234.5, 325.0);
		auto a7InputPosition = Vec(278.5, 290.0);
		auto b7InputPosition = Vec(278.5, 325.0);
		auto a8InputPosition = Vec(322.5, 290.0);
		auto b8InputPosition = Vec(322.5, 325.0);
		auto lAInputPosition = Vec(366.5, 62.0);
		auto rAInputPosition = Vec(366.5, 97.0);
		auto levelAInputPosition = Vec(366.5, 170.0);
		auto lBInputPosition = Vec(366.5, 252.0);
		auto rBInputPosition = Vec(366.5, 287.0);

		auto sendAOutputPosition = Vec(366.5, 24.0);
		auto sendBOutputPosition = Vec(366.5, 214.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob16>(low1ParamPosition, module, Mix8x::LOW1_PARAM));
		addParam(createParam<Knob16>(mid1ParamPosition, module, Mix8x::MID1_PARAM));
		addParam(createParam<Knob16>(high1ParamPosition, module, Mix8x::HIGH1_PARAM));
		addParam(createParam<Knob16>(a1ParamPosition, module, Mix8x::A1_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(preA1ParamPosition, module, Mix8x::PRE_A1_PARAM));
		addParam(createParam<Knob16>(b1ParamPosition, module, Mix8x::B1_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(preB1ParamPosition, module, Mix8x::PRE_B1_PARAM));
		addParam(createParam<Knob16>(low2ParamPosition, module, Mix8x::LOW2_PARAM));
		addParam(createParam<Knob16>(mid2ParamPosition, module, Mix8x::MID2_PARAM));
		addParam(createParam<Knob16>(high2ParamPosition, module, Mix8x::HIGH2_PARAM));
		addParam(createParam<Knob16>(a2ParamPosition, module, Mix8x::A2_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(preA2ParamPosition, module, Mix8x::PRE_A2_PARAM));
		addParam(createParam<Knob16>(b2ParamPosition, module, Mix8x::B2_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(preB2ParamPosition, module, Mix8x::PRE_B2_PARAM));
		addParam(createParam<Knob16>(low3ParamPosition, module, Mix8x::LOW3_PARAM));
		addParam(createParam<Knob16>(mid3ParamPosition, module, Mix8x::MID3_PARAM));
		addParam(createParam<Knob16>(high3ParamPosition, module, Mix8x::HIGH3_PARAM));
		addParam(createParam<Knob16>(a3ParamPosition, module, Mix8x::A3_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(preA3ParamPosition, module, Mix8x::PRE_A3_PARAM));
		addParam(createParam<Knob16>(b3ParamPosition, module, Mix8x::B3_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(preB3ParamPosition, module, Mix8x::PRE_B3_PARAM));
		addParam(createParam<Knob16>(low4ParamPosition, module, Mix8x::LOW4_PARAM));
		addParam(createParam<Knob16>(mid4ParamPosition, module, Mix8x::MID4_PARAM));
		addParam(createParam<Knob16>(high4ParamPosition, module, Mix8x::HIGH4_PARAM));
		addParam(createParam<Knob16>(a4ParamPosition, module, Mix8x::A4_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(preA4ParamPosition, module, Mix8x::PRE_A4_PARAM));
		addParam(createParam<Knob16>(b4ParamPosition, module, Mix8x::B4_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(preB4ParamPosition, module, Mix8x::PRE_B4_PARAM));
		addParam(createParam<Knob16>(low5ParamPosition, module, Mix8x::LOW5_PARAM));
		addParam(createParam<Knob16>(mid5ParamPosition, module, Mix8x::MID5_PARAM));
		addParam(createParam<Knob16>(high5ParamPosition, module, Mix8x::HIGH5_PARAM));
		addParam(createParam<Knob16>(a5ParamPosition, module, Mix8x::A5_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(preA5ParamPosition, module, Mix8x::PRE_A5_PARAM));
		addParam(createParam<Knob16>(b5ParamPosition, module, Mix8x::B5_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(preB5ParamPosition, module, Mix8x::PRE_B5_PARAM));
		addParam(createParam<Knob16>(low6ParamPosition, module, Mix8x::LOW6_PARAM));
		addParam(createParam<Knob16>(mid6ParamPosition, module, Mix8x::MID6_PARAM));
		addParam(createParam<Knob16>(high6ParamPosition, module, Mix8x::HIGH6_PARAM));
		addParam(createParam<Knob16>(a6ParamPosition, module, Mix8x::A6_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(preA6ParamPosition, module, Mix8x::PRE_A6_PARAM));
		addParam(createParam<Knob16>(b6ParamPosition, module, Mix8x::B6_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(preB6ParamPosition, module, Mix8x::PRE_B6_PARAM));
		addParam(createParam<Knob16>(low7ParamPosition, module, Mix8x::LOW7_PARAM));
		addParam(createParam<Knob16>(mid7ParamPosition, module, Mix8x::MID7_PARAM));
		addParam(createParam<Knob16>(high7ParamPosition, module, Mix8x::HIGH7_PARAM));
		addParam(createParam<Knob16>(a7ParamPosition, module, Mix8x::A7_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(preA7ParamPosition, module, Mix8x::PRE_A7_PARAM));
		addParam(createParam<Knob16>(b7ParamPosition, module, Mix8x::B7_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(preB7ParamPosition, module, Mix8x::PRE_B7_PARAM));
		addParam(createParam<Knob16>(low8ParamPosition, module, Mix8x::LOW8_PARAM));
		addParam(createParam<Knob16>(mid8ParamPosition, module, Mix8x::MID8_PARAM));
		addParam(createParam<Knob16>(high8ParamPosition, module, Mix8x::HIGH8_PARAM));
		addParam(createParam<Knob16>(a8ParamPosition, module, Mix8x::A8_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(preA8ParamPosition, module, Mix8x::PRE_A8_PARAM));
		addParam(createParam<Knob16>(b8ParamPosition, module, Mix8x::B8_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(preB8ParamPosition, module, Mix8x::PRE_B8_PARAM));
		addParam(createParam<Knob16>(levelAParamPosition, module, Mix8x::LEVEL_A_PARAM));
		addParam(createParam<Knob16>(levelBParamPosition, module, Mix8x::LEVEL_B_PARAM));

		addInput(createInput<Port24>(a1InputPosition, module, Mix8x::A1_INPUT));
		addInput(createInput<Port24>(b1InputPosition, module, Mix8x::B1_INPUT));
		addInput(createInput<Port24>(a2InputPosition, module, Mix8x::A2_INPUT));
		addInput(createInput<Port24>(b2InputPosition, module, Mix8x::B2_INPUT));
		addInput(createInput<Port24>(a3InputPosition, module, Mix8x::A3_INPUT));
		addInput(createInput<Port24>(b3InputPosition, module, Mix8x::B3_INPUT));
		addInput(createInput<Port24>(a4InputPosition, module, Mix8x::A4_INPUT));
		addInput(createInput<Port24>(b4InputPosition, module, Mix8x::B4_INPUT));
		addInput(createInput<Port24>(a5InputPosition, module, Mix8x::A5_INPUT));
		addInput(createInput<Port24>(b5InputPosition, module, Mix8x::B5_INPUT));
		addInput(createInput<Port24>(a6InputPosition, module, Mix8x::A6_INPUT));
		addInput(createInput<Port24>(b6InputPosition, module, Mix8x::B6_INPUT));
		addInput(createInput<Port24>(a7InputPosition, module, Mix8x::A7_INPUT));
		addInput(createInput<Port24>(b7InputPosition, module, Mix8x::B7_INPUT));
		addInput(createInput<Port24>(a8InputPosition, module, Mix8x::A8_INPUT));
		addInput(createInput<Port24>(b8InputPosition, module, Mix8x::B8_INPUT));
		addInput(createInput<Port24>(lAInputPosition, module, Mix8x::L_A_INPUT));
		addInput(createInput<Port24>(rAInputPosition, module, Mix8x::R_A_INPUT));
		addInput(createInput<Port24>(levelAInputPosition, module, Mix8x::LEVEL_A_INPUT));
		addInput(createInput<Port24>(lBInputPosition, module, Mix8x::L_B_INPUT));
		addInput(createInput<Port24>(rBInputPosition, module, Mix8x::R_B_INPUT));

		addOutput(createOutput<Port24>(sendAOutputPosition, module, Mix8x::SEND_A_OUTPUT));
		addOutput(createOutput<Port24>(sendBOutputPosition, module, Mix8x::SEND_B_OUTPUT));
	}
};

Model* modelMix8x = createModel<Mix8x, Mix8xWidget>("Bogaudio-Mix8x", "MIX8X", "Expander for MIX8, adds EQs and sends", "Mixer", "Expander");