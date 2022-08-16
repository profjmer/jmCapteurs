/**
 * @file application.h
 * @author Jean Mercier profjmer@gmail.com
 * @brief headers de application.cpp, le fichier de démarrage.
 * architecture jmObjean_2023
 * @version 1.0
 */
#ifndef INCLUDE_APPLICATION_H_
#define INCLUDE_APPLICATION_H_
	#define version_app "jmCapteurs-A23"

	#include <SmingCore/SmingCore.h>
	#include <Libraries/OneWire/OneWire.h>
	#include "../include/ds18s20.h"
	#include <Libraries/DHT/DHT.h>
	#include <WString.h>
	#include <stdio.h>
	#include "../include/pin_.h"


	#include "../include/config_.h"	
    #include "../include/param_.h"	
	#include "../include/adc_.h"
	#include "../include/bme280_.h"	
	#include "../include/gpio_.h"
	#include "../include/i2c_.h"
	#include "../include/mqtt_.h"	
	#include "../include/oled_.h"	
	#include "../include/ota_.h"
	#include "../include/power_.h"	
	#include "../include/pulse_.h"	
	#include "../include/routeur_.h"	
	#include "../include/rtc_.h"	
	#include "../include/station_.h"
	#include "../include/timers_.h"		
	#include "../include/spi_.h"	
	#include "../include/tft_.h"	
	#include "../include/uart_.h"
	#include "../include/udp_.h"	
	#include "../include/web_.h"
	#include "../include/dht22_.h"
	#include "../include/onewire_.h"
	#include "../include/ds1820_.h"


#endif /* INCLUDE_APPLICATION_H_ */