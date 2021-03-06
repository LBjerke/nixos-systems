
#include "Detune.hpp"

bool Detune::active() {
	return outputs[OUT_PLUS_OUTPUT].isConnected() || outputs[OUT_MINUS_OUTPUT].isConnected() || outputs[THRU_OUTPUT].isConnected();
}

int Detune::channels() {
	return inputs[IN_INPUT].getChannels();
}

void Detune::modulateChannel(int c) {
	_cents[c] = params[CENTS_PARAM].getValue();
	if (inputs[CV_INPUT].isConnected()) {
		_cents[c] *= clamp(inputs[CV_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
		_cents[c] = roundf(_cents[c]);
	}
	_cents[c] /= 100.0f;
}

void Detune::processChannel(const ProcessArgs& args, int c) {
	float inCV = inputs[IN_INPUT].getVoltage(c);

	if (_cents[c] != _lastCents[c] || inCV != _lastInCV[c]) {
		_lastCents[c] = _cents[c];
		_lastInCV[c] = inCV;
		if (_cents[c] < 0.001f) {
			_plusCV[c] = inCV;
			_minusCV[c] = inCV;
		}
		else {
			float semitone = cvToSemitone(inCV);
			_plusCV[c] = semitoneToCV(semitone + _cents[c]);
			_minusCV[c] = semitoneToCV(semitone - _cents[c]);
		}
	}

	outputs[THRU_OUTPUT].setChannels(_channels);
	outputs[THRU_OUTPUT].setVoltage(inCV, c);
	outputs[OUT_PLUS_OUTPUT].setChannels(_channels);
	outputs[OUT_PLUS_OUTPUT].setVoltage(_plusCV[c], c);
	outputs[OUT_MINUS_OUTPUT].setChannels(_channels);
	outputs[OUT_MINUS_OUTPUT].setVoltage(_minusCV[c], c);
}

void Detune::processBypass(const ProcessArgs& args) {
	int cn = channels();
	outputs[THRU_OUTPUT].setChannels(cn);
	outputs[OUT_PLUS_OUTPUT].setChannels(cn);
	outputs[OUT_MINUS_OUTPUT].setChannels(cn);
	for (int c = 0; c < cn; ++c) {
		float out = inputs[IN_INPUT].getVoltage(c);
		outputs[THRU_OUTPUT].setVoltage(out, c);
		outputs[OUT_PLUS_OUTPUT].setVoltage(out, c);
		outputs[OUT_MINUS_OUTPUT].setVoltage(out, c);
	}
}

struct DetuneWidget : BGModuleWidget {
	static constexpr int hp = 3;

	DetuneWidget(Detune* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "Detune");
		createScrews();

		// generated by svg_widgets.rb
		auto centsParamPosition = Vec(9.5, 32.5);

		auto cvInputPosition = Vec(10.5, 77.0);
		auto inInputPosition = Vec(10.5, 126.0);

		auto thruOutputPosition = Vec(10.5, 164.0);
		auto outPlusOutputPosition = Vec(10.5, 200.0);
		auto outMinusOutputPosition = Vec(10.5, 236.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob26>(centsParamPosition, module, Detune::CENTS_PARAM));

		addInput(createInput<Port24>(cvInputPosition, module, Detune::CV_INPUT));
		addInput(createInput<Port24>(inInputPosition, module, Detune::IN_INPUT));

		addOutput(createOutput<Port24>(thruOutputPosition, module, Detune::THRU_OUTPUT));
		addOutput(createOutput<Port24>(outPlusOutputPosition, module, Detune::OUT_PLUS_OUTPUT));
		addOutput(createOutput<Port24>(outMinusOutputPosition, module, Detune::OUT_MINUS_OUTPUT));
	}
};

Model* modelDetune = bogaudio::createModel<Detune, DetuneWidget>("Bogaudio-Detune", "DETUNE", "Pitch CV processor for detuning oscillator pairs", "Tuner", "Polyphonic");
