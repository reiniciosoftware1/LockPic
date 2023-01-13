/*
* GenerateLicense
 * CFile.cpp
 *
 *  Created on: 07092022
 *      Author: german
 */

#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include "CFile.h"
#include <cstring>

/*------------------------------------------------------------------------------------------------*/
/*!
 * \fn void CFile::AddLog ( std::string tempText )
 * \brief Add text to log file
 * \param tempText: Text for adding
 * \return
 *
 * \pre
 * \post
 *
 *description: set new path for save log files
 *
/*------------------------------------------------------------------------------------------------*/
void CFile::AddLog ( std::string tempText )
{
	int idFile;
	time_t tempTime;
	struct tm * timeInfo;
	char tempValue [ 10000 ];
	int bytes;
	int tempStatus;
    struct timeval current_time;
    char tempDirectory [ 100 ];

    // Lock to prevent to change status
    mutexBusy.lock ( );

    // Get base time
    gettimeofday ( &current_time, NULL );

	// Get current time
	time ( &tempTime );

	// Convert format local time
	timeInfo = localtime ( &tempTime );

    // Attemp to create directory
    sprintf ( tempDirectory, "%s%02d-%02d/", pathLog.c_str ( ), timeInfo -> tm_mon + 1, timeInfo -> tm_mday );
    tempStatus = mkdir ( tempDirectory, S_IRWXU );

	// Format name file
    sprintf ( tempValue, "%s%02d-%02d-%04d_%02d-%02d_log.txt", tempDirectory, timeInfo -> tm_mday,
    		timeInfo -> tm_mon + 1, timeInfo -> tm_year + 1900, timeInfo -> tm_hour, timeInfo -> tm_min / 5 );  
            

/*
	// Format name file
    sprintf ( tempValue, "%s%02d%02d%04d-%04d_log.txt", pathLog.c_str(), timeInfo -> tm_mday,
    		timeInfo -> tm_mon + 1, timeInfo -> tm_year + 1900, countLines / linesFile );                       
*/

    // open file in create mode
    idFile = open ( tempValue, O_CREAT | O_WRONLY | O_APPEND , 0644 );

    // Valida que no haya ocurrido un error.
    if ( idFile < 1 )
    {
        // Registra el error.
        printf ( "\nError creando - abriendo el archivo: %s\n", tempValue );
        printf ( "\nError texo: %s\n", tempText.c_str ( ) );

        printf ( "Error recv: %s\n", strerror ( errno ) );
    }

    // Assemble time to log
    sprintf ( tempValue, "%02d-%02d-%04d %02d:%02d:%02d.%03d %s\n", timeInfo -> tm_mday, timeInfo -> tm_mon + 1,
            timeInfo -> tm_year + 1900,
    		timeInfo -> tm_hour, timeInfo -> tm_min, timeInfo -> tm_sec, ( int )( current_time.tv_usec / 1000 ), tempText.c_str () );

    // Calcula el numero de bytes a escribir.
	bytes = strlen ( tempValue );

    countLines ++;
	// Agregar el texto del mensaje
	tempStatus = write ( idFile, tempValue, bytes );

	// Valida si pudo escribir todos los bytes.
	if ( tempStatus != bytes )
	{
        // Informa del error.
        printf ( "Error escribiendo en el archivo:%s %s\n", tempValue, tempText.c_str() );
        
        // Cierra el archivo.
        close ( idFile );

        // Unlock process
        mutexBusy.unlock();        

        return;
	}

	// Cierra el archivo.
	close ( idFile );

    // Unlock process
    mutexBusy.unlock();
}

/*------------------------------------------------------------------------------------------------*/
/*!
 * \fn void CFile::Add ( std::string tempText, int tempLevel )
 * \brief Add a text with level restriction
 * \param tempText: Text
 * \param tempLevel: Levle
 * \param tempSize: Length bytes from buffer to printf
 * \return
 *
 * \pre
 * \post
 *
 *description:
 *
/*------------------------------------------------------------------------------------------------*/
void CFile::AddLog ( std::string tempText, int tempLevel )
{
	// Check current level
	if ( tempLevel > levelLog )
	{
		// Not add text to log
		return;
	}

	// Add to file log
	AddLog ( tempText );
}

/*------------------------------------------------------------------------------------------------*/
/*!
 * \fn void CFile::AddHex ( std::string tempLabel,unsigned char * tempBuffer, int tempSize )
 * \brief Add a buffer hex to log file
 * \param tempLabel: Lael of line
 * \param tempBuffer: Pointer buffer data
 * \param tempSize: Length bytes from buffer to printf
 * \return
 *
 * \pre
 * \post
 *
 *description:
 *
/*------------------------------------------------------------------------------------------------*/
void CFile::AddHex ( std::string tempLabel, unsigned char * tempBuffer, int tempSize )
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

	// Add to file log
	AddLog ( tempLabel + ":" + tempLine );
}

/*------------------------------------------------------------------------------------------------*/
/*!
 * \fn void CFile::AddHex ( std::string tempLabel,unsigned char * tempBuffer, int tempSize, int tempLevel )
 * \brief Add a buffer hex to log file with level restriction
 * \param tempLabel: Lael of line
 * \param tempBuffer: Pointer buffer data
 * \param tempSize: Length bytes from buffer to printf
 * \param tempLevel: Level to log information
 * \return
 *
 * \pre
 * \post
 *
 *description:
 *
/*------------------------------------------------------------------------------------------------*/
void CFile::AddHex ( std::string tempLabel, unsigned char * tempBuffer, int tempSize, int tempLevel )
{
	// Check current level
	if ( tempLevel > levelLog )
	{
		// Not add text to log
		return;
	}

	// Add informatino to log
	AddHex ( tempLabel, tempBuffer, tempSize );
}

/*------------------------------------------------------------------------------------------------*/
/*!
 * \fn CFile::CFile() ( )
 * \brief Constructor
 * \param none
 * \return
 *
 * \pre
 * \post
 *
 *description:
 *
/*------------------------------------------------------------------------------------------------*/
CFile::CFile() {
	// Initialize variables
	pathLog = "";
	levelLog = 0;
    countLines = 0;
    linesFile = 10000;
}

/*------------------------------------------------------------------------------------------------*/
/*!
 * \fn bool CFile::CreateFile ( std::string tempName, unsigned char * tempData, int tempSize )
 * \brief Create file with text
 * \param tempName: Path file
 * \param tempData: Pointer datas
 * \return true:File created
 *
 * \pre
 * \post
 *
 *description:
 *
/*------------------------------------------------------------------------------------------------*/
bool CFile::CreateFile ( std::string tempName, unsigned char * tempData, int tempSize  )
{
    int idFile;
	char tempValue [ 1000 ];
	int statusTemp;

    // Create file in create mode
    idFile = open ( tempName.c_str (), O_CREAT | O_WRONLY | O_TRUNC , 0644 );

    // Valida que no haya ocurrido un error.
    if ( idFile < 1 )
    {
      // Registra el error.
      printf ( "\nError creando el archivo: %s.\n", tempName.c_str ( ) );
      return false;
    }

	// Agregar el texto del mensaje
	statusTemp = write ( idFile, tempData, tempSize );

	// Valida si pudo escribir todos los bytes.
	if ( statusTemp != tempSize )
	{
	  // Informa del error.
	  printf ( "Error escribiendo en el archivo\n" );

	  // Cierra el archivo.
	  close ( idFile );

      return false;
	}

	// Cierra el archivo.
	close ( idFile );
    return true;
}

/*------------------------------------------------------------------------------------------------*/
/*!
 * \fn CFile::~CFile()
 * \brief Destroyer
 * \param none
 * \return
 *
 * \pre
 * \post
 *
 *description:
 *
/*------------------------------------------------------------------------------------------------*/
CFile::~CFile()
{

}

/*------------------------------------------------------------------------------------------------
 * \fn std::string CFile::HexToText ( unsigned char * tempBuffer, unsigned tempSize )
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
std::string CFile::HexToText ( unsigned char * tempBuffer, unsigned int tempSize )
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

/*------------------------------------------------------------------------------------------------*/
/*!
 * \fn void CFile::SetLevelLog ( int tempLevel )
 * \brief Set level log
 * \param tempLevel: level log to register
 * \return
 *
 * \pre
 * \post
 *
 *description: set level since a tet is adds to log file
 *
/*------------------------------------------------------------------------------------------------*/
void CFile::SetLevelLog ( int tempLevel )
{
	levelLog = tempLevel;
}

/*------------------------------------------------------------------------------------------------*/
/*!
 * \fn void CFile::SetPathDatabase ( std::string tempName )
 * \brief Set directory path for save database files
 * \param tempName: absolute path directory
 * \return
 *
 * \pre
 * \post
 *
 *description: set new path for save database files
 *
/*------------------------------------------------------------------------------------------------*/
void CFile::SetPathDatabase ( std::string tempName ) 
{

	pathDatabase = tempName;
}

/*------------------------------------------------------------------------------------------------*/
/*!
 * \fn void CFile::SetPathLog( std::string tempName )
 * \brief Set directory path for save log files
 * \param tempName: absolute path directory
 * \return
 *
 * \pre
 * \post
 *
 *description: set new path for save log files
 *
/*------------------------------------------------------------------------------------------------*/
void CFile::SetPathLog ( std::string tempName ) 
{

	int borrar;

	pathLog = tempName;    
}


