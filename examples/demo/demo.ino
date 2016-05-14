#include <CT8Z.h>

CT8Z ct8z;

void setup()
{
	pinMode(2, INPUT);
	pinMode(3, INPUT);
	pinMode(4, INPUT);
	pinMode(7, INPUT);
	pinMode(8, INPUT);

	ct8z.begin();	
}

void loop()
{
	ct8z.analogPulse(0, (digitalRead(2) == HIGH) ? 1023 : ((digitalRead(3) == HIGH) ? 0 : 512));
	ct8z.analogPulse(1, (digitalRead(4) == HIGH) ? 1023 : 0);
	ct8z.analogPulse(2, (digitalRead(7) == HIGH) ? 1023 : ((digitalRead(8) == HIGH) ? 0 : 512));
}

