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

class CT8Z
{
protected:
	bool	m_inverted;

public:
	CT8Z(bool inverted = false);

	void begin();
	void analogWrite(int channel, int value);
	void analogMix(int channel, int value1, int value2, float mix);
	void analogPulse(int channel, int value);
	void analogPulseMix(int channel, int value1, int value2, float mix);
};

#endif
