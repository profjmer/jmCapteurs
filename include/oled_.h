/**
 * @file oled_.h
 * @author Jean Mercier profjmer@gmail.com
 * @brief header du module oled_.cpp, architecture jmObjean_2023
 * @version 1.0
 * @date 2020-03-04
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#ifndef INCLUDE_gestionOLED_H_
#define INCLUDE_gestionOLED_H_

	void oled_ini();
	void oled_update();
	void oled_setZone1(String);
	void oled_setZone2(String);
	void oled_setZone3(String);
	void oled_setZone4(String);
	String oled_getZone4();
	String oled_menu();
	String oled_interpret(String line);
	void oled_off();
	void oled_temoin_routeur(String req,String rep);	
	uint8_t oled_getOLED_interval();
	String oled_timerStop();
	String oled_timerRestart();	
	String oled_pages();

#endif //INCLUDE_gestionOLED_H_




