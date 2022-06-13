#pragma once

#include "Mix8_shared.hpp"

using namespace bogaudio::dsp;

namespace bogaudio {

struct Mix8 : ExpandableModule<Mix8ExpanderMessage, DimmableMixerModule> {
	enum ParamsIds {
		LEVEL1_PARAM,
		MUTE1_PARAM,
		PAN1_PARAM,
		LEVEL2_PARAM,
		MUTE2_PARAM,
		PAN2_PARAM,
		LEVEL3_PARAM,
		MUTE3_PARAM,
		PAN3_PARAM,
		LEVEL4_PARAM,
		MUTE4_PARAM,
		PAN4_PARAM,
		LEVEL5_PARAM,
		MUTE5_PARAM,
		PAN5_PARAM,
		LEVEL6_PARAM,
		MUTE6_PARAM,
		PAN6_PARAM,
		LEVEL7_PARAM,
		MUTE7_PARAM,
		PAN7_PARAM,
		LEVEL8_PARAM,
		MUTE8_PARAM,
		PAN8_PARAM,
		MIX_PARAM,
		MIX_MUTE_PARAM,
		MIX_DIM_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		CV1_INPUT,
		PAN1_INPUT,
		IN1_INPUT,
		CV2_INPUT,
		PAN2_INPUT,
		IN2_INPUT,
		CV3_INPUT,
		PAN3_INPUT,
		IN3_INPUT,
		CV4_INPUT,
		PAN4_INPUT,
		IN4_INPUT,
		CV5_INPUT,
		PAN5_INPUT,
		IN5_INPUT,
		CV6_INPUT,
		PAN6_INPUT,
		IN6_INPUT,
		CV7_INPUT,
		PAN7_INPUT,
		IN7_INPUT,
		CV8_INPUT,
		PAN8_INPUT,
		IN8_INPUT,
		MIX_CV_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		L_OUTPUT,
		R_OUTPUT,
		NUM_OUTPUTS
	};

	int _polyChannelOffset = -1;
	MixerChannel* _channels[8] {};
	bool _channelActive[8] {};
	Panner _panners[8];
	bogaudio::dsp::SlewLimiter _panSLs[8];
	Amplifier _amplifier;
	bogaudio::dsp::SlewLimiter _slewLimiter;
	Saturator _saturator;
	RootMeanSquare _rms;
	float _rmsLevel = 0.0f;
	Mix8ExpanderMessage _dummyExpanderMessage;
	int _wasActive = 0;
	bogaudio::dsp::SlewLimiter _levelCVSL;

	Mix8() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		float levelDefault = fabsf(MixerChannel::minDecibels) / (MixerChannel::maxDecibels - MixerChannel::minDecibels);
		configParam(LEVEL1_PARAM, 0.0f, 1.0f, levelDefault, "Channel 1 level", " dB", 0.0f, MixerChannel::maxDecibels - MixerChannel::minDecibels, MixerChannel::minDecibels);
		configParam(PAN1_PARAM, -1.0f, 1.0f, 0.0f, "Channel 1 panning", "%", 0.0f, 100.0f);
		configSwitch(MUTE1_PARAM, 0.0f, 3.0f, 0.0f, "Channel 1 mute", {"Unmuted", "Muted", "Soloed", "Soloed"});
		configParam(LEVEL2_PARAM, 0.0f, 1.0f, levelDefault, "Channel 2 level", " dB", 0.0f, MixerChannel::maxDecibels - MixerChannel::minDecibels, MixerChannel::minDecibels);
		configParam(PAN2_PARAM, -1.0f, 1.0f, 0.0f, "Channel 2 panning", "%", 0.0f, 100.0f);
		configSwitch(MUTE2_PARAM, 0.0f, 3.0f, 0.0f, "Channel 2 mute", {"Unmuted", "Muted", "Soloed", "Soloed"});
		configParam(LEVEL3_PARAM, 0.0f, 1.0f, levelDefault, "Channel 3 level", " dB", 0.0f, MixerChannel::maxDecibels - MixerChannel::minDecibels, MixerChannel::minDecibels);
		configParam(PAN3_PARAM, -1.0f, 1.0f, 0.0f, "Channel 3 panning", "%", 0.0f, 100.0f);
		configSwitch(MUTE3_PARAM, 0.0f, 3.0f, 0.0f, "Channel 3 mute", {"Unmuted", "Muted", "Soloed", "Soloed"});
		configParam(LEVEL4_PARAM, 0.0f, 1.0f, levelDefault, "Channel 4 level", " dB", 0.0f, MixerChannel::maxDecibels - MixerChannel::minDecibels, MixerChannel::minDecibels);
		configParam(PAN4_PARAM, -1.0f, 1.0f, 0.0f, "Channel 4 panning", "%", 0.0f, 100.0f);
		configSwitch(MUTE4_PARAM, 0.0f, 3.0f, 0.0f, "Channel 4 mute", {"Unmuted", "Muted", "Soloed", "Soloed"});
		configParam(LEVEL5_PARAM, 0.0f, 1.0f, levelDefault, "Channel 5 level", " dB", 0.0f, MixerChannel::maxDecibels - MixerChannel::minDecibels, MixerChannel::minDecibels);
		configParam(PAN5_PARAM, -1.0f, 1.0f, 0.0f, "Channel 5 panning", "%", 0.0f, 100.0f);
		configSwitch(MUTE5_PARAM, 0.0f, 3.0f, 0.0f, "Channel 5 mute", {"Unmuted", "Muted", "Soloed", "Soloed"});
		configParam(LEVEL6_PARAM, 0.0f, 1.0f, levelDefault, "Channel 6 level", " dB", 0.0f, MixerChannel::maxDecibels - MixerChannel::minDecibels, MixerChannel::minDecibels);
		configParam(PAN6_PARAM, -1.0f, 1.0f, 0.0f, "Channel 6 panning", "%", 0.0f, 100.0f);
		configSwitch(MUTE6_PARAM, 0.0f, 3.0f, 0.0f, "Channel 6 mute", {"Unmuted", "Muted", "Soloed", "Soloed"});
		configParam(LEVEL7_PARAM, 0.0f, 1.0f, levelDefault, "Channel 7 level", " dB", 0.0f, MixerChannel::maxDecibels - MixerChannel::minDecibels, MixerChannel::minDecibels);
		configParam(PAN7_PARAM, -1.0f, 1.0f, 0.0f, "Channel 7 panning", "%", 0.0f, 100.0f);
		configSwitch(MUTE7_PARAM, 0.0f, 3.0f, 0.0f, "Channel 7 mute", {"Unmuted", "Muted", "Soloed", "Soloed"});
		configParam(LEVEL8_PARAM, 0.0f, 1.0f, levelDefault, "Channel 8 level", " dB", 0.0f, MixerChannel::maxDecibels - MixerChannel::minDecibels, MixerChannel::minDecibels);
		configParam(PAN8_PARAM, -1.0f, 1.0f, 0.0f, "Channel 8 panning", "%", 0.0f, 100.0f);
		configSwitch(MUTE8_PARAM, 0.0f, 3.0f, 0.0f, "Channel 8 mute", {"Unmuted", "Muted", "Soloed", "Soloed"});
		configParam(MIX_PARAM, 0.0f, 1.0f, levelDefault, "Master level", " dB", 0.0f, MixerChannel::maxDecibels - MixerChannel::minDecibels, MixerChannel::minDecibels);
		configSwitch(MIX_MUTE_PARAM, 0.0f, 1.0f, 0.0f, "Master mute", {"Unmuted", "Muted"});
		configSwitch<DimSwitchQuantity>(MIX_DIM_PARAM, 0.0f, 1.0f, 0.0f, "Master dim", {"Disabled", "Enabled"});
		getParamQuantity(MUTE1_PARAM)->randomizeEnabled = false;
		getParamQuantity(MUTE2_PARAM)->randomizeEnabled = false;
		getParamQuantity(MUTE3_PARAM)->randomizeEnabled = false;
		getParamQuantity(MUTE4_PARAM)->randomizeEnabled = false;
		getParamQuantity(MUTE5_PARAM)->randomizeEnabled = false;
		getParamQuantity(MUTE6_PARAM)->randomizeEnabled = false;
		getParamQuantity(MUTE7_PARAM)->randomizeEnabled = false;
		getParamQuantity(MUTE8_PARAM)->randomizeEnabled = false;
		getParamQuantity(MIX_MUTE_PARAM)->randomizeEnabled = false;
		getParamQuantity(MIX_DIM_PARAM)->randomizeEnabled = false;

		configInput(CV1_INPUT, "Channel 1 level CV");
		configInput(PAN1_INPUT, "Channel 1 pan CV");
		configInput(IN1_INPUT, "Channel 1");
		configInput(CV2_INPUT, "Channel 2 level CV");
		configInput(PAN2_INPUT, "Channel 2 pan CV");
		configInput(IN2_INPUT, "Channel 2");
		configInput(CV3_INPUT, "Channel 3 level CV");
		configInput(PAN3_INPUT, "Channel 3 pan CV");
		configInput(IN3_INPUT, "Channel 3");
		configInput(CV4_INPUT, "Channel 4 level CV");
		configInput(PAN4_INPUT, "Channel 4 pan CV");
		configInput(IN4_INPUT, "Channel 4");
		configInput(CV5_INPUT, "Channel 5 level CV");
		configInput(PAN5_INPUT, "Channel 5 pan CV");
		configInput(IN5_INPUT, "Channel 5");
		configInput(CV6_INPUT, "Channel 6 level CV");
		configInput(PAN6_INPUT, "Channel 6 pan CV");
		configInput(IN6_INPUT, "Channel 6");
		configInput(CV7_INPUT, "Channel 7 level CV");
		configInput(PAN7_INPUT, "Channel 7 pan CV");
		configInput(IN7_INPUT, "Channel 7");
		configInput(CV8_INPUT, "Channel 8 level CV");
		configInput(PAN8_INPUT, "Channel 8 pan CV");
		configInput(IN8_INPUT, "Channel 8");
		configInput(MIX_CV_INPUT, "Mix level CV");

		configOutput(L_OUTPUT, "Left signal");
		configOutput(R_OUTPUT, "Right signal");

		_channels[0] = new MixerChannel(params[LEVEL1_PARAM], params[MUTE1_PARAM], inputs[CV1_INPUT]);
		_channels[1] = new MixerChannel(params[LEVEL2_PARAM], params[MUTE2_PARAM], inputs[CV2_INPUT]);
		_channels[2] = new MixerChannel(params[LEVEL3_PARAM], params[MUTE3_PARAM], inputs[CV3_INPUT]);
		_channels[3] = new MixerChannel(params[LEVEL4_PARAM], params[MUTE4_PARAM], inputs[CV4_INPUT]);
		_channels[4] = new MixerChannel(params[LEVEL5_PARAM], params[MUTE5_PARAM], inputs[CV5_INPUT]);
		_channels[5] = new MixerChannel(params[LEVEL6_PARAM], params[MUTE6_PARAM], inputs[CV6_INPUT]);
		_channels[6] = new MixerChannel(params[LEVEL7_PARAM], params[MUTE7_PARAM], inputs[CV7_INPUT]);
		_channels[7] = new MixerChannel(params[LEVEL8_PARAM], params[MUTE8_PARAM], inputs[CV8_INPUT]);

		sampleRateChange();
		_rms.setSensitivity(0.05f);
		setExpanderModelPredicate([](Model* m) { return m == modelMix8x; });
	}
	virtual ~Mix8() {
		for (int i = 0; i < 8; ++i) {
			delete _channels[i];
		}
	}

	void onRandomize(const RandomizeEvent& e) override;
	json_t* saveToJson(json_t* root) override;
	void loadFromJson(json_t* root) override;
	void sampleRateChange() override;
	void processAll(const ProcessArgs& args) override;
};

} // namespace bogaudio
