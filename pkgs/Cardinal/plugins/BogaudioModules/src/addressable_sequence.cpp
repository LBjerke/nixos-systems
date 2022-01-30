
#include "addressable_sequence.hpp"

#define POLY_INPUT "poly_input"
#define SELECT_ON_CLOCK "select_on_clock"
#define TRIGGERED_SELECT "triggered_select"
#define REVERSE_ON_NEGATIVE_CLOCK "reverse_on_negative_clock"
#define WRAP_SELECT_AT_STEPS "wrap_select_at_steps"

void AddressableSequenceModule::setInputIDs(int clockInputID, int selectInputID) {
	_polyInputID = clockInputID;
	_clockInputID = clockInputID;
	_selectInputID = selectInputID;
}

void AddressableSequenceModule::reset() {
	for (int i = 0; i < maxChannels; ++i) {
		_step[i] = 0;
		_select[i] = 0;
		_clock[i].reset();
		_negativeClock[i].reset();
		_reset[i].reset();
		_selectTrigger[i].reset();
	}
}

void AddressableSequenceModule::sampleRateChange() {
	float sr = APP->engine->getSampleRate();
	for (int i = 0; i < maxChannels; ++i) {
		_timer[i].setParams(sr, 0.001f);
	}
}

json_t* AddressableSequenceModule::saveToJson(json_t* root) {
	json_object_set_new(root, POLY_INPUT, json_integer(_polyInputID));
	json_object_set_new(root, SELECT_ON_CLOCK, json_boolean(_selectOnClock));
	json_object_set_new(root, TRIGGERED_SELECT, json_boolean(_triggeredSelect));
	json_object_set_new(root, REVERSE_ON_NEGATIVE_CLOCK, json_boolean(_reverseOnNegativeClock));
	json_object_set_new(root, WRAP_SELECT_AT_STEPS, json_boolean(_wrapSelectAtSteps));
	return root;
}

void AddressableSequenceModule::loadFromJson(json_t* root) {
	json_t* p = json_object_get(root, POLY_INPUT);
	if (p) {
		_polyInputID = json_integer_value(p);
	}

	json_t* s = json_object_get(root, SELECT_ON_CLOCK);
	if (s) {
		_selectOnClock = json_is_true(s);
	}

	json_t* t = json_object_get(root, TRIGGERED_SELECT);
	if (t) {
		_triggeredSelect = json_is_true(t);
	}

	json_t* r = json_object_get(root, REVERSE_ON_NEGATIVE_CLOCK);
	if (r) {
		_reverseOnNegativeClock = json_is_true(r);
	}

	json_t* w = json_object_get(root, WRAP_SELECT_AT_STEPS);
	if (w) {
		_wrapSelectAtSteps = json_is_true(w);
	}
}

int AddressableSequenceModule::channels() {
	assert(_polyInputID >= 0);
	assert(_clockInputID >= 0);
	assert(_selectInputID >= 0);
	return _polyInputID == _selectInputID ? inputs[_selectInputID].getChannels() : inputs[_clockInputID].getChannels();
}

int AddressableSequenceModule::nextStep(
	int c,
	Input* resetInput,
	Input& clockInput,
	Param* stepsParam,
	Param& directionParam,
	Param* selectParam,
	Input& selectInput,
	int n
) {
	bool reset = false;
	if (resetInput) {
		reset = _reset[c].process(resetInput->getPolyVoltage(c));
		if (reset) {
			_timer[c].reset();
		}
	}
	bool timer = _timer[c].next();
	float clockVoltage = clockInput.getPolyVoltage(c);
	bool clock = _clock[c].process(clockVoltage) && !timer;
	bool negativeClock = _negativeClock[c].process(clockVoltage) && _reverseOnNegativeClock && !timer && !clock;

	int steps = n;
	if (stepsParam) {
		float s = clamp(stepsParam->getValue(), 1.0f, 8.0f);
		s -= 1.0f;
		s /= 7.0f;
		s *= n - 1;
		s += 1.0f;
		steps = s;
	}
	int reverse = 1 - 2 * (directionParam.getValue() == 0.0f);
	_step[c] = (_step[c] + reverse * clock + -reverse * negativeClock) % steps;
	_step[c] += (_step[c] < 0) * steps;
	_step[c] -= _step[c] * reset;

	float select = n;
	if (selectParam) {
		float s = clamp(selectParam->getValue(), 0.0f, 7.0f) / 7.0f;
		select = s * (n - 1);
	}
	if (_triggeredSelect) {
		if (_selectTrigger[c].process(selectInput.getPolyVoltage(c))) {
			_select[c] = (1 + (int)_select[c]) % ((int)select + 1);
		}
		_select[c] -= _select[c] * reset;
	}
	else {
		select += (clamp(selectInput.getPolyVoltage(c), -9.99f, 9.99f) / 10.f) * (float)n;
		if (!_selectOnClock || clock) {
			_select[c] = select;
		}
	}

	int s = (_step[c] + (int)_select[c]) % (_wrapSelectAtSteps ? steps : n);
	if (s < 0) {
		return n + s;
	}
	return s;
}

int AddressableSequenceModule::setStep(int c, int i, int n) {
	return _step[c] = i % n;
}


void AddressableSequenceBaseModuleWidget::contextMenu(Menu* menu) {
	auto m = dynamic_cast<AddressableSequenceModule*>(module);
	assert(m);

	OptionsMenuItem* p = new OptionsMenuItem("Polyphony channels from");
	p->addItem(OptionMenuItem("CLOCK input", [m]() { return m->_polyInputID == m->_clockInputID; }, [m]() { m->_polyInputID = m->_clockInputID; }));
	p->addItem(OptionMenuItem("SELECT input", [m]() { return m->_polyInputID == m->_selectInputID; }, [m]() { m->_polyInputID = m->_selectInputID; }));
	OptionsMenuItem::addToMenu(p, menu);

	menu->addChild(new BoolOptionMenuItem("Reverse step on negative clock", [m]() { return &m->_reverseOnNegativeClock; }));
	menu->addChild(new BoolOptionMenuItem("Triggered select mode", [m]() { return &m->_triggeredSelect; }));
	menu->addChild(new BoolOptionMenuItem("Wrap select at steps", [m]() { return &m->_wrapSelectAtSteps; }));
}


void AddressableSequenceModuleWidget::contextMenu(Menu* menu) {
	AddressableSequenceBaseModuleWidget::contextMenu(menu);

	auto m = dynamic_cast<AddressableSequenceModule*>(module);
	assert(m);
	menu->addChild(new BoolOptionMenuItem("Select on clock mode", [m]() { return &m->_selectOnClock; }));
}
