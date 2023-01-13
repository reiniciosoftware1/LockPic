/*
 * CTool.cpp
 *
 *  Created on: Mar 18, 2021
 *      Author: german
 */

#include "CTool.h"

/*------------------------------------------------------------------------------------------------
 * \fn CTool::CTool ( )
 * \brief Builder class
 * \param None.

 * \return None
 *
 * \pre
 * \post
 *
 * description:
------------------------------------------------------------------------------------------------*/
CTool::CTool() {
	// TODO Auto-generated constructor stub

}

/*------------------------------------------------------------------------------------------------
 * \fn CTool::~CTool ( )
 * \brief Destroyer class
 * \param None.

 * \return None
 *
 * \pre
 * \post
 *
 * description:
------------------------------------------------------------------------------------------------*/
CTool::~CTool() {
	// TODO Auto-generated destructor stub
}


/*------------------------------------------------------------------------------------------------
 * \fn double CTool::FinishTimer ( void )
 * \brief End timer
 * \param None.

 * \return Elapsed Time
 *
 * \pre
 * \post
 *
 * description:
------------------------------------------------------------------------------------------------*/
double CTool::FinishTimer ( void ) 
{
    double tempTime;
    
    // Get Fynal time
    gettimeofday ( &timeEnd, 0 );

    long seconds = timeEnd.tv_sec - timeStart.tv_sec;
    long microseconds = timeEnd.tv_usec - timeStart.tv_usec;
    tempTime = seconds + microseconds*1e-6;
    
    return tempTime;

}

/*------------------------------------------------------------------------------------------------
 * \fn doubvoidle CTool::StartTimer ( void )
 * \brief Start timer
 * \param None.

 * \return Elapsed Time
 *
 * \pre
 * \post
 *
 * description:
------------------------------------------------------------------------------------------------*/
void CTool::StartTimer ( void ) 
{   
    // Get Fynal time
    gettimeofday ( &timeStart, 0 );
}
