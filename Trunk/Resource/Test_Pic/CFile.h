/*
* GenerateLicense
 * CFile.h
 *
 *  Created on: Mar 7, 2021
 *      Author: german
 */

#ifndef CFILE_H_
#define CFILE_H_

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <mutex>
#include <sys/time.h>
#include <sys/stat.h>
#include <errno.h>

#define CFILE_TRUE 1
#define CFILE_FALSE 0

class CFile 
{
public:
	// Levels to add information to log
	const int LEVEL_LOG_1 = 1;  // General operation information
	const int LEVEL_LOG_2 = 2;
	const int LEVEL_LOG_3 = 3;
	const int LEVEL_LOG_4 = 4;  // Lower level information communication devices
private:
	std::string pathLog;
	std::string pathDatabase;
	int levelLog;
	int countLines;
	int linesFile;
	std::mutex mutexBusy;
public:
	CFile();
	void AddHex ( std::string tempLabel, unsigned char * tempBuffer, int tempSize );
	void AddHex ( std::string tempLabel, unsigned char * tempBuffer, int tempSize, int tempLevel );
	void AddLog ( std::string tempText );
	void AddLog ( std::string tempText, int tempLevel );    
	virtual ~CFile();
    bool CreateFile ( std::string tempName, unsigned char * tempData, int tempSize );
	std::string HexToText ( unsigned char * tempBuffer, unsigned int tempSize );    
	void SetLevelLog ( int tempLevel );
	void SetPathDatabase ( std::string tempName );
	void SetPathLog ( std::string tempName );


};

#endif /* CFILE_H_ */
