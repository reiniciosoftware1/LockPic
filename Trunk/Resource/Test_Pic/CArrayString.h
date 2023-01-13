/*
 * CArrayString.h
 *
 *  Created on: Mar 18, 2021
 *      Author: german
 */

#ifndef CARRAYSTRING_H_
#define CARRAYSTRING_H_

#include <string>
#include <string.h>


class CArrayString {
private:
	std::string list [ 1000 ];
	int size;
    //char listText [ 100 ] [ 1000 ];
public:
	CArrayString();
	virtual ~CArrayString();
	void Add ( std::string tempValue );
	void Clear ( void );
	std::string GetIndex ( int tempPos );
	std::string GetLast ( void );
	int GetSize ( void );
	void PrintProperties ( std::string tempTag );
    void RemoveSpace ( std::string * tempInput );
    int Search ( std::string tempValue );
	void Split ( std::string tempText, std::string tempSeparater );
    


};

#endif /* CARRAYSTRING_H_ */
