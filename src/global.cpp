#include "global.h"

#include <QDateTime>

namespace app {
	Config conf;

	bool parsArgs(int argc, char *argv[])
	{
		bool ret = true;
		for(int i=0;i<argc;i++){
			if(QString(argv[i]).indexOf("-")==0){
				if(QString(argv[i]) == "--help" or QString(argv[1]) == "-h"){
					printf("Usage: %s [OPTIONS]\n"
							"  -l <FILE>    log file\n"
							"  -v    Verbose output\n"
							"\n", argv[0]);
					ret = false;
				}
				if(QString(argv[i]) == "-l") app::conf.logFile = QString(argv[++i]);
				if(QString(argv[i]) == "-v") app::conf.verbose = true;
			}
		}
		return ret;
	}

	unsigned short calculateCRC(char *data, unsigned char length)
	{
		return calculateCRC( (unsigned char*)data, length );
	}

	unsigned short calculateCRC(unsigned char *data, unsigned char length)
	{
//		unsigned short crc, temp2, flag;
//		crc = 0xFFFF;

//		for( unsigned char i = 0; i < length; i++){
//			crc = crc ^ data[ i ];
//			for( unsigned char j = 1; j <= 8; j++ ){
//				flag = crc & 0x0001;
//				crc >>= 1;
//				if (flag) crc ^= 0xA001;
//			}
//		}



		unsigned char j;
		unsigned short crc = 0xFFFF;

		while( length-- ){
			crc ^= *data++;
			for( j = 0; j < 8; j++ ){
				if( crc & 0x01 ){
					crc = ( crc >> 1 ) ^ 0xA001;
				}else{
					crc = crc >> 1;
				}
			}
		}

		// Reverse byte order.
		unsigned short temp2 = crc >> 8;
		crc = (crc << 8) | temp2;
		crc &= 0xFFFF;

		return crc;
	}

}
