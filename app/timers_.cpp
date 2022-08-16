/**
 * @file timers_.cpp
 * @author Jean Mercier profjmer@gmail.com
 * @brief Module permettant d'activer/désacctiver toutes les minuteries
 * @version 1.0
 */
#include "../include/application.h"

#define facteur_secondes 1000
#define facteur_minutes 60000
#define facteur_heures 3600000

/**
 * @brief retourne l'intervalle en ms
 * 
 * @param interval valeur numérique
 * @param units secondes minutes ou heures
 * @return uint32_t 
 */
uint32_t timers_interval_ms(uint32_t interval, String units){
    uint32_t facteur;
    if(units == "secondes")  facteur= facteur_secondes ;
	else if(units == "minutes")  facteur = facteur_minutes ;
	else if(units == "heures")  facteur = facteur_heures ;	
    return interval*facteur;
}

/**
 * @brief Menu des commandes du module timers_
 * 
 * @return String 
 */
String timers_menu(){  return "\ntimers [start stop]\ntimers info\n";}

/**
 * @brief arrêt des minuteries du projet
 * 
 * @return String 
 */
String timers_stop(){
    #ifdef config_adc
        adc_timerStop();
    #endif
    #ifdef config_bme280
        bme280_timerStop();
    #endif
    #ifdef config_dht22
        dht22_timerStop();
    #endif    
    #ifdef config_ds1820
        ds1820_timerStop();
    #endif
    #ifdef config_oled
        oled_timerStop();
    #endif    

    return "Arrêt des Minuteries\n";
}

/**
 * @brief retourne les intervalles des minuteries du projet
 * 
 * @return String 
 */
String timers_intervals(){
    String intervalles;
    #ifdef config_adc
        intervalles += "adc interval "+String(adc_getADC_interval())+ " sec\n";
    #endif
    #ifdef config_bme280
        intervalles += "bme280 interval "+String(bme280_getBME280_interval())+ " sec\n";
    #endif
    #ifdef config_dht22
        intervalles += "dht22 interval "+String(dht22_getDHT22_interval())+ " sec\n";
    #endif     
    #ifdef config_ds1820
        intervalles += "ds1820 interval "+String(ds1820_getDS1820_interval())+ " sec\n";
    #endif
    #ifdef config_oled
        intervalles += "oled interval "+String(oled_getOLED_interval())+ " sec\n";
    #endif  

    return intervalles ;    
}

/**
 * @brief affiche les intervalles des minuteries du projet au port série
 * 
 */
void timers_info(){
    //Serial.println(timers_intervals());
}

/**
 * @brief redémarrage des minuteries du projet
 * 
 */
String timers_restart(){
    #ifdef config_adc
        adc_timerRestart();       
    #endif
    #ifdef config_bme280
        bme280_timerRestart();       
    #endif   
    #ifdef config_dht22
        dht22_timerRestart();       
    #endif  
    #ifdef config_ds1820
        ds1820_timerRestart();       
    #endif           
    #ifdef config_oled
        oled_timerRestart();          
    #endif 

    timers_info();
   return "Minuteries restart\n";
}


/****************************************************************** Initialisation du module timers_  *******************************************/

/**
 * @brief Initialisation des minuteries utilisées
 * 
 */
void timers_ini(){
    //Serial.println("timers_ini()");
}

/******************************************************************** Interpréteur de commandes **************************************************/

/**
/**
 * @brief Interpréteur de commandes du module timers_
 * 
 * @param line 
 * @return String 
 */
String timers_interpret(String line){
	// Permet de séparer les mots d'une line de caractères
	Vector<String> Mot;
	int qteMots = splitString(line, ' ' , Mot);

  	if(qteMots == 1)return timers_menu();	

	if(qteMots == 2){
		if(Mot[1]== "menu")	return timers_menu();		       
		if(Mot[1]== "start") return timers_restart();	
		if(Mot[1]== "stop")	return timers_stop();    
		if(Mot[1]== "info")	return timers_intervals();        
    }

    return line + ("???\n");	    
}