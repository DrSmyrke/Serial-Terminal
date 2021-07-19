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
	unsigned short calculateCRC(char *data, unsigned char length);
	unsigned short calculateCRC(unsigned char *data, unsigned char length);
}

#endif // GLOBAL_H
