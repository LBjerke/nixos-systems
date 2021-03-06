
#include "PolyOff8.hpp"

#define OFFSET_FIRST "offset_first"

json_t* PolyOff8::saveToJson(json_t* root) {
	root = OutputRangeModule<BGModule>::saveToJson(root);
	json_object_set_new(root, OFFSET_FIRST, json_boolean(_offsetFirst));
	return root;
}

void PolyOff8::loadFromJson(json_t* root) {
	OutputRangeModule<BGModule>::loadFromJson(root);
	json_t* of = json_object_get(root, OFFSET_FIRST);
	if (of) {
		_offsetFirst = json_boolean_value(of);
	}
}

void PolyOff8::processAll(const ProcessArgs& args) {
	int cn = 1;
	if (inputs[IN_INPUT].isConnected()) {
		cn = clamp(inputs[IN_INPUT].getChannels(), 1, 8);
		outputs[OUT_OUTPUT].setChannels(cn);

		for (int c = 0; c < cn; ++c) {
			float offset = clamp(params[OFFSET1_PARAM + 2 * c].getValue(), -1.0f, 1.0f);
			if (inputs[CV1_INPUT + c].isConnected()) {
				offset *= clamp(inputs[CV1_INPUT + c].getVoltage() / 5.0f, -1.0f, 1.0f);
			}
			offset += _rangeOffset;
			offset *= _rangeScale;

			float scale = clamp(params[SCALE1_PARAM + 2 * c].getValue(), -1.0f, 1.0f);

			float out = inputs[IN_INPUT].getPolyVoltage(c);
			if (_offsetFirst) {
				out += offset;
				out *= scale;
			}
			else {
				out *= scale;
				out += offset;
			}
			outputs[OUT_OUTPUT].setVoltage(clamp(out, -12.0f, 12.0f), c);
		}
	}
	else {
		cn = clamp(params[CHANNELS_PARAM].getValue(), 1.0f, 8.0f);
		outputs[OUT_OUTPUT].setChannels(cn);

		for (int c = 0; c < cn; ++c) {
			float offset = clamp(params[OFFSET1_PARAM + 2 * c].getValue(), -1.0f, 1.0f);
			offset += _rangeOffset;
			offset *= _rangeScale;

			float scale = clamp(params[SCALE1_PARAM + 2 * c].getValue(), -1.0f, 1.0f);

			float out = inputs[CV1_INPUT + c].getVoltage();
			if (_offsetFirst) {
				out += offset;
				out *= scale;
			}
			else {
				out *= scale;
				out += offset;
			}
			outputs[OUT_OUTPUT].setVoltage(clamp(out, -12.0f, 12.0f), c);
		}
	}

	for (int c = 0; c < 8; ++c) {
		lights[CHANNEL1_LIGHT + c].value = (c < cn) * 1.0f;
	}
}

struct PolyOff8Widget : BGModuleWidget {
	static constexpr int hp = 8;

	PolyOff8Widget(PolyOff8* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "PolyOff8");
		createScrews();

		// generated by svg_widgets.rb
		auto offset1ParamPosition = Vec(62.5, 33.0);
		auto scale1ParamPosition = Vec(93.5, 33.0);
		auto offset2ParamPosition = Vec(62.5, 68.0);
		auto scale2ParamPosition = Vec(93.5, 68.0);
		auto offset3ParamPosition = Vec(62.5, 103.0);
		auto scale3ParamPosition = Vec(93.5, 103.0);
		auto offset4ParamPosition = Vec(62.5, 138.0);
		auto scale4ParamPosition = Vec(93.5, 138.0);
		auto offset5ParamPosition = Vec(62.5, 173.0);
		auto scale5ParamPosition = Vec(93.5, 173.0);
		auto offset6ParamPosition = Vec(62.5, 208.0);
		auto scale6ParamPosition = Vec(93.5, 208.0);
		auto offset7ParamPosition = Vec(62.5, 243.0);
		auto scale7ParamPosition = Vec(93.5, 243.0);
		auto offset8ParamPosition = Vec(62.5, 278.0);
		auto scale8ParamPosition = Vec(93.5, 278.0);
		auto channelsParamPosition = Vec(23.0, 332.0);

		auto cv1InputPosition = Vec(10.5, 29.0);
		auto cv2InputPosition = Vec(10.5, 64.0);
		auto cv3InputPosition = Vec(10.5, 99.0);
		auto cv4InputPosition = Vec(10.5, 134.0);
		auto cv5InputPosition = Vec(10.5, 169.0);
		auto cv6InputPosition = Vec(10.5, 204.0);
		auto cv7InputPosition = Vec(10.5, 239.0);
		auto cv8InputPosition = Vec(10.5, 274.0);
		auto inInputPosition = Vec(55.5, 322.0);

		auto outOutputPosition = Vec(86.5, 322.0);

		auto channel1LightPosition = Vec(46.8, 43.0);
		auto channel2LightPosition = Vec(46.8, 78.0);
		auto channel3LightPosition = Vec(46.8, 113.0);
		auto channel4LightPosition = Vec(46.8, 148.0);
		auto channel5LightPosition = Vec(46.8, 183.0);
		auto channel6LightPosition = Vec(46.8, 218.0);
		auto channel7LightPosition = Vec(46.8, 253.0);
		auto channel8LightPosition = Vec(46.8, 288.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob16>(offset1ParamPosition, module, PolyOff8::OFFSET1_PARAM));
		addParam(createParam<Knob16>(scale1ParamPosition, module, PolyOff8::SCALE1_PARAM));
		addParam(createParam<Knob16>(offset2ParamPosition, module, PolyOff8::OFFSET2_PARAM));
		addParam(createParam<Knob16>(scale2ParamPosition, module, PolyOff8::SCALE2_PARAM));
		addParam(createParam<Knob16>(offset3ParamPosition, module, PolyOff8::OFFSET3_PARAM));
		addParam(createParam<Knob16>(scale3ParamPosition, module, PolyOff8::SCALE3_PARAM));
		addParam(createParam<Knob16>(offset4ParamPosition, module, PolyOff8::OFFSET4_PARAM));
		addParam(createParam<Knob16>(scale4ParamPosition, module, PolyOff8::SCALE4_PARAM));
		addParam(createParam<Knob16>(offset5ParamPosition, module, PolyOff8::OFFSET5_PARAM));
		addParam(createParam<Knob16>(scale5ParamPosition, module, PolyOff8::SCALE5_PARAM));
		addParam(createParam<Knob16>(offset6ParamPosition, module, PolyOff8::OFFSET6_PARAM));
		addParam(createParam<Knob16>(scale6ParamPosition, module, PolyOff8::SCALE6_PARAM));
		addParam(createParam<Knob16>(offset7ParamPosition, module, PolyOff8::OFFSET7_PARAM));
		addParam(createParam<Knob16>(scale7ParamPosition, module, PolyOff8::SCALE7_PARAM));
		addParam(createParam<Knob16>(offset8ParamPosition, module, PolyOff8::OFFSET8_PARAM));
		addParam(createParam<Knob16>(scale8ParamPosition, module, PolyOff8::SCALE8_PARAM));
		addParam(createParam<Knob16>(channelsParamPosition, module, PolyOff8::CHANNELS_PARAM));

		addInput(createInput<Port24>(cv1InputPosition, module, PolyOff8::CV1_INPUT));
		addInput(createInput<Port24>(cv2InputPosition, module, PolyOff8::CV2_INPUT));
		addInput(createInput<Port24>(cv3InputPosition, module, PolyOff8::CV3_INPUT));
		addInput(createInput<Port24>(cv4InputPosition, module, PolyOff8::CV4_INPUT));
		addInput(createInput<Port24>(cv5InputPosition, module, PolyOff8::CV5_INPUT));
		addInput(createInput<Port24>(cv6InputPosition, module, PolyOff8::CV6_INPUT));
		addInput(createInput<Port24>(cv7InputPosition, module, PolyOff8::CV7_INPUT));
		addInput(createInput<Port24>(cv8InputPosition, module, PolyOff8::CV8_INPUT));
		addInput(createInput<Port24>(inInputPosition, module, PolyOff8::IN_INPUT));

		addOutput(createOutput<Port24>(outOutputPosition, module, PolyOff8::OUT_OUTPUT));

		addChild(createLight<BGTinyLight<GreenLight>>(channel1LightPosition, module, PolyOff8::CHANNEL1_LIGHT));
		addChild(createLight<BGTinyLight<GreenLight>>(channel2LightPosition, module, PolyOff8::CHANNEL2_LIGHT));
		addChild(createLight<BGTinyLight<GreenLight>>(channel3LightPosition, module, PolyOff8::CHANNEL3_LIGHT));
		addChild(createLight<BGTinyLight<GreenLight>>(channel4LightPosition, module, PolyOff8::CHANNEL4_LIGHT));
		addChild(createLight<BGTinyLight<GreenLight>>(channel5LightPosition, module, PolyOff8::CHANNEL5_LIGHT));
		addChild(createLight<BGTinyLight<GreenLight>>(channel6LightPosition, module, PolyOff8::CHANNEL6_LIGHT));
		addChild(createLight<BGTinyLight<GreenLight>>(channel7LightPosition, module, PolyOff8::CHANNEL7_LIGHT));
		addChild(createLight<BGTinyLight<GreenLight>>(channel8LightPosition, module, PolyOff8::CHANNEL8_LIGHT));
	}

	void contextMenu(Menu* menu) override {
		OutputRangeOptionMenuItem::addOutputRangeOptionsToMenu(module, menu);

		auto m = dynamic_cast<PolyOff8*>(module);
		assert(m);

		OptionsMenuItem* ooo = new OptionsMenuItem("Order of operations");
		ooo->addItem(OptionMenuItem("Scale, then offset", [m]() { return !m->_offsetFirst; }, [m]() { m->_offsetFirst = false; }));
		ooo->addItem(OptionMenuItem("Offset, then scale", [m]() { return m->_offsetFirst; }, [m]() { m->_offsetFirst = true; }));
		OptionsMenuItem::addToMenu(ooo, menu);
	}
};

Model* modelPolyOff8 = createModel<PolyOff8, PolyOff8Widget>("Bogaudio-PolyOff8", "POLYOFF8", "Polyphonic per-channel offset and scale", "Polyphonic");
