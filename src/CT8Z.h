/**************************************************************************/
/*!
	@file     CT8Z.h
	@author   Romuald Rousseau
	@license  BSD (see license.txt)

	Driver for the CT8Z transmiter

	This is a library for the CT8Z transmiter

	@section  HISTORY

	v1.0  - First release
*/
/**************************************************************************/
#ifndef CT8Z_h
#define CT8Z_h

#define PPM_MAX_CHANNELS	8

class CT8ZClass
{
public:
	CT8ZClass();

	void begin(bool inverted = false);
	
	void analogWrite(char channel, int value);
	void analogWrite(int values[], int matrix[][PPM_MAX_CHANNELS]);
};

static CT8ZClass CT8Z;
#endif
