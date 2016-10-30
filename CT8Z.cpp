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
const int pulseMax = 2300;

volatile int PPM_array[PPM_MAX_CHANNELS + 1];

/**************************************************************************/
/*!
    @brief  Instantiates a new CT8Z class w/appropriate properties
*/
/**************************************************************************/
CT8ZClass::CT8ZClass()
{
#if defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__)
	pinMode(12, OUTPUT);
#elif defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__)
	pinMode(10, OUTPUT);
#endif

	// Initialize 8 PPM channel array
	char i = 0;
	for(; i < PPM_MAX_CHANNELS; i++)
	{
		PPM_array[i] = (pulseMax + pulseMin) / 2;
	}
	PPM_array[PPM_MAX_CHANNELS] = -1; // Mark end
}

/**************************************************************************/
/*!
    @brief  Sets up the HW
*/
/**************************************************************************/
void CT8ZClass::begin(bool inverted)
{
	TCCR1A = inverted ? B00100001 : B00110001;		// Compare register B used in mode 3 with non-inverted / inverted PPM output
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
void CT8ZClass::analogWrite(char channel, int value)
{
	PPM_array[channel] = map(value, 0, 1023, pulseMin, pulseMax);
}

/**************************************************************************/
/*!
    @brief  Mix values with a matrix
*/
/**************************************************************************/
void CT8ZClass::analogWrite(int values[], int matrix[][PPM_MAX_CHANNELS])
{
	for(int i  = 0; i < PPM_MAX_CHANNELS; i++)
	{
		long s = 0;
		for(int j = 0; j < (PPM_MAX_CHANNELS + 1); j++)
		{
			s += long(values[j] - 512) * long(matrix[j][i]) / 256;
		}
		analogWrite(i, constrain(s, -512, 511) + 512);
	}
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

