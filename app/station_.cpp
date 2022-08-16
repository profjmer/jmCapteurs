/**
 * @file station_.cpp
 * @author Jean Mercier profjmer@gmail.com
 * @brief Module station_ de gestion de la station WiFi
 * @version 1.0
 */

#include "application.h"

BssList networks;
String network, password;
Timer station_retardExec_timer;

uint16_t station_retardExec_delai = 500; // ms

/**
 * @brief menu des commandes du module station_
 * 
 * @return String 
 */
String station_menu(){return "\nstation\nstation info\nstation active\nstation inactive\nstation autocon [0..1]\nstation ssid\nstation scan\nstation joindre ssid [ssid mot_de_passe]\nstation seule\nstation ip\n";}

/**
 * @brief reset du microcontrôleur
 * 
 */
void station_systemRestart(){
	//Serial.println("Redémarrage station");	
	System.restart();
}

/**
 * @brief déconnexion et désactivation de la station dans le réseau WiFi
 * 
 */
void station_fermeture(){
	wifi_station_disconnect();
	WifiStation.enable(false);
}

/**
 * @brief démarre recherche de points d'accès WiFi
 * 
 * @return String 
 */
String station_startScan(){
	networks.removeAllElements();
	WifiStation.startScan(station_networkScanCompleted); 
	return "Recherche réseaux\n";
}


/**
 * @brief Initialisation du module station_
 * 
 */
void station_ini(){
	//Serial.println("station_ini()");

	WifiAccessPoint.enable(false);
	WifiStation.enable(true);
		
	// Set callback that should be triggered when we have assigned IP
	WifiEvents.onStationGotIP(station_connectOk);

	// Set callback that should be triggered if we are disconnected or connection attempt failed
	WifiEvents.onStationDisconnect(station_connectFail);

	WifiStation.startScan(station_networkScanCompleted); 		
}

/**
 * @brief Inactive le point d'accès WiFi du microcontrôleur
 * 
 * @return String 
 */
String station_seule(){
	WifiAccessPoint.enable(false);
	return "Inactive le point d'accès\n";
}

/**
 * @brief Retrouve les points d'accès WiFI dans l'environnement
 * 
 * @param succeeded 
 * @param list 
 */
void station_networkScanCompleted(bool succeeded, BssList list)
{ 
	if (succeeded)
	{
		for (int i = 0; i < list.count(); i++)
			if (!list[i].hidden && list[i].ssid.length() > 0)
				networks.add(list[i]);
	}
	networks.sort([](const BssInfo& a, const BssInfo& b){ return b.rssi - a.rssi; } );
	//Serial.println("Fin de la recherche réseaux");

	if(WifiStation.isConnected() || WifiAccessPoint.isEnabled()){
		if(web_getActiveClients() >0){
			web_wsSend(station_ssid());
		}
	}
	else ota_activeAccessPoint();	
}

/**
 * @brief État de la station dans le réseau actuel
 * 
 * @return json de l'état actuel de la station et son adresse ip
 */
String station_info(){
	bool connected = WifiStation.isConnected();
	String rep;
	rep ="{\"type\":\"reseau\",\"actuel\":\""+WifiStation.getSSID()+"\",\"actif\":";
	if(connected){
		rep += "true,\"IP\":\""+WifiStation.getIP().toString()+"\"}\n";
	}
	else rep += "false,\"IP\":\"0.0.0.0\"}\n";

	return rep;
}

/**
 * @brief À faire lorsque la station se conecte au réseau
 * 
 */
void station_connectOk(IPAddress ip, IPAddress mask, IPAddress gateway)
{
	//Serial.println("Station connectée, IP "+WifiStation.getIP().toString());

	// tâches a effectuer à la connexion
	udp_broadcastStation_ip(param_chipJSON());

	if(WifiAccessPoint.isEnabled()){
		if(web_getActiveClients() >0){
			web_wsSend(station_info());
		}
	}
	WifiAccessPoint.enable(false); // fermeture du point d'accès
}

/**
 * @brief À faire lorsque la station ne peut se connecter
 * 
 */
void station_connectFail(String ssid, uint8_t ssidLength, uint8_t* bssid, uint8_t reason)
{
	//Serial.println("Station inaccessible");
	WifiStation.enable(false);
	//Serial.println("Inactive station et active "+ String(version_app));
	ota_activeAccessPoint();
}

/**
 * @brief Retourne la liste des réseaux en format json
 * 
 * @return String 
 */
String station_ssid(){
	uint8_t qte =  networks.count();
	bool connected = WifiStation.isConnected();	
	String rep = "{\"type\":\"reseaux\", \"qte\":"+String(qte);
	if(qte == 0) return rep += "}\n";

	rep += ",\"actuel\":\""+WifiStation.getSSID()+"\",\"actif\":";
	if(connected){
		rep += "true";
	}
	else rep += "false";
	
	rep += ",\"liste\":[";

	for (int i = 0; i < qte; i++)
	{
		if (networks[i].hidden) continue;

		rep += "{\"titre\":\""+String(networks[i].ssid);
		rep += "\",\"id\":"+ String((int)networks[i].getHashId());
		rep += ",\"rssi\":"+ String(networks[i].rssi);
		rep += ",\"encodage\":\""+ String(networks[i].getAuthorizationMethodName());
		rep += "\"}";
		if(i<qte-1) rep+=",";	
	}

	rep += "]}\n";

	//Serial.println(rep);	
	return rep;
}

/**
 * @brief joindre la station à un point d'accès externe avec mot de passe
 * 
 * @param ssid 
 * @param password 
 * @return String 
 */
String station_joindrePassword(String ssid, String password){
	Serial.println("station_ joindre "+ ssid);
	timers_stop();	
	WifiAccessPoint.enable(false);

	WifiStation.config(ssid, password,true,true);
	WifiStation.enable(true);
	wifi_station_set_auto_connect(true);

	// retarder le system restart afin de transmettre le message
	station_retardExec_timer.initializeMs(station_retardExec_delai, station_systemRestart).startOnce();	

	return "Essai de joindre réseau " +ssid +"\n";
}

/**
 * @brief inactive la connexion de la station et active le point d'accès WiFi du microcontrôleur
 * 
 * @return String 
 */
String station_inactive(){
	WifiAccessPoint.enable(true);
	wifi_station_set_auto_connect(false);

	// retarde la fermeture afin de transmettre le message
	station_retardExec_timer.initializeMs(station_retardExec_delai, station_fermeture).startOnce();	
	return "Inactive la station et active le PA\n";
}

/**
 * @brief rretourne état d'auto connexion de la station
 * 
 * @return String 
 */
String station_getAutoConnect(){
	if(wifi_station_get_auto_connect())return "station auto connexion active\n";
	else return "station auto connexion inactive\n";
}

/**
 * @brief active / inactive l'autoconnexion de la station
 * 
 * @param s 1/0
 * @return String 
 */
String station_setAutoConnect(String s){
	uint8_t etat=(int8_t)atoi(s.c_str());
	wifi_station_set_auto_connect(etat);
	return station_getAutoConnect();
}

/**
 * @brief active la station dans le réseau WiFi sélectionné
 * inactive le point d'accès du microcontrôleur
 * 
 * @return String 
 */
String station_active(){
	wifi_station_connect();
	WifiStation.enable(true);
	WifiAccessPoint.enable(false);

	// retarder le system restart afin de transmettre le message
	station_retardExec_timer.initializeMs(station_retardExec_delai, station_systemRestart).startOnce();	

	return "Active la station Wifi, inactive le PA\n";
}

/**
 * @brief interpréteur de commandes du module station_
 * 
 * @param line 
 * @return String 
 */
String station_interpret(String line){
	// Permet de séparer les mots d'une line de caractères
	Vector<String> Mot;
	int qteMots = splitString(line, ' ' , Mot);

	if(qteMots == 1){
		return station_menu();
	}

	if(qteMots == 2){
		if(Mot[1]== "menu")	return station_menu();
		if(Mot[1]== "info")	return station_info();		
		if(Mot[1]== "ssid")	return station_ssid();
		if(Mot[1]== "ip")	return station_info();	
		if(Mot[1]== "seule")	return station_seule();							
		if(Mot[1]== "scan")	return station_startScan();	
		if(Mot[1]== "inactive")	return station_inactive();	
		if(Mot[1]== "active")	return station_active();	
		if(Mot[1]== "autocon")	return station_getAutoConnect();										
	}
	if(qteMots == 3){
		if(Mot[1]== "joindre"){
			return station_joindrePassword(Mot[2],"");
		}

		if(Mot[1]== "autocon"){
			return station_setAutoConnect(Mot[2]);
		}
	}

	if(qteMots == 4){
		if(Mot[1]== "joindre"){
			return station_joindrePassword(Mot[2],Mot[3]);
		}
	}
  
  return line + ("???\n");	    
}