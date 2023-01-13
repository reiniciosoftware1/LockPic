/*
 * CPic.cpp
 *
 *  Created on: 07092022
 *      Author: german
 */

#include "CPic.h"

/*------------------------------------------------------------------------------------------------
 * \fn CPic::(const CPic & obj )
 * \brief Builder overload class
 * \param Ninguno.

 * \return Nada
 *
 * \pre
 * \post
 *
 * description:
------------------------------------------------------------------------------------------------*/
CPic::CPic ( const CPic & obj )
{

}

/*------------------------------------------------------------------------------------------------
 * \fn CPic::CPic ( )
 * \brief Builder class
 * \param Ninguno.

 * \return Nada
 *
 * \pre
 * \post
 *
 * description:
------------------------------------------------------------------------------------------------*/
CPic::CPic()
{
    errorPort = false;		
    levelDebug = LEVEL_LOG_1;    
    globalError = false;
    initialize = false; 
}

/*------------------------------------------------------------------------------------------------
 * \fn CPic::~CPic ( )
 * \brief Destroyer class
 * \param Ninguno.

 * \return Nada
 *
 * \pre
 * \post
 *
 * description:
------------------------------------------------------------------------------------------------*/
CPic::~CPic()
{
	// TODO Auto-generated destructor stub


}

/*------------------------------------------------------------------------------------------------*/
/*
 * \fn void CPic::AddHex ( std::string tempString, unsigned char * tempBuffer, unsigned char tempSize, unsigned char tempLevel )
 * \brief Add buffer hex to log file
 * \param buffer: Pointer buffer to send datas
 * \return true: Got CRC .
 * \return false: Error
 *
 * \pre
 * \post
 *
 * DESCRIPCION: Calculate CRC, it is put into 2 last bytes. This function was based from Innovative
 * CRC
 */
/*------------------------------------------------------------------------------------------------*/
void CPic::AddHex ( std::string tempString, unsigned char * tempBuffer, int tempSize, int tempLevel )
{
    mutexFile.lock ( );    

    // Chek if it is enabled log
    if ( levelDebug == 0 )
    {        
        mutexFile.unlock ( );
        return;
    }    

    // Add to log only if it is lower to level
    if ( tempLevel <= levelDebug )
    {
        objFile -> AddHex ( TAG_DEBUG + ":" + tempString, tempBuffer, tempSize, tempLevel );
    }

    mutexFile.unlock ( );
}

/*------------------------------------------------------------------------------------------------*/
/*
 * \fn void CPic::AddLog ( std::string tempString, unsigned char tempLevel )
 * \brief Add text to log file
 * \param buffer: Pointer buffer to send datas
 * \return true: Got CRC .
 * \return false: Error
 *
 * \pre
 * \post
 *
 * DESCRIPCION: Calculate CRC, it is put into 2 last bytes. This function was based from Innovative
 * CRC
 */
/*------------------------------------------------------------------------------------------------*/
void CPic::AddLog ( std::string tempString, unsigned char tempLevel )
{
    mutexFile.lock ( );

    // Chek if it is enabled log
    if ( levelDebug == 0 )
    {
        mutexFile.unlock ( );
        return;
    }

    // Add to log only if it is lower to level
    if ( tempLevel <= levelDebug )
    {
        objFile -> AddLog ( TAG_DEBUG + ":" + tempString );
    }

    mutexFile.unlock ( );
}

/*------------------------------------------------------------------------------------------------*/
/*
 * \fn bool CPic::CalcCRC ( unsigned char * buffer )
 * \brief Calculate CRC over a data buffer to send them
 * \param buffer: Pointer buffer to send datas
 * \return true: Got CRC .
 * \return false: Error
 *
 * \pre
 * \post
 *
 * DESCRIPCION: Calculate CRC, it is put intolast byte. 
 * CRC
 */
/*------------------------------------------------------------------------------------------------*/
bool CPic::CalcCRC ( unsigned char * tempBuffer )
{
    int tempSize;
    unsigned char tempCrc;
    unsigned char pos;    

    // Get total bytes CRC to calc
    tempSize = *( tempBuffer + 1 ) + 1;

    // Clear CRC
    tempCrc = 0x00;

    // Cycle for take each bytes into buffer
    for ( pos = 1; pos < tempSize; pos ++ )
    {
      tempCrc ^= tempBuffer [ pos ];
    }

    // Set byte CRC
    tempBuffer [ tempSize ] = tempCrc;

    return true;
}

/*------------------------------------------------------------------------------------------------*/
/*
 * \fn unsigned int CPic::CalcTimeToken ( void )
 * \brief Calculate time
 * \param None
 * \return time seconds since january 1 2000
 *
 * \pre
 * \post
 *
 * DESCRIPCION:
 * CRC
 */
/*------------------------------------------------------------------------------------------------*/
unsigned int CPic::CalcTimeToken ( void )
{
    time_t tempCurrentTime;
    time_t tempStartTime;
    unsigned int tempSeconds;
    struct tm tempTime;
    char tempText [ 200 ];
    std::string tempString;

    // Get current time
    time ( &tempCurrentTime );

    // Set start time
    tempTime.tm_sec = 0;
    tempTime.tm_min = 0;
    tempTime.tm_hour = 0;
    tempTime.tm_mday = 1;
    tempTime.tm_mon = 0;
    tempTime.tm_year = 100;
    tempStartTime = mktime ( &tempTime );

    sprintf ( tempText, "actual: %lu  inicial: %lu\n", tempCurrentTime, tempStartTime );
    tempString = tempText;
    AddLog ( tempString, LEVEL_LOG_1 );

    tempStartTime = 946699200;

    // Calculate differ times
    tempSeconds = tempCurrentTime - tempStartTime;

    sprintf ( tempText, "Diferencia: %d", tempSeconds );
    tempString = tempText;
    AddLog ( tempString, LEVEL_LOG_1 );

    return tempSeconds;
}

/*------------------------------------------------------------------------------------------------
 * \fn bool CPic::ChangeStatus ( std::string tempChange )
 * \brief Change operation status
 * \param tempChange: New status
 * \return true: Change status executed
 * \return false: Error changing status
 *
 * \pre
 * \post
 *
 * description:
 ------------------------------------------------------------------------------------------------*/
bool CPic::ChangeStatus ( std::string tempChange )
{         
    // Lock variable
    mutexBusy.lock ( );    

	// Set new status
	nextStatus = tempChange;

	AddLog ( "Asigando nuevo estado: " + nextStatus, LEVEL_LOG_2 );

    // Unlock
    mutexBusy.unlock();
    
	return true;
}

/*------------------------------------------------------------------------------------------------*/
/*
 * \fn void CPic::Clear ( void )
 * \brief Clear data
 * \param None
 * \return  None
 *
 * \pre
 * \post
 *
 * DESCRIPCION:
 * CRC
 */
/*------------------------------------------------------------------------------------------------*/
void CPic::Clear ( void )
{   
}

/*------------------------------------------------------------------------------------------------
 * \fn bool CPic::FlushPort ( void )
 * \brief Flush serial port buffer
 * \param ninugno.
 * \return true: deleted bytes
 * \return false: Error
 *
 * \pre
 * \post
 *
 * description:
------------------------------------------------------------------------------------------------*/
bool CPic::FlushPort ( void )
{
  int bytesRx;
  unsigned char buffer [ 200 ];
  int tempStatus;

  // Get size bytes received
  ioctl ( idPort, FIONREAD, &bytesRx );

  // Read currently bytes
  tempStatus = read ( idPort, buffer, bytesRx );

  // Check number bytes read
  if ( tempStatus != bytesRx )
  {
    AddLog ( "Error limpiando el puerto", LEVEL_LOG_1 );

    return false;
  }

  return true;
}

/*------------------------------------------------------------------------------------------------
 * \fn bool CPic::GenerateToken ( unsigned int tempTime, unsigned int tempKey, unsigned int * tempToken, unsigned int * tempRandom )
 * \brief Generate a token time
 * \param tempTime: Time in seconds use to generate token.
 * \param tempKey: Key to code token
 * \param tempToken: Pointer to final token
 * \param tempRandom: Pointer to left random data
 * \return true: Token generated
 * \return false: Error
 *
 * \pre
 * \post
 *
 * description:
------------------------------------------------------------------------------------------------*/
bool CPic::GenerateToken ( unsigned int tempTime, unsigned int tempKey, unsigned char * tempToken, unsigned int * tempRandom )
{
    unsigned char pos;   
    unsigned char tempByte; 
    unsigned int tempNum;
    unsigned char tempBuffRandom [ 4 ];
    unsigned char tempBuffData [ 4 ];
    unsigned char tempBuffKey [ 4 ];
    unsigned int tempData;
    unsigned char tempSh;
    unsigned char tempSl;
    char tempText [ 500 ];
    std::string tempString;

    // Update random seed
    srand ( clock ( ) );    

    *tempRandom = 0x00;

    // Generate random number
    for ( pos = 0; pos < 4; pos ++ )
    {       
        // Generate byte random
        tempByte = rand ( ) % 255;           

        tempBuffRandom [ pos ] = tempByte;
       
        tempNum = tempByte << ( pos * 8 );
        *tempRandom += tempNum;
    }        

    // Calculate data (T-K)
    tempData = tempTime - tempKey;
    tempData = ~( tempData );


    sprintf ( tempText, "Genear token time: %08X key: %08X random: %08X DATO: %08X\n", tempTime, tempKey, *tempRandom, tempData );
    tempString = tempText;
    AddLog ( tempString, LEVEL_LOG_1 );

    // Separate bytes offset 0 is lowest byte
    tempBuffData [ 0 ] = tempData & 0xFF;
    tempBuffData [ 1 ] = ( tempData >> 8 ) & 0xFF;
    tempBuffData [ 2 ] = ( tempData >> 16 ) & 0xFF;
    tempBuffData [ 3 ] = ( tempData >> 24 ) & 0xFF;        

    // Separate bytes offset 0 is lowest byte
    tempBuffKey [ 0 ] = tempKey & 0xFF;
    tempBuffKey [ 1 ] = ( tempKey >> 8 ) & 0xFF;
    tempBuffKey [ 2 ] = ( tempKey >> 16 ) & 0xFF;
    tempBuffKey [ 3 ] = ( tempKey >> 24 ) & 0xFF;       

    // Calculate SH
    tempSh = tempBuffRandom [ 2 ] + tempBuffData [ 0 ] + tempBuffKey [ 3 ] + tempBuffKey [ 2 ];
    tempSl = tempBuffRandom [ 1 ] + tempBuffData [ 0 ] + tempBuffKey [ 1 ] + tempBuffKey [ 0 ];    

    tempToken [ 0 ] = tempSh;
    tempToken [ 1 ] = tempBuffRandom [ 0 ];
    tempToken [ 2 ] = tempBuffData [ 1 ] + tempBuffRandom [ 2 ];
    tempToken [ 3 ] = tempBuffRandom [ 1 ];
    tempToken [ 4 ] = tempBuffData [ 3 ] + tempBuffRandom [ 0 ];
    tempToken [ 5 ] = tempBuffRandom [ 2 ];
    tempToken [ 6 ] = tempBuffData [ 0 ] + tempBuffRandom [ 3 ];
    tempToken [ 7 ] = tempBuffRandom [ 3 ];
    tempToken [ 8 ] = tempBuffData [ 2 ] + tempBuffRandom [ 1 ];
    tempToken [ 9 ] = tempSl;    

    AddHex ( "Token", tempToken, 10, LEVEL_LOG_1 );

    return true;
}

/*------------------------------------------------------------------------------------------------
 * \fn std::string  CPic::GetCurrentStatus ( void )
 * \brief Get current operative status
 * \param None
 * \return presentStatus
 *
 * \pre
 * \post
 *
 * description:
 *
------------------------------------------------------------------------------------------------*/
std::string  CPic::GetCurrentStatus ( void )
{
	return presentStatus;
}

/*------------------------------------------------------------------------------------------------
 * \fn bool CPic::GetErrorStatus ( unsigned int *tempCode, std::string *tempError )
 * \brief Return code last error device
 * \param tempCode: Pointer to left error code
 *\param tempError: Text code error
 * \return true: Get status error
 *
 * \pre
 * \post
 *
 * description:
------------------------------------------------------------------------------------------------*/
bool CPic::GetErrorStatus ( unsigned int *tempCode, std::string *tempError )
{
    std::string tempString;

    tempString = "";

    // Check error port
    if ( errorPort == true )
    {
        *tempCode |= ERROR_PORT;

        if ( tempString.length ( ) > 0 )
        {
            tempString += ",";
        }

        tempString += "ERROR_PUERTO";        
    }

    // Check error without communication
    if ( flagCommunication == false )
    {
        *tempCode |= ERROR_COMMUNICATION;

        if ( tempString.length ( ) > 0 )
        {
            tempString += ",";
        }

        tempString += "SIN_COMUNICACION";
    }

    *tempError = tempString;

    // Comaprte is ther any errro
    if ( tempString.length ( ) == 0 )
    {
        *tempCode = ERROR_NONE;
        *tempError = "NINGUNO";
    }
    
    return true;
}

/*------------------------------------------------------------------------------------------------
 * \fn bool CPic::CommandBuzzer ( unsigned char tempValue )
 * \brief enable buzzer
 * \param tempValue: time in 10ms
 * \return true: Status get
 * \return false: Error
 *
 * \pre
 * \post
 *
 * description:
 *
------------------------------------------------------------------------------------------------*/
bool CPic::CommandBuzzer ( unsigned char tempValue )
{
    unsigned char tempCommand [ ] = { BYTE_STX, 0x03, COMMAND_BUZZER, 0x00, 0x00 };
    unsigned char tempBuffer [ 20 ];
    bool tempStatus;    
    std::string tempString;
    char tempText [ 250 ];

    AddLog ( "CommandBuzzer", LEVEL_LOG_4 );

    tempCommand [ 3 ] = tempValue;

    // Send command
    tempStatus = SendCommand ( tempCommand, tempBuffer );

    // Check if was correct
    if ( tempStatus != true )
    {
        // Communication error
        flagCommunication = false;

        AddLog ( "Error comunicacion CommandBuzzer.", LEVEL_LOG_1 );

        return false;
    }

    // Communication works
    flagCommunication = true;    

    return true;
}

/*------------------------------------------------------------------------------------------------
 * \fn bool CPic::CommandChangeKey ( unsigned char tempBlock, unsigned int tempCurrentKey, unsigned int tempNewKey )
 * \brief Set new key
 * \param tempId: Number block key
 * \param tempCurrentKey: current key
 * \param tempNewKey: New key
 * \return true: success
 * \return false: Error
 *
 * \pre
 * \post
 *
 * description:
------------------------------------------------------------------------------------------------*/
bool CPic::CommandChangeKey ( unsigned char tempBlock, unsigned int tempCurrentKey, unsigned int tempNewKey, unsigned int tempTime )
{
    unsigned char tempCommand [ ] = { BYTE_STX, 0x11, COMMAND_CHANGEKEY, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    unsigned char tempAnsBuffer [ 20 ];
    bool tempStatus;    
    std::string tempString;
    char tempText [ 100 ];
    unsigned char tempBuffer [ 10 ];
    unsigned char tempToken [ 10 ];
    unsigned int tempRandom;    
    unsigned char tempBuffKey [ 4 ];    

    AddLog ( "CommandChangeKey", LEVEL_LOG_4 ); 
    

    sprintf ( tempText, "cambair llave actual: %08X nueva: %08X\n", tempCurrentKey, tempNewKey );
    tempString = tempText;
    AddLog ( tempString, LEVEL_LOG_1 );

    // Set block
    tempCommand [ 3 ] = tempBlock;    

    // Generate token
    GenerateToken ( tempTime, tempCurrentKey, tempToken, &tempRandom );

    // Copy token
    memcpy ( tempCommand + 8, tempToken, 10 );      

    // Calc new key random
    tempNewKey += tempRandom;    

    // Separate bytes offset 0 is lowest byte
    tempBuffKey [ 3 ] = tempNewKey & 0xFF;
    tempBuffKey [ 2 ] = ( tempNewKey >> 8 ) & 0xFF;
    tempBuffKey [ 1 ] = ( tempNewKey >> 16 ) & 0xFF;
    tempBuffKey [ 0 ] = ( tempNewKey >> 24 ) & 0xFF;   

    memcpy ( tempCommand + 4, tempBuffKey, 4 );

    // Send command
    tempStatus = SendCommand ( tempCommand, tempAnsBuffer );

    // Check if was correct
    if ( tempStatus != true )
    {
        // Communication error
        flagCommunication = false;

        AddLog ( "Error comunicacion CommandChangeKey.", LEVEL_LOG_1 );

        return false;
    }

    // Check Ans
    if ( tempAnsBuffer [ 3 ] != ANS_OK )
    {
        if ( tempAnsBuffer [ 3 ] == ANS_ERROR_COMMAND )
        {
            AddLog ( "Comando no reconocido posible tarjeta V1.0", LEVEL_LOG_1 );
            printf ( "Comando no reconocido posible tarjeta V1.0\n" );            
            return true;
        }

        // Check kind error
        if ( tempAnsBuffer [ 3 ] == ANS_ERROR_TOKEN )
        {
            AddLog ( "Error token", LEVEL_LOG_1 );
            printf ( "Error token\n" );
        }

        // Check kind error
        if ( tempAnsBuffer [ 3 ] == ANS_ERROR_TIME )
        {
            AddLog ( "Error tiempo token", LEVEL_LOG_1 );
            printf ( "Error tiempo token\n" );
        }

        sprintf ( tempText, "Codigo error:%02X", tempAnsBuffer [ 3 ] );
        tempString = tempText;

        AddLog ( tempString, LEVEL_LOG_1 );        
        
        return false;
    }       

    return true;
}

/*------------------------------------------------------------------------------------------------
 * \fn bool CPic::CommandClose2 ( unsigned char tempDoor, unsigned int tempKeyToken, unsigned int tempKeyLock, unsigned int tempTime )
 * \brief open door
 * \param tempDoor: Number door
 * \param tempToken: toke to open 
 * \return true: success
 * \return false: Error
 *
 * \pre
 * \post
 *
 * description:
------------------------------------------------------------------------------------------------*/
bool CPic::CommandClose2 ( unsigned char tempDoor, unsigned int tempKeyLock, unsigned int tempKeyToken, unsigned int tempTime )
{
    unsigned char tempCommand [ ] = { BYTE_STX, 0x11, COMMAND_CLOSE2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    unsigned char tempAnsBuffer [ 20 ];
    bool tempStatus;    
    std::string tempString;
    char tempText [ 100 ];
    unsigned char tempBuffer [ 10 ];
    unsigned char tempToken [ 10 ];
    unsigned int tempRandom;    
    unsigned int tempNewKey;
    unsigned char tempBuffKey [ 4 ];    

    AddLog ( "CommandClose2", LEVEL_LOG_4 ); 

    // Set block
    tempCommand [ 3 ] = tempDoor;    

    // Generate token
    GenerateToken ( tempTime, tempKeyToken, tempToken, &tempRandom );

    // Copy token
    memcpy ( tempCommand + 8, tempToken, 10 );      

    // Calc new key random
    tempNewKey = tempKeyLock + tempRandom;    

    // Separate bytes offset 0 is lowest byte
    tempBuffKey [ 3 ] = tempNewKey & 0xFF;
    tempBuffKey [ 2 ] = ( tempNewKey >> 8 ) & 0xFF;
    tempBuffKey [ 1 ] = ( tempNewKey >> 16 ) & 0xFF;
    tempBuffKey [ 0 ] = ( tempNewKey >> 24 ) & 0xFF;   

    memcpy ( tempCommand + 4, tempBuffKey, 4 );

    // Send command
    tempStatus = SendCommand ( tempCommand, tempAnsBuffer );

    // Check if was correct
    if ( tempStatus != true )
    {
        // Communication error
        flagCommunication = false;

        AddLog ( "Error comunicacion CommandClose2.", LEVEL_LOG_1 );

        return false;
    }

    // Check Ans
    if ( tempAnsBuffer [ 3 ] != ANS_OK )
    {
        if ( tempAnsBuffer [ 3 ] == ANS_ERROR_COMMAND )
        {
            AddLog ( "Comando no reconocido posible tarjeta V1.0", LEVEL_LOG_1 );
            printf ( "Comando no reconocido posible tarjeta V1.0\n" );            
            return true;
        }

        // Check kind error
        if ( tempAnsBuffer [ 3 ] == ANS_ERROR_TOKEN )
        {
            AddLog ( "Error token", LEVEL_LOG_1 );
            printf ( "Error token\n" );
        }

        // Check kind error
        if ( tempAnsBuffer [ 3 ] == ANS_ERROR_TIME )
        {
            AddLog ( "Error tiempo token", LEVEL_LOG_1 );
            printf ( "Error tiempo token\n" );
        }

        sprintf ( tempText, "Codigo error:%02X", tempAnsBuffer [ 3 ] );
        tempString = tempText;

        AddLog ( tempString, LEVEL_LOG_1 );        
        
        return false;
    }       

    return true;
}

/*------------------------------------------------------------------------------------------------
 * \fn bool CPic::CommandGetCashBox ( unsigned char * tempValue )
 * \brief Get cashbox sensor
 * \param tempValue: Ponter where left value
 * \return true: success
 * \return false: Error
 *
 * \pre
 * \post
 *
 * description:
------------------------------------------------------------------------------------------------*/
bool CPic::CommandGetCashBox ( unsigned char * tempValue )
{
    unsigned char tempCommand [ ] = { BYTE_STX, 0x02, COMMAND_CASHBOX, 0x00 };
    unsigned char tempAnsBuffer [ 20 ];
    bool tempStatus;    
    std::string tempString;
    char tempText [ 100 ];
    unsigned char tempBuffer [ 10 ];
    unsigned char tempToken [ 10 ];
    unsigned int tempRandom;
    unsigned int tempTime;
    unsigned char tempBuffKey [ 4 ];

    AddLog ( "CommandGetCashBox", LEVEL_LOG_4 ); 

    // Send command
    tempStatus = SendCommand ( tempCommand, tempAnsBuffer );

    // Check if was correct
    if ( tempStatus != true )
    {
        // Communication error
        flagCommunication = false;

        AddLog ( "Error comunicacion CommandGetCashBox.", LEVEL_LOG_1 );

        return false;
    }

    // Check Ans
    if ( tempAnsBuffer [ 3 ] != ANS_OK )
    {
        if ( tempAnsBuffer [ 3 ] == ANS_ERROR_COMMAND )
        {
            AddLog ( "Comando no reconocido posible tarjeta V1.0", LEVEL_LOG_1 );
            printf ( "Comando no reconocido posible tarjeta V1.0\n" );            
            return true;
        }     

        sprintf ( tempText, "Codigo error:%02X", tempAnsBuffer [ 3 ] );
        tempString = tempText;

        AddLog ( tempString, LEVEL_LOG_1 );        
        
        return false;
    }    

    *tempValue = tempAnsBuffer [ 4 ];

    return true;
}

/*------------------------------------------------------------------------------------------------
 * \fn bool CPic::CommandGetSensor ( unsigned char tempDoor, unsigned char * tempValue )
 * \brief Get door sensor status 
 * \param tempDoor: Number door
 * \param tempValue: Ponter where left value
 * \return true: success
 * \return false: Error
 *
 * \pre
 * \post
 *
 * description:
------------------------------------------------------------------------------------------------*/
bool CPic::CommandGetSensor ( unsigned char tempDoor, unsigned char * tempValue )
{
    unsigned char tempCommand [ ] = { BYTE_STX, 0x03, COMMAND_GETSENSOR, 0x00, 0x00 };
    unsigned char tempAnsBuffer [ 20 ];
    bool tempStatus;    
    std::string tempString;
    char tempText [ 100 ];
    unsigned char tempBuffer [ 10 ];
    unsigned char tempToken [ 10 ];
    unsigned int tempRandom;
    unsigned int tempTime;
    unsigned char tempBuffKey [ 4 ];

    AddLog ( "CommandGetSensor", LEVEL_LOG_4 ); 

    // Set block
    tempCommand [ 3 ] = tempDoor;  

    // Send command
    tempStatus = SendCommand ( tempCommand, tempAnsBuffer );

    // Check if was correct
    if ( tempStatus != true )
    {
        // Communication error
        flagCommunication = false;

        AddLog ( "Error comunicacion CommandGetSensor.", LEVEL_LOG_1 );

        return false;
    }

    // Check Ans
    if ( tempAnsBuffer [ 3 ] != ANS_OK )
    {
        if ( tempAnsBuffer [ 3 ] == ANS_ERROR_COMMAND )
        {
            AddLog ( "Comando no reconocido posible tarjeta V1.0", LEVEL_LOG_1 );
            printf ( "Comando no reconocido posible tarjeta V1.0\n" );            
            return true;
        }     

        sprintf ( tempText, "Codigo error:%02X", tempAnsBuffer [ 3 ] );
        tempString = tempText;

        AddLog ( tempString, LEVEL_LOG_1 );        
        
        return false;
    }    

    *tempValue = tempAnsBuffer [ 4 ];

    return true;
}


/*------------------------------------------------------------------------------------------------
 * \fn bool CPic::CommandInitBlock ( unsigned char tempBlock, unsigned char tempAccess, unsigned int tempNewKey )
 * \brief Initialize block
 * \param tempBlock: Number block key
 * \param tempAccess: Condition access
 * \param tempNewKey: New key
 * \return true: success
 * \return false: Error
 *
 * \pre
 * \post
 *
 * description:
------------------------------------------------------------------------------------------------*/
bool CPic::CommandInitBlock ( unsigned char tempBlock, unsigned char tempAccess, unsigned int tempNewKey )
{
    unsigned char tempCommand [ ] = { BYTE_STX, 0x08, COMMAND_BLOCK_FORMAT, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    unsigned char tempAnsBuffer [ 20 ];
    bool tempStatus;    
    std::string tempString;
    char tempText [ 100 ];
    unsigned char tempBuffer [ 10 ];
    unsigned char tempToken [ 10 ];
    unsigned int tempRandom;
    unsigned int tempTime;
    unsigned char tempBuffKey [ 4 ];

    AddLog ( "CommandInitBlock", LEVEL_LOG_4 ); 

    // Set block
    tempCommand [ 3 ] = tempBlock;

    // Set access condition
    tempCommand [ 4 ] = tempAccess;

    // Separate bytes offset 0 is lowest byte
    tempBuffKey [ 3 ] = tempNewKey & 0xFF;
    tempBuffKey [ 2 ] = ( tempNewKey >> 8 ) & 0xFF;
    tempBuffKey [ 1 ] = ( tempNewKey >> 16 ) & 0xFF;
    tempBuffKey [ 0 ] = ( tempNewKey >> 24 ) & 0xFF;   

    memcpy ( tempCommand + 5, tempBuffKey, 4 );    

    // Send command
    tempStatus = SendCommand ( tempCommand, tempAnsBuffer );

    // Check if was correct
    if ( tempStatus != true )
    {
        // Communication error
        flagCommunication = false;

        AddLog ( "Error comunicacion CommandInitBlock.", LEVEL_LOG_1 );

        return false;
    }

    // Check Ans
    if ( tempAnsBuffer [ 3 ] != ANS_OK )
    {
        if ( tempAnsBuffer [ 3 ] == ANS_ERROR_COMMAND )
        {
            AddLog ( "Comando no reconocido posible tarjeta V1.0", LEVEL_LOG_1 );
            printf ( "Comando no reconocido posible tarjeta V1.0\n" );            
            return true;
        }     

        sprintf ( tempText, "Codigo error:%02X", tempAnsBuffer [ 3 ] );
        tempString = tempText;

        AddLog ( tempString, LEVEL_LOG_1 );        
        
        return false;
    }       

    return true;
}

/*------------------------------------------------------------------------------------------------
 * \fn bool CPic::CommandInitBlock2 ( unsigned char tempBlock, unsigned char tempAccess, unsigned int tempKeyBlock, unsigned int tempKeyToken, unsigned int tempTime )
 * \brief Initialize block version 2
 * \param tempBlock: Number block key
 * \param tempAccess: Condition access
 * \param tempNewKey: key check operation block
 * \param tempKeyToken: Key token validation
 * \param tempTime time token
 * \return true: success
 * \return false: Error
 *
 * \pre
 * \post
 *
 * description:
------------------------------------------------------------------------------------------------*/
bool CPic::CommandInitBlock2 ( unsigned char tempBlock, unsigned char tempAccess, unsigned int tempKeyBlock, unsigned int tempKeyToken, unsigned int tempTime )
{
    unsigned char tempCommand [ ] = { BYTE_STX, 0x12, COMMAND_BLOCK_FORMAT2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    unsigned char tempAnsBuffer [ 20 ];
    bool tempStatus;    
    std::string tempString;
    char tempText [ 100 ];
    unsigned char tempBuffer [ 10 ];
    unsigned char tempToken [ 10 ];
    unsigned int tempRandom;    
    unsigned char tempBuffKey [ 4 ];

    AddLog ( "CommandInitBlock2", LEVEL_LOG_4 ); 

    // Set block
    tempCommand [ 3 ] = tempBlock;

    // Set access condition
    tempCommand [ 4 ] = tempAccess;

    // Generate token
    GenerateToken ( tempTime, tempKeyToken, tempToken, &tempRandom );

    // Random key
    tempKeyBlock += tempRandom;

    // Separate bytes offset 0 is lowest byte
    tempBuffKey [ 3 ] = tempKeyBlock & 0xFF;
    tempBuffKey [ 2 ] = ( tempKeyBlock >> 8 ) & 0xFF;
    tempBuffKey [ 1 ] = ( tempKeyBlock >> 16 ) & 0xFF;
    tempBuffKey [ 0 ] = ( tempKeyBlock >> 24 ) & 0xFF;   

    memcpy ( tempCommand + 5, tempBuffKey, 4 );   
    memcpy ( tempCommand + 9,tempToken, 10 ); 

    // Send command
    tempStatus = SendCommand ( tempCommand, tempAnsBuffer );

    // Check if was correct
    if ( tempStatus != true )
    {
        // Communication error
        flagCommunication = false;

        AddLog ( "Error comunicacion CommandInitBlock2.", LEVEL_LOG_1 );

        return false;
    }

    // Check Ans
    if ( tempAnsBuffer [ 3 ] != ANS_OK )
    {
        if ( tempAnsBuffer [ 3 ] == ANS_ERROR_COMMAND )
        {
            AddLog ( "Comando no reconocido posible tarjeta V1.0", LEVEL_LOG_1 );
            printf ( "Comando no reconocido posible tarjeta V1.0\n" );            
            return true;
        }     

        sprintf ( tempText, "Codigo error:%02X", tempAnsBuffer [ 3 ] );
        tempString = tempText;

        AddLog ( tempString, LEVEL_LOG_1 );        
        
        return false;
    }       

    return true;
}

/*------------------------------------------------------------------------------------------------
 * \fn bool CPic::CommandOpen ( unsigned char tempDoor, unsigned int tempKeyLock, unsigned int tempKeyToken, unsigned int tempTime )
 * \brief open door
 * \param tempDoor: Number door
 * \param tempToken: toke to open 
 * \return true: success
 * \return false: Error
 *
 * \pre
 * \post
 *
 * description:
------------------------------------------------------------------------------------------------*/
bool CPic::CommandOpen2 ( unsigned char tempDoor, unsigned int tempKeyLock, unsigned int tempKeyToken, unsigned int tempTime )
{
    unsigned char tempCommand [ ] = { BYTE_STX, 0x11, COMMAND_OPEN2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    unsigned char tempAnsBuffer [ 20 ];
    bool tempStatus;    
    std::string tempString;
    char tempText [ 100 ];
    unsigned char tempBuffer [ 10 ];
    unsigned char tempToken [ 10 ];
    unsigned int tempRandom;    
    unsigned int tempNewKey;
    unsigned char tempBuffKey [ 4 ];    

    AddLog ( "CommandOpen2", LEVEL_LOG_4 ); 

    // Set block
    tempCommand [ 3 ] = tempDoor;    
    
     sprintf ( tempText, "Abrtir llavepuerta: %08X\n", tempKeyLock );
    tempString = tempText;
    AddLog ( tempString, LEVEL_LOG_1 );    

    // Generate token
    GenerateToken ( tempTime, tempKeyToken, tempToken, &tempRandom );

    // Copy token
    memcpy ( tempCommand + 8, tempToken, 10 );      

    // Calc new key random
    tempNewKey = tempKeyLock + tempRandom;    

    // Separate bytes offset 0 is lowest byte
    tempBuffKey [ 3 ] = tempNewKey & 0xFF;
    tempBuffKey [ 2 ] = ( tempNewKey >> 8 ) & 0xFF;
    tempBuffKey [ 1 ] = ( tempNewKey >> 16 ) & 0xFF;
    tempBuffKey [ 0 ] = ( tempNewKey >> 24 ) & 0xFF;   

    memcpy ( tempCommand + 4, tempBuffKey, 4 );

    // Send command
    tempStatus = SendCommand ( tempCommand, tempAnsBuffer );

    // Check if was correct
    if ( tempStatus != true )
    {
        // Communication error
        flagCommunication = false;

        AddLog ( "Error comunicacion CommandOpen2.", LEVEL_LOG_1 );

        return false;
    }

    // Check Ans
    if ( tempAnsBuffer [ 3 ] != ANS_OK )
    {
        if ( tempAnsBuffer [ 3 ] == ANS_ERROR_COMMAND )
        {
            AddLog ( "Comando no reconocido posible tarjeta V1.0", LEVEL_LOG_1 );
            printf ( "Comando no reconocido posible tarjeta V1.0\n" );            
            return true;
        }

        // Check kind error
        if ( tempAnsBuffer [ 3 ] == ANS_ERROR_TOKEN )
        {
            AddLog ( "Error token", LEVEL_LOG_1 );
            printf ( "Error token\n" );
        }

        // Check kind error
        if ( tempAnsBuffer [ 3 ] == ANS_ERROR_TIME )
        {
            AddLog ( "Error tiempo token", LEVEL_LOG_1 );
            printf ( "Error tiempo token\n" );
        }

        sprintf ( tempText, "Codigo error:%02X", tempAnsBuffer [ 3 ] );
        tempString = tempText;

        AddLog ( tempString, LEVEL_LOG_1 );        
        
        return false;
    }       

    return true;
}

/*------------------------------------------------------------------------------------------------
 * \fn bool CPic::CommandPoll ( unsigned char * buffer )
 * \brief Get status general from device
 * \param buffer: Pointer where leave datas
 * \return true: Status get
 * \return false: Error
 *
 * \pre
 * \post
 *
 * description:
 *
------------------------------------------------------------------------------------------------*/
bool CPic::CommandPoll ( unsigned char * tempAns  )
{
    unsigned char tempCommand [ ] = { BYTE_STX, 0x02, COMMAND_POLL, 0x00 };
    unsigned char tempAnsBuffer [ 20 ];
    bool tempStatus;    
    std::string tempString;
    char tempText [ 250 ];

    AddLog ( "CommandPoll", LEVEL_LOG_4 );

    // Send command
    tempStatus = SendCommand ( tempCommand, tempAnsBuffer );

    // Check if was correct
    if ( tempStatus != true )
    {
        // Communication error
        flagCommunication = false;

        AddLog ( "Error comunicacion CommandPoll.", LEVEL_LOG_1 );

        return false;
    }

    // Check Ans
    if ( tempAnsBuffer [ 3 ] != ANS_OK )
    {
        if ( tempAnsBuffer [ 3 ] == ANS_ERROR_COMMAND )
        {
            AddLog ( "Comando no reconocido posible tarjeta V1.0", LEVEL_LOG_1 );
            printf ( "Comando no reconocido posible tarjeta V1.0\n" );
            tempAns [ 0 ] = 0x00;
            tempAns [ 1 ] = 0x00;
            return true;
        }

        AddLog ( "Error Poll", LEVEL_LOG_4 );
        printf ( "Error Poll\n" );
        return false;
    } 

    // Copy data
    tempAns [ 0 ] = tempAnsBuffer [ 0 ];
    tempAns [ 1 ] = tempAnsBuffer [ 1 ];

    // Communication works
    flagCommunication = true;    

    return true;
}

/*------------------------------------------------------------------------------------------------
 * \fn bool CPic::CommandSetToken ( unsigned long tempTime )
 * \brief Set new token time
 * \param tempTime: time new token
 * \return true: success
 * \return false: Error
 *
 * \pre
 * \post
 *
 * description:
------------------------------------------------------------------------------------------------*/
bool CPic::CommandSetToken ( unsigned int tempTime )
{
    unsigned char tempCommand [ ] = { BYTE_STX, 0x0C, COMMAND_SETTOKEN, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                                    0x00, 0x00, 0x00 };
    unsigned char tempAnsBuffer [ 20 ];
    bool tempStatus;    
    std::string tempString;
    char tempText [ 100 ];
    unsigned char tempBuffer [ 10 ];
    unsigned char tempToken [ 10 ];
    unsigned int tempRandom;

    AddLog ( "CommandSetToken", LEVEL_LOG_4 ); 

    // Generate token
    GenerateToken ( tempTime, mainKey, tempToken, &tempRandom );

    // Copy token
    memcpy ( tempCommand + 3, tempToken, 10 );

    // Send command
    tempStatus = SendCommand ( tempCommand, tempAnsBuffer );

    // Check if was correct
    if ( tempStatus != true )
    {
        // Communication error
        flagCommunication = false;

        AddLog ( "Error comunicacion CommandSetToken.", LEVEL_LOG_1 );

        return false;
    }

    // Check Ans
    if ( tempAnsBuffer [ 3 ] != ANS_OK )
    {
        if ( tempAnsBuffer [ 3 ] == ANS_ERROR_COMMAND )
        {
            AddLog ( "Comando no reconocido posible tarjeta V1.0", LEVEL_LOG_1 );
            printf ( "Comando no reconocido posible tarjeta V1.0\n" );            
            return true;
        }

        // Check kind error
        if ( tempAnsBuffer [ 3 ] == ANS_ERROR_TOKEN )
        {
            AddLog ( "Error token", LEVEL_LOG_1 );
            printf ( "Error token\n" );
        }

        // Check kind error
        if ( tempAnsBuffer [ 3 ] == ANS_ERROR_TIME )
        {
            AddLog ( "Error tiempo token", LEVEL_LOG_1 );
            printf ( "Error tiempo token\n" );
        }

        sprintf ( tempText, "Codigo error:%02X", tempAnsBuffer [ 3 ] );
        tempString = tempText;

        AddLog ( tempString, LEVEL_LOG_1 );        
        
        return false;
    }       

    return true;
}

/*------------------------------------------------------------------------------------------------
 * \fn bool CPic::CommandVersion ( std::string * tempVersion )
 * \brief Get Ivizion version firmware
 * \param tempVersion: Pointer to save version name
 * \return true: Got version
 * \return false: Error
 *
 * \pre
 * \post
 *
 * description:
------------------------------------------------------------------------------------------------*/
bool CPic::CommandVersion ( std::string * tempVersion )
{
    unsigned char tempCommand [ ] = { BYTE_STX, 0x02, COMMAND_VERSION, 0x00 };
    unsigned char tempAnsBuffer [ 20 ];
    bool tempStatus;    
    std::string tempString;
    char tempText [ 100 ];

    AddLog ( "CommandVersion", LEVEL_LOG_4 );

    // Send command
    tempStatus = SendCommand ( tempCommand, tempAnsBuffer );

    // Check if was correct
    if ( tempStatus != true )
    {
        // Communication error
        flagCommunication = false;

        AddLog ( "Error comunicacion CommandVersion.", LEVEL_LOG_1 );

        return false;
    }

    // Check Ans
    if ( tempAnsBuffer [ 3 ] != ANS_OK )
    {
        if ( tempAnsBuffer [ 3 ] == ANS_ERROR_COMMAND )
        {
            AddLog ( "Comando no reconocido posible tarjeta V1.0", LEVEL_LOG_1 );
            printf ( "Comando no reconocido posible tarjeta V1.0\n" );
            *tempVersion = "1.0";
            return true;
        }

        AddLog ( "Error Version", LEVEL_LOG_4 );
        printf ( "Error Version\n" );
        return false;
    }    

    sprintf ( tempText, "%d.%d", tempAnsBuffer [ 4 ], tempAnsBuffer [ 5 ] );
    tempString = tempText;
    *tempVersion = tempString;        

    AddLog ( "Version:" + tempString, LEVEL_LOG_1 );

    return true;
}



/*------------------------------------------------------------------------------------------------
 * \fn bool CPic::CommandReadEEProm ( unsigned char tempAddress, unsigned char tempSize, unsigned char *tempData )
 * \brief Read a byte in EEPROM
 * \param tempAddress Address to read
 * \param tempSize Bytes to read
 * \param tempData buffer to left data
 * \return true: 
 * \return false: Error
 *
 * \pre
 * \post
 *
 * description:
------------------------------------------------------------------------------------------------*/
bool CPic::CommandReadEEProm ( unsigned char tempAddress, unsigned char tempSize, unsigned char *tempData )
{
    unsigned char tempCommand [ ] = { BYTE_STX, 0x04, COMMAND_EEPROM_READ, 0x00, 0x00, 0x00 };
    unsigned char tempAnsBuffer [ 300 ];
    bool tempStatus;    
    std::string tempString;
    char tempText [ 100 ];

    AddLog ( "CommandReadEEProm", LEVEL_LOG_4 );

    tempCommand [ 3 ] = tempAddress;
    tempCommand [ 4 ] = tempSize;

    // Send command
    tempStatus = SendCommand ( tempCommand, tempAnsBuffer );

    // Check if was correct
    if ( tempStatus != true )
    {
        // Communication error
        flagCommunication = false;

        AddLog ( "Error comunicacion CommandReadEEProm.", LEVEL_LOG_1 );

        return false;
    }

    // Check Ans
    if ( tempAnsBuffer [ 3 ] != ANS_OK )
    {        
        AddLog ( "Error leyendo en EEPROM", LEVEL_LOG_4 );
        printf ( "Error leyendo en EEPROM\n" );
        return false;
    }    

    memcpy ( tempData, tempAnsBuffer + 4, tempSize );    

    return true;
}

/*------------------------------------------------------------------------------------------------
 * \fn bool CPic::CommandWriteEEProm ( unsigned char tempAddress, unsigned char tempValue )
 * \brief Write a byte in EEPROM
 * \param tempAddress Address to write
 * \param tempValue Byte to write
 * \return true: Got version
 * \return false: Error
 *
 * \pre
 * \post
 *
 * description:
------------------------------------------------------------------------------------------------*/
bool CPic::CommandWriteEEProm ( unsigned char tempAddress, unsigned char tempValue )
{
    unsigned char tempCommand [ ] = { BYTE_STX, 0x04, COMMAND_EEPROM_WRITE, 0x00, 0x00, 0x00 };
    unsigned char tempAnsBuffer [ 20 ];
    bool tempStatus;    
    std::string tempString;
    char tempText [ 100 ];

    AddLog ( "CommandWriteEEProm", LEVEL_LOG_4 );

    tempCommand [ 3 ] = tempAddress;
    tempCommand [ 4 ] = tempValue;

    // Send command
    tempStatus = SendCommand ( tempCommand, tempAnsBuffer );

    // Check if was correct
    if ( tempStatus != true )
    {
        // Communication error
        flagCommunication = false;

        AddLog ( "Error comunicacion CommandWriteEEProm.", LEVEL_LOG_1 );

        return false;
    }

    // Check Ans
    if ( tempAnsBuffer [ 3 ] != ANS_OK )
    {        
        AddLog ( "Error escribiendo en EEPROM", LEVEL_LOG_4 );
        printf ( "Error escribiendo en EEPROM\n" );
        return false;
    }    

    sprintf ( tempText, "Escrito en %02X=%02X", tempAddress, tempValue );
    tempString = tempText;    

    AddLog ( "Version:" + tempString, LEVEL_LOG_1 );

    return true;
}

/*------------------------------------------------------------------------------------------------
 * \fn std::string CPic::HexToText ( unsigned char * tempBuffer, unsigned tempSize )
 * \brief Convert to text an array bytes
 * \param tempBuffer: Pointer to array
 * \param tempSize: length bytes
 * \return Text
 *
 * \pre
 * \post
 *
 * description:
------------------------------------------------------------------------------------------------*/
std::string CPic::HexToText ( unsigned char * tempBuffer, unsigned int tempSize )
{
    int pos;
    std::string tempVal;
    std::string tempLine;
    char tempNum [ 4 ];

    // Cycle for convert every bytes
    tempLine = "";
    for ( pos = 0; pos < tempSize; pos ++ )
    {
        // Format each data
        sprintf ( tempNum, "%02X", tempBuffer [ pos ] );

        // Convert to string
        tempVal = tempNum;

        // Add to line
        tempLine += " " + tempVal;
    }

    return tempLine;
}

/*------------------------------------------------------------------------------------------------
 * \fn std::string CPic::DecodeSensor ( unsigned char tempValue, unsigned char tempMode, std::string * tempString )
 * \brief Convert to text sensor status
 * \param tempBuffer: Pointer to array
 * \param tempSize: length bytes
 * \return Text
 *
 * \pre
 * \post
 *
 * description:
------------------------------------------------------------------------------------------------*/
bool CPic::DecodeSensor ( unsigned char tempValue, unsigned char tempMode, std::string tempSeparator, std::string * tempString )
{
    int pos;
    std::string tempVal;
    std::string tempLine;

    tempLine = ""; 
    
    // Check mode cashbox
    if ( tempMode == SENSOR_CASHBOX )
    {
        // SD
        if ( ( tempValue & 0x01 ) == 0x01 )
        {
            tempLine += "Bolsa no detectada" + tempSeparator;
        }
        else
        {
            tempLine += "                    Bolsa instalada" + tempSeparator;
        }

        *tempString = tempLine;

        return true;
    }



    // Check mode solenoide
    if ( tempMode == SENSOR_SOLENOIDE )
    {
        // SD
        if ( ( tempValue & 0x01 ) == 0x01 )
        {
            tempLine += "Puerta abierta" + tempSeparator;
        }
        else
        {
            tempLine += "Puerta cerrada" + tempSeparator;
        }

        *tempString = tempLine;

        return true;
    }

    // Mode lineal
    if ( tempMode == SENSOR_BLACK )
    {
        // SD
        if ( ( tempValue & 0x01 ) == 0x01 )
        {
            tempLine += "Puerta abierta" + tempSeparator;
        }
        else
        {
            tempLine += "Puerta cerrada" + tempSeparator;
        }

        // SL
        if ( ( tempValue & 0x02 ) == 0x02 )
        {
            tempLine += "Bloqueada puerta" + tempSeparator;
        }
        else
        {
            tempLine += "Desbloqueada Puerta" + tempSeparator;
        }

        // LO
        if ( ( tempValue & 0x30 ) == 0x20 )
        {
            tempLine += "Pestillo fuera" + tempSeparator;
        }
        else
        {                   
            // LI
            if ( ( tempValue & 0x30 ) == 0x10 )
            {
                tempLine += "Pestillo guardado" + tempSeparator;
            }
            else
            {
                tempLine += "Pestillo sin confirmar" + tempSeparator;
            }
        }        
    }    

    *tempString = tempLine;

    return true;
}

/*------------------------------------------------------------------------------------------------
 * \fn bool CPic::InitializeEEprom ( void )
 * \brief Initialize EEPROM 
 * \param None
 * \return true: Star values mapping memory
 * \return false: Error
 *
 * \pre
 * \post
 *
 * description:
------------------------------------------------------------------------------------------------*/
bool CPic::InitializeEEprom ( void )
{
    bool tempStatus;

    // Clear block 0
    tempStatus = CommandWriteEEProm ( 0x00, 0xFF );

    if ( tempStatus != true )
    {
        AddLog ( "Error iniciando EEPROM", LEVEL_LOG_1 );
        return false;
    }

    tempStatus = CommandWriteEEProm ( 0x01, 0xFF );

    if ( tempStatus != true )
    {
        AddLog ( "Error iniciando EEPROM", LEVEL_LOG_1 );
        return false;
    }

    tempStatus = CommandWriteEEProm ( 0x02, 0xFF );

    if ( tempStatus != true )
    {
        AddLog ( "Error iniciando EEPROM", LEVEL_LOG_1 );
        return false;
    }

    tempStatus = CommandWriteEEProm ( 0x03, 0xFF );

    if ( tempStatus != true )
    {
        AddLog ( "Error iniciando EEPROM", LEVEL_LOG_1 );
        return false;
    }            

    tempStatus = CommandWriteEEProm ( 0x04, 0xFF );

    if ( tempStatus != true )
    {
        AddLog ( "Error iniciando EEPROM", LEVEL_LOG_1 );
        return false;
    }

    // Clear token
    tempStatus = CommandWriteEEProm ( 0x20, 0xFF );

    if ( tempStatus != true )
    {
        AddLog ( "Error iniciando EEPROM", LEVEL_LOG_1 );
        return false;
    }
    
    tempStatus = CommandWriteEEProm ( 0x21, 0xFF );
    if ( tempStatus != true )
    {
        AddLog ( "Error iniciando EEPROM", LEVEL_LOG_1 );
        return false;
    }
    
    tempStatus = CommandWriteEEProm ( 0x22, 0xFF );
    if ( tempStatus != true )
    {
        AddLog ( "Error iniciando EEPROM", LEVEL_LOG_1 );
        return false;
    }

    tempStatus = CommandWriteEEProm ( 0x23, 0xFF );
    if ( tempStatus != true )
    {
        AddLog ( "Error iniciando EEPROM", LEVEL_LOG_1 );
        return false;
    }

    tempStatus = CommandWriteEEProm ( 0x24, 0xFF );
    if ( tempStatus != true )
    {
        AddLog ( "Error iniciando EEPROM", LEVEL_LOG_1 );
        return false;
    }

    return true;
}

/*------------------------------------------------------------------------------------------------*/
/*
 * \fn bool CPic::OpenPort ( void )
 * \brief Open serial port
 * \param None
 * \return true: port opened
 * \return false: Error opening port
 *
 * \pre
 * \post
 *
 * DESCRIPCION:
 */
/*------------------------------------------------------------------------------------------------*/
bool CPic::OpenPort ( void )
{
    CFile log;
    std::string textLog;
    struct termios options;
    bool tempStatus;
    char tempText [ 250 ];    

    sprintf ( tempText, "OpenPort: %s",namePort.c_str ( ) );
    textLog = tempText;
    AddLog ( textLog, LEVEL_LOG_1 );

    // Check if port is opened
    if ( idPort > 0 )
    {
        close ( idPort );
    }

    // Open port
    idPort = open ( namePort.c_str ( ), O_RDWR | O_NOCTTY | O_NONBLOCK | O_NDELAY );

    // Check if can do it
    if ( idPort == -1 )
    {    	
        // register error
        textLog = "Error Abriendo el puerto: " + namePort;
        AddLog ( textLog, LEVEL_LOG_1 );    

        errorPort = true;

        return false;
    }    

    AddLog ( "Puerto abierto.", LEVEL_LOG_1 );

    // Clean structure
    memset ( ( void * ) ( &options ) ,0 , sizeof ( options ) );

    // 8 bits data
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    // Set speed
    options.c_cflag |= speed;


    // Even parity
    options.c_cflag |= PARENB;
    options.c_cflag &= ~PARODD;

    // Ignore errors
   // options.c_iflag = IGNPAR;

    // without parity
    //options.c_cflag &= ~PARENB;

    // 1 bit stop
    options.c_cflag &= ~CSTOPB;

    // Local mode and read datas
    options.c_cflag |= CLOCAL | CREAD;

    // Without least byte to receive
    options.c_cc [ VMIN ] = 0;

    options.c_lflag = 0;

    // Time out to receive data in 0.1 second
    options.c_cc [ VTIME ] = 50;

    // Set params to port
    tcsetattr ( idPort, TCSANOW, &options );    

    errorPort = false;    

    AddLog ( "Iniciado.", LEVEL_LOG_1 );

    return true;
}

/*------------------------------------------------------------------------------------------------
 * \fn void CPic::Run ( int x )
 * \brief Main process
 * \param x.
 * \return None.
 *
 * \pre
 * \post
 *
 * description:
 ------------------------------------------------------------------------------------------------*/
void CPic::Run ( int x )
{
    bool tempStatus;    
    std::string tempString;
    unsigned int tempNewKey;
    int pos;    
    char tempText [ 200 ];        
    bool tempError;
    unsigned char tempAnsBuffer [ 10 ];
    int tempOkCount;
    int tempErrorCount;
    unsigned char tempSensor;
    unsigned int tempTime;
    unsigned int tempOkCountOpen; 
    unsigned int tempErrorCountOpen;
    unsigned int tempOkCountClose;
    unsigned int tempErrorCountClose;
    unsigned int tempKeyLock;

    
    // Initialize status
    presentStatus = STATUS_UNINITIALIZE;
    nextStatus = STATUS_INITIALIZE;
    previousStatus = STATUS_UNINITIALIZE;

    AddLog ( "Inicio hilo", LEVEL_LOG_1 );    

    printf ( "Inicio hilo pic\n");

    printf ( "puerto:%s\n", namePort.c_str ( ) );

    initialize = false;    

    // Main cycle
	flagMainExit = false;    

	while ( flagMainExit == false )
	{                
		// Unlock to change status
        mutexBusy.unlock();        

		// Rest processor
		usleep ( 200000 ); // 200000        

		// Lock to prevent to change status
        mutexBusy.lock ( );               
		
        //AddLog ( "Presente:" + presentStatus + " futuro:" + nextStatus, LEVEL_LOG_4 );


        //AddLog ( "GAPB Presente:" + presentStatus + " futuro:" + nextStatus, LEVEL_LOG_1 );        

		// Check before initalized device
		if ( initialize == true )
		{
            usleep ( 500000 );

            // send command poll
            tempStatus = CommandPoll ( tempAnsBuffer );

            // Check command
            if ( tempStatus != true )
            {
                printf ( "Error poll\n");
                AddLog ( "Error Poll", LEVEL_LOG_1 );                
            }			
		}

        // 
        if ( flagCommunication == false )
        {
            
        }

		// Check change to initialize status
		if ( nextStatus.compare ( STATUS_INITIALIZE ) == 0 )
		{
            printf ( "Perparando incializar\n" );
			AddLog ( "Cambiando a inicializar", LEVEL_LOG_4 );                        
        
			// Open port
			tempStatus = OpenPort ( );

			// Check if port was opened
			if ( tempStatus == false )
			{
				AddLog ( "Error reiniciando", LEVEL_LOG_1 );
                printf ( "Error abriendo Puerto\n" );
                usleep ( 1000000 );

				continue;
			}

            printf ( "Puerto abierto\n" );

            versionDevice = "";

            // Get version
            tempStatus = CommandVersion ( &versionDevice );

            if ( tempStatus != true )
            {
                printf ( "Error obteniendo version\n" );

                continue;
            }
            else
            {
                printf ( "Version:%s\n", versionDevice.c_str ( ) ); 
            }            

			// Register device initialized
			initialize = true;   

            // Test Buzzer
            tempStatus = CommandBuzzer ( 200 );

            usleep ( 2000000 );

            // Initalize EEPROM
            tempStatus = InitializeEEprom ( );        

            if ( tempStatus != true )
            {

                continue;
            }

			// Set new status
			previousStatus = presentStatus;
			presentStatus = nextStatus;
			nextStatus = STATUS_DEBUG6;
            nextStep = STATUS_STEP1;

			// Finish restart
			flagRestarting = false;

			AddLog ( "iniciado", LEVEL_LOG_4 );

			continue;
		}		

		// Check change to enabled status
		if (  ( nextStatus.compare ( STATUS_DEBUG1 ) == 0 ) )
		{			
			// Set new status
			previousStatus = presentStatus;
			presentStatus = nextStatus;		

            // Initialize test
            if ( ( nextStep.compare ( STATUS_STEP1 ) == 0 ) )
            {
                tempOkCount = 0;
                tempErrorCount = 0;
                nextStep = STATUS_STEP2;

                // Calculate time
                tempTime = CalcTimeToken ( );
            }

            // Test
            if (  ( nextStep.compare ( STATUS_STEP2 ) == 0 ) )
            {
                // Calculate time
                //tempTime = CalcTimeToken ( );

                // Generate token new to PIC
                tempStatus = CommandSetToken ( tempTime );

                if ( tempStatus == true )
                {
                    printf ( "Bien set token\n" );
                    tempOkCount ++;
                }
                else
                {
                    printf ( "Error set token\n" );
                    tempErrorCount ++;
                }

                tempTime += 3600;
            }    

            printf ( "Proceso set token bien= %d mal= %d\n", tempOkCount, tempErrorCount );   
            usleep ( 2000000 );

			continue;
		}

		// Check change status
		if (  ( nextStatus.compare ( STATUS_DEBUG2 ) == 0 ) )
		{			
			// Set new status
			previousStatus = presentStatus;
			presentStatus = nextStatus;		

            // Initialize test
            if (  ( nextStep.compare ( STATUS_STEP1 ) == 0 ) )
            {
                tempOkCount = 0;
                tempErrorCount = 0;                

                // Initialize key block 0
                tempStatus = CommandInitBlock ( 0, 0x73, mainKey );

                if ( tempStatus != true )
                {
                    printf ( "Error init block\n" );       

                    continue;
                }

                // Calculate time
                tempTime = CalcTimeToken ( );               

                // Generate token new to PIC
                tempStatus = CommandSetToken ( tempTime );

                if ( tempStatus == true )
                {
                    printf ( "Bien set token\n" );       

                    nextStep = STATUS_STEP2;
                }
                else
                {
                    printf ( "Error set token\n" );                    
                }

                usleep ( 2000000 );

                continue;
            }

            // Test
            if (  ( nextStep.compare ( STATUS_STEP2 ) == 0 ) )
            {                
                tempNewKey = mainKey + 1; 

                // Calculate time
                tempTime = CalcTimeToken ( );                         

                // Cambia llave bloque 0
                tempStatus = CommandChangeKey ( 0, mainKey, tempNewKey, tempTime );

                if ( tempStatus == true )
                {
                    printf ( "Bien nueva llave\n" );
                    tempOkCount ++;

                    mainKey = tempNewKey;
                }
                else
                {
                    printf ( "Error nueva llave\n" );
                    tempErrorCount ++;
                }                                
            }    

            printf ( "Proceso cambio llave bien= %d mal= %d\n", tempOkCount, tempErrorCount );   
            usleep ( 1000000 );

			continue;
		}       

        // Check change status
		if (  ( nextStatus.compare ( STATUS_DEBUG3 ) == 0 ) )
		{			
			// Set new status
			previousStatus = presentStatus;
			presentStatus = nextStatus;		

            // Initialize test
            if (  ( nextStep.compare ( STATUS_STEP1 ) == 0 ) )
            {
                tempOkCount = 0;
                tempErrorCount = 0;                

                // Initialize key block 0
                tempStatus = CommandInitBlock ( 0, 0x73, mainKey );

                if ( tempStatus != true )
                {
                    printf ( "Error init block\n" );       

                    continue;
                }

                // Calculate time
                tempTime = CalcTimeToken ( );               

                // Generate token new to PIC
                tempStatus = CommandSetToken ( tempTime );

                if ( tempStatus == true )
                {
                    printf ( "Bien set token\n" );       

                    nextStep = STATUS_STEP2;
                }
                else
                {
                    printf ( "Error set token\n" );                    
                }

                usleep ( 2000000 );

                continue;
            }

            // Test
            if ( ( nextStep.compare ( STATUS_STEP2 ) == 0 ) )
            {                                
                // Calculate time
                tempTime = CalcTimeToken ( );                         

                // abrir puerta 1
                tempKeyLock = 0x01020304;
                tempStatus = CommandOpen2 ( 1, tempKeyLock, mainKey, tempTime );

                if ( tempStatus == true )
                {
                    printf ( "Bien abierta\n" );
                    tempOkCount ++;
                }
                else
                {
                    printf ( "Error abriendo\n" );
                    tempErrorCount ++;
                }                                
            }    

            printf ( "Proceso apertura puerta 1-  bien= %d mal= %d\n", tempOkCount, tempErrorCount );   
            usleep ( 2000000 );

			continue;
		}   


        // Check change status
		if (  ( nextStatus.compare ( STATUS_DEBUG4 ) == 0 ) )
		{			
			// Set new status
			previousStatus = presentStatus;
			presentStatus = nextStatus;		

            // Initialize test
            if (  ( nextStep.compare ( STATUS_STEP1 ) == 0 ) )
            {
                tempOkCount = 0;
                tempErrorCount = 0;                

                // Initialize key block 0
                tempStatus = CommandInitBlock ( 0, 0x73, mainKey );

                if ( tempStatus != true )
                {
                    printf ( "Error init block\n" );       

                    continue;
                }

                // Calculate time
                tempTime = CalcTimeToken ( );               

                // Generate token new to PIC
                tempStatus = CommandSetToken ( tempTime );

                if ( tempStatus == true )
                {
                    printf ( "Bien set token\n" );       

                    nextStep = STATUS_STEP2;
                }
                else
                {
                    printf ( "Error set token\n" );                    
                }

                usleep ( 2000000 );

                continue;
            }

            // Test
            if ( ( nextStep.compare ( STATUS_STEP2 ) == 0 ) )
            {                                
                // Calculate time
                tempTime = CalcTimeToken ( );                         

                // abrir puerta 1
                tempKeyLock = 0x01020304;
                tempStatus = CommandClose2 ( 1, tempKeyLock, mainKey, tempTime );

                if ( tempStatus == true )
                {
                    printf ( "Bien cerrar\n" );
                    tempOkCount ++;
                }
                else
                {
                    printf ( "Error cerando\n" );
                    tempErrorCount ++;
                }                                
            }    

            printf ( "Proceso cerrar puerta 1-  bien= %d mal= %d\n", tempOkCount, tempErrorCount );   
            usleep ( 2000000 );

			continue;
		}   


        // Check change status
		if (  ( nextStatus.compare ( STATUS_DEBUG5 ) == 0 ) )
		{			
			// Set new status
			previousStatus = presentStatus;
			presentStatus = nextStatus;		

            // Initialize test
            if (  ( nextStep.compare ( STATUS_STEP1 ) == 0 ) )
            {
                tempOkCountClose = 0;
                tempErrorCountClose = 0;     
                tempOkCountOpen = 0;
                tempErrorCountOpen = 0;            

                // Initialize key block 0
                tempStatus = CommandInitBlock ( 0, 0x73, mainKey );

                if ( tempStatus != true )
                {
                    printf ( "Error init block 0\n" );       

                    continue;
                }

                // Initialize key block 0
                tempStatus = CommandInitBlock ( 1, 0x73, mainKey );

                if ( tempStatus != true )
                {
                    printf ( "Error init block 1\n" );       

                    continue;
                }

                // Initialize key block 0
                tempStatus = CommandInitBlock ( 2, 0x73, mainKey );

                if ( tempStatus != true )
                {
                    printf ( "Error init block 2\n" );       

                    continue;
                }

                // Calculate time
                tempTime = CalcTimeToken ( );               

                // Generate token new to PIC
                tempStatus = CommandSetToken ( tempTime );

                if ( tempStatus == true )
                {
                    printf ( "Bien set token\n" );       

                    nextStep = STATUS_STEP2;
                }
                else
                {
                    printf ( "Error set token\n" );                    
                }

                usleep ( 2000000 );

                continue;
            }

            // Test
            if ( ( nextStep.compare ( STATUS_STEP2 ) == 0 ) )
            {                                
                // Calculate time
                tempTime = CalcTimeToken ( );                         

                // abrir puerta 1
                tempKeyLock = 0x01020304;
                tempStatus = CommandOpen2 ( 2, tempKeyLock, mainKey, tempTime );

                if ( tempStatus == true )
                {
                    printf ( "Bien cerrar\n" );
                    tempOkCountOpen ++;                    
                }
                else
                {
                    printf ( "Error cerando\n" );
                    tempErrorCountOpen ++;
                }  

                nextStep = STATUS_STEP3;  
                
                printf ( "Proceso puerta 1-  ABRIR bien= %d mal= %d\nCERRAR bien= %d mal= %d\n", tempOkCountOpen, tempErrorCountOpen,
                        tempOkCountClose, tempErrorCountClose );   
                usleep ( 5000000 );                            
                continue;
            }    

            // Test
            if ( ( nextStep.compare ( STATUS_STEP3 ) == 0 ) )
            {                                
                // Calculate time
                tempTime = CalcTimeToken ( );                         

                // abrir puerta 1
                tempKeyLock = 0x01020304;
                tempStatus = CommandClose2 ( 2, tempKeyLock, mainKey, tempTime );

                if ( tempStatus == true )
                {
                    printf ( "Bien cerrar\n" );
                    tempOkCountClose ++;                    
                }
                else
                {
                    printf ( "Error cerando\n" );
                    tempErrorCountClose ++;
                }  

                nextStep = STATUS_STEP2;                              

                printf ( "Proceso puerta 1-  ABRIR bien= %d mal= %d\nCERRAR bien= %d mal= %d\n", tempOkCountOpen, tempErrorCountOpen,
                        tempOkCountClose, tempErrorCountClose );   
                usleep ( 5000000 );

                continue;
            }                         

			continue;
		}  

        // Check change status
		if (  ( nextStatus.compare ( STATUS_DEBUG6 ) == 0 ) )
		{			
			// Set new status
			previousStatus = presentStatus;
			presentStatus = nextStatus;		

            // Initialize test
            if (  ( nextStep.compare ( STATUS_STEP1 ) == 0 ) )
            {
                tempOkCountClose = 0;
                tempErrorCountClose = 0;     
                tempOkCountOpen = 0;
                tempErrorCountOpen = 0;            

                // Initialize key block 0
                tempStatus = CommandGetSensor ( 1, &tempSensor );

                if ( tempStatus != true )
                {
                    printf ( "Error obteniendo sensor\n" );       

                    continue;
                }                         

                DecodeSensor ( tempSensor, SENSOR_BLACK, "\n", &tempString );

                printf ( "Sensor: %02X \n%s\n", tempSensor, tempString.c_str ( ) );

                usleep ( 2000000 );

                continue;
            } 

			continue;
		}  
	}    


    AddLog ( "Termino proceso...", LEVEL_LOG_1 );
    printf ( "Termino proceso..." );

    initialize = false;    

    // Unlock to change status
    mutexBusy.unlock();       

    flagRestarting = false;
}

/*------------------------------------------------------------------------------------------------
 * \fn bool CPic::SendCommand ( unsigned char * command, unsigned char * ans )
 * \brief Send a command to device
 * \param command: pointer to command data
 * \param ans: pointer to answer data
 * \return true: Command sent.
 * \return false: Error
 *
 * \pre
 * \post
 *
 * DESCRIPCION:
------------------------------------------------------------------------------------------------*/
bool CPic::SendCommand ( unsigned char * tempCommand, unsigned char * tempAns )
{
	unsigned char tempSize;
	bool tempStatus;	
	int tempByteTx;
	int tempByteRx;
    std::string tempString;
    int pos;
    unsigned char tempBuffer [ 300 ];
    unsigned char tempCrc;
    unsigned char tempValue;

	// Flush data port
	FlushPort ( );

	// Calculate CRC
	CalcCRC ( tempCommand );

	// Get number bytes to send
	tempSize = * ( tempCommand + 1 ) + 2;

	// Register information low level
    tempString = HexToText ( tempCommand, tempSize );
	AddLog ( "Datos a enviar:" + tempString, LEVEL_LOG_4 );

	// Send command
	tempByteTx = write ( idPort, tempCommand, tempSize );

	// Check total bytes sent
	if ( tempByteTx != tempSize )
	{
        // Error sending command
        AddLog ( "Error enviando comando.", LEVEL_LOG_1 );

        // Communication works
        flagCommunication = false;    

        return false;
	}

    memset ( tempBuffer, 0x00, sizeof ( tempBuffer ) );

	// Wait header answer
	tempStatus = WaitData ( 2 );

	if ( tempStatus != true )
	{
        AddLog ( "Timeout Rx 1", LEVEL_LOG_1 );  

        printf ( "Error time out Rx\n" );    
        
        // Communication works
        flagCommunication = false;    

        return false;
	}    

	// Get 2 first bytes from header
	tempByteRx = read ( idPort, tempBuffer, 2 );

	// Check bytes received
	if ( tempByteRx != 2 )
	{
        // did not receive bytes
        AddLog ( "Error en la comunicación.", LEVEL_LOG_1 );

        // Communication works
        flagCommunication = false;    

        return false;
	}

    // Get size frame
    tempSize = 2;

	// Calculate miss bytes
	tempByteRx = * ( tempBuffer + 1 );

	// Wait other bytes
	tempStatus = WaitData ( tempByteRx );

   // printf ( "Esperando:%d  llegaron: %d\n", size, tempStatus );

	// Check miss bytes
	if ( tempStatus  != true )
	{    
		AddLog ( "Timeout Rx 2", LEVEL_LOG_1 );

        // Communication works
        flagCommunication = false;  

		return false;
	}

	// Receive other bytes
	tempValue = read ( idPort, tempBuffer + 2, tempByteRx );

	// Check total bytes
	if ( tempByteRx != tempValue )
	{
		// lost some byte
		AddLog ( "Error recibiendo los datos faltantes.", LEVEL_LOG_1 );

        // Communication works
        flagCommunication = false;    

		// Error no se recibieron los bytes faltantes
		return false;
	}

    tempSize += tempByteRx;

    	// REgister low level
    tempString = HexToText ( tempBuffer, tempSize );
	AddLog ( "RX total " + tempString, LEVEL_LOG_4 );

    // Check CRC
    tempCrc = tempBuffer [ tempSize - 1 ];
    tempBuffer [ tempSize - 1 ] = 0x00;

    // calculate CRC data
    CalcCRC ( tempBuffer );

    // Check CRC
    if ( tempCrc != tempBuffer [ tempSize - 1 ] )
    {
        printf ( "Error CRC\n" );
        AddLog ( "Error CRC", LEVEL_LOG_1 );
    }

    // Copy data Rx
    memcpy ( tempAns, tempBuffer, tempSize );

	// Communication well
	flagCommunication = true;

	return true;
}

/*------------------------------------------------------------------------------------------------*/
/*!
 * \fn void CPic::SetLevelDebug ( unsigned char tempValue )
 * \brief Set level debug
 * \param  tempValue: Limit cash
 * \return None
 *
 * \pre
 * \post
 *
 *description:
 *
/*------------------------------------------------------------------------------------------------*/
void CPic::SetLevelDebug ( unsigned char tempValue )
{
    char tempText [ 1000 ];
	std::string tempString;

    levelDebug = tempValue;

	sprintf ( tempText, "Nivel Debug: %d", levelDebug );    
	tempString = tempText;
	AddLog ( tempString, LEVEL_LOG_1 );	
}

/*------------------------------------------------------------------------------------------------*/
/*!
 * \fn void CPic::SetMainKey ( unsigned int tempKey )
 * \brief Set level debug
 * \param  tempValue: Limit cash
 * \return None
 *
 * \pre
 * \post
 *
 *description:
 *
/*------------------------------------------------------------------------------------------------*/
void CPic::SetMainKey ( unsigned int tempKey )
{
    mainKey = tempKey;
}

/*------------------------------------------------------------------------------------------------*/
/*!
 * \fn void CPic::SetNamePort ( std::string tempName )
 * \brief Set serial port
 * \param  tempName: Name serial port
 * \return None
 *
 * \pre
 * \post
 *
 *description:
 *
/*------------------------------------------------------------------------------------------------*/
void CPic::SetNamePort ( std::string tempName )
{
	namePort = tempName;
}

/*------------------------------------------------------------------------------------------------*/
/*!
 * \fn void CPic:::SetSpeedPort ( std::string tempValue )
 * \brief Set speed port
 * \param  tempValue: Speed serial port
 * \return None
 *
 * \pre
 * \post
 *
 *description:
 *
/*------------------------------------------------------------------------------------------------*/
void CPic::SetSpeedPort ( std::string tempValue )
{
	// Check 9600 bps
	if ( tempValue.compare ( "9600" ) == 0 )
	{
		speed = B9600;
	}
}

/*------------------------------------------------------------------------------------------------*/
/*!
 * \fn CPic:::SetObjFile ( CFile * tempLog )
 * \brief Set log file object
 * \param  tempLog: main log file object
 * \return None
 *
 * \pre
 * \post
 *
 *description:
 *
/*------------------------------------------------------------------------------------------------*/
void CPic::SetObjFile ( CFile * tempLog )
{
    printf ("Obefile\n");
	objFile = tempLog;
}

/*------------------------------------------------------------------------------------------------
 * \fn bool CPic::WaitData ( int bytes )
 * \brief Wait for receive some bytes
 * \param Bytes: number bytes to receive
 * \return true: Bytes into buffer
 * \return false: Did not receive enough bytes
 *
 * \pre
 * \post
 *
 * description:
 *
------------------------------------------------------------------------------------------------*/
bool CPic::WaitData ( int bytes )
{
    int bytesRx;
    int cont;
    bool flagExit;
    struct timeval begin, end;

    // Get start time
    gettimeofday ( &begin, 0 );

    // Ciclo para esperar que lleguen los datos
    flagExit = false;

    do
    {
        // Check count avalilable bytes into receiver buffer port
        ioctl ( idPort, FIONREAD, &bytesRx );

        // Check if there are enough bytes
        if ( bytesRx >= bytes )
        {
          // There are enough bytes
          return true;
        }

        // Wait a moment
        usleep ( 2000 );

        // Get current time
        gettimeofday ( &end, 0 );

		long seconds = end.tv_sec - begin.tv_sec;
		long microseconds = end.tv_usec - begin.tv_usec;
		double elapsed = seconds + microseconds*1e-6;

        // Check timeout to receive bytes
        if ( elapsed >= 2 )
        {
            flagExit = true;
            return false;
        }
    }
    while ( flagExit == false );

    return false;
}
