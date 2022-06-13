#pragma once

#include "bogaudio.hpp"
#include "trigger_on_load.hpp"
#include "dsp/signal.hpp"

extern Model* modelManual;

namespace bogaudio {

struct Manual : TriggerOnLoadModule {
	enum ParamsIds {
		TRIGGER_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		OUT3_OUTPUT,
		OUT4_OUTPUT,
		OUT5_OUTPUT,
		OUT6_OUTPUT,
		OUT7_OUTPUT,
		OUT8_OUTPUT,
		NUM_OUTPUTS
	};

	Trigger _trigger;
	rack::dsp::PulseGenerator _pulse;
	float _sampleTime;
	bogaudio::dsp::Timer* _initialDelay = NULL;
	float _outputScale = 1.0f;

	Manual() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configButton(TRIGGER_PARAM, "Trigger");

		configOutput(OUT1_OUTPUT, "Trigger");
		configOutput(OUT2_OUTPUT, "Trigger");
		configOutput(OUT3_OUTPUT, "Trigger");
		configOutput(OUT4_OUTPUT, "Trigger");
		configOutput(OUT5_OUTPUT, "Trigger");
		configOutput(OUT6_OUTPUT, "Trigger");
		configOutput(OUT7_OUTPUT, "Trigger");
		configOutput(OUT8_OUTPUT, "Trigger");

		_triggerOnLoad = false;
		_initialDelay = new bogaudio::dsp::Timer(APP->engine->getSampleRate(), 0.01f);
	}
	virtual ~Manual() {
		if (_initialDelay) {
			delete _initialDelay;
		}
	}

	void reset() override;
	void sampleRateChange() override;
	json_t* saveToJson(json_t* root) override;
	void loadFromJson(json_t* root) override;
	void processAll(const ProcessArgs& args) override;
	bool shouldTriggerOnNextLoad() override {
		return true;
	}
};

} // namespace bogaudio
