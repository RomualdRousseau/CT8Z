/**************************************************************************/
/*!
    @file     CT8Z.cpp
    @author   Romuald Rousseau
    @license  BSD (see license.txt)

    Driver for the CT8Z transmiter

    This is a library for the CT8Z transmiter

    @section  HISTORY

    v1.0  - First release
*/
/**************************************************************************/
#include "Arduino.h"

#include "CT8Z.h"

const int PPMFreq_uS = 22500;		// PPM frame length total in uS
const int Fixed_uS = 300;			// PPM frame padding LOW phase in uS

const int pulseMin = 900;			// PPM Pulse widths in uS
const int pulseMid = 1600;
const int pulseMax = 2300;			

volatile int PPM_array[9];

/**************************************************************************/
/*!
    @brief  Instantiates a new CT8Z class w/appropriate properties
*/
/**************************************************************************/
CT8Z::CT8Z(bool inverted)
{
	pinMode(10, OUTPUT);

	// Initialize 8 PPM channel array
	char i = 0;
	for(i = 0; i < 8; i++)
	{
		PPM_array[i] = pulseMid;
	}
	PPM_array[i] = -1; // Mark end

	m_inverted = inverted;
}

/**************************************************************************/
/*!
    @brief  Sets up the HW
*/
/**************************************************************************/
void CT8Z::begin()
{
	TCCR1A = m_inverted ? B00100001 : B00110001;	// Compare register B used in mode 3 with non-inverted / inverted PPM output
	TCCR1B = B00010010;								// WGM13 & CS11 set to 1
	TCCR1C = B00000000;								// All set to 0
	TIMSK1 = B00000010;								// Interrupt on compare B
	TIFR1  = B00000010; 							// Interrupt on compare B
	OCR1A = PPMFreq_uS;								// PPM frequency
	OCR1B = Fixed_uS;								// PPM off time (lo padding)  
}

/**************************************************************************/
/*!
    @brief  Write a value
*/
/**************************************************************************/
void CT8Z::analogWrite(int channel, int value)
{
	float a = PPM_array[channel];
	float b = pulseMin + value * ((pulseMax - pulseMin) / 1023.0);
	PPM_array[channel] = int(a * 0.98 + b * 0.02); // Complementary filter
}

/**************************************************************************/
/*!
    @brief  Write a value mixed with another
*/
/**************************************************************************/
void CT8Z::analogMix(int channel, int value1, int value2, float mix)
{
	analogWrite(channel, value1 * mix + (1.0 - mix) * value2);
}

/**************************************************************************/
/*!
    @brief  Write a value
*/
/**************************************************************************/
void CT8Z::analogPulse(int channel, int value)
{
	int pulseValue = pulseMin + value * ((pulseMax - pulseMin) / 1023.0);
	if(PPM_array[channel] < pulseValue)
	{
		PPM_array[channel]+=10;
	}
	else if(PPM_array[channel] > pulseValue)
	{
		PPM_array[channel]-=10;
	}
}

/**************************************************************************/
/*!
    @brief  Write a value mixed with another
*/
/**************************************************************************/
void CT8Z::analogPulseMix(int channel, int value1, int value2, float mix)
{
	analogPulse(channel, value1 * mix + (1.0 - mix) * value2);
}

// *********************** BEGIN TIMER **************************
ISR(TIMER1_COMPA_vect)
{
	static volatile int *PPM_pointer = PPM_array;
	static int ACC_PPM_len = 0;				// Pulse length current total, used to calculate sync pulse
	int curr_value = *(PPM_pointer++);

	if(curr_value > -1)
	{
		OCR1A = curr_value;					// Set pulse length
		ACC_PPM_len += curr_value;			// Add pulse length to accumulator
	}
	else
	{
		PPM_pointer = PPM_array;			// Reset table position pointer
		OCR1A = PPMFreq_uS - ACC_PPM_len;	// Calculate final sync pulse length
		ACC_PPM_len = 0;					// Reset accumulator
	}
}
// *********************** END TIMER **************************

