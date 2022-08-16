/**
 * @file application.cpp
 * @author Jean Mercier profjmer@gmail.com
 * @brief Module d'entrée du projet. Dépend du module de configuration config.h
 * @version 1.0
 * 
 */
#include "../include/application.h"

/**
 * @brief lorsque le système est prêt, initialise les modules définis dans config.h
 * 
 */
void modules_ini(){
	#ifdef config_gpio
		gpio_ini();
	#endif
	#ifdef config_i2c
		i2c_ini();
	#endif
	#ifdef config_spi
		spi_ini();
	#endif
	#ifdef config_max7219
		 max7219_ini();
	#endif	
	#ifdef config_mqtt
		 mqtt_ini();
	#endif			
	#ifdef config_oled
		oled_ini();			
	#endif
	#ifdef config_param
		 param_ini();
	#endif	
	#ifdef config_power
		 power_ini();
	#endif	
	#ifdef config_pulse
		 pulse_ini();
	#endif		
	#ifdef config_rtc
		 rtc_ini();
	#endif
	#ifdef config_ssr
		 ssr_ini();
	#endif	
	#ifdef config_tft
		 tft_ini();
	#endif	
	#ifdef config_timers
		 timers_ini();
	#endif	
	#ifdef config_touch
		 touch_ini();
	#endif	
	#ifdef config_udp
		udp_ini();
	#endif	
	#ifdef config_web
		web_ini();	
	#endif	
	#ifdef config_adc 
		adc_ini();
	#endif
	#ifdef config_bme280
		 bme280_ini();
	#endif	
	#ifdef config_dht22
		 dht22_ini();
	#endif	
	#ifdef config_onewire
		 onewire_ini();
	#endif	
	#ifdef config_ds1820
		 ds1820_ini();
	#endif
	#ifdef config_uart
		uart_ini();	
	#endif

	pin_ini();


	// conserver en dernier
	station_ini();
	routeur_ini();

	config_execute(config_charge("config.ini"));		
}

void init(){
	#ifdef config_uart
		Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	#endif	

	#ifndef config_uart
		// disable all debug output for release builds
		Serial.systemDebugOutput(false); // Debug output to serial
		system_set_os_print(0);
	#endif

	ota_mountSPIFFS();
	System.onReady(modules_ini);	


}


