#pragma once

#include <pch.h>

#define ENABLE_LOG

namespace CPURenderer {
	template<typename ...Args>
	inline void Log(std::format_string<Args...> format, Args&& ...args) {
#ifdef ENABLE_LOG
		std::cout << std::format(format, std::forward<Args>(args)...) << std::endl;
#else do {} while (0)
#endif
	}
}

