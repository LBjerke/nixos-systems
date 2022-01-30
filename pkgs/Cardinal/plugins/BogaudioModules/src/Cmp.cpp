
#include "Cmp.hpp"

void Cmp::reset() {
	for (int i = 0; i < maxChannels; ++i) {
		_thresholdState[i] = LOW;
		_windowState[i] = LOW;
	}
}

bool Cmp::active() {
	return (
		outputs[GREATER_OUTPUT].isConnected() ||
		outputs[LESS_OUTPUT].isConnected() ||
		outputs[EQUAL_OUTPUT].isConnected() ||
		outputs[NOT_EQUAL_OUTPUT].isConnected()
	);
}

int Cmp::channels() {
	return inputs[A_INPUT].getChannels();
}

void Cmp::modulate() {
	_highValue = 10.0f;
	_lowValue = 0.0f;
	if (params[OUTPUT_PARAM].getValue() > 0.5f) {
		_highValue = 5.0f;
		_lowValue = -5.0f;
	}
}

void Cmp::modulateChannel(int c) {
	float lag = params[LAG_PARAM].getValue();
	if (inputs[LAG_INPUT].isConnected()) {
		lag *= clamp(inputs[LAG_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}
	_lagInSamples[c] = lag * lag * APP->engine->getSampleRate();
}

void Cmp::processChannel(const ProcessArgs& args, int c) {
	float a = params[A_PARAM].getValue() * 10.0f;
	if (inputs[A_INPUT].isConnected()) {
		a = clamp(a + inputs[A_INPUT].getPolyVoltage(c), -12.0f, 12.0f);
	}

	float b = params[B_PARAM].getValue() * 10.0f;
	if (inputs[B_INPUT].isConnected()) {
		b = clamp(b + inputs[B_INPUT].getPolyVoltage(c), -12.0f, 12.0f);
	}

	float window = params[WINDOW_PARAM].getValue();
	if (inputs[WINDOW_INPUT].isConnected()) {
		window *= clamp(inputs[WINDOW_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}
	window *= 10.0f;

	stepChannel(
		c,
		a >= b,
		_thresholdState[c],
		_thresholdLag[c],
		outputs[GREATER_OUTPUT],
		outputs[LESS_OUTPUT]
	);
	stepChannel(
		c,
		fabsf(a - b) <= window,
		_windowState[c],
		_windowLag[c],
		outputs[EQUAL_OUTPUT],
		outputs[NOT_EQUAL_OUTPUT]
	);
}

void Cmp::stepChannel(
	int c,
	bool high,
	State& state,
	int& channelLag,
	Output& highOutput,
	Output& lowOutput
) {
	switch (state) {
		case LOW: {
			if (high) {
				if (_lagInSamples[c] < 1) {
					state = HIGH;
				}
				else {
					state = LAG_HIGH;
					channelLag = _lagInSamples[c];
				}
			}
			break;
		}
		case HIGH: {
			if (!high) {
				if (_lagInSamples[c] < 1) {
					state = LOW;
				}
				else {
					state = LAG_LOW;
					channelLag = _lagInSamples[c];
				}
			}
			break;
		}
		case LAG_LOW: {
			if (!high) {
				--channelLag;
				if (channelLag == 0) {
					state = LOW;
				}
			}
			else {
				state = HIGH;
			}
			break;
		}
		case LAG_HIGH: {
			if (high) {
				--channelLag;
				if (channelLag == 0) {
					state = HIGH;
				}
			}
			else {
				state = LOW;
			}
			break;
		}
	};

	highOutput.setChannels(_channels);
	lowOutput.setChannels(_channels);
	switch (state) {
		case LOW:
		case LAG_HIGH: {
			highOutput.setVoltage(_lowValue, c);
			lowOutput.setVoltage(_highValue, c);
			break;
		}
		case HIGH:
		case LAG_LOW: {
			highOutput.setVoltage(_highValue, c);
			lowOutput.setVoltage(_lowValue, c);
			break;
		}
	}
}

struct CmpWidget : BGModuleWidget {
	static constexpr int hp = 6;

	CmpWidget(Cmp* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "Cmp");
		createScrews();

		// generated by svg_widgets.rb
		auto aParamPosition = Vec(8.0, 46.0);
		auto bParamPosition = Vec(53.0, 46.0);
		auto windowParamPosition = Vec(8.0, 151.0);
		auto lagParamPosition = Vec(53.0, 151.0);
		auto outputParamPosition = Vec(25.5, 251.0);

		auto aInputPosition = Vec(10.5, 87.0);
		auto bInputPosition = Vec(55.5, 87.0);
		auto windowInputPosition = Vec(10.5, 192.0);
		auto lagInputPosition = Vec(55.5, 192.0);

		auto greaterOutputPosition = Vec(16.0, 283.0);
		auto lessOutputPosition = Vec(50.0, 283.0);
		auto equalOutputPosition = Vec(16.0, 319.0);
		auto notEqualOutputPosition = Vec(50.0, 319.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob29>(aParamPosition, module, Cmp::A_PARAM));
		addParam(createParam<Knob29>(bParamPosition, module, Cmp::B_PARAM));
		addParam(createParam<Knob29>(windowParamPosition, module, Cmp::WINDOW_PARAM));
		addParam(createParam<Knob29>(lagParamPosition, module, Cmp::LAG_PARAM));
		{
			auto w = createParam<Knob16>(outputParamPosition, module, Cmp::OUTPUT_PARAM);
			auto k = dynamic_cast<SvgKnob*>(w);
			k->minAngle = 3.0f * (M_PI / 8.0f);
			k->maxAngle = 5.0f * (M_PI / 8.0f);
			k->speed = 3.0;
			addParam(w);
		}

		addInput(createInput<Port24>(aInputPosition, module, Cmp::A_INPUT));
		addInput(createInput<Port24>(bInputPosition, module, Cmp::B_INPUT));
		addInput(createInput<Port24>(windowInputPosition, module, Cmp::WINDOW_INPUT));
		addInput(createInput<Port24>(lagInputPosition, module, Cmp::LAG_INPUT));

		addOutput(createOutput<Port24>(greaterOutputPosition, module, Cmp::GREATER_OUTPUT));
		addOutput(createOutput<Port24>(lessOutputPosition, module, Cmp::LESS_OUTPUT));
		addOutput(createOutput<Port24>(equalOutputPosition, module, Cmp::EQUAL_OUTPUT));
		addOutput(createOutput<Port24>(notEqualOutputPosition, module, Cmp::NOT_EQUAL_OUTPUT));
	}
};

Model* modelCmp = bogaudio::createModel<Cmp, CmpWidget>("Bogaudio-Cmp", "CMP", "Window comparator", "Logic", "Polyphonic");