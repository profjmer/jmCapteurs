/*
 * @file tft_.h
 * @author Jean Mercier profjmer@gmail.com
 * @brief header du module tft_.cpp, architecture jmObjean_2023
 * @version 1.0
 */
	#include "Adafruit_ILI9341.h"
	#include "BPMDraw.h"

#ifndef INCLUDE_tft_H_
#define INCLUDE_tft_H_
/* définitions pour l'affichage de différentes pages tft */
	#define bme280 0
	#define adc 1
	#define dht22 2
	#define ds1820 3
	
	String tft_menu();
	void tft_update(uint8_t capteur);
	void tft_String(uint16_t x, uint16_t y, uint8_t size, uint16_t couleur, uint8_t rotation, String texte );		
	void tft_draw_axeX();
	void tft_horloge();
	void tft_showTimeUnits(String echTime);
	uint8_t tft_getOrientation();
	String tft_interpret(String line);

	String tft_setTFT_interval(String interval);
	String tft_getTimeUnits();
	String tft_timerRestart();
	String tft_timerStop();
	void tft_ini();
#endif //INCLUDE_tft_H_