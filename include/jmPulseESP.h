/**
 * @file jmPulseESP.h
 * @author Jean Mercier profjmer@gmail.com
 * @brief header de la classe jmPulseESP_.cpp, architecture jmObjean_2023
 * @version 1.0
 */
#ifndef jmPulse_H
#define jmPulse_H

#include <stdint.h>

class jmPulse
{
	public:
		jmPulse();
		jmPulse(uint8_t pinNo);	// associe une broche avec un objet pulse

		void sm();
		void low(uint32_t timeLow);	//met la broche low pendant timeLow puis revient à high
		void high(uint32_t timeHigh);	// met la broche à high pendant timeHigh puis revient à low
		void pinState(uint8_t state);	// met la broche à low avec 0 à high avec 1 et bascule la broche avec 2
		void highLow(uint32_t timeHigh, uint32_t timeLow, uint16_t cycles ); // plusieurs cycles high low
		void lowHigh(uint32_t timeLow, uint32_t timeHigh, uint16_t cycles );	// plusieurs cycles low high
		void stop();	// arrêt des cycles.

	private:
		uint8_t pin;
		uint32_t tLow;
		uint32_t tHigh;
		uint8_t output;
		uint8_t state;
		uint32_t eggTimer;
		uint16_t cycles;
};

#endif
