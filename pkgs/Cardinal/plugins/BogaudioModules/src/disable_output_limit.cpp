
#include "disable_output_limit.hpp"

#define DISABLE_OUTPUT_LIMIT "disableOutputLimit"

json_t* DisableOutputLimitModule::saveToJson(json_t* root) {
	json_object_set_new(root, DISABLE_OUTPUT_LIMIT, json_boolean(_disableOutputLimit));
	return root;
}

void DisableOutputLimitModule::loadFromJson(json_t* root) {
	json_t* dol = json_object_get(root, DISABLE_OUTPUT_LIMIT);
	if (dol) {
		_disableOutputLimit = json_is_true(dol);
	}
}


void DisableOutputLimitModuleWidget::contextMenu(Menu* menu) {
	auto m = dynamic_cast<DisableOutputLimitModule*>(module);
	assert(m);
	menu->addChild(new BoolOptionMenuItem("Disable output limit", [m]() { return &m->_disableOutputLimit; }));
}
