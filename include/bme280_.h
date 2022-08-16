/**
 * @file bme280_.h
 * @author profjmer@gmail.com
 * @brief header du module bme280_.cpp, architecture jmObjean_2023
 */
#define er 0
#ifndef __bme280_H__
	#define __bme280_H__
		
	void bme280_ini();
	String bme280_lire();
	String bme280_last();
	String bme280_menu();
	String bme280_JSON();
	String bme280_JSONF(); // format influx
	String bme280_last_JSON();
	String bme280_field();
	String bme280_histoJSON();	
	String bme280_interpret(String line);
	void bme280_printdata();	
	String bme280_temperature();
	String bme280_pression();
	String bme280_humidity();	
	double bme280_temperature_last();
	double bme280_pression_last();
	double bme280_humidity_last();	

	uint32_t bme280_getBME280_interval();
	String bme280_getTimeUnits();
	String bme280_setBME280_interval(String interval);
	String bme280_timerRestart();
	String bme280_timerStop();
	void bme280_TPH();

#endif
