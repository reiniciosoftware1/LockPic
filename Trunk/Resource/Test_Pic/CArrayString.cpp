/*
 * CArrayString.cpp
 *
 *  Created on: Mar 18, 2021
 *      Author: german
 */

#include "CArrayString.h"

/*------------------------------------------------------------------------------------------------
 * \fn CArrayString::CArrayString ( )
 * \brief Builder class
 * \param None.

 * \return None
 *
 * \pre
 * \post
 *
 * description:
------------------------------------------------------------------------------------------------*/
CArrayString::CArrayString() {
	// TODO Auto-generated constructor stub
	size = 0;
}

/*------------------------------------------------------------------------------------------------
 * \fn CArrayString::~CArrayString ( )
 * \brief Destroyer class
 * \param None.

 * \return None
 *
 * \pre
 * \post
 *
 * description:
------------------------------------------------------------------------------------------------*/
CArrayString::~CArrayString() {
	// TODO Auto-generated destructor stub
}

/*------------------------------------------------------------------------------------------------
 * \fn void CArrayString::~CArrayString ( )
 * \brief Add a new element to the list
 * \param None.

 * \return None
 *
 * \pre
 * \post
 *
 * description: Add an element to the list
------------------------------------------------------------------------------------------------*/
void CArrayString::Add ( std::string tempValue )
{
	list [ size ] = tempValue;    
    
	size ++;
}

/*------------------------------------------------------------------------------------------------
 * \fn void CArrayString::Clear ( )
 * \brief Delete list
 * \param None.

 * \return None
 *
 * \pre
 * \post
 *
 * description:
------------------------------------------------------------------------------------------------*/
void CArrayString::Clear ( void )
{
	size = 0;
}

/*------------------------------------------------------------------------------------------------
 * \fn std::string CArrayString::GetIndex ( int tempPos )
 * \brief Get string off a position
 * \param tempPos: Index at array to get.

 * \return text
 * \return "" position wrong
 *
 * \pre
 * \post
 *
 * description:
------------------------------------------------------------------------------------------------*/
std::string CArrayString::GetIndex ( int tempPos )
{
	// Check limit index
	if ( tempPos > size )
	{
		return "";
	}

	// return value
	return list [ tempPos ];
}

/*------------------------------------------------------------------------------------------------
 * \fn std::string CArrayString::GetLast ( void )
 * \brief Get string off last position
 * \param None.

 * \return text
 *
 * \pre
 * \post
 *
 * description:
------------------------------------------------------------------------------------------------*/
std::string CArrayString::GetLast ( void )
{
	// return value
	return list [ size - 1 ];
}

/*------------------------------------------------------------------------------------------------
 * \fn int CArrayString::GetSize ( void )
 * \brief Get size list
 * \param None.

 * \return None
 *
 * \pre
 * \post
 *
 * description:
------------------------------------------------------------------------------------------------*/
int CArrayString::GetSize ( void )
{
	return size;
}

/*------------------------------------------------------------------------------------------------
 * \fn void CArrayString::PrintProperties ( std::string  tempTag )
 * \brief Print all properties  list
 * \param tempTag: Tag array denomination

 * \return denomination
 *
 * \pre
 * \post
 *
 * description:
------------------------------------------------------------------------------------------------*/
void CArrayString::PrintProperties ( std::string tempTag )
{
	int pos;
	char tempText [ 200 ];
	char tempTotal [ 5000 ];
	std::string tempString;

	memset ( tempTotal, '\0', sizeof ( tempTotal ) );

    sprintf ( tempTotal, "Debug %s size: %d\n", tempTag.c_str ( ), size );
    
	// Cycle
	for ( pos = 0; pos < size; pos ++ )
	{
		// Get each denomination
		tempString = list [ pos ];

		sprintf ( tempText, "%d %s\n", pos, tempString.c_str() );

		// Add whole text
		strcat ( tempTotal, tempText );
	}

	printf ( "\n%s\n", tempTotal );
}

/*------------------------------------------------------------------------------------------------*/
/*!
 * \fn std::string CArrayString::RemoveSpace ( std::string * tempInput )
 * \brief REmove initall and ending spaces
 * \param  tempInput: pointer input and output string
 * \return None
 *
 * \pre
 * \post
 *
 *description:
 *
/*------------------------------------------------------------------------------------------------*/
void CArrayString::RemoveSpace ( std::string * tempInput )
{
    char tempText [ 200 ];
    char tempText2 [ 200 ];
    int pos;
    int cont;
     
    // Convert to char
    sprintf ( tempText, "%s", tempInput -> c_str ( ) );
     
    // Cycle search last spaces
    for ( pos = strlen ( tempText ) - 1; pos >= 0; pos -- )
    {
        // Check space
        if ( tempText [ pos ] == ' ' )
        {           
            // Truncate
            tempText [ pos ] = '\0';                       
        }
        else
        {
            break;
        }
    }
    
    // Find star position
    cont = 0;
    for ( pos = 0; pos < strlen ( tempText ); pos ++ )
    {
        // Check space
        if ( tempText [ pos ] != ' ' )
        {
            cont = pos;
            
            break;
        }
    }
    
    // copy datas
    strcpy ( tempText2, tempText + cont );
    
    // copy data
    *tempInput = tempText2;    
}

/*------------------------------------------------------------------------------------------------
 * \fn int CArrayString::Search ( std::string tempValue )
 * \brief Search position text
 * \param tempValue: String to search.

 * \return >= 0 = position
 \return -1 Not found
 *
 * \pre
 * \post
 *
 * description: Search each character ands separate a substring to structure into a array
------------------------------------------------------------------------------------------------*/
int CArrayString::Search ( std::string tempValue )
{
    int pos;
    std::string tempString;
    
    // Cycle to search
    for ( pos = 0; pos < size; pos ++ )
    {
        // Get string
        tempString = list [ pos ];

        // Compare
        if ( tempString.compare ( tempValue ) == 0 )
        {
            return pos;
        }
    }
    
    return -1;
}

/*------------------------------------------------------------------------------------------------
 * \fn void CArrayString::Split ( std::string tempText, std::String tempSeparater )
 * \brief Divide text for space character
 * \param tempText: String to split.
 * \param tempSeparater: String to split

 * \return None
 *
 * \pre
 * \post
 *
 * description: Search each character ands separate a substring to structure into a array
------------------------------------------------------------------------------------------------*/
void CArrayString::Split ( std::string tempText, std::string tempSeparater )
{
	int posStart;
	int posEnd;
	bool flagExit;
	std::string tempParam;

	// Clear array
	size = 0;

	// Cycle to get substring
	flagExit = false;
	posStart = 0;
	while ( flagExit == false )
	{
		// Check next position character
		posEnd = tempText.find ( tempSeparater, posStart );

		// Check if it was found
		if ( posEnd != std::string::npos )
		{

		}
		else
		{
			// Get substring
			posEnd = tempText.length ( );

			// Finish
			flagExit = true;
		}

		// Get each substring
		tempParam = tempText.substr ( posStart, ( posEnd - posStart ) );

		// Point next character to space
		posStart = posEnd + tempSeparater.size ( );

		// Not add empty space
		if ( tempParam.compare ( tempSeparater ) == 0 )
		{
			continue;
		}

		// Not add empty space
		if ( tempParam.length ( ) == 0 )
		{
			continue;
		}

		// Add new string
		Add ( tempParam );
	}

	return;
}


