/**
 * @file dht22_.h
 * @author Jean Mercier profjmer@gmail.com
 * @brief header du module dht22_.cpp, architecture jmObjean_2023
 */

#ifndef INCLUDE_DHT22_H_
#define INCLUDE_DHT22_H_
	void dht22_ini();
	String dht22_lire();
	String dht22_last();
	String dht22_menu();
	String dht22_field();
	String dht22_JSON();
	String dht22_JSONF(); // format influx	
	String dht22_last_JSON();
	void dht22_TRH();
	String dht22_interpret(String line);	

	uint32_t dht22_getDHT22_interval();
	String dht22_getTimeUnits();	
	String dht22_setDHT22_interval(String interval);
	String dht22_timerRestart();
	String dht22_timerStop();

	double dht22_temperature_last();
	double dht22_humidity_last();
	
#endif //INCLUDE_DHT22_H_

