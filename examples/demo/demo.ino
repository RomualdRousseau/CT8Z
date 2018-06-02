#include <CT8Z.h>

int mixes[PPM_MAX_CHANNELS][PPM_MAX_CHANNELS] = 
{
  {+256,    0,    0, -256,    0,  +64,    0,    0},
  {   0, +256,    0,    0,    0,    0,    0,    0},
  {   0,    0, +256,    0,    0,    0,    0,    0},
  {   0,    0,    0, +256,    0,    0,    0,    0},
  {   0,    0,    0,    0, +256,    0,    0,    0},
  {   0,    0,    0,    0,    0, +256,    0,    0},
  {   0,    0,    0,    0,    0,    0, +256,    0},
  {   0,    0,    0,    0,    0,    0,    0, +256}
};

int values[PPM_MAX_CHANNELS] =
{
  512, 512, 512, 512, 512, 512, 512, 512
};

void setup()
{
	CT8Z.begin();	
}

void loop()
{
  values[0] = analogRead(A0);
  values[1] = analogRead(A1);
  CT8Z.analogWrite(values, mixes);
}

