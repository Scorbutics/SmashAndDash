#include <iostream>
#include <fstream>
#include "Base/IO/Files/LogFilePath.h"
#include "LoggerConfig.h"

static inline GameLogger BuildGameLogger(const char * filename) {
	static auto LogFileOutput = ska::OpenLogFile(filename);
	auto logger = GameLogger{};
	logger.get<0>().addOutputTarget(LogFileOutput);
	logger.get<1>().addOutputTarget(std::cout);
 
    
    return logger;
}

GameLogger& GetGameLogger() {
	static auto gameLogger = BuildGameLogger("Game.log.txt");   
    return gameLogger;
}

