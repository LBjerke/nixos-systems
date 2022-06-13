#pragma once

#include "rack.hpp"
#include "module.hpp"
#include <functional>
#include <string>

using namespace rack;

extern Plugin *pluginInstance;

namespace bogaudio {

template <class BASE>
struct LightEmittingWidget : BASE {
	virtual bool isLit() = 0;

	void drawLayer(const typename BASE::DrawArgs& args, int layer) override {
		if (layer == 1 && isLit()) {
			drawLit(args);
		}
		BASE::drawLayer(args, layer);
	}

	virtual void drawLit(const typename BASE::DrawArgs& args) {}
};

struct DisplayWidget : LightEmittingWidget<OpaqueWidget> {
	Module* _module = NULL;

	DisplayWidget(Module* module);

	bool isLit() override;
	virtual bool isScreenshot();
	void draw(const DrawArgs& args) override;
	void drawLit(const DrawArgs& args) override;
	virtual void drawOnce(const DrawArgs& args, bool screenshot, bool lit) = 0;
};

struct SkinnableWidget : SkinChangeListener {
	void skinChanged(const std::string& skin) override {}
	std::string skinSVG(const std::string& base, const std::string& skin = "default");
};

struct Screw : SvgScrew, SkinnableWidget {
	Screw();

	void skinChanged(const std::string& skin) override;
};

struct BGKnob : RoundKnob, SkinnableWidget {
	std::string _svgBase;

	BGKnob(const char* svgBase, int dim);

	void redraw();
	void skinChanged(const std::string& skin) override;
};

struct Knob16 : BGKnob {
	Knob16();
};

struct Knob19 : BGKnob {
	Knob19();
};

struct Knob26 : BGKnob {
	Knob26();
};

struct Knob29 : BGKnob {
	Knob29();
};

struct Knob38 : BGKnob {
	Knob38();
};

struct Knob45 : BGKnob {
	Knob45();
};

struct Knob68 : BGKnob {
	Knob68();
};

struct IndicatorKnob : LightEmittingWidget<Knob>, SkinnableWidget {
	struct IKWidget : widget::Widget {
		float _angle = 0.0f;
		NVGcolor _color = nvgRGBA(0x00, 0x00, 0x00, 0x00);
		NVGcolor _rim = nvgRGBA(0x33, 0x33, 0x33, 0xff);
		NVGcolor _center = nvgRGBA(0xee, 0xee, 0xee, 0xff);
		std::function<bool()> _drawColorsCB;
		std::function<bool()> _unipolarCB;

		void setAngle(float a);
		void draw(const DrawArgs& args) override;
	};

	widget::FramebufferWidget* fb;
	CircularShadow* shadow;
	IKWidget* w;

	IndicatorKnob(int dim);
	void onHover(const event::Hover& e) override;
	void onChange(const event::Change& e) override;
	inline void setDrawColorsCallback(std::function<bool()> fn) { w->_drawColorsCB = fn; }
	inline void setUnipolarCallback(std::function<bool()> fn) { w->_unipolarCB = fn; }
	void redraw();
	bool isLit() override;
	void draw(const DrawArgs& args) override;
	void drawLit(const DrawArgs& args) override;
	void skinChanged(const std::string& skin) override;
};

struct IndicatorKnob19 : IndicatorKnob {
	IndicatorKnob19() : IndicatorKnob(19) {}
};

struct Port24 : SvgPort, SkinnableWidget {
	Port24();

	void skinChanged(const std::string& skin) override;
};

struct BlankPort24 : SvgPort {
	BlankPort24();
};

struct SliderSwitch : SvgSwitch {
	CircularShadow* shadow = NULL;
	SliderSwitch();
};

struct SliderSwitch2State14 : SliderSwitch {
	SliderSwitch2State14();
};

struct Button18 : SvgSwitch {
	Button18();
};

struct StatefulButton : ParamWidget {
	std::vector<std::shared_ptr<Svg>> _frames;
	SvgWidget* _svgWidget; // deleted elsewhere.
	CircularShadow* shadow = NULL;

	StatefulButton(const char* offSvgPath, const char* onSvgPath);
	void onDragStart(const event::DragStart& e) override;
	void onDragEnd(const event::DragEnd& e) override;
	void onDoubleClick(const event::DoubleClick& e) override {}
};

struct StatefulButton9 : StatefulButton {
	StatefulButton9();
};

struct StatefulButton18 : StatefulButton {
	StatefulButton18();
};

struct ToggleButton : SvgSwitch {
};

struct ToggleButton18 : ToggleButton {
	ToggleButton18();
};

struct IndicatorButtonGreen9 : LightEmittingWidget<SvgSwitch> {
	IndicatorButtonGreen9();

	bool isLit() override;
	void draw(const DrawArgs& args) override;
	void drawLit(const DrawArgs& args) override;
};

struct InvertingIndicatorButton : LightEmittingWidget<ParamWidget> {
	struct IIBWidget : widget::Widget {
		int _dim;
		NVGcolor _color = nvgRGBA(0x00, 0x00, 0x00, 0x00);

		IIBWidget(int dim) : _dim(dim) {}

		void setValue(float v);
		void draw(const DrawArgs& args) override;
	};

	widget::FramebufferWidget* fb;
	CircularShadow* shadow;
	IIBWidget* w;
	std::function<bool()> clickToInvertCB;
	std::function<void(int, float)> onChangeCB;

	InvertingIndicatorButton(int dim);

	inline void setClickToInvertCallback(std::function<bool()> fn) { clickToInvertCB = fn; }
	inline void setOnChangeCallback(std::function<void(int, float)> fn) { onChangeCB = fn; }
	void onHover(const event::Hover& e) override;
	void onDoubleClick(const event::DoubleClick& e) override {}
	void onButton(const event::Button& e) override;
	void onChange(const event::Change& e) override;
	bool isLit() override;
	void draw(const DrawArgs& args) override;
	void drawLit(const DrawArgs& args) override;
};

struct InvertingIndicatorButton9 : InvertingIndicatorButton {
	InvertingIndicatorButton9() : InvertingIndicatorButton(9) {}
};

struct InvertingIndicatorButton18 : InvertingIndicatorButton {
	InvertingIndicatorButton18() : InvertingIndicatorButton(18) {}
};

NVGcolor decibelsToColor(float db);

struct VUSlider : LightEmittingWidget<SliderKnob> {
	const float slideHeight = 13.0f;
	float* _vuLevel = NULL;
	float* _stereoVuLevel = NULL;

	VUSlider(float height = 183.0f) {
		box.size = Vec(18.0f, height);
	}

	inline void setVULevel(float* level) {
		_vuLevel = level;
	}
	inline void setStereoVULevel(float* level) {
		_stereoVuLevel = level;
	}
	bool isLit() override;
	void draw(const DrawArgs& args) override;
	void drawLit(const DrawArgs& args) override;
	void drawTranslate(const DrawArgs& args);
};

struct VUSlider151 : VUSlider {
	VUSlider151() : VUSlider(151.0f) {}
};

template <typename TBase>
struct BGTinyLight : TinyLight<TBase> {
	void drawLight(const typename TinyLight<TBase>::DrawArgs& args) override {
		if (!TinyLight<TBase>::module || !TinyLight<TBase>::module->isBypassed()) {
			TinyLight<TBase>::drawLight(args);
		}
	}

	void drawHalo(const typename TinyLight<TBase>::DrawArgs& args) override {
		if (!TinyLight<TBase>::module || !TinyLight<TBase>::module->isBypassed()) {
			TinyLight<TBase>::drawHalo(args);
		}
	}
};

template <typename TBase>
struct BGSmallLight : SmallLight<TBase> {
	void drawLight(const typename SmallLight<TBase>::DrawArgs& args) override {
		if (!SmallLight<TBase>::module || !SmallLight<TBase>::module->isBypassed()) {
			SmallLight<TBase>::drawLight(args);
		}
	}

	void drawHalo(const typename SmallLight<TBase>::DrawArgs& args) override {
		if (!SmallLight<TBase>::module || !SmallLight<TBase>::module->isBypassed()) {
			SmallLight<TBase>::drawHalo(args);
		}
	}
};

} // namespace bogaudio
