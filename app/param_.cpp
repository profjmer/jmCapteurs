/**
 * @file param_.cpp
 * @author Jean Mercier profjmer@gmail.com
 * @brief gestion des paramètres du projet
 * @version 1.0
 * 
 */
#
#include "../include/application.h"

/** permet de personaliser le point d'accès **/
String nomDuPA = ota_WifiAccessPoint_name();
uint8_t canalWiFi =3;

/********************************************************* Paramètres d'identification du matériel ********************************************************/

String BOARD = "jmCApteurs-A22.BRD";   
String CHIP ="ESP8266-12S";  
String FIRMWARE = version_app;
String FLASH = "4MB";
String LIEU ="maison";

uint8_t fonctionnement;

/**
 * @brief Initialisation des paramètres et du fonctionnement par défaut
 * L'interptéteur de commande de chaque module peut modifier temporairement les paramètres associés au module
 * Le fichier config.ini peut modifier les paramètres par défaut au démarrage.
 * 
 */
void param_ini(){
	//Serial.println("param_ini()");
  param_setFonctionnement(eveil);
	//Serial.println(param_getModeFonctionnement());

}

/*****************************            PARAM Board and FIRMWARE              ****************/

/**
 * @brief retourne l'identification du PCB
 * 
 * @return String 
 */
String param_getBoardID(){
  return BOARD;
}

/**
 * @brief fixe le nom du PCB
 * 
 * @param id 
 * @return String 
 */
String param_setBoardID(String id){
  BOARD = id;
  return "Board id "+id+"\n";
}

/**
 * @brief identifie le lieu du PCB
 * 
 * @return String 
 */
String param_getLieu(){
  return LIEU;
}

/**
 * @brief Fixe le lieu du PCB
 * 
 * @param id 
 * @return String 
 */
String param_setLieu(String id){
  LIEU = id;
  return "Lieu "+id+"\n";
}

/**
 * @brief identifie la version du firmware
 * 
 * @return String 
 */
String  param_getFirmwareID(){
  return FIRMWARE;
}

/**
 * @brief retourne l'identification du microcontrôleur
 * 
 * @return String 
 */
String param_getChipID(){
  return CHIP;
}

/**
 * @brief fixe l'identification du microcontrôleur
 * 
 * @param id 
 * @return String 
 */
String param_setChipID(String id){
  CHIP = id;
  return "CHIP id "+id+"\n";
}

/**
 * @brief identifie les modules du firmware
 * 
 * @return String 
 */
String param_getModules(){
  String modules="Modules:";
  #ifdef config_adc 
		modules +=" adc";
	#endif
	#ifdef config_bme280
		modules +=" bme280";
	#endif	
	#ifdef config_dht22
		modules +=" dht22";
	#endif	
	#ifdef config_ds1820
		modules +=" ds1820";
	#endif		
	#ifdef config_gpio
		modules +=" gpio";
	#endif
	#ifdef config_i2c
		modules +=" i2c";
	#endif
	#ifdef config_oled
		modules +=" oled";
	#endif
	#ifdef config_param
		modules +=" param";
	#endif	
	#ifdef config_max7219
		modules +=" max7219";
	#endif
	#ifdef config_mqtt
		modules +=" mqtt";
	#endif			
	#ifdef config_power
		modules +=" power";
	#endif	
	#ifdef config_pulse
		modules +=" pulse";
	#endif
	#ifdef config_rtc
		modules +=" rtc";
	#endif	
	#ifdef config_spi
		modules +=" spi";
	#endif
	#ifdef config_ssr
		modules +=" ssr";
	#endif
	#ifdef config_tft
		modules +=" tft";
	#endif			
	#ifdef config_timers
		modules +=" timers";
	#endif
	#ifdef config_touch
		modules +=" touch";
	#endif	
	#ifdef config_udp
		modules +=" udp";
	#endif		
  #ifdef config_web
		modules +=" http ftp websocket";
	#endif	
  return modules +"\n";
}

/**
 * @brief identifie les modules du firmware en format json
 * 
 * @return String 
 */
String param_getModulesJSON(){
    String modulesJSON ="{\"modules\":[";
  #ifdef config_adc 
		modulesJSON +="\" adc\",";
	#endif
	#ifdef config_bme280
		modulesJSON +="\" bme280\",";
	#endif	
	#ifdef config_dht22
		modulesJSON +="\" dht22\",";
	#endif	
	#ifdef config_ds1820
		modulesJSON +="\" ds1820\",";
	#endif		
	#ifdef config_gpio
		modulesJSON +="\" gpio\",";
	#endif
	#ifdef config_i2c
		modulesJSON +="\" i2c\",";
	#endif
	#ifdef config_oled
		modulesJSON +="\" oled\",";
	#endif
	#ifdef config_max7219
		modulesJSON +="\" max7219\",";
	#endif
	#ifdef config_mqtt
		modulesJSON +="\" maqtt\",";
	#endif		
	#ifdef config_power
		modulesJSON +="\" power\",";
	#endif	
	#ifdef config_pulse
		modulesJSON +="\" pulse\",";
	#endif	
	#ifdef config_rtc
		modulesJSON +="\" rtc\",";
	#endif
	#ifdef config_spi
		modulesJSON +="\" spi\",";
	#endif		
	#ifdef config_ssr
		modulesJSON +="\" ssr\",";
	#endif		
	#ifdef config_tft
		modulesJSON +="\" tft\",";
	#endif		
		#ifdef config_timers
		modulesJSON +="\" timers\",";
	#endif
	#ifdef config_touch
		modulesJSON +="\" touch\",";
	#endif		
	#ifdef config_udp
		modulesJSON +="\" udp\",";
	#endif	
	#ifdef config_web
		modulesJSON +="\" http\", \"ftp\", \"ws\",";
	#endif	

	modulesJSON.remove(modulesJSON.length()-1);
  return modulesJSON +"]}\"\n";

}

/**
 * @brief identifie dimension de la flash
 * 
 * @return String 
 */
String param_getFLASH(){
  return FLASH ;
}

/**
 * @brief pour spécifier la dimension de la flash
 * 
 * @param flash 
 * @return String 
 */
String param_setFLASH(String flash){
  FLASH = flash;
  return "FLASH "+flash+"\n";
}

/**
 * @brief Get mode de fonctionnement
 * 
 * @return uint8_t 
 */
uint8_t param_getFonctionnement(){
  return fonctionnement;
}

/**
 * @brief Set mode de fonctionnement
 * 
 * @param mode 
 */
void param_setFonctionnement(uint8_t modeFonctionnement){
  fonctionnement=modeFonctionnement;
}

/**
 * @brief retourne le mode de fonctionnement: faible consommation ou éveil permanent
 * 
 * @return String 
 */
String param_getModeFonctionnement(){
	#ifdef config_rtc
		if(fonctionnement == faibleConsommation)return "faible consommation";
	#endif
	return "éveil permanent";
}

/**
 * @brief fixe le mode de fonctionnement: 
 * 
 * @param modeFonctionnement: faibleconsommation eveil 
 * @return String 
 */
String param_setModeFonctionnement(String modeFonctionnement){
	fonctionnement=(uint8_t)atoi(modeFonctionnement.c_str());
	return param_getModeFonctionnement();
}
 

/**
 * @brief Menu des commandes du module param_
 * 
 * @return String 
 */
String param_menu(){ return"\nparam\nparam info\nparam wifi\nparam canal [1..11]\nparam board [string]\nparam chip [string]\nparam lieu\nparam firmware\nparam modules\nparam mode [0..1]\n";}


/**
 * @brief Information pour cette puce ESP
 * 
 * @return String 
 */
String id(){return ",\"id\":\"ESP8266\",\"version_app\":\""+String(version_app)+"\",\"MAC\":\""+WifiStation.getMAC()+"\",\"station\":\""
  +WifiStation.getIP().toString()+"\",\"ota\":\""+ota_getIP()+"}";}

String param_ackChipJSON(){return "{\"type\":\"ack\""+id();}
String param_chipJSON(){return "{\"type\":\"chip\""+id();}
String param_chipInfo(){return "chip: ESP8266, version_app: "+String(version_app)+", MAC: "+WifiStation.getMAC()+", station: "+WifiStation.getIP().toString()+" ota: "
  +ota_getIP()+"\n";}

/**
 * @brief information json du projet
 * 
 * @return String information du projet
 */
String param_get_jmObjean_ID_JSON(){return "{\"tag\":{\"BOARD\":\""+param_getBoardID()+ "\",\"LIEU\":\""+param_getLieu()+ "\",\"CHIP\":\""+param_getChipID()+"\",\"FIRMWARE\":\""+param_getFirmwareID()+"\",\"MAC\":\""+WifiStation.getMAC()+"\",\"STATION\":\""+WifiStation.getIP().toString()+"\",\"OTA\":\""+ota_getIP()+"\",";}

String param_wifi(){
	if(WifiAccessPoint.isEnabled()){
		String canal = String(param_getCanalWiFi());
		return "PA 192.168.4.1 C"+ canal;
	}
	if(WifiStation.isConnected() ){
		return "STA IP "+WifiStation.getIP().toString();
	}
}

/**
 * @brief Retourne les paramètres de configuration
 * 
 * @return String 
 */
String param_info(){
  return  "\nParamètres\nparam_:\nPROJET PA: " + ota_WifiAccessPoint_name()+
		"\nWIFI- "+ param_wifi() +
		"\nBOARD "+param_getBoardID()+
    "\nCHIP "+param_getChipID()+
    "\nFIRMWARE "+param_getFirmwareID()+
		"\nLIEU "+param_getLieu()+
    "\nFLASH "+FLASH+
		"\nMODE " +param_getModeFonctionnement()+
		"\n"+param_getModules()+
		"\nTimers:\n"+timers_intervals() + 
		#ifdef config_udp
    	"\nudp_:"+udp_info()+
		#endif
		#ifdef config_mqtt
    	"\nmqtt_:"+mqtt_info()+
		#endif
		"\n";

}

/**
 * @brief retourne le canal du point d'accès Wifi du ESP8266
 * 
 * @return String feedback
 */
uint8_t param_getCanalWiFi(){
  return canalWiFi;
}

/**
 * @brief fixe le canal WiFi en mode point d'accès
 * 
 * @param canal 1..11
 * @return String 
 */
String param_setCanalWiFi(String canal){
  int no = (uint8_t)atoi(canal.c_str());
  if(no  >=1 && no <= 11){
    canalWiFi =no;
    return "ota ap pour modifier temporairement\nconfig.ini pour modification permanente\n";
  }
  return "Erreur de canal\n";
}

/**
 * @brief retourne le nom du point d'accès Wifi du microcontrôleur
 * 
 * @return String 
 */
String param_getNomDuPA(){
  return nomDuPA;
}

String param_email(){
  return "profjmer@gmail.com\n";
}

/****************************** Interpreteur **********************/

/**
 * @brief Interpréteur de commandes du module param_
 * 
 * @param line 
 * @return String feedback
 */
String param_interpret(String line){
  // Permet de séparer les mots d'une ligne de caractères
  Vector<String> Mot;
  int qteMots = splitString(line, ' ' , Mot);
  if(qteMots == 1)return param_menu();

  if(qteMots == 2){
    if(Mot[1]== "menu")	return param_menu();	   
    else if(Mot[1]== "info")	return param_info();	   
    else if(Mot[1]== "board"){return param_getBoardID()+"\n";} 
    else if(Mot[1]== "chip"){return param_getChipID()+"\n";}  
    else if(Mot[1]== "firmware"){return param_getFirmwareID()+"\n";}  
    else if(Mot[1]== "lieu"){return param_getLieu()+"\n";}  		
    else if(Mot[1]== "mode"){return param_getModeFonctionnement()+"\n";}  		
    else if(Mot[1]== "modules"){return param_getModules()+"\n";}  
    else if(Mot[1]== "modulesJSON"){return param_getModulesJSON()+"\n";}      
    else if(Mot[1]== "id") return param_get_jmObjean_ID_JSON()+"\n";   
    else if(Mot[1]== "pa"){return "Point d'Accès: " +param_getNomDuPA()+"\n";} 
    else if(Mot[1]== "canal"){return "Canal WiFi: "+String(param_getCanalWiFi())+"\n";}     
    else if(Mot[1]== "wifi")return param_wifi()+"\n";	             	      
  }


  if(qteMots == 3){    
    // Modifier les paramètres pa et canal du fichier jmObjeanParam.ini
    if(Mot[1]=="pa"){
      if(Mot[2] == "?")nomDuPA=ota_WifiAccessPoint_name();
      else nomDuPA = Mot[2];
      return "Nom du PA: " +param_getNomDuPA()+"\n";
    } 

    else if(Mot[1]=="canal")return param_setCanalWiFi(Mot[2]);
    else if(Mot[1]== "board") return param_setBoardID(Mot[2]);  
    else if(Mot[1]== "chip") return param_setChipID(Mot[2]); 
    else if(Mot[1]== "lieu") return param_setLieu(Mot[2]);  		  
		else if(Mot[1]== "mode") return param_setModeFonctionnement(Mot[2]); 
  }
  return line + ("???\n");
}