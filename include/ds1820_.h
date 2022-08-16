/**
 * @file ds1820_.h
 * @author Jean Mercier profjmer@gmail.com
 * @brief header du module ds1820_.cpp, architecture jmObjean_2023
 */

#ifndef INCLUDE_DS18S20_H_
#define INCLUDE_DS18S20_H_
	void ds1820_ini();
	String ds1820_menu();
	String ds1820_start();
	String ds1820_lire();
	String ds1820_scan();
	String ds1820_last();
	String ds1820_JSON();
	String ds1820_JSONF(); // format influx	
	void ds1820_T();
	String ds1820_last_JSON();	
	String ds1820_interpret(String line);

	uint32_t ds1820_getDS1820_interval();
	String ds1820_getTimeUnits();
	String ds1820_setDS1820_interval(String interval);
	String ds1820_timerRestart();
	String ds1820_timerStop();
	
#endif //INCLUDE_DS18S20_H_

