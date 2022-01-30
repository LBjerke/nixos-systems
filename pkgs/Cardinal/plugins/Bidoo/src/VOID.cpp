#include "plugin.hpp"
#include "BidooComponents.hpp"

// Windows defines VOID as a macro, which breaks the build...
#undef VOID

using namespace std;

struct VOID : Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		NUM_INPUTS
	};
	enum OutputIds {
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	VOID() {	}
};

struct VOIDWidget : ModuleWidget {
	VOIDWidget(VOID *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/VOID.svg")));

		addChild(createWidget<ScrewSilver>(Vec(15, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 0)));
		addChild(createWidget<ScrewSilver>(Vec(15, 365)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 365)));
	}
};

Model *modelVOID = createModel<VOID, VOIDWidget>("vOId");
