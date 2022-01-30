#include <common.hpp>
#include <string.hpp>


#if defined ARCH_WIN
#include <windows.h>

FILE* fopen_u8(const char* filename, const char* mode) {
	return _wfopen(rack::string::UTF8toUTF16(filename).c_str(), rack::string::UTF8toUTF16(mode).c_str());
}

#endif


namespace rack {


const std::string APP_NAME = "VCV Rack";
const std::string APP_EDITION = "Free";
const std::string APP_EDITION_NAME = "Free";
const std::string APP_VERSION_MAJOR = "2";
const std::string APP_VERSION = TOSTRING(_APP_VERSION);
#if defined ARCH_WIN
	const std::string APP_OS = "win";
#elif ARCH_MAC
	const std::string APP_OS = "mac";
#elif defined ARCH_LIN
	const std::string APP_OS = "lin";
#endif
const std::string API_URL = "https://api.vcvrack.com";


Exception::Exception(const char* format, ...) {
	va_list args;
	va_start(args, format);
	msg = string::fV(format, args);
	va_end(args);
}


} // namespace rack
