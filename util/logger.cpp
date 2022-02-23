#include "cvpch.h"
#include "logger.h"

namespace team45
{
	void log::init()
	{
		// https://github.com/gabime/spdlog/wiki/3.-Custom-formatting
		// %^ Start color range (can be used only once)
		// %L Short log level of the message
		// %T ISO 8601 time format (HH:MM:SS), equivalent to %H:%M:%S
		// %@ Source file and line (use SPDLOG_TRACE(..), SPDLOG_INFO(...) etc. instead of spdlog::trace(...)
		// %! Source function (use SPDLOG_TRACE(..), SPDLOG_INFO(...) etc. see tweakme for pretty-print)
		// %$ End color range (can be used only once)
		spdlog::set_pattern("%^%L/[%T] >> %@ >> %!\n--%v%$");	
		auto logger = spdlog::stdout_color_mt("console");
		logger->set_level(spdlog::level::trace);
		spdlog::set_default_logger(logger);
	}

	void log::shutdown()
	{
		spdlog::shutdown();
	}
}
