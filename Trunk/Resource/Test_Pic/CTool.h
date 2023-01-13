/*
 * CTool.h
 *
 *  Created on: Mar 18, 2021
 *      Author: german
 */

#ifndef CTOOL_H_
#define CTOOL_H_

#include <sys/time.h>

class CTool {
public:
    
private:
    struct timeval timeEnd;
    struct timeval timeStart;
public:
	CTool();
	virtual ~CTool();
    double FinishTimer ( void );
    void StartTimer ( void );
    

};

#endif /* CTOOL_H_ */
