/**
 * @file config_.h
 * @author Jean Mercier profjmer@gmail.com
 * @brief header du module de configuration initiale, architecture jmObjean_2023 sur WEMOS D1
 * Permet de charger un fichier de configuration de la flash et de reconfigurer l'objet
 * En plus ce fichier est utilisé pour définir les modules qui seront compilés.
 */
#ifndef INCLUDE_CONFIG_H_
#define INCLUDE_CONFIG_H_
	String config_charge(String file);
    void config_execute(String commandes);    

    /* ********* définir les capteurs utilisés dans ce projet ***************/
    #define config_adc 
    #define config_bme280
    #define config_dht22 
    #define config_ds1820  
    #define config_onewire  

    /*  définir les modules utilisés dans ce projet ************************/
    #define config_param
    #define config_i2c 
    #define config_oled 
    #define config_ota 
    #define config_routeur 
    #define config_station 
    #define config_timers
    #define config_uart       
    #define config_udp     
    #define config_web      
    
    //#define config_spi 
    //#define config_tft 
    //#define config_touch 
    //#define config_max7219 

    //#define config_rtc 
    //#define config_eeprom 

    //#define config_gpio
    //#define config_pulse 
    //#define config_ssr 
    //#define config_power         
  
    #define config_mqtt 


#endif /* INCLUDE_CONFIG_H_ */

