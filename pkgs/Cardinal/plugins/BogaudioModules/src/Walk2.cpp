
#include "Walk2.hpp"

#define ZOOM_OUT_KEY "zoom_out"
#define GRID_KEY "grid"
#define COLOR_KEY "color"

extern float zoom;

void Walk2::reset() {
	_jumpTrigger.reset();
}

void Walk2::sampleRateChange() {
	_historySteps = (historySeconds * APP->engine->getSampleRate()) / historyPoints;
}

json_t* Walk2::saveToJson(json_t* root) {
	json_object_set_new(root, ZOOM_OUT_KEY, json_boolean(_zoomOut));
	json_object_set_new(root, GRID_KEY, json_boolean(_drawGrid));
	json_object_set_new(root, COLOR_KEY, json_integer(_traceColor));
	return root;
}

void Walk2::loadFromJson(json_t* root) {
	json_t* zo = json_object_get(root, ZOOM_OUT_KEY);
	if (zo) {
		_zoomOut = json_is_true(zo);
	}
	json_t* g = json_object_get(root, GRID_KEY);
	if (g) {
		_drawGrid = json_is_true(g);
	}
	json_t* c = json_object_get(root, COLOR_KEY);
	if (c) {
		_traceColor = (TraceColor)json_integer_value(c);
	}
}

inline float scaleRate(float rate) {
	return 0.2f * powf(rate, 5.0f);
}

void Walk2::modulate() {
	float sampleRate = APP->engine->getSampleRate();

	float rateX = params[RATE_X_PARAM].getValue();
	if (inputs[RATE_X_INPUT].isConnected()) {
		rateX *= clamp(inputs[RATE_X_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
	}
	rateX = scaleRate(rateX);
	_walkX.setParams(sampleRate, rateX);
	_slewX.setParams(sampleRate, std::max((1.0f - rateX) * 100.0f, 0.0f), 10.0f);

	_offsetX = params[OFFSET_X_PARAM].getValue();
	if (inputs[OFFSET_X_INPUT].isConnected()) {
		_offsetX *= clamp(inputs[OFFSET_X_INPUT].getVoltage() / 5.0f, -1.0f, 1.0f);
	}
	_offsetX *= 5.0f;

	_scaleX = params[SCALE_X_PARAM].getValue();
	if (inputs[SCALE_X_INPUT].isConnected()) {
		_scaleX *= clamp(inputs[SCALE_X_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
	}

	float rateY = params[RATE_Y_PARAM].getValue();
	if (inputs[RATE_Y_INPUT].isConnected()) {
		rateY *= clamp(inputs[RATE_Y_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
	}
	rateY = scaleRate(rateY);
	_walkY.setParams(sampleRate, rateY);
	_slewY.setParams(sampleRate, std::max((1.0f - rateY) * 100.0f, 0.0f), 10.0f);

	_offsetY = params[OFFSET_Y_PARAM].getValue();
	if (inputs[OFFSET_Y_INPUT].isConnected()) {
		_offsetY *= clamp(inputs[OFFSET_Y_INPUT].getVoltage() / 5.0f, -1.0f, 1.0f);
	}
	_offsetY *= 5.0f;

	_scaleY = params[SCALE_Y_PARAM].getValue();
	if (inputs[SCALE_Y_INPUT].isConnected()) {
		_scaleY *= clamp(inputs[SCALE_Y_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
	}

	float jm = clamp(params[JUMP_MODE_PARAM].getValue(), 0.0f, 2.0f);
	if (jm > 1.5f) {
		_jumpMode = Walk::TRACKHOLD_JUMPMODE;
	}
	else if (jm > 0.5f) {
		_jumpMode = Walk::SAMPLEHOLD_JUMPMODE;
	}
	else {
		_jumpMode = Walk::JUMP_JUMPMODE;
	}
}

void Walk2::processAlways(const ProcessArgs& args) {
	lights[JUMP_LIGHT].value = _jumpMode == Walk::JUMP_JUMPMODE;
	lights[SAMPLEHOLD_LIGHT].value = _jumpMode == Walk::SAMPLEHOLD_JUMPMODE;
	lights[TRACKHOLD_LIGHT].value = _jumpMode == Walk::TRACKHOLD_JUMPMODE;
}

void Walk2::processAll(const ProcessArgs& args) {
	Vec* jumpTo = _jumpTo;
	if (jumpTo != NULL) {
		_jumpTo = NULL;
		_lastOutX = jumpTo->x;
		_walkX.tell(jumpTo->x);
		_lastOutY = jumpTo->y;
		_walkY.tell(jumpTo->y);
		delete jumpTo;
	}

	bool triggered = _jumpTrigger.process(inputs[JUMP_INPUT].getVoltage());
	float outX = _walkX.next();
	float outY = _walkY.next();

	switch (_jumpMode) {
		case Walk::JUMP_JUMPMODE: {
			if (triggered) {
				_walkX.jump();
				_walkY.jump();
			}
			break;
		}
		case Walk::TRACKHOLD_JUMPMODE: {
			if (_jumpTrigger.isHigh()) {
				_lastOutX = outX;
				_lastOutY = outY;
			}
			else {
				outX = _lastOutX;
				outY = _lastOutY;
			}
			break;
		}
		case Walk::SAMPLEHOLD_JUMPMODE: {
			if (triggered) {
				_lastOutX = outX;
				_lastOutY = outY;
			}
			else {
				outX = _lastOutX;
				outY = _lastOutY;
			}
			break;
		}
	}

	outX = _slewX.next(outX);
	outX *= _scaleX;
	outX += _offsetX;
	outputs[OUT_X_OUTPUT].setVoltage(outX);

	outY = _slewY.next(outY);
	outY *= _scaleY;
	outY += _offsetY;
	outputs[OUT_Y_OUTPUT].setVoltage(outY);

	if (outputs[DISTANCE_OUTPUT].isConnected()) {
		outputs[DISTANCE_OUTPUT].setVoltage(sqrtf(outX*outX + outY*outY) * 0.707107f); // scaling constant is 10 / squrt(200)
	}

	if (_historyStep == 0) {
		_outsX.push(outX);
		_outsY.push(outY);
	}
	++_historyStep;
	_historyStep %= _historySteps;
}

struct Walk2Display : DisplayWidget {
	const int _insetAround = 4;

	const NVGcolor _axisColor = nvgRGBA(0xff, 0xff, 0xff, 0x70);
	const NVGcolor _defaultTraceColor = nvgRGBA(0x00, 0xff, 0x00, 0xee);

	Walk2* _module;
	const Vec _size;
	const Vec _drawSize;
	int _midX, _midY;
	NVGcolor _traceColor = _defaultTraceColor;
	Vec _dragLast;

	Walk2Display(
		Walk2* module,
		Vec size
	)
	: DisplayWidget(module)
	, _module(module)
	, _size(size)
	, _drawSize(2 * (_size.x - 2 * _insetAround), 2 * (_size.y - 2 * _insetAround))
	, _midX(_insetAround + _drawSize.x/2)
	, _midY(_insetAround + _drawSize.y/2)
	{
	}

	void onButton(const event::Button& e) override {
		if (!(e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_LEFT && (e.mods & RACK_MOD_MASK) == 0)) {
			return;
		}
		e.consume(this);
		_dragLast = e.pos;
		maybeJump(e.pos);
	}

	void onDragMove(const event::DragMove& e) override {
		float zoom = APP->scene->rackScroll->zoomWidget->zoom;
		_dragLast.x += e.mouseDelta.x / zoom;
		_dragLast.y += e.mouseDelta.y / zoom;
		maybeJump(_dragLast);
	}

	void maybeJump(Vec pos) {
		if (
			pos.x > _insetAround &&
			pos.x < _size.x - _insetAround &&
			pos.y > _insetAround &&
			pos.y < _size.y - _insetAround
		) {
			float x = 20.0f * ((pos.x - _insetAround) / (float)_drawSize.x);
			x -= 5.0f;
			float y = 20.0f * ((pos.y - _insetAround) / (float)_drawSize.y);
			y = 5.0f - y;
			_module->_jumpTo = new Vec(x, y);
		}
	}

	void drawOnce(const DrawArgs& args, bool screenshot, bool lit) override {
		float strokeWidth = std::max(1.0f, 3.0f - getZoom());

		nvgSave(args.vg);
		drawBackground(args);
		nvgScissor(args.vg, _insetAround, _insetAround, _drawSize.x / 2, _drawSize.y / 2);
		if (_module && _module->_zoomOut) {
			nvgScale(args.vg, 0.5f, 0.5f);
			strokeWidth *= 2.0f;
		}
		else {
			float offsetX = _module ? _module->_offsetX : 0.0f;
			float offsetY = _module ? _module->_offsetY : 0.0f;
			float tx = 1.0f + (clamp(offsetX, -5.0f, 5.0f) / 5.0f);
			tx *= -_drawSize.x / 4;
			float ty = 1.0f - (clamp(offsetY, -5.0f, 5.0f) / 5.0f);
			ty *= -_drawSize.y / 4;
			nvgTranslate(args.vg, tx, ty);
		}
		drawAxes(args, strokeWidth);

		if (lit) {
			switch (_module->_traceColor) {
				case Walk2::ORANGE_TRACE_COLOR: {
					_traceColor = nvgRGBA(0xff, 0x80, 0x00, 0xee);
					break;
				}
				case Walk2::RED_TRACE_COLOR: {
					_traceColor = nvgRGBA(0xff, 0x00, 0x00, 0xee);
					break;
				}
				case Walk2::BLUE_TRACE_COLOR: {
					_traceColor = nvgRGBA(0x00, 0xdd, 0xff, 0xee);
					break;
				}
				case Walk2::GREEN_TRACE_COLOR:
				default: {
					_traceColor = _defaultTraceColor;
				}
			}
			drawTrace(args, _traceColor, _module->_outsX, _module->_outsY);
		}

		nvgRestore(args.vg);
	}

	void drawBackground(const DrawArgs& args) {
		nvgSave(args.vg);
		nvgBeginPath(args.vg);
		nvgRect(args.vg, 0, 0, _size.x, _size.y);
		nvgFillColor(args.vg, nvgRGBA(0x00, 0x00, 0x00, 0xff));
		nvgFill(args.vg);
		nvgStrokeColor(args.vg, nvgRGBA(0x50, 0x50, 0x50, 0xff));
		nvgStroke(args.vg);
		nvgRestore(args.vg);
	}

	void drawAxes(const DrawArgs& args, float strokeWidth) {
		const float shortTick = 4.0f;
		const float longTick = 8.0f;
		float dot = 0.5f * strokeWidth;

		nvgSave(args.vg);
		nvgStrokeColor(args.vg, _axisColor);
		nvgStrokeWidth(args.vg, strokeWidth);

		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, _insetAround, _midY);
		nvgLineTo(args.vg, _insetAround + _drawSize.x, _midY);
		nvgStroke(args.vg);

		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, _midX, _insetAround);
		nvgLineTo(args.vg, _midX, _insetAround + _drawSize.y);
		nvgStroke(args.vg);

		for (int i = 1; i <= 10; ++i) {
			float tick = i % 5 == 0 ? longTick : shortTick;

			float x = (i * 0.1f) * 0.5f * _drawSize.x;
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, _midX + x, _midY - tick);
			nvgLineTo(args.vg, _midX + x, _midY + tick);
			nvgStroke(args.vg);

			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, _midX - x, _midY - tick);
			nvgLineTo(args.vg, _midX - x, _midY + tick);
			nvgStroke(args.vg);

			float y = (i * 0.1f) * 0.5f * _drawSize.y;
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, _midX - tick, _midY + y);
			nvgLineTo(args.vg, _midX + tick, _midY + y);
			nvgStroke(args.vg);

			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, _midX - tick, _midY - y);
			nvgLineTo(args.vg, _midX + tick, _midY - y);
			nvgStroke(args.vg);

			if (!_module || _module->_drawGrid) {
				for (int j = 1; j <= 10; ++j) {
					float y = (j * 0.1f) * 0.5f * _drawSize.y;

					nvgBeginPath(args.vg);
					nvgMoveTo(args.vg, _midX + x - dot, _midY + y);
					nvgLineTo(args.vg, _midX + x + dot, _midY + y);
					nvgStroke(args.vg);

					nvgBeginPath(args.vg);
					nvgMoveTo(args.vg, _midX - x - dot, _midY + y);
					nvgLineTo(args.vg, _midX - x + dot, _midY + y);
					nvgStroke(args.vg);

					nvgBeginPath(args.vg);
					nvgMoveTo(args.vg, _midX - x - dot, _midY - y);
					nvgLineTo(args.vg, _midX - x + dot, _midY - y);
					nvgStroke(args.vg);

					nvgBeginPath(args.vg);
					nvgMoveTo(args.vg, _midX + x - dot, _midY - y);
					nvgLineTo(args.vg, _midX + x + dot, _midY - y);
					nvgStroke(args.vg);
				}
			}
		}

		if (!_module || _module->_drawGrid) {
			const float tick = shortTick;
			{
				float x = _midX - _drawSize.x / 4;
				float y = _midY - _drawSize.y / 4;

				nvgBeginPath(args.vg);
				nvgMoveTo(args.vg, x - tick, y);
				nvgLineTo(args.vg, x + tick, y);
				nvgStroke(args.vg);

				nvgBeginPath(args.vg);
				nvgMoveTo(args.vg, x, y - tick);
				nvgLineTo(args.vg, x, y + tick);
				nvgStroke(args.vg);
			}
			{
				float x = _midX + _drawSize.x / 4;
				float y = _midY - _drawSize.y / 4;

				nvgBeginPath(args.vg);
				nvgMoveTo(args.vg, x - tick, y);
				nvgLineTo(args.vg, x + tick, y);
				nvgStroke(args.vg);

				nvgBeginPath(args.vg);
				nvgMoveTo(args.vg, x, y - tick);
				nvgLineTo(args.vg, x, y + tick);
				nvgStroke(args.vg);
			}
			{
				float x = _midX + _drawSize.x / 4;
				float y = _midY + _drawSize.y / 4;

				nvgBeginPath(args.vg);
				nvgMoveTo(args.vg, x - tick, y);
				nvgLineTo(args.vg, x + tick, y);
				nvgStroke(args.vg);

				nvgBeginPath(args.vg);
				nvgMoveTo(args.vg, x, y - tick);
				nvgLineTo(args.vg, x, y + tick);
				nvgStroke(args.vg);
			}
			{
				float x = _midX - _drawSize.x / 4;
				float y = _midY + _drawSize.y / 4;

				nvgBeginPath(args.vg);
				nvgMoveTo(args.vg, x - tick, y);
				nvgLineTo(args.vg, x + tick, y);
				nvgStroke(args.vg);

				nvgBeginPath(args.vg);
				nvgMoveTo(args.vg, x, y - tick);
				nvgLineTo(args.vg, x, y + tick);
				nvgStroke(args.vg);
			}
		}

		nvgRestore(args.vg);
	}

	void drawTrace(const DrawArgs& args, NVGcolor color, HistoryBuffer<float>& x, HistoryBuffer<float>& y) {
		nvgSave(args.vg);
		// nvgGlobalCompositeOperation(args.vg, NVG_LIGHTER);

		// int n = _module->historyPoints;
		// float beginRadius = std::max(1.0f, 2.0f - getZoom());
		// float endRadius = std::max(0.01f, 0.8f - getZoom());
		// float radiusStep = (beginRadius - endRadius) / (float)n;
		// float radius = beginRadius;
		// float alphaStep = (color.a - 0.1f) / (float)n;
		// for (int i = 0; i < n; ++i) {
		// 	nvgBeginPath(args.vg);
		// 	nvgCircle(args.vg, cvToPixel(_midX, _drawSize.x, x.value(i)), cvToPixel(_midY, _drawSize.y, y.value(i)), radius);
		// 	nvgStrokeColor(args.vg, color);
		// 	nvgFillColor(args.vg, color);
		// 	nvgStroke(args.vg);
		// 	nvgFill(args.vg);
		// 	radius -= radiusStep;
		// 	color.a -= alphaStep;
		// }

		int n = _module->historyPoints;
		float beginWidth = std::max(1.0f, 4.0f - getZoom());
		float endWidth = std::max(0.5f, 2.0f - getZoom());
		if (_module->_zoomOut) {
			beginWidth *= 2.0f;
			endWidth *= 2.0f;
		}
		float widthStep = (beginWidth - endWidth) / (float)n;
		float width = endWidth;
		float endAlpha = 0.1f;
		float alphaStep = (color.a - endAlpha) / (float)n;
		color.a = endAlpha;
		for (int i = n - 1; i > 0; --i) {
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, cvToPixelX(_midX, _drawSize.x, x.value(i - 1)), cvToPixelY(_midY, _drawSize.y, y.value(i - 1)));
			nvgLineTo(args.vg, cvToPixelX(_midX, _drawSize.x, x.value(i)), cvToPixelY(_midY, _drawSize.y, y.value(i)));
			nvgStrokeWidth(args.vg, width);
			nvgStrokeColor(args.vg, color);
			nvgStroke(args.vg);
			width += widthStep;
			color.a += alphaStep;
		}
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, cvToPixelX(_midX, _drawSize.x, x.value(0)), cvToPixelY(_midY, _drawSize.y, y.value(0)), 0.5f * width);
		nvgStrokeColor(args.vg, color);
		nvgFillColor(args.vg, color);
		nvgStroke(args.vg);
		nvgFill(args.vg);

		nvgRestore(args.vg);
	}

	inline float cvToPixelX(float mid, float extent, float cv) {
		return mid + 0.05f * extent * cv;
	}

	inline float cvToPixelY(float mid, float extent, float cv) {
		return mid - 0.05f * extent * cv;
	}
};

struct Walk2Widget : BGModuleWidget {
	static constexpr int hp = 14;

	Walk2Widget(Walk2* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "Walk2");
		createScrews();

		{
			auto inset = Vec(10, 25);
			int dim = box.size.x - 2*inset.x;
			auto size = Vec(dim, dim);
			auto display = new Walk2Display(module, size);
			display->box.pos = inset;
			display->box.size = size;
			addChild(display);
		}

		// generated by svg_widgets.rb
		auto rateXParamPosition = Vec(28.0, 240.0);
		auto rateYParamPosition = Vec(151.5, 240.0);
		auto offsetXParamPosition = Vec(75.0, 234.0);
		auto offsetYParamPosition = Vec(119.0, 234.0);
		auto scaleXParamPosition = Vec(75.0, 262.5);
		auto scaleYParamPosition = Vec(119.0, 262.5);
		auto jumpModeParamPosition = Vec(122.0, 341.7);

		auto offsetXInputPosition = Vec(10.5, 284.0);
		auto scaleXInputPosition = Vec(41.5, 284.0);
		auto rateXInputPosition = Vec(10.5, 323.0);
		auto offsetYInputPosition = Vec(145.5, 284.0);
		auto scaleYInputPosition = Vec(176.5, 284.0);
		auto rateYInputPosition = Vec(145.5, 323.0);
		auto jumpInputPosition = Vec(78.0, 291.0);

		auto outXOutputPosition = Vec(41.5, 323.0);
		auto outYOutputPosition = Vec(176.5, 323.0);
		auto distanceOutputPosition = Vec(109.0, 291.0);

		auto jumpLightPosition = Vec(90.5, 333.0);
		auto sampleholdLightPosition = Vec(90.5, 343.0);
		auto trackholdLightPosition = Vec(90.5, 353.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob29>(rateXParamPosition, module, Walk2::RATE_X_PARAM));
		addParam(createParam<Knob29>(rateYParamPosition, module, Walk2::RATE_Y_PARAM));
		addParam(createParam<Knob16>(offsetXParamPosition, module, Walk2::OFFSET_X_PARAM));
		addParam(createParam<Knob16>(offsetYParamPosition, module, Walk2::OFFSET_Y_PARAM));
		addParam(createParam<Knob16>(scaleXParamPosition, module, Walk2::SCALE_X_PARAM));
		addParam(createParam<Knob16>(scaleYParamPosition, module, Walk2::SCALE_Y_PARAM));
		addParam(createParam<StatefulButton9>(jumpModeParamPosition, module, Walk2::JUMP_MODE_PARAM));

		addInput(createInput<Port24>(offsetXInputPosition, module, Walk2::OFFSET_X_INPUT));
		addInput(createInput<Port24>(scaleXInputPosition, module, Walk2::SCALE_X_INPUT));
		addInput(createInput<Port24>(rateXInputPosition, module, Walk2::RATE_X_INPUT));
		addInput(createInput<Port24>(offsetYInputPosition, module, Walk2::OFFSET_Y_INPUT));
		addInput(createInput<Port24>(scaleYInputPosition, module, Walk2::SCALE_Y_INPUT));
		addInput(createInput<Port24>(rateYInputPosition, module, Walk2::RATE_Y_INPUT));
		addInput(createInput<Port24>(jumpInputPosition, module, Walk2::JUMP_INPUT));

		addOutput(createOutput<Port24>(outXOutputPosition, module, Walk2::OUT_X_OUTPUT));
		addOutput(createOutput<Port24>(outYOutputPosition, module, Walk2::OUT_Y_OUTPUT));
		addOutput(createOutput<Port24>(distanceOutputPosition, module, Walk2::DISTANCE_OUTPUT));

		addChild(createLight<BGSmallLight<GreenLight>>(jumpLightPosition, module, Walk2::JUMP_LIGHT));
		addChild(createLight<BGSmallLight<GreenLight>>(sampleholdLightPosition, module, Walk2::SAMPLEHOLD_LIGHT));
		addChild(createLight<BGSmallLight<GreenLight>>(trackholdLightPosition, module, Walk2::TRACKHOLD_LIGHT));
	}

	void contextMenu(Menu* menu) override {
		auto m = dynamic_cast<Walk2*>(module);
		assert(m);

		{
			OptionsMenuItem* mi = new OptionsMenuItem("Display range");
			mi->addItem(OptionMenuItem("+/-5V", [m]() { return m->_zoomOut == false; }, [m]() { m->_zoomOut = false; }));
			mi->addItem(OptionMenuItem("+/-10V", [m]() { return m->_zoomOut == true; }, [m]() { m->_zoomOut = true; }));
			OptionsMenuItem::addToMenu(mi, menu);
		}
		menu->addChild(new BoolOptionMenuItem("Show grid", [m]() { return &m->_drawGrid; }));
		{
			OptionsMenuItem* mi = new OptionsMenuItem("Trace color");
			mi->addItem(OptionMenuItem("Green", [m]() { return m->_traceColor == Walk2::GREEN_TRACE_COLOR; }, [m]() { m->_traceColor = Walk2::GREEN_TRACE_COLOR; }));
			mi->addItem(OptionMenuItem("Orange", [m]() { return m->_traceColor == Walk2::ORANGE_TRACE_COLOR; }, [m]() { m->_traceColor = Walk2::ORANGE_TRACE_COLOR; }));
			mi->addItem(OptionMenuItem("Red", [m]() { return m->_traceColor == Walk2::RED_TRACE_COLOR; }, [m]() { m->_traceColor = Walk2::RED_TRACE_COLOR; }));
			mi->addItem(OptionMenuItem("Blue", [m]() { return m->_traceColor == Walk2::BLUE_TRACE_COLOR; }, [m]() { m->_traceColor = Walk2::BLUE_TRACE_COLOR; }));
			OptionsMenuItem::addToMenu(mi, menu);
		}
	}
};

Model* modelWalk2 = bogaudio::createModel<Walk2, Walk2Widget>("Bogaudio-Walk2", "WALK2", "2D random-walk and X/Y controller", "Random", "Sample and hold", "Controller");
