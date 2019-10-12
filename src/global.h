#ifndef GLOBAL_H
#define GLOBAL_H

#include <QDir>
#include <QString>
#include <vector>

struct Config{
	bool verbose						= false;
	uint8_t logLevel					= 3;
#ifdef __linux__
	QString logFile						= "/var/log/SerialTerminal.log";
#elif _WIN32
	QString logFile						= QDir::homePath() + "/SerialTerminal.log";
#endif
	QString version;
};

namespace app {
	extern Config conf;

	bool parsArgs(int argc, char *argv[]);
	void setLog(const uint8_t logLevel, const QString &mess);
}

#endif // GLOBAL_H
