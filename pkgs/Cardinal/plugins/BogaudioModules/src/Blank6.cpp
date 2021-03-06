
#include "Blank6.hpp"
#include "skins.hpp"

void Blank6::sampleRateChange() {
	_rms.setSampleRate(APP->engine->getSampleRate());
}

void Blank6::processAll(const ProcessArgs& args) {
	if (inputs[IN_INPUT].isConnected()) {
		_haveLevel = true;
		_level = _rms.next(inputs[IN_INPUT].getVoltageSum()) / 5.0f;
	}
	else {
		_haveLevel = false;
		_level = 0.0f;
	}
}

struct Blank6Display : DisplayWidget {
	Blank6* _module;
	const char* _text;
	std::string _fontPath;

	Blank6Display(Blank6* module, const char* text)
	: DisplayWidget(module)
	, _module(module)
	, _text(text)
	, _fontPath(asset::plugin(pluginInstance, "res/fonts/audiowide.ttf"))
	{
	}

	void drawOnce(const DrawArgs& args, bool screenshot, bool lit) override {
		const Skins& skins = Skins::skins();
		std::string skin = skins.defaultKey();
		bool haveLevel = false;
		float level = 0.0f;
		if (lit) {
			haveLevel = _module->_level;
			level = _module->_level;
			skin = _module->_skin;
		}

		NVGcolor textColor = nvgRGBA(0x33, 0x33, 0x33, 0xff);
		NVGcolor bgColor = nvgRGBA(0xdd, 0xdd, 0xdd, 0xff);
		const char* pathStroke = skins.skinCssValue(skin, "path-stroke");
		if (pathStroke) {
			textColor = Skins::cssColorToNVGColor(pathStroke, textColor);
		}
		NVGcolor bgTextColor = nvgRGBAf(0.5f * (textColor.r + bgColor.r), 0.5f * (textColor.g + bgColor.g), 0.5f * (textColor.b + bgColor.b), 1.0f);

		const int split = 107;
		drawText(args, 0, 0, box.size.x, split - 10, haveLevel, level, textColor, bgTextColor);
		drawText(args, 0, split, box.size.x, box.size.y, haveLevel, level, textColor, bgTextColor);
		drawText(args, 0, split - 10, box.size.x / 2 - 5, 10, haveLevel, level, textColor, bgTextColor);
		drawText(args, box.size.x / 2 + 5, split - 10, box.size.x, 10, haveLevel, level, textColor, bgTextColor);
	}

	void drawText(const DrawArgs& args, int sx, int sy, int sw, int sh, bool haveLevel, float level, const NVGcolor& textColor, const NVGcolor& bgTextColor) {
		std::shared_ptr<Font> font = APP->window->loadFont(_fontPath);
		float offsetX = box.size.x / 2.0f;
		float offsetY = box.size.y / 2.0f;
		nvgSave(args.vg);
		nvgScissor(args.vg, 0, 0, box.size.x, box.size.y);
		nvgIntersectScissor(args.vg, sx, sy, sw, sh);
		nvgTranslate(args.vg, offsetX, offsetY);
		nvgRotate(args.vg, M_PI/2.0f);
		nvgTranslate(args.vg, -offsetY, offsetX);
		nvgFontSize(args.vg, 52.0f);
		nvgFontFaceId(args.vg, font->handle);
		nvgTextLetterSpacing(args.vg, 9.0f);
		if (!haveLevel) {
			nvgFillColor(args.vg, textColor);
			nvgText(args.vg, 0, 0, _text, NULL);
		}
		else {
			nvgFillColor(args.vg, bgTextColor);
			nvgText(args.vg, 0, 0, _text, NULL);
			if (level > 0.0001f) {
				nvgFillColor(args.vg, decibelsToColor(amplitudeToDecibels(level)));
				nvgText(args.vg, 0, 0, _text, NULL);
			}
		}
		nvgRestore(args.vg);
	}
};

struct Blank6Widget : BGModuleWidget {
	static constexpr int hp = 6;

	Blank6Widget(Blank6* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "Blank6");
		createScrews();

		{
			auto display = new Blank6Display(module, "BOGAUDIO");
			display->box.pos = Vec(30, 32);
			display->box.size = Vec(30, 316);
			addChild(display);
		}

		addInput(createInput<BlankPort24>(Vec(33, 346), module, Blank6::IN_INPUT));
	}
};

Model* modelBlank6 = bogaudio::createModel<Blank6, Blank6Widget>("Bogaudio-Blank6", "BLANK6", "6HP blank panel", "Blank");
