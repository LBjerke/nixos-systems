
#include "trigger_on_load.hpp"

using namespace bogaudio;

#define TRIGGER_ON_LOAD "triggerOnLoad"
#define SHOULD_TRIGGER_ON_LOAD "shouldTriggerOnLoad"

json_t* TriggerOnLoadModule::saveToJson(json_t* root) {
	json_object_set_new(root, TRIGGER_ON_LOAD, json_boolean(_triggerOnLoad));
	json_object_set_new(root, SHOULD_TRIGGER_ON_LOAD, json_boolean(shouldTriggerOnNextLoad()));
	return root;
}

void TriggerOnLoadModule::loadFromJson(json_t* root) {
	json_t* tol = json_object_get(root, TRIGGER_ON_LOAD);
	if (tol) {
		_triggerOnLoad = json_is_true(tol);
	}
	json_t* stol = json_object_get(root, SHOULD_TRIGGER_ON_LOAD);
	if (stol) {
		_shouldTriggerOnLoad = json_is_true(stol);
	}
}


void TriggerOnLoadModuleWidget::contextMenu(Menu* menu) {
	auto m = dynamic_cast<TriggerOnLoadModule*>(module);
	assert(m);
	menu->addChild(new BoolOptionMenuItem(_menuItemLabel.c_str(), [m]() { return &m->_triggerOnLoad; }));
}
