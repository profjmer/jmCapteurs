/**
 * @file adc_.h
 * @author profjmer@gmail.com
 * @brief header du module adc_.cpp module de gestion du Convertisseur Analogique à numérique CAN
 * architecture jmObjean_2022
 * @version 1.0
 */
#ifndef INCLUDE_adc_H_
#define INCLUDE_adc_H_

	#include <WString.h>

String adc_menu();
String adc_field();
String adc_JSON();
String adc_JSONF(); // format influx
String adc_lire();
String adc_last();
String adc_last_JSON();
double adc_vBat();
void adc_voltsToHisto(String Temps, double volts);
String adc_voltsToHistoJSON();
String adc_interpret(String line);

uint32_t adc_getADC_interval();
String adc_getTimeUnits();
String adc_setADC_interval(String interval);
String adc_timerRestart();
String adc_timerStop();
void adc_volts();
void adc_ini();
		
#endif //INCLUDE_adc_H_




