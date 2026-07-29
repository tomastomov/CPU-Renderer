#pragma once

#include <pch.h>

namespace CPURenderer {
	template<typename ...Args>
	inline void Log(std::format_string<Args...> format, Args&& ...args) {
		std::cout << std::format(format, std::forward<Args>(args)...) << std::endl;
	}
}

