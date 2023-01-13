/*
 * CPic.h
 *
 *  Created on: 07092022
 *      Author: german
 */

#ifndef CPIC_H_
#define CPIC_H_

#include <sys/time.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <math.h>
#include <mutex>
#include "CFile.h"

class CPic {
public:

    const std::string TAG_DEBUG = "PIC";
    const unsigned char LEVEL_LOG_0 = 0;
    const unsigned char LEVEL_LOG_1 = 1;
    const unsigned char LEVEL_LOG_2 = 2;
    const unsigned char LEVEL_LOG_3 = 3;
    const unsigned char LEVEL_LOG_4 = 4;

	// Functional status
	const std::string STATUS_UNINITIALIZE = "STATUS_UNINITIALIZE";
	const std::string STATUS_INITIALIZE = "STATUS_INITIALIZE";
	const std::string STATUS_DEBUG1 = "STATUS_DEBUG1";
	const std::string STATUS_DEBUG2 = "STATUS_DEBUG2";
	const std::string STATUS_DEBUG3 = "STATUS_DEBUG3";
	const std::string STATUS_DEBUG4 = "STATUS_DEBUG4";
	const std::string STATUS_DEBUG5 = "STATUS_DEBUG5";
	const std::string STATUS_DEBUG6 = "STATUS_DEBUG6";

	const std::string STATUS_STEP1 = "STATUS_STEP1";
	const std::string STATUS_STEP2 = "STATUS_STEP2";
	const std::string STATUS_STEP3 = "STATUS_STEP3";
	const std::string STATUS_STEP4 = "STATUS_STEP4";
	const std::string STATUS_ERROR = "STATUS_ERROR";

	// Define bytes
	const unsigned char BYTE_STX = 0x03;

	// Commands
	const unsigned char COMMAND_OPEN = 0x10;
	const unsigned char COMMAND_CLOSE = 0x11;
	const unsigned char COMMAND_OPEN2 = 0x12;
	const unsigned char COMMAND_CLOSE2 = 0x13;
	const unsigned char COMMAND_GETSENSOR = 0x20;
	const unsigned char COMMAND_EEPROM_WRITE = 0x40;
	const unsigned char COMMAND_EEPROM_READ = 0x41;
	const unsigned char COMMAND_BLOCK_FORMAT = 0x42;	
	const unsigned char COMMAND_CHANGEKEY = 0x43;	
	const unsigned char COMMAND_BLOCK_FORMAT2 = 0x44;
	const unsigned char COMMAND_VERSION = 0x50;
	const unsigned char COMMAND_BUZZER = 0x52;
	const unsigned char COMMAND_CASHBOX = 0x60;
	const unsigned char COMMAND_POLL = 0x70;
	const unsigned char COMMAND_SETTOKEN = 0x80;
	

	// Answers
	const unsigned char ANS_OK = 0xA0;
	const unsigned char ANS_ERROR_INIT = 0xF0;
	const unsigned char ANS_ERROR_ACCESSS = 0xF1;
	const unsigned char ANS_ERROR_KEY = 0xF2;
	const unsigned char ANS_ERROR_TOKEN = 0xF3;
	const unsigned char ANS_ERROR_TIME = 0xF4;
	const unsigned char ANS_ERROR_FORMAT = 0xE0;
	const unsigned char ANS_ERROR_CRC = 0xE1;
	const unsigned char ANS_ERROR_COMMAND = 0xE2;
	const unsigned char ANS_ERROR_VALUE = 0xE3;
	const unsigned char ANS_ERROR_LENGTH = 0xE4;

    // Mask error codes
    const unsigned int ERROR_NONE = 0x0000;
    const unsigned int ERROR_BOXREMOVE = 0x0001;
    const unsigned int ERROR_FULL = 0x0002;
    const unsigned int ERROR_JAM = 0x0004;
    const unsigned int ERROR_COMMUNICATION = 0x0008;
    const unsigned int ERROR_FULLSOFTWARE = 0x0010;
    const unsigned int ERROR_HARDWARE = 0x0020;
	const unsigned int ERROR_PORT = 0x0040;
	const unsigned int ERROR_RESTARTING = 0x0080;

	// Mode sensors decode
	const unsigned char SENSOR_SOLENOIDE = 1;
	const unsigned char SENSOR_BLACK = 2;
	const unsigned char SENSOR_CASHBOX = 3;
	
private:
	CFile *objFile;	
	std::string presentStatus;
	std::string nextStatus;
	std::string nextStep;
	std::string previousStatus;
	std::string namePort;
	int idPort;
	bool errorPort;	    
    bool flagCommunication;    
    int codeError;    
    unsigned char valueParam;
    int speed;    
    std::string versionDevice; 
    bool initialize;                
    unsigned char levelDebug;
    std::mutex mutexFile;
    bool flagMainExit;
    bool flagRestarting;	    
	std::mutex mutexBusy;	
	bool globalError;
	unsigned int mainKey;
	unsigned char statusBytes [ 20 ];		

public:
	CPic();
	CPic ( const CPic & obj );
	virtual ~CPic();	
	bool ChangeStatus ( std::string tempChange );
	void AddHex ( std::string tempString, unsigned char * tempBuffer, int tempSize, int tempLevel );   
    void AddLog ( std::string tempString, unsigned char tempLevel );   	
	void Clear ( void );	
    std::string GetCurrentStatus ( void );    		
	bool GetErrorStatus ( unsigned int *tempCode, std::string *tempError );		
	bool InitializeEEprom ( void );
	void Run ( int x );
    void SetLevelDebug ( unsigned char tempValue );
	void SetMainKey ( unsigned int tempKey );    
	void SetNamePort ( std::string tempName );    
	void SetObjFile ( CFile * tempLog );
	void SetSpeedPort ( std::string tempValue );
	bool CommandBuzzer ( unsigned char tempValue );
	bool CommandChangeKey ( unsigned char tempBlock, unsigned int tempCurrentKey, unsigned int tempNewKey, unsigned int tempTime );
	bool CommandClose2 ( unsigned char tempDoor, unsigned int tempKeyLock, unsigned int tempKeyToken,unsigned int tempTime );
	bool CommandGetCashBox ( unsigned char * tempValue );
	bool CommandGetSensor ( unsigned char tempDoor, unsigned char * tempValue );
	bool CommandInitBlock ( unsigned char tempBlock, unsigned char tempAccess, unsigned int tempNewKey );	
	bool CommandInitBlock2 ( unsigned char tempBlock, unsigned char tempAccess, unsigned int tempKeyBlock, unsigned int tempKeyToken, unsigned int tempTime );	
	bool CommandOpen2 ( unsigned char tempDoor, unsigned int tempKeyLock, unsigned int tempKeyToken, unsigned int tempTime );
    bool CommandPoll ( unsigned char * buffer );	
	bool CommandReadEEProm ( unsigned char tempAddress, unsigned char tempSize, unsigned char *tempData );
	bool CommandSetToken ( unsigned int tempTime );
    bool CommandVersion ( std::string * tempVersion );
	bool CommandWriteEEProm ( unsigned char tempAddress, unsigned char tempValue );	
	bool DecodeSensor ( unsigned char tempValue, unsigned char tempMode, std::string tempSeparator, std::string * tempString );
    std::string HexToText ( unsigned char * tempBuffer, unsigned tempSize );    
	bool OpenPort ( void );
	unsigned int CalcTimeToken ( void );
	bool GenerateToken ( unsigned int tempTime, unsigned int tempKey, unsigned char * tempToken, unsigned int * tempRandom );	    	

private:
	
	bool CalcCRC ( unsigned char * tempBuffer ); 	
    bool FlushPort ( void );    	
    bool SendCommand ( unsigned char * command, unsigned char * ans );		
    bool WaitData ( int bytes );
};

#endif /* CSOCKETRX_H_ */
