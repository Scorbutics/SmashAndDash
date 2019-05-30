#pragma once
#include <Logging/MultiLogger.h>
#include <Logging/LogAsync.h>
#include <Logging/Logger.h>

using GameLogger = ska::MultiLogger<
	ska::Logger<ska::LogLevel::Debug, ska::LogLevel::Error, ska::LogAsync>,
	ska::Logger<ska::LogLevel::Debug, ska::LogLevel::Error, ska::LogSync>
>;

GameLogger& GetGameLogger();

#ifndef SLOG_STATIC
#define SLOG_STATIC(level, currentClass) SKA_LOGC_STATIC(GetGameLogger(), level, currentClass)
#define SLOG(level) SKA_LOGC(GetGameLogger(), level)
#endif
