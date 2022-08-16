/**
 * @file bme280_.cpp
 * @author Jean Mercier profjmer@gmail.com
 * @brief Module de gestion du BME280, dépend de la librairie Adafruit_BME280
 * @version 1.0
 */

#include "../include/application.h"

#ifdef config_bme280

	#include "../include/Adafruit_BME280.h"

	// tableaux pour sauvegarder les valeurs
	#define qte 24
	float bmeTemperature[qte], bmePression[qte], bmeHumidity[qte];
	String bmeTemps[qte];

	Adafruit_BME280 bme;
	String t,p,h,ts_bme;
	float temp,pression,humid;
	String bme280_time;

	uint32_t interval_bme280;
	String bme280_units;
	Timer timerBME280;
	bool bme280_jsonf;		// false format simple  ou true format influx
	bool bme280_UDPpub;
	bool bme280_MQTTpub;

	/**
	 * @brief Retourne l'intervalle numérique du temps entre les mesures
	 * 
	 * @return uint32_t 
	 */
	uint32_t bme280_getBME280_interval(){return interval_bme280;}    


	/**
	 * @brief Retourne le temps utilisée pour les intervalles de mesures
	 * 
	 * @return String 
	 */
	String bme280_getTimeUnits(){
		return String(interval_bme280)+" "+bme280_units;
	}

	/**
	 * @brief Ajuste l'unité de temps utilisée pour les intervalles de mesures
	 * effectue une lecture par la fonction bme280_TPH() et 
	 * si le module TFT est défini, met à jour l'intervalle de lecture du capteur BME280 à l'écran du TFT
	 * 
	 * @param units  secondes, minutes, heures
	 * @return String 
	 */
	String bme280_setTimeUnits(String units){
		if(units == "secondes")  bme280_units = units ;
		else if(units == "minutes")  bme280_units = units ;
		else if(units == "heures")  bme280_units = units ;	
		else return "Mauvais units\n";

		timerBME280.stop();
		timerBME280.setIntervalMs(timers_interval_ms(interval_bme280,bme280_units));
		timerBME280.restart();	

		bme280_TPH();		// effectue les mesures T P et RH
		#ifdef config_tft
			tft_showTimeUnits(bme280_getTimeUnits());
		#endif
		return "Lecture BME280  aux "+bme280_getTimeUnits()+"\n";
	}

	/**
	 * @brief Ajuste la valeur numérique de temps utilisée pour les intervalles de mesures.
	 * Effectue une lecture par la fonction bme280_TPH() et 
	 * si le module TFT est défini, met à jour l'intervalle de lecture du capteur BME280 à l'écran du TFT
	 * 
	 * @param interval 
	 * @return String 
	 */
	String bme280_setBME280_interval(String interval)
	{
		timerBME280.stop();
		interval_bme280 = atol(interval.c_str());
		timerBME280.setIntervalMs(timers_interval_ms(interval_bme280,bme280_units));
		timerBME280.restart();
		bme280_TPH();
		#ifdef config_tft
			tft_showTimeUnits(bme280_getTimeUnits());
		#endif
		return "Lecture BME280  aux "+bme280_getTimeUnits()+"\n";
	}

	/**
	 * @brief Redémarre la minuterie des lectures des mesures du BME280
	 * Effectue une lecture par la fonction bme280_TPH() et 
	 * si le module TFT est défini, met à jour l'intervalle de lecture du capteur BME280 à l'écran du TFT
	 * @return String 
	 */
	String bme280_timerRestart()
	{
		timerBME280.stop();
		if(param_getFonctionnement()==faibleConsommation){
			interval_bme280 = 0;
			return "";     
		}
		timerBME280.setIntervalMs(timers_interval_ms(interval_bme280,bme280_units));    
		timerBME280.restart();
		bme280_TPH();
		#ifdef config_tft
			tft_showTimeUnits(bme280_getTimeUnits());
		#endif
		return "Lecture BME280  aux "+bme280_getTimeUnits()+"\n";
	}   

	/**
	 * @brief Arrêt des lectures des mesures du BME280
	 * 
	 * @return String 
	 */
	String bme280_timerStop()
	{
		timerBME280.stop();
		return "Arrêt lecture BME280\n";
	} 

	/**
	 * @brief Retourne l'état de publication des mesures du BME280 par UDP
	 * 
	 * @return String 
	 */
	String bme280_getPub_udp(){
		if(bme280_UDPpub ==true)return "Publication BME280-UDP on\n";
		else return "Publication BME280-UDP off\n";
	}

	/**
	 * @brief Active ou inactive la publication des mesures du BME280 par UDP
	 * 
	 * @param active 
	 * @return String 
	 */
	String bme280_setPub_udp(String active){
		if(active == "on"){bme280_UDPpub = true;udp_setPub("on");}
		else bme280_UDPpub = false;
		return bme280_getPub_udp();
	}

	String bme280_getPub_mqtt(){
		if(bme280_MQTTpub ==true)return "Publication BME280-MQTT on\n";
		else return "Publication BME280-MQTT off\n";
	}

	String bme280_setPub_mqtt(String active){
		if(active == "on"){bme280_MQTTpub = true;}
		else bme280_MQTTpub = false;
		return bme280_getPub_mqtt();
	}

	/**
	 * @brief Initialisation du timer de lecture du bme280
	 * 
	 */
	void bme280_timer_ini(){
		//Serial.println("bme280_timer_ini()");
		interval_bme280 =5;
		bme280_units="secondes";
		timerBME280.initializeMs(timers_interval_ms(interval_bme280,bme280_units), bme280_TPH).start();  
	}

	/**
	 * @brief Initialisation du module bme280_
	 * 
	 */
	void bme280_ini(){
		//Serial.println("bme280_ini()");
		if (!bme.begin(0x76)) {  
			//Serial.println("BME280 ?, vérifier câblage!");
			return;
		}
		bme280_timer_ini();
	}



	/**
	 * @brief Gère l'historique des mesures dans le tableau des mesures
	 * Pousse les dernières valeurs du capteurs dans les tableaux
	 * Les plus anciennes valeurs des tableaux sont écrasées
	 * Les nouvelles valeurs sont plaçées à la fin
	 * 
	 * @param Temps 
	 * @param Temperature 
	 * @param Humidity 
	 * @param Pression 
	 */
	void bme280_toHisto(String Temps, float Temperature, float Humidity, float Pression)
	{
		int i;
		for(i=0;i<qte-1;i++){
			bmeTemps[i]= bmeTemps[i+1];
			bmeTemperature[i]=bmeTemperature[i+1];
			bmeHumidity[i]=bmeHumidity[i+1];
			bmePression[i]=bmePression[i+1];        
		}
		bmeTemps[i]= Temps;
		bmeTemperature[i]=Temperature;
		bmeHumidity[i]=Humidity;
		bmePression[i]=Pression;    
		//printHistorique();
	}

	/**
	 * @brief Retourne le tableau des valeurs des mesures du BME280 en format JSON
	 * 
	 * @return String 
	 */
	String bme280_histoJSON(){
		#ifndef config_rtc
			return "{\"type\":\"noRTC\"}";
		#endif
		int i;
		String rep = "{\"type\":\"BME280Histo\",\"ts\":[\"";
		for(i=0;i<qte;i++){
			if(bmeTemps[i] == "")continue;
			rep += bmeTemps[i];
			if(i<qte-1)rep+="\",\"";
		}
		rep+="\"],\"T\":[";
		
		for(i=0;i<qte;i++){
			if(bmeTemps[i] == "")continue;
			rep += String(bmeTemperature[i],1);
			if(i<qte-1)rep+=",";
		}
		rep+="],\"HR\":[";
		
		for(i=0;i<qte;i++){
			if(bmeTemps[i] == "")continue;		
			rep += String(bmeHumidity[i],0);
			if(i<qte-1)rep+=",";		
		} 

		rep+="],\"P\":[";

	for(i=0;i<qte;i++){
			if(bmeTemps[i] == "")continue;
			rep += String(bmePression[i],1);
			if(i<qte-1)rep+=",";		
		} 

		rep+="]}";  
		return rep;	
	}

	/**
	 * @brief Retourne le menu des commandes du module bme280_
	 * 
	 * @return String 
	 */
	String bme280_menu(){
		return "\nbme\nbme lire\nbme last\nbme json\nbme jsonf [off on]\nbme pubudp [off on]\nbme pubmqtt [off on]\nbme histo\nbme t\nbme p\nbme h\nbme interval [0.59]\nbme intervalunits [secondes minutes heures]\nbme stop\nbme start\n";
	}

	/**
	 * @brief Effectue une lecture des mesures du BME280
	 * Place les valeurs en mémoire
	 * Si le RTC est défini, l'historique est mis a jour
	 * Si la publication UDP est active, les valeurs sont publiées en format JSON
	 * Publie les valeurs sur les clients du serveur HTTP par websocket
	 * Si le TFT est défini, met à jour l'écran du TFT
	 * 
	 */
	void bme280_TPH(){
		//Serial.println("bme280_TPH()");
		ts_bme = "";
		temp = bme.readTemperature();
		pression = bme.readPressure()/1000;
		humid = bme.readHumidity();
		t = String(temp,1);
		p = String(pression,1);
		h = String(humid,0);  
		#ifdef config_rtc
			ts_bme = rtc_ts();
			bme280_toHisto(ts_bme,temp, humid, pression);	
		#endif
		#ifdef config_udp
			if(bme280_UDPpub)udp_send(bme280_JSONF());
		#endif
		web_wsSend(bme280_JSON());
		#ifdef config_tft
			tft_update(bme280);
		#endif
		#ifdef config_mqtt
			if(bme280_MQTTpub){
				mqtt_pub(".bme280-t", t);
				mqtt_pub(".bme280-hr", h);
				mqtt_pub(".bme280-p", p);																
			}
		#endif
	}

	/**
	 * @brief Effectue un lecture du BME280 comme la fonction bme280_TPH() et retourne en plus le résultat au canal de communication ayant fait la requête
	 * 
	 * @return String 
	 */
	String bme280_lire(){
		bme280_TPH();
		return "T "+t+"C  P "+p+" kPa  H "+h+" %";
	}

	/**
	 * @brief lance une acquisition et retourne la temperature
	 * 
	 * @return String 
	 */
	String bme280_temperature(){
		float temp;
		temp = bme.readTemperature();
		t = String(temp,1);
		return t;
	}

	/**
	 * @brief lance une acquisition et retourne la pression divisée par 1000
	 * 
	 * @return String 
	 */
	String bme280_pression(){
		float pression;
		pression = bme.readPressure()/1000;
		p = String(pression,1);
		return p;
	}

	/**
	 * @brief lance une acquisition et retourne l'humidité relative
	 * 
	 * @return String 
	 */
	String bme280_humidity(){
		float humidity;
		humidity = bme.readHumidity();
		h = String(humidity,1);
		return h;
	}

	// lance une acquisition, sauve dans l'historique et retourne résultat pour client UDP
	/**
	 * @brief Retourne les dernières lecture du BME280 en format JSON
	 * 
	 * @return String 
	 */
	String bme280_JSON(){
		return	"{\"type\":\"BME280\",\"Temperature\":"+t+",\"Pression\":"+p+",\"Humidity\":"+h+",\"ts\":\""+ts_bme+"\"}";
		
	}

	/**
	 * @brief Retourne le type de format JSON utilisé: simple ou InfluxDB
	 * 
	 * @return String 
	 */
	String bme280_getJSONF(){
		if(bme280_jsonf)return "Format JSON INFLUX\n";
		else return "Format JSON simple\n";
	}

	/**
	 * @brief Sélection du format InfluxDB
	 * 
	 * @param format  off on
	 * @return String 
	 */
	String bme280_setJSONF(String format){
		if(format=="off")bme280_jsonf = false;
		else bme280_jsonf =true;
		return bme280_getJSONF();
	}

	/**
	 * @brief Retourne les mesures du capteur BME280 en format InfluxDB
	 * 
	 * @return String 
	 */
	String bme280_JSONF(){
		if(bme280_jsonf)return param_get_jmObjean_ID_JSON()+"\"Capteur\":\"BME280\"},"+bme280_field();
		else return bme280_JSON();
	}

	/**
	 * @brief Retourne la dernière acquisition des mesures du BME280 en format JSON
	 * 
	 * @return String 
	 */
	String bme280_last_JSON(){
		return "{\"type\":\"BME280\",\"Temperature\":"+t+",\"Pression\":"+p+",\"Humidity\":"+h+",\"ts\":\""+ts_bme+"\"}";
	}

	/**
	 * @brief retourne le champ du format influxDB des valeurs de la dernière lecture du capteur bme280
	 * 
	 * @return String 
	 */
	String bme280_field(){
		return "\"field\":{\"Temperature\":"+t+",\"Pression\":"+p+",\"Humidity\":"+h+"},\"ts\":\""+ts_bme+"\"}";		
	}	  	  


	/**
	 * @brief retourne les résultats de la dernière acquisition pour afficheur OLED
	 * 
	 * @return String 
	 */
	String bme280_last(){
		//Serial.println("bme280_last()");
		return "T"+t+" P"+p+" H"+h+"\n";
	}

	/**
	 * @brief Retourne une valeur numérique double pour la lecture de la température du capteur BM280
	 * 
	 * @return double 
	 */
	double bme280_temperature_last(){return (double)temp;}

	/**
	 * @brief Retourne une valeur numérique double pour la lecture de la pression du capteur BM280
	 * 
	 * @return double 
	 */
	double bme280_pression_last(){return (double)pression;}

	/**
	 * @brief Retourne une valeur numérique double pour la lecture de l'humidité relative du capteur BM280
	 * 
	 * @return double 
	 */
	double bme280_humidity_last(){return (double)humid;}

	/**
	 * @brief Pour afficher la dernière acquisition des mesures du capteur BM280
	 * 
	 */
	void bme280_printdata(){   
		//Serial.println(bme280_last());
	}

	/**
	 * @brief Interpréteur de commandes du module bme280_
	 * 
	 * @param line 
	 * @return String 
	 */
	String bme280_interpret(String line){
		// Permet de séparer les mots d'une line de caractères
		Vector<String> Mot;
		int qteMots = splitString(line, ' ' , Mot);

		if(qteMots == 1){ return bme280_menu();	}

		if(qteMots == 2){
			if(Mot[1]== "menu")	return bme280_menu();		
			else if(Mot[1]== "last")	return bme280_last();		
			else if(Mot[1]== "lire")	return bme280_lire()+"\n";			
			else if(Mot[1]== "json")	return bme280_JSON()+"\n";	
			else if(Mot[1]== "jsonf") 	return bme280_getJSONF();		
			else if(Mot[1]== "pubudp") 	return bme280_getPub_udp();	
			else if(Mot[1]== "pubmqtt") 	return bme280_getPub_mqtt();						
			else if(Mot[1]== "histo")	return bme280_histoJSON()+"\n";				
			else if(Mot[1]== "t")	return bme280_temperature()+"C\n";		
			else if(Mot[1]== "p")	return bme280_pression()+" kPa\n";		
			else if(Mot[1]== "h")	return bme280_humidity()+" %\n";	
			else if (Mot[1]== "interval") return "Lecture BME280 aux "+bme280_getTimeUnits()+"\n";	
			else if (Mot[1]== "intervalunits") return bme280_units+"\n";	
			else if (Mot[1]== "stop") return bme280_timerStop(); 		
			else if (Mot[1]== "start") return bme280_timerRestart(); 
		}	

		if(qteMots == 3){
			if (Mot[1]== "pubudp") return bme280_setPub_udp(Mot[2]);
			else if (Mot[1]== "pubmqtt") return bme280_setPub_mqtt(Mot[2]);			
			else if (Mot[1]== "interval") return bme280_setBME280_interval(Mot[2]);
			else if (Mot[1]== "intervalunits") return bme280_setTimeUnits(Mot[2]);
			else if(Mot[1]== "jsonf") 	return bme280_setJSONF(Mot[2]);		
		}

		return line + ("???\n");	
	}

#endif