/**
 * @file dht22_.cpp
 * @author Jean Mercier profjmer@gmail.com
 * @brief Module de gestion du capteur DHT22
 * @version 1.0 
 */

#include "../include/application.h"
#ifdef config_dht22

	// tableau pour sauvegarder les valeurs
	#define qte 24
	float ttemperature[qte], thumidity[qte];
	String dht22_ttemps[qte];
	// structure pour sauver la lecture du capteur
	TempAndHumidity th; 
	String ts_dht;

	DHT dht(pin_DHT,DHT22);		// utilise la librairie DHT de Sming

	uint32_t interval_dht22;
	uint32_t interval_dht22_back;
	String dht22_units;

	Timer timerDHT22;
	bool dht22_jsonf;		// false format simple  ou true format influx
	bool dht22_UDPpub;
	bool dht22_MQTTpub;	


	/**
	 * @brief Retourne secondes, minutes ou heures pour l'unité du temps des intervalles de lecture du DHT22
	 * 
	 * @return String 
	 */
	String dht22_getTimeUnits(){
		return String(interval_dht22)+" "+dht22_units;
	}

	/**
	 * @brief Sélectionne secondes, minutes ou heures pour l'unité du temps des intervalles de lecture du DHT22
	 * Effectue une lecture par la fonction dht22_TRH() et
	 * Si le module tft est défini, met à jour l'intervalle de lecture du capteur DHT22 à l'écran
	 * 
	 * @param units secondes, minutes ou heures
	 * @return String 
	 */
	String dht22_setTimeUnits(String units){
		if(units == "secondes")  dht22_units = units ;
		else if(units == "minutes")  dht22_units = units ;
		else if(units == "heures")  dht22_units = units ;	
		else return "Mauvais units\n";

		timerDHT22.stop();
		timerDHT22.setIntervalMs(timers_interval_ms(interval_dht22,dht22_units));
		timerDHT22.restart();	
		dht22_TRH();
		#ifdef config_tft
			tft_showTimeUnits(dht22_getTimeUnits());
		#endif
	return "Lecture DHT22 aux "+dht22_getTimeUnits()+"\n";
	}

	/**
	 * @brief Retourne l'intervalle de lecture du capteur DHT22
	 * Effectue une lecture par la fonction dht22_TRH() et
	 * Si le module tft est défini, met à jour l'intervalle de lecture du capteur DHT22 à l'écran
	 * @return uint32_t 
	 */
	uint32_t dht22_getDHT22_interval(){return interval_dht22;}

	String dht22_setDHT22_interval(String interval){
		interval_dht22 = atol(interval.c_str());
		timerDHT22.stop();
		if(interval_dht22 == 0)return "Arrêt lecture et publications DHT22\n";
		interval_dht22_back = interval_dht22;
		timerDHT22.setIntervalMs(timers_interval_ms(interval_dht22,dht22_units));
		timerDHT22.restart();
		dht22_TRH();
		#ifdef config_tft
			tft_showTimeUnits(dht22_getTimeUnits());
		#endif	
		return "Lecture DHT22 aux "+dht22_getTimeUnits()+"\n";
	} 

	/**
	 * @brief Redémarre la lecture du capteur
	 * Effectue une lecture par la fonction dht22_TRH() et
	 * Si le module tft est défini, met à jour l'intervalle de lecture du capteur DHT22 à l'écran
	 * @return String 
	 */
	String dht22_timerRestart(){
		timerDHT22.stop();
		if(param_getFonctionnement()==faibleConsommation){
			interval_dht22 = 0;	
			return "";
		}
		interval_dht22 = interval_dht22_back;  // redemarrage avec ancienne intervalle

		timerDHT22.setIntervalMs(timers_interval_ms(interval_dht22,dht22_units));
		timerDHT22.restart();	
		dht22_TRH();
		#ifdef config_tft
			tft_showTimeUnits(dht22_getTimeUnits());
		#endif
		return "Lecture DHT22 aux "+String(dht22_getDHT22_interval())+" "+dht22_units+"\n";
	}

	/**
	 * @brief Arrêt de lecture du capteur DHT22
	 * 
	 * @return String 
	 */
	String dht22_timerStop(){
		interval_dht22_back =interval_dht22; // sauvegarde intervalle de publication
		timerDHT22.stop();
		interval_dht22 =0;	// arrêt des publications
		return "Arrêt lecture DHT22\n";	
	}

	/**
	 * @brief Retourne l'état de publication UDP 
	 * 
	 * @return String 
	 */
	String dht22_getPub_udp(){
		if(dht22_UDPpub ==true)return "Publication DHT22-UDP on\n";
		else return "Publication DHT22-UDP off\n";
	}

	/**
	 * @brief Active ou inactive la publication UDP
	 * 
	 * @param active 
	 * @return String 
	 */
	String dht22_setPub_udp(String active){
		if(active == "on"){dht22_UDPpub = true;udp_setPub("on");}
		else dht22_UDPpub = false;
		return dht22_getPub_udp();
	}

		
	String dht22_getPub_mqtt(){
		if(dht22_MQTTpub ==true)return "Publication DHT22-MQTT on\n";
		else return "Publication DHT22-MQTT off\n";
	}

	String dht22_setPub_mqtt(String active){
		if(active == "on"){dht22_MQTTpub = true;}
		else dht22_MQTTpub = false;
		return dht22_getPub_mqtt();
	}

	/**
	 * @brief Initialise de la minuterie de lecture du capteur DHT22
	 * Effectue une première lecture du capteur
	 */
	void dht22_timer_ini(){
		interval_dht22 =5;
		interval_dht22_back = interval_dht22;
		dht22_units = "secondes";
		timerDHT22.initializeMs(timers_interval_ms(interval_dht22,dht22_units), dht22_TRH).start();  
		dht22_TRH();
	}

	/**
	 * @brief Initialise le capteur DHT22
	 * 
	 */
	void dht22_ini(){
		dht.begin();
		dht22_timer_ini();
	}



	/**
	 * @brief Insère les valeurs de mesure du capteur DHT22 dans un tableau
	 * Pousse les dernières valeurs du capteurs dans les tableaux
	 * Les plus anciennes valeurs des tableaux sont écrasées
	 * Les nouvelles valeurs sont plaçées à la fin
	 * 
	 * @param Temps 
	 * @param Temperature 
	 * @param Humidity 
	 */
	void dht22_toHisto(String Temps, float Temperature, float Humidity)
	{
		int i;
		for(i=0;i<qte-1;i++){
			dht22_ttemps[i]= dht22_ttemps[i+1];
			ttemperature[i]=ttemperature[i+1];
			thumidity[i]=thumidity[i+1];
		}
		dht22_ttemps[i]= Temps;
		ttemperature[i]=Temperature;
		thumidity[i]=Humidity;
		//printHistorique();
	}

	/**
	 * @brief Retourne le tableau des mesures historique en format JSON
	 * 
	 * @return String 
	 */
	String dht22_histoJSON(){
		#ifndef config_rtc
			return "{\"type\":\"noRTC\"}";
		#endif
		int i;
		String rep = "{\"type\":\"DHT22Histo\",\"ts\":[\"";
		for(i=0;i<qte;i++){
			rep += dht22_ttemps[i];
			if(i<qte-1)rep+="\",\"";
		}
		rep+="\"],\"T\":[";
		
		for(i=0;i<qte;i++){
			rep += String(ttemperature[i]);
			if(i<qte-1)rep+=",";
		}
		rep+="],\"HR\":[";
		
		for(i=0;i<qte;i++){
			rep += String(thumidity[i]);
			if(i<qte-1)rep+=",";		
		} 

		rep+="]}";  
		return rep;
		
	}

	/**
	 * @brief Retourne les valeurs de la dernière lecture en format JSON
	 * 
	 * @return String 
	 */
	String dht22_last_JSON(){
		return "{\"type\":\"DHT22\",\"ts\":\""+ts_dht+"\",\"T\":" +String(th.temp,1)+",\"HR\":" +String(th.humid,0)+"}";  
	}

	/**
	 * @brief Retourne les valeurs de la dernière lecture en format JSON plus un LF
	 * 
	 * @return String  Les valeurs en format JSON avec un time stamp
	 */
	String dht22_JSON(){
		return dht22_last_JSON() + "\n";
	}

	/**
	 * @brief Retourne le champ du format InfluxDB pour la dernière lecture du capteur DHT22
	 * 
	 * @return String 
	 */
	String dht22_field(){
		return "\"field\":{\"Temperature\":"+String(th.temp,1)+",\"Humidity\":"+String(th.humid,0)+"},\"ts\":\""+ts_dht+"\"}";		
	}	  	  

	/**
	 * @brief Retourne si le format json InfluxDB est utilisé
	 * 
	 * @return String 
	 */
	String dht22_getJSONF(){
		if(dht22_jsonf)return "Format JSON INFLUX\n";
		else return "Format JSON simple\n";
	}

	/**
	 * @brief Active / inactive le format json InfluxDB
	 * 
	 * @param format on / off
	 * @return String 
	 */
	String dht22_setJSONF(String format){
		if(format=="off")dht22_jsonf = false;
		else dht22_jsonf =true;
		return dht22_getJSONF();
	}

	/**
	 * @brief retourne la dernière lecture du capteur DHT22 selon le format activé
	 * 
	 * @return String 
	 */
	String dht22_JSONF(){
		if(dht22_jsonf)return param_get_jmObjean_ID_JSON()+"\"Capteur\":\"DHT22\"},"+dht22_field();
		else return dht22_JSON();
	}

	/**
	 * @brief retourne la dernière lecture du capteur DHT22
	 * 
	 * @return String 
	 */
	String dht22_last(){
			String T(th.temp,1);
			String RH(th.humid,0);
			String rep = ("T "+T +"C  HR " + RH+"%");
			return rep;
	}

	/**
	 * @brief Effectue une lecture du capteur DHT22 et place les valeurs en RAM
	 * Si le module RTC est déféni, ajoute la lecture des valeurs à l'historique des mesures
	 * Si la publication UDP est ativée, publie les valeurs en UDP
	 * Envoi les valeurs aux clients du serveur WEB par websocket
	 * Si le module TFT est défini, met à jour l'afficheur TFT
	 * 
	 */
	void dht22_TRH(){
		ts_dht = "";

		if(!dht.readTempAndHumidity(th))
		{
			int error = dht.getLastError();
			if(error == errDHT_Timeout){
				//Serial.println("DHT timeout\n");
			}
			if(error == errDHT_Checksum){		
				//Serial.println("DHT checksum\n");
			}
			//Serial.println ("DHT erreur\n");
		}
		#ifdef config_rtc
			ts_dht = rtc_ts();
			dht22_toHisto(ts_dht,th.temp,th.humid);
		#endif
		if(dht22_UDPpub)udp_send(dht22_JSONF());
		web_wsSend(dht22_JSON());
		#ifdef config_tft
			tft_update(dht22);
		#endif	
		#ifdef config_mqtt
			if(dht22_MQTTpub){
				mqtt_pub(".dht22-t", String(th.temp,1));
				mqtt_pub(".dht22-hr", String(th.humid,0));																
			}
		#endif
	}

	/**
	 * @brief Effectue un lecture du DHT22 comme la fonction dht22_TRH() et retourne en plus le résultat au canal de communication ayant fait la requête
	 * 
	 * @return String 
	 */
	String dht22_lire(){
		dht22_TRH();
		return("T "+String(th.temp,1)+"C RH " +String(th.humid,0)+"%\n");
	}

	/**
	 * @brief retourne une double de la dernière lecture de la température du capteur DHT22
	 * 
	 * @return double 
	 */
	double dht22_temperature_last(){return (double)th.temp;}

	/**
	 * @brief retourne une double de la dernière lecture de l'humidité relative du capteur DHT22
	 * 
	 * @return double 
	 */
	double dht22_humidity_last(){return (double)th.humid;}

	/**
	 * @brief retourne le menu des commandes du module dht22_
	 * 
	 * @return String 
	 */
	String dht22_menu(){
		return "\ndht\ndht lire\ndht last\ndht json\ndht jsonf [off on]\ndht pubudp [off on]\ndht pubmqtt [off on]\ndht interval [0.59]\ndht intervalunits [secondes minutes heures]\ndht stop\ndht start\n";
	}

	/**
	 * @brief interpréteur de commandes du module dht22_
	 * 
	 * @param line 
	 * @return String 
	 */
	String dht22_interpret(String line){
		// Permet de séparer les mots d'une line de caractères
		Vector<String> Mot;
		int qteMots = splitString(line, ' ' , Mot);

		if(qteMots == 1) return dht22_menu();		
		
		if(qteMots == 2){
			if(Mot[1]== "menu")	return dht22_menu();		
			else if(Mot[1]== "last")	return dht22_last();		
			else if(Mot[1]== "lire")	return dht22_lire();		
			else if(Mot[1]== "json")	return dht22_JSON();
			else if(Mot[1]== "jsonf") 	return dht22_getJSONF();	
			else if(Mot[1]== "pubudp") 	return dht22_getPub_udp();
			else if(Mot[1]== "pubmqtt") 	return dht22_getPub_mqtt();								
			else if(Mot[1]== "histo")	return dht22_histoJSON();		
			else if (Mot[1]== "interval") return "Lecture DHT22 aux "+dht22_getTimeUnits()+"\n";
			else if (Mot[1]== "intervalunits") return dht22_units+"\n";		
			else if (Mot[1]== "stop") return dht22_timerStop(); 		
			else if (Mot[1]== "start") return dht22_timerRestart(); 
		}	

		if(qteMots == 3){
			if (Mot[1]== "pubudp") return dht22_setPub_udp(Mot[2]);		
			else if (Mot[1]== "pubmqtt") return dht22_setPub_mqtt(Mot[2]);				
			else if (Mot[1]== "interval") return dht22_setDHT22_interval(Mot[2]);
			else if (Mot[1]== "intervalunits") return dht22_setTimeUnits(Mot[2]);	
			else if(Mot[1]== "jsonf") 	return dht22_setJSONF(Mot[2]);			
		}
		
		return line + ("???\n");		
	}
#endif