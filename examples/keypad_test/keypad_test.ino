#include "variant.h"'
#include <due_can.h>
#include <powerkey_j1939.h>

//Leave defined if you use native port, comment if using programming port
#define Serial SerialUSB

void keypresses(int key_state)
{
	int key = key_state & 0x7F;
	Serial.print("Key ");
	Serial.print(key);

	if (key_state & 0x80)
	{
		Serial.println(" was pressed");	
	}
	else Serial.println("was released");
}

void setup()
{
	Serial.begin(115200);
	PowerKeyPro0.begin(250000);
	PowerKeyPro0.setKeyCallback(keypresses);
}

void loop()
{
	static int led, state;

	delay(200);
	led++;
	if (led > 11)
	{
		led = 0;
		state++;
		if (state > 9) state = 0;
	}
	PowerKeyPro0.setLEDState(led + 1, (LEDTYPE)state);
}
