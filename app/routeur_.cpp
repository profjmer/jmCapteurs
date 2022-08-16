/*************************************************************************
 * @file	routeur_.c				 
 * @version	1.0
 * @date	2020-07-30
 * @brief
 		Module permettant de router les lignes de commandes recues des modules de communications
		vers les modules spécifiés par le premier mot de la ligne de commande.
		La réponse sous forme d'une String est retransmise vers le module de communication solliciteur.
*/
#include "../include/application.h"

String list = "\n\n************** Liste des Menus ***************\n\nota\nstation\nparam";

/**
 * @brief retourne le menu des modules pour ce projet
 * 
 * @return String 
 */
String menu(){return list + "\n***************************************\n";}

/**
 * @brief Construction du menu selon les modules utilisés
 * 
 */
void routeur_ini(){
	//Serial.println("routeur_ini()");
	#ifdef config_adc 
		list +="\nadc";
	#endif
	#ifdef config_gpio
		list +="\ngpio";
	#endif
	#ifdef config_i2c
		list +="\ni2c";
	#endif
	#ifdef config_oled
		list +="\noled";	
	#endif
	#ifdef config_timers
		list +="\ntimers";
	#endif
	#ifdef config_udp
		list +="\nudp";
	#endif	
	#ifdef config_rtc
		list +="\nrtc";
	#endif
	#ifdef config_bme280
		list +="\nbme";
	#endif	
	#ifdef config_dht22
		list +="\ndht";
	#endif	
	#ifdef config_ds1820
		list +="\nds";
	#endif	
	#ifdef config_onewire
		list +="\now";
	#endif	
	#ifdef config_tft
		list +="\ntft";
	#endif	
	#ifdef config_spi
		list +="\nspi";
	#endif	
	#ifdef config_touch
		list +="\ntouch";
	#endif	
	#ifdef config_max7219
		list +="\nmax7219";
	#endif
	#ifdef config_mqtt
		list +="\nmqtt";
	#endif		
	#ifdef config_ssr
		list +="\nssr";
	#endif	
	#ifdef config_power
		list +="\npower";
	#endif	
	#ifdef config_pulse
		list +="\npulse";
	#endif	
		
	list += "\nota\nparam\npin\nstation\ntimers\n";	// toujours défini
		
}


/** Commande routeur
 * @brief		Aiguilleur des lignes de commande et des réponses
 * @param[in]	Ligne de commande dont le premier mot est le module destinataire de la ligne de commande
 * ou bien une commande système
 * @returns		Retourne la réponse du module visé par la commande
 */
String routeur(String line)
{
	// Permet de séparer les mots d'une line de caractères
	Vector<String> Mot;
	int qteMots = splitString(line, ' ' , Mot);

	// liste des commandes possibles
	if(line =="menu" || line =="list")	return menu();
	
	if(Mot[0]== "pin") return pin_interpret(line); 

	// Modules
	#ifdef config_gpio
		else if(Mot[0]== "gpio") return gpio_interpret(line); 
	#endif
	#ifdef config_adc
	else	if(Mot[0]== "adc") return adc_interpret(line);
	#endif
	#ifdef config_i2c
		else if(Mot[0]== "i2c") return i2c_interpret(line);
	#endif
	#ifdef config_oled
		else if(Mot[0]== "oled") return oled_interpret(line);		
	#endif
	#ifdef config_timers
		else if(Mot[0]== "timers") return timers_interpret(line);
	#endif
	#ifdef config_parm
		if(Mot[0]== "param") return  param_interpret(line);	
	#endif
	#ifdef config_udp
		if(Mot[0]== "udp") return  udp_interpret(line);	
	#endif	
	#ifdef config_spi
		else if(Mot[0]== "spi") return  spi_interpret(line);	
	#endif
	#ifdef config_tft
		else if(Mot[0]== "tft") return  tft_interpret(line);	
	#endif	
	#ifdef config_rtc
		if(Mot[0]== "rtc") return  rtc_interpret(line);	
	#endif
	#ifdef config_bme280
		if(Mot[0]== "bme") return  bme280_interpret(line);	
	#endif
	#ifdef config_dht22
		if(Mot[0]== "dht") return  dht22_interpret(line);	
	#endif
	#ifdef config_ds1820
		if(Mot[0]== "ds") return  ds1820_interpret(line);	
	#endif
	#ifdef config_mqtt
		if(Mot[0]== "mqtt") return  mqtt_interpret(line);	
	#endif		
	#ifdef config_onewire
		if(Mot[0]== "ow") return  onewire_interpret(line);	
	#endif		
	#ifdef config_power
		if(Mot[0]== "power") return  power_interpret(line);	
	#endif
	#ifdef config_pulse
		if(Mot[0]== "pulse" || Mot[0]== "pu") return  pulse_interpret(line);	
	#endif
	#ifdef config_ssr
		if(Mot[0]== "ssr") return  ssr_interpret(line);	
	#endif		

	else if(Mot[0]== "param") return param_interpret(line);
	else if(Mot[0]== "station")return station_interpret(line);	
	else if(Mot[0]== "ota") return ota_interpret(line);		

			// Information sur ce projet
	else if(line == "espJSON?")	return param_chipJSON();
	else if(line == "esp?")	return param_chipInfo();	
		
	// retourne réponse
	return line +" ???\n";
	
}
