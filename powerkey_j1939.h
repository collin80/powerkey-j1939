#ifndef POWERKEY_H_
#define POWERKEY_H_

#include <Arduino.h>
#include <due_can.h>

#define DEFAULT_ID	0x21

enum LEDTYPE
{
	OFF = 0,
	RED = 1,
	RED_BLINK = 2,
	RED_ALT_BLINK = 3,
	GREEN = 4,
	GREEN_BLINK = 5,
	GREEN_ALT_BLINK = 6,
	AMBER = 7,
	AMBER_BLINK = 8,
	AMBER_ALT_BLINK = 9,
};

enum PKP_CMD
{

};

class POWERKEYJ1939
{
public:
	POWERKEYJ1939(int); //canbus channel to use

	void begin(int speed = 250000, int id = DEFAULT_ID); //begin the comm, pass desired canbus speed
	bool isConnected(); //is a keypad responding to us?		
	void setDeviceCANSpeed(int); //set a new comm speed. Updates the keybox with this new value
	void setKeypadID(int id = DEFAULT_ID); //that is the actual default ID they come with.
	int findKeypadID(); //listen on the bus to find the keybox and return it's ID
	void setKeyCallback(void (*cb)(int));
	bool getKeyState(int key); //true = pressed, false = unpressed
	void setLEDState(int LED, LEDTYPE newState);
	LEDTYPE getLEDState(int LED);
	void setLEDBrightness(int brightness);
	void setBacklight(int brightness);

	void loop();
	void receiveFrame(CAN_FRAME *frame);
private:
	CANRaw *channel;
	int keypadID; //which ID is the keypad on?
	bool buttonState[15];
	LEDTYPE LEDState[15]; //LED state for all potential keys
	void (*keystateChange)(int keyStates); //callback used when the keypad state changes (button up or down)
};

extern POWERKEYJ1939 PowerKeyPro0; //keypad connected on CAN0
extern POWERKEYJ1939 PowerKeyPro1; //keypad connected on CAN1

#endif