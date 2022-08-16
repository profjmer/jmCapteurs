/**
* @file	jmPulse_ESP8266.c
* @brief classe pour la gestion des broches gpio
* @version	1.0 
*/

/* inclut des instructions C++ de Digital.cpp pour modifier les broches */
#include "../include/application.h"
#include "../include/jmPulseESP.h"

#ifndef nonStop
#define nonStop 65535  // for continuous mode operation
#endif

// output defenitions
#define outLow 0
#define outHigh 1
#define StatusOffset 3

#define RESET 0
#define SET 1
#define TOGGLE 2

// State definitions
#define puNotInit	0
#define puStopped 2
#define Pulse 1
#define puLow 4
#define puHigh 5
#define puHighLow 6
#define puLowHigh 7

 uint8_t pin;
 uint32_t tLow;
 uint32_t tHigh;
 uint8_t output;
 uint8_t state;
 uint32_t eggTimer;
 uint16_t cycles;

/**
 * @brief Construct a new jm Pulse::jm Pulse object
 * 
 */
jmPulse::jmPulse()
{
	state = puNotInit;
	pin = 0;
	output=0;
	eggTimer = 0;
	cycles=0;
	tHigh=0;
	tLow=0;
}

/**
 * @brief Construct a new jm Pulse::jm Pulse object
 * 
 * @param pinNo 
 */
jmPulse::jmPulse(uint8_t pinNo)
{
	state = puNotInit;
	pin = pinNo;
	output=0;
	eggTimer = 0;
	cycles=0;
	tHigh=0;
	tLow=0;
	if(pinNo >=0 && pinNo <16)	pinMode(pinNo,OUTPUT);
}

/**
 * @brief arrêt de la machine d'état de fonctionnement
 * 
 */
void jmPulse::stop(){
	cycles = 0;
	state = puStopped;
}

/**
 * @brief détermine l'état de la broche gpio
 * 
 * @param etat  états: low, high, toggle
 */
void jmPulse::pinState(uint8_t etat){

	// valide les valeurs
	if(etat > 2)return;

	switch(etat)
	{
	case RESET:		// reset pin low
		digitalWrite(pin,0);
		output = outLow;
		break;

	case SET:
		// set pin High
		digitalWrite(pin,1);
		output = outHigh;
		break;

	case TOGGLE:
		// toggle pin
		if(digitalRead(pin)==1)
		{
			digitalWrite(pin,0);
			output = outLow;
		}
		else
		{
			digitalWrite(pin,1);
			output = outHigh;
		}
		break;
	}
	state = puStopped;
	cycles = 0;
	eggTimer=0;
	return;

}

/**
 * @brief durée de la broche à l'état low
 * 
 * @param time 
 */
void jmPulse::low(uint32_t time)
{
	state = puLow;
	eggTimer = time;
	digitalWrite(pin,0);
}

/**
 * @brief durée de la broche à l'état high
 * 
 * @param time 
 */
void jmPulse::high(uint32_t time)
{
	state = puHigh;
	eggTimer = time;
	digitalWrite(pin,1);
}

/**
 * @brief durée état high, durée état low et nombre de répétition
 * 
 * @param timeHigh 
 * @param timeLow 
 * @param nCycles 
 */
void jmPulse::highLow(uint32_t timeHigh, uint32_t timeLow, uint16_t nCycles)
{
	tHigh = timeHigh;
	tLow = timeLow;
	cycles = nCycles;
	if(timeHigh == 0  || timeLow == 0) 
	{
		pinState(0);
		state = puStopped;
		return;
	}
	else state = puHighLow;
}

/**
 * @brief durée état low, durée état high et nombre de répétition
 * 
 * @param timeLow 
 * @param timeHigh 
 * @param nCycles 
 */
void jmPulse::lowHigh(uint32_t timeLow, uint32_t timeHigh, uint16_t nCycles)
{
	tHigh = timeHigh;
	tLow = timeLow;
	cycles = nCycles;
	if(timeHigh == 0  || timeLow == 0) 
	{
		pinState(1);
		state = puStopped;
		return;
	}
	else state = puLowHigh;
}

/**
 * @brief machine d'état du contrôle d'une broche gpio
 * 
 */
void jmPulse::sm(void){

	if (state == puStopped || state == puNotInit) return; //not running, bye bye ?

	if(eggTimer > 0)eggTimer--;

	if(state == puLow)
	{
		if(eggTimer == 0)
		{
		   digitalWrite(pin,1);	// remise a high rah apr�s l'impulsion basse
		   output = outHigh;
		   state = puStopped;
		}
		return;
	}

	if(state == puHigh)
	{
		if(eggTimer == 0)
		{
		   digitalWrite(pin,0);	//ral
		   output = outLow;
		   state = puStopped;
		}
		return;
	}

	if(state == puHighLow)
	{
		if (cycles != 0){  // Cycles to DO ?
			switch (output){
				// output is low
			case  outLow: if (eggTimer == 0){           // time to change ?
						   digitalWrite(pin,1);
						   eggTimer = tHigh;     		// load timer with tHigh
						   output = outHigh;

			}
					   break;

					   // output is High
			case  outHigh: if (eggTimer == 0){			// time to change ?
							digitalWrite(pin,0);
							output = outLow;
							eggTimer = tLow;    		// load timer with tLow
							if (cycles != nonStop){     // continuous mode ?
								if (--cycles == 0){     // decrease qty if not continuous mode
									state = puStopped;	// stop controller
									// report end of cycles
									//rGPPP0(i); //Message: GPPP0 id pin tHigh tLow status
								}
							}
			}
						break;
			}//switch
		}//if cycles
		return;
	}

	if(state == puLowHigh)
	{
		if (cycles != 0){  // Cycles to DO ?
			switch (output){
				// output is high
			case  outHigh: if (eggTimer == 0){         	// time to change ?
						   digitalWrite(pin,0);
						   eggTimer = tLow;     		// load timer with tLow
						   output = outLow;           	// udpate

			}
					   break;

					   // output is Low
			case  outLow: if (eggTimer == 0){			// time to change ?
							digitalWrite(pin,1);
							output = outHigh;           //
							eggTimer = tHigh;    		// load timer with tHigh
							if (cycles != nonStop){     // continuous mode ?
								if (--cycles == 0){     // decrease qty if not continuous mode
									state = puStopped;	// stop controller
									// report end of cycles
									//rGPPP0(i); //Message: GPPP0 id pin tHigh tLow status
								}
							}
			}
						break;
			}//switch
		}//if cycles
	}

}//PulseSM

/* [] END OF FILE */
