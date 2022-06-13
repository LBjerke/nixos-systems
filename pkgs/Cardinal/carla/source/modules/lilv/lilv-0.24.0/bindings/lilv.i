%module lilv
%typedef unsigned uint32_t;
%{
#define SWIG_FILE_WITH_INIT
#include "lilv/lilv.h"
#include "lilv/lilvmm.hpp"
%}

%include "numpy.i"
%init %{
	import_array();
%}
%apply (float* INPLACE_ARRAY1) {(void* data_location)}

%feature("compactdefaultargs") %{
	lilv_plugin_get_num_ports_of_class;
	get_num_ports_of_class;
%}
%varargs(3, LilvNode* node = NULL) lilv_plugin_get_num_ports_of_class;
%varargs(3, LilvNode* node = NULL) get_num_ports_of_class;
%typemap(in, numinputs=0) LilvNode *node3 ""; // Make sure it's NULL terminated

%include "lilv/lilv.h"
%include "lilv/lilvmm.hpp"

namespace Lilv {

%extend Plugins {
%pythoncode %{
	def __iter__(self):
		class Iterator(object):
			def __init__(self, plugins):
				self.plugins = plugins
				self.iter    = plugins.begin()

			def __next__(self):
				if self.plugins.is_end(self.iter):
					raise StopIteration
				plugin = self.plugins.get(self.iter)
				self.iter = self.plugins.next(self.iter)
				return plugin

			def next(self):
				"Python 2 iterator protocol"
				return Iterator.__next__(self)

		return Iterator(self)

	def get_by_uri(self, *args):
		"""get_by_uri(self, LilvNode uri) -> PluginClass"""
		ret = _lilv.Plugins_get_by_uri(self, *args)
		if ret.me is None:
			return None
		else:
			return ret
%}
};

%extend Node {
%pythoncode %{
	def __str__(self):
		return self.get_turtle_token()
%}
};

} /* namespace Lilv */
