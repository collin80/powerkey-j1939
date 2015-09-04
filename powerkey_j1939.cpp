#include "powerkey_j1939.h"

void bounceFrame0(CAN_FRAME *frame)
{
	PowerKeyPro0.receiveFrame(frame);
}

void bounceFrame1(CAN_FRAME *frame)
{
	PowerKeyPro1.receiveFrame(frame);
}

POWERKEYJ1939::POWERKEYJ1939(int chan)
{
	if (chan == 0)
	{
		channel = &Can0;
	}
	else
	{
		channel = &Can1;
	}
	for (int x = 0; x < 15; x++) buttonState[x] = false;
	keypadID = DEFAULT_ID;
}

void POWERKEYJ1939::begin(int speed, int id)
{
	int mailbox;
	channel->begin(speed);
	keypadID = id;
	mailbox = channel->watchFor(0x18EF0000, 0x1FFF0000);
	if (channel == &Can0)
	{
		channel->setCallback(mailbox, bounceFrame0);
	}
	else
	{
		channel->setCallback(mailbox, bounceFrame1);
	}
}

bool POWERKEYJ1939::isConnected()
{
	return true;
}

void POWERKEYJ1939::setKeypadID(int id)
{
	if (id < 17) return;
	if (id > 252) return;
	CAN_FRAME frame;
	frame.id = 0x18EF0000 + (keypadID << 8);
	frame.extended = true;
	frame.length = 8;
	frame.data.byte[0] = 0xD3;
	frame.data.byte[1] = 0x98;
	frame.data.byte[2] = 0x70; //set ECU address
	frame.data.byte[3] = id;
	frame.data.byte[4] = frame.data.byte[5] = frame.data.byte[6] = frame.data.byte[7] = 0xFF;

	channel->sendFrame(frame);

	keypadID = id;
}

//Not actually in existence yet...
int POWERKEYJ1939::findKeypadID()
{
	return -1;
}

void POWERKEYJ1939::setKeyCallback(void(*cb)(int))
{
	keystateChange = cb;
}

bool POWERKEYJ1939::getKeyState(int key)
{
	if (key < 0) return false;
	if (key > 14) return false;
	return buttonState[key];
}

void POWERKEYJ1939::setLEDBrightness(int brightness)
{
	if (brightness < 0) return;
	if (brightness > 63) return;
	CAN_FRAME frame;
	frame.id = 0x18EF0000 + (keypadID << 8);
	frame.extended = true;
	frame.length = 8;
	frame.data.byte[0] = 0xD3;
	frame.data.byte[1] = 0x98;
	frame.data.byte[2] = 2; //dim level
	frame.data.byte[3] = brightness;
	frame.data.byte[4] = frame.data.byte[5] = frame.data.byte[6] = frame.data.byte[7] = 0xFF;

	channel->sendFrame(frame);
}

void POWERKEYJ1939::setBacklight(int brightness)
{
	if (brightness < 0) return;
	if (brightness > 63) return;
	CAN_FRAME frame;
	frame.id = 0x18EF0000 + (keypadID << 8);
	frame.extended = true;
	frame.length = 8;
	frame.data.byte[0] = 0xD3;
	frame.data.byte[1] = 0x98;
	frame.data.byte[2] = 3; //backlight level
	frame.data.byte[3] = brightness;
	frame.data.byte[4] = frame.data.byte[5] = frame.data.byte[6] = frame.data.byte[7] = 0xFF;

	channel->sendFrame(frame);
}

void POWERKEYJ1939::setLEDState(int LED, LEDTYPE newState)
{
	if (LED < 0) return;
	if (LED > 14) return;
	CAN_FRAME frame;	
	frame.id = 0x18EF0000 + (keypadID << 8);
	frame.extended = true;
	frame.length = 8;
	frame.data.byte[0] = 0xD3;
	frame.data.byte[1] = 0x98;
	frame.data.byte[2] = 1; //set state of an LED
	frame.data.byte[3] = LED;
	frame.data.byte[4] = (int)newState;
	frame.data.byte[5] = frame.data.byte[6] = frame.data.byte[7] = 0xFF;

	channel->sendFrame(frame);
}

LEDTYPE POWERKEYJ1939::getLEDState(int LED)
{
	if (LED < 0) return OFF;
	if (LED > 14) return OFF;
	return LEDState[LED];
}

void POWERKEYJ1939::setDeviceCANSpeed(int speed)
{
	channel->begin(speed);
}

void POWERKEYJ1939::loop()
{
}

void POWERKEYJ1939::receiveFrame(CAN_FRAME *frame)
{
	int outputValue;
	int idBase = frame->id & 0x1FFF0000;
	int destAddr = (frame->id && 0xFF00) >> 8;
	int srcAddr = (frame->id && 0xFF);
	if (idBase == 0x18EF0000)
	{		
		if (srcAddr == keypadID)
		{
			if ((frame->data.byte[0] == 0xD3) && (frame->data.byte[1] == 0x98))
			{
				if (frame->data.byte[2] == 1) //key press / release event
				{ 
					if (frame->data.byte[4] == 1) buttonState[frame->data.byte[3]] = true;
					else buttonState[frame->data.byte[3]] = false;
					if (keystateChange != NULL)
					{
						outputValue = frame->data.byte[3];
						if (frame->data.byte[4] == 1) outputValue += 0x8; //set bit 7 to indicate the button was pressed, otherwise it was let up
						keystateChange(outputValue); //call the callback function
					}					
				}
			}
		}		
	}
}

POWERKEYJ1939 PowerKeyPro0(0);
POWERKEYJ1939 PowerKeyPro1(1);
