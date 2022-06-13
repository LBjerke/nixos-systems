
#include <algorithm>

#include "noise.hpp"
#ifdef ARCH_WIN
#include <ctime>
#endif

using namespace bogaudio::dsp;


Seeds::Seeds() {
#ifdef ARCH_WIN
  _generator.seed(time(0));
#else
  std::random_device rd;
  _generator.seed(rd());
#endif
}

unsigned int Seeds::_next() {
  return _generator();
}

Seeds& Seeds::getInstance() {
  static Seeds instance;
  return instance;
}

unsigned int Seeds::next() {
  return getInstance()._next();
};


void RandomWalk::setParams(float sampleRate, float change) {
	assert(sampleRate > 0.0f);
	assert(change >= 0.0f);
	assert(change <= 1.0f);

	_filter.setParams(sampleRate, std::max(2.0f, change * 0.49f * std::min(44100.0f, sampleRate)));

	const float maxDamp = 0.98;
	const float minDamp = 0.9999;
	_damp = maxDamp + (1.0f - change)*(minDamp - maxDamp);

	_biasDamp = 1.0f - change*(2.0f / sampleRate);
}

void RandomWalk::jump() {
	float x = fabsf(_noise.next()) * (_max - _min);
	x += _min;
	tell(x);
}

void RandomWalk::tell(float v) {
	assert(v >= _min && v <= _max);
	_last = _bias = v;
	_filter.reset();
}

float RandomWalk::_next() {
	float delta = _noise.next();
	if ((_lastOut >= _max && delta > 0.0f) || (_lastOut <= _min && delta < 0.0f)) {
		delta = -delta;
	}
	_last = _damp*_last + delta;
	_bias *= _biasDamp;
	return _lastOut = std::min(std::max(_bias + _filter.next(_last), _min), _max);
}
