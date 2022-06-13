
#include "Arp.hpp"

#define NOTES_IMMEDIATE_MODE "notes_immediate"
#define FIXED_GATE_MODE "fixed_gate"

void Arp::NoteSet::Note::reset() {
	pitch = 0.0f;
	channel = -1;
}

bool Arp::NoteSet::nextPitch(Mode mode, float& pitchOut) {
	if (_syncNext) {
		_syncNext = false;
		sync();
	}
	if (_noteCount <= 0) {
		return false;
	}

	switch (mode) {
		case UP_MODE: {
			_playIndex = (_playIndex + 1) % _noteCount;
			_syncNext = _syncTo && _playIndex == _noteCount - 1;
			pitchOut = _notesByPitch[_playIndex].pitch;
			return true;
		}

		case DOWN_MODE: {
			--_playIndex;
			if (_playIndex < 0) {
				_playIndex = _noteCount - 1;
			}
			_syncNext = _syncTo && _playIndex == 0;
			pitchOut = _notesByPitch[_playIndex].pitch;
			return true;
		}

		case UP_DOWN_MODE: {
			if (_up) {
				++_playIndex;
				if (_playIndex >= _noteCount) {
					_playIndex = std::max(0, _noteCount - 2);
					_up = false;
				}
			}
			else {
				--_playIndex;
				if (_playIndex < 0) {
					_playIndex = 1 % _noteCount;
					_up = true;
				}
				_syncNext = _syncTo && (_playIndex == 0 || _playIndex == 1);
			}
			pitchOut = _notesByPitch[_playIndex].pitch;
			return true;
		}

		case UP_DOWN_REPEAT_MODE: {
			if (_up) {
				++_playIndex;
				if (_playIndex >= _noteCount) {
					_playIndex = _noteCount - 1;
					_up = false;
				}
			}
			else {
				--_playIndex;
				if (_playIndex < 0) {
					_playIndex = 0;
					_up = true;
				}
				_syncNext = _syncTo && _playIndex == 0;
			}
			pitchOut = _notesByPitch[_playIndex].pitch;
			return true;
		}

		case IN_ORDER_MODE: {
			_playIndex = (_playIndex + 1) % _noteCount;
			_syncNext = _syncTo && _playIndex == _noteCount - 1;
			pitchOut = _notesAsPlayed[_playIndex].pitch;
			return true;
		}

		case RANDOM_MODE: {
			_playIndex = (_playIndex + 1) % _noteCount;
			_syncNext = _syncTo && _playIndex == _noteCount - 1;
			pitchOut = _notesAsPlayed[random::u32() % _noteCount].pitch;
			return true;
		}

		case SHUFFLE_MODE: {
			_playIndex = (_playIndex + 1) % _noteCount;
			if (_playIndex == 0) {
				std::fill(_shuffleMask, _shuffleMask + maxChannels, false);
			}
			_syncNext = _syncTo && _playIndex == _noteCount - 1;

			int n = random::u32() % (_noteCount - _playIndex);
			int i = 0;
			for (; i < _noteCount; ++i) {
				if (!_shuffleMask[i]) {
					if (n < 1) {
						_shuffleMask[i] = true;
						break;
					}
					--n;
				}
			}
			pitchOut = _notesAsPlayed[i].pitch;
			return true;
		}
	}

	assert(false);
	return false;
}

void Arp::NoteSet::reset() {
	resetSequence();
	_notesDirty = false;
	_noteCount = 0;
	for (int c = 0; c < maxChannels; ++c) {
		_noteOn[c] = false;
		_notesAsPlayed[c].reset();
		_notesByPitch[c].reset();
	}
}

void Arp::NoteSet::resetSequence() {
	_playIndex = -1;
	_up = true;
}

void Arp::NoteSet::addNote(int c, float pitch) {
	Note n;
	n.pitch = pitch;
	n.channel = c;
	int i = 0;
	while (n.pitch >= _notesByPitch[i].pitch && i < _noteCount) {
		if (n.pitch == _notesByPitch[i].pitch) {
			return;
		}
		++i;
	}
	assert(i <= _noteCount);

	dropNote(c);
	_noteOn[c] = true;
	_notesDirty = true;

	shuffleUp(_notesByPitch, i);
	_notesByPitch[i] = n;

	_notesAsPlayed[_noteCount] = n;
	++_noteCount;
	assert(_noteCount <= maxChannels);
}

void Arp::NoteSet::dropNote(int c) {
	if (!_noteOn[c]) {
		return;
	}
	_noteOn[c] = false;
	_notesDirty = true;

	int i = 0;
	while (_notesAsPlayed[i].channel != c && i < _noteCount) {
		++i;
	}
	assert(i < _noteCount);
	shuffleDown(_notesAsPlayed, i);
	_notesAsPlayed[_noteCount].reset();

	i = 0;
	while (_notesByPitch[i].channel != c && i < _noteCount) {
		++i;
	}
	assert(i < _noteCount);
	shuffleDown(_notesByPitch, i);
	_notesByPitch[_noteCount].reset();

	--_noteCount;
	assert(_noteCount >= 0);
}

void Arp::NoteSet::shuffleUp(Note* notes, int index) {
	for (int i = _noteCount; i > index; --i) {
		notes[i] = notes[i - 1];
	}
}

void Arp::NoteSet::shuffleDown(Note* notes, int index) {
	for (int n = _noteCount - 1; index < n; ++index) {
		notes[index] = notes[index + 1];
	}
}

void Arp::NoteSet::sync() {
	if (!_syncTo || !_syncTo->_notesDirty) {
		return;
	}

	_noteCount = _syncTo->_noteCount;
	_playIndex = -1;
	std::copy(_syncTo->_noteOn, _syncTo->_noteOn + maxChannels, _noteOn);
	std::copy(_syncTo->_notesAsPlayed, _syncTo->_notesAsPlayed + _noteCount, _notesAsPlayed);
	std::copy(_syncTo->_notesByPitch, _syncTo->_notesByPitch + _noteCount, _notesByPitch);
	_syncTo->_notesDirty = false;
}

void Arp::reset() {
	_clockTrigger.reset();
	_resetTrigger.reset();
	_anyHigh = false;
	_secondsSinceLastClock = -1.0f;
	_clockSeconds = 0.1f;
	_gateGenerator.process(1000.0f);
	for (int c = 0; c < maxChannels; ++c) {
		_gateTrigger[c].reset();
		_gateHigh[c] = false;
	}
	_currentNotes->reset();
	_playbackNotes->reset();
}

void Arp::sampleRateChange() {
	_sampleTime = APP->engine->getSampleTime();
}

json_t* Arp::saveToJson(json_t* root) {
	json_object_set_new(root, NOTES_IMMEDIATE_MODE, json_boolean(_notesImmediate));
	json_object_set_new(root, FIXED_GATE_MODE, json_boolean(_fixedGate));
	return root;
}

void Arp::loadFromJson(json_t* root) {
	json_t* ni = json_object_get(root, NOTES_IMMEDIATE_MODE);
	if (ni) {
		_notesImmediate = json_is_true(ni);
	}

	json_t* fg = json_object_get(root, FIXED_GATE_MODE);
	if (fg) {
		_fixedGate = json_is_true(fg);
	}
}

int Arp::channels() {
	return inputs[PITCH_INPUT].getChannels();
}

void Arp::addChannel(int c) {
	_gateTrigger[c].reset();
}

void Arp::removeChannel(int c) {
	_currentNotes->dropNote(c);
}

void Arp::modulate() {
	_mode = (Mode)clamp(params[MODE_PARAM].getValue(), 0.0f, 6.0f);
	_gateLength = clamp(params[GATE_LENGTH_PARAM].getValue(), 0.0f, 1.0f);

	bool hold = params[HOLD_PARAM].getValue() > 0.5f;
	if (_hold && !hold) {
		dropAllNotes();
	}
	_hold = hold;
}

void Arp::processAll(const ProcessArgs& args) {
	lights[UP_LIGHT].value = _mode == UP_MODE;
	lights[DOWN_LIGHT].value = _mode == DOWN_MODE;
	lights[UP_DOWN_LIGHT].value = _mode == UP_DOWN_MODE;
	lights[UP_DOWN_REPEAT_LIGHT].value = _mode == UP_DOWN_REPEAT_MODE;
	lights[IN_ORDER_LIGHT].value = _mode == IN_ORDER_MODE;
	lights[RANDOM_LIGHT].value = _mode == RANDOM_MODE;
	lights[SHUFFLE_LIGHT].value = _mode == SHUFFLE_MODE;

	if (_resetTrigger.process(inputs[RESET_INPUT].getVoltage())) {
		_currentNotes->resetSequence();
		_playbackNotes->resetSequence();
	}

	bool wasAnyHigh = _anyHigh;
	_anyHigh = false;
	bool firstAdd = true;
	for (int c = 0; c < _channels; ++c) {
		if (_gateTrigger[c].process(inputs[GATE_INPUT].getPolyVoltage(c))) {
			if (_hold && !wasAnyHigh && firstAdd) {
				dropAllNotes();
			}
			firstAdd = false;
			_anyHigh = true;
			_gateHigh[c] = true;
			_currentNotes->addNote(c, inputs[PITCH_INPUT].getPolyVoltage(c));
			if (_currentNotes->noteCount() == 1) {
				_playbackNotes->sync();
			}
		}
		else if (_gateHigh[c]) {
			if (!_gateTrigger[c].isHigh()) {
				_gateHigh[c] = false;
				if (!_hold) {
					_currentNotes->dropNote(c);
				}
			}
			else {
				_anyHigh = true;
			}
		}
	}

	bool clock = false;
	if (inputs[CLOCK_INPUT].isConnected()) {
		clock = _clockTrigger.process(inputs[CLOCK_INPUT].getVoltage());
		if (clock) {
			if (_secondsSinceLastClock > 0.0f) {
				_clockSeconds = _secondsSinceLastClock;
			}
			_secondsSinceLastClock = 0.0f;
		}
		_secondsSinceLastClock += _sampleTime;
	}

	NoteSet* notes = _notesImmediate ? _currentNotes : _playbackNotes;
	if (clock) {
		if (notes->nextPitch(_mode, _pitchOut)) {
			_gateGenerator.reset();
			float gl = _gateLength;
			if (_fixedGate) {
				gl *= 0.5f;
			}
			else {
				gl *= _clockSeconds;
			}
			_gateGenerator.trigger(std::max(0.001f, gl));
		}
	}
	outputs[PITCH_OUTPUT].setVoltage(_pitchOut);
	outputs[GATE_OUTPUT].setVoltage(_gateGenerator.process(_sampleTime) * 5.0f);
}

void Arp::dropAllNotes() {
	for (int c = 0; c < _channels; ++c) {
		if (!_gateHigh[c]) {
			_currentNotes->dropNote(c);
		}
	}
}

struct ArpWidget : BGModuleWidget {
	static constexpr int hp = 3;

	ArpWidget(Arp* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "Arp");
		createScrews();

		// generated by svg_widgets.rb
		auto modeParamPosition = Vec(24.0, 57.0);
		auto gateLengthParamPosition = Vec(14.5, 87.5);
		auto holdParamPosition = Vec(29.0, 114.0);

		auto clockInputPosition = Vec(10.5, 132.0);
		auto resetInputPosition = Vec(10.5, 167.0);
		auto pitchInputPosition = Vec(10.5, 202.0);
		auto gateInputPosition = Vec(10.5, 237.0);

		auto pitchOutputPosition = Vec(10.5, 275.0);
		auto gateOutputPosition = Vec(10.5, 310.0);

		auto upLightPosition = Vec(3.0, 28.0);
		auto downLightPosition = Vec(24.0, 28.0);
		auto upDownLightPosition = Vec(3.0, 38.0);
		auto upDownRepeatLightPosition = Vec(24.0, 38.0);
		auto inOrderLightPosition = Vec(3.0, 48.0);
		auto randomLightPosition = Vec(24.0, 48.0);
		auto shuffleLightPosition = Vec(3.0, 58.0);
		// end generated by svg_widgets.rb

		addParam(createParam<StatefulButton9>(modeParamPosition, module, Arp::MODE_PARAM));
		addParam(createParam<Knob16>(gateLengthParamPosition, module, Arp::GATE_LENGTH_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(holdParamPosition, module, Arp::HOLD_PARAM));

		addInput(createInput<Port24>(clockInputPosition, module, Arp::CLOCK_INPUT));
		addInput(createInput<Port24>(resetInputPosition, module, Arp::RESET_INPUT));
		addInput(createInput<Port24>(pitchInputPosition, module, Arp::PITCH_INPUT));
		addInput(createInput<Port24>(gateInputPosition, module, Arp::GATE_INPUT));

		addOutput(createOutput<Port24>(pitchOutputPosition, module, Arp::PITCH_OUTPUT));
		addOutput(createOutput<Port24>(gateOutputPosition, module, Arp::GATE_OUTPUT));

		addChild(createLight<BGSmallLight<GreenLight>>(upLightPosition, module, Arp::UP_LIGHT));
		addChild(createLight<BGSmallLight<GreenLight>>(downLightPosition, module, Arp::DOWN_LIGHT));
		addChild(createLight<BGSmallLight<GreenLight>>(upDownLightPosition, module, Arp::UP_DOWN_LIGHT));
		addChild(createLight<BGSmallLight<GreenLight>>(upDownRepeatLightPosition, module, Arp::UP_DOWN_REPEAT_LIGHT));
		addChild(createLight<BGSmallLight<GreenLight>>(inOrderLightPosition, module, Arp::IN_ORDER_LIGHT));
		addChild(createLight<BGSmallLight<GreenLight>>(randomLightPosition, module, Arp::RANDOM_LIGHT));
		addChild(createLight<BGSmallLight<GreenLight>>(shuffleLightPosition, module, Arp::SHUFFLE_LIGHT));
	}

	void contextMenu(Menu* menu) override {
		auto m = dynamic_cast<Arp*>(module);
		assert(m);

		OptionsMenuItem* ni = new OptionsMenuItem("Use new notes");
		ni->addItem(OptionMenuItem("On arpeggio restart", [m]() { return !m->_notesImmediate; }, [m]() { m->_notesImmediate = false; }));
		ni->addItem(OptionMenuItem("Immediately", [m]() { return m->_notesImmediate; }, [m]() { m->_notesImmediate = true; }));
		OptionsMenuItem::addToMenu(ni, menu);

		OptionsMenuItem* fg = new OptionsMenuItem("Max gate length");
		fg->addItem(OptionMenuItem("Clock interval", [m]() { return !m->_fixedGate; }, [m]() { m->_fixedGate = false; }));
		fg->addItem(OptionMenuItem("Fixed (500ms)", [m]() { return m->_fixedGate; }, [m]() { m->_fixedGate = true; }));
		OptionsMenuItem::addToMenu(fg, menu);
	}
};

Model* modelArp = createModel<Arp, ArpWidget>("Bogaudio-Arp", "ARP", "Polyphonic-input arpeggiator", "Arpeggiator", "Polyphonic");
