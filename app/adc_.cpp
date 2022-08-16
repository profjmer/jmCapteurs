/*************************************************************************
 * @file	adc_.c				 
 * @version	1.0
 * @author Jean Mercier profjmer@gmail.com
 * @brief Module de gestion du CAN
*/
#include "../include/application.h"
#ifdef config_adc

	#define diviseur 195.23

	// tableau pour sauvegarder les valeurs du adc
	#define qte 24
	double tvolts[qte];
	String ttemps[qte];
	double vBat;
	String adc_ts;
	bool adc_jsonf;		// false format simple  ou true format influx

	uint32_t interval_adc;
	String adc_units;
	Timer timerADC;
	bool adc_UDPpub;
	bool adc_MQTTpub;

	String adc_getTimeUnits(){
		return String(interval_adc)+" "+adc_units;
	}

	String adc_setTimeUnits(String units){
		if(units == "secondes")  adc_units = units ;
		else if(units == "minutes")  adc_units = units ;
		else if(units == "heures")  adc_units = units ;	
		else return "Mauvais units\n";

		timerADC.stop();
		timerADC.setIntervalMs(timers_interval_ms(interval_adc,adc_units));
		timerADC.restart();	

		adc_volts();
		#ifdef config_tft
			tft_showTimeUnits(adc_getTimeUnits());
		#endif
		return "Lecture ADC aux "+adc_getTimeUnits()+"\n";
	}

	uint32_t adc_getADC_interval(){return interval_adc;}

	String adc_setADC_interval(String interval){
		interval_adc = atol(interval.c_str());
		timerADC.stop();
		timerADC.setIntervalMs(timers_interval_ms(interval_adc,adc_units));
		timerADC.restart();
		adc_volts();	
		#ifdef config_tft
			tft_showTimeUnits(adc_getTimeUnits());
		#endif
		return "Lecture ADC aux "+adc_getTimeUnits()+"\n";
	} 

	String adc_timerRestart(){
		timerADC.stop();
		if(param_getFonctionnement()==faibleConsommation){
			interval_adc = 0;	
			return "";
		}
		interval_adc = adc_getADC_interval(); 
		timerADC.setIntervalMs(timers_interval_ms(interval_adc,adc_units));
		timerADC.restart();		
		adc_volts();	
		#ifdef config_tft
			tft_showTimeUnits(adc_getTimeUnits());
		#endif
		return "Lecture ADC aux "+adc_getTimeUnits()+"\n";
	}

	String adc_timerStop(){
		timerADC.stop();
		return "Arrêt lecture ADC\n";	
	}

	String adc_getPub_udp(){
		if(adc_UDPpub ==true)return "Publication ADC-UDP on\n";
		else return "Publication ADC-UDP off\n";
	}

	String adc_setPub_udp(String active){
		if(active == "on"){adc_UDPpub = true;}
		else adc_UDPpub = false;
		return adc_getPub_udp();
	}

	String adc_getPub_mqtt(){
		if(adc_MQTTpub ==true)return "Publication ADC-MQTT on\n";
		else return "Publication ADC-MQTT off\n";
	}

	String adc_setPub_mqtt(String active){
		if(active == "on"){adc_MQTTpub = true;}
		else adc_MQTTpub = false;
		return adc_getPub_mqtt();
	}

	/**
	 * @brief Lit le CNA et place la valeur dans vBat
	 * 
	 */
	void adc_volts()
	{
		adc_ts = "";
		#ifdef config_rtc
			adc_ts = rtc_ts();
		#endif
		vBat = (double)analogRead(A0)/diviseur;
		#ifdef config_rtc
			adc_voltsToHisto(adc_ts,vBat);
		#endif
		if(adc_UDPpub)udp_send(adc_JSONF());
		web_wsSend(adc_JSON());
		#ifdef config_tft
			tft_update(adc);
		#endif
		#ifdef config_mqtt
			if(adc_MQTTpub)mqtt_pub(".adc", String(vBat,1));
		#endif
	}

	void adc_ini(){
		if(param_getFonctionnement()==faibleConsommation){
			interval_adc = 0;
			timerADC.stop();
			return;
		}
		interval_adc =10;
		adc_units="secondes";
		timerADC.initializeMs(timers_interval_ms(interval_adc,adc_units), adc_volts).start();  
		adc_volts();
	}

	//prototypes
	void adc_voltsToHisto(String Temps, double volts);

	/** @brief		Retourne le menu
	 * @param[in]	rien
	 * @returns		menu
	 */
	String adc_menu(){
	return "\nadc\nadc lire\nadc json\nadc jsonf [off on]\nadc pubudp [off on]\nadc pubmqtt [off on]\nadc histo\nadc interval [0..59]\nadc intervalunits [secondes minutes heures]\nadc stop\nadc start\n";
	}

	String adc_getJSONF(){
		if(adc_jsonf)return "Format JSON INFLUX\n";
		else return "Format JSON simple\n";
	}

	String adc_setJSONF(String format){
		if(format=="false")adc_jsonf = false;
		else adc_jsonf =true;
		return "JSONF fait\n";
	}

	String adc_JSONF(){
		if(adc_jsonf)return param_get_jmObjean_ID_JSON()+"\"Capteur\":\"ADC\"},"+adc_field();
		else return adc_JSON();
	}

	/**
	 * @brief Lecture du ADC. Il y a un diviseur résistif 1/16 à l'entrée
	 * afin de permettre des tensions d'entrée jusqu'à 16 volts.
	 * Comme la valeur vaut 1/1024 volts, il faut retouner la valeur /64
	 * 
	 * @return String de format json 
	 */
	String adc_JSON(){
		return "{\"type\":\"ADC\",\"volts\":" +String(vBat,1) +",\"ts\":\"" +adc_ts+"\"}";
	}

	/**
	 * @brief Retourne les données pour publication sur Broker mosca et Dashboard Grafana/Chronograf
	 * 
	 * @return String 
	 */
	String adc_field(){
		return ("\"field\":{\"Alim Volts\":" +String(vBat,1) +"},\"ts\":\"" +adc_ts+"\"}"); // format json pour ilc
	}

	String adc_last(){
		return String(vBat,1); // format numérique
	}

	String adc_last_JSON(){
		return "{\"type\":\"ADC\",\"volts\":" +String(vBat,1) +",\"ts\":\"" +adc_ts+"\"}}";
	}

	/**
	 * @brief Lit la valeur analogique sur la broche TOUT
	 * 
	 * @return String retourne la valeur en volt
	 */
	String adc_lire(){
		return String(vBat,1); // format numérique
	}

	double adc_vBat(){
		return vBat;
	}


	/**
	 * @brief  Pousse les dernières valeurs du adc dans les tableaux
	 * La plus ancienne valeur des tableaux est écrasée
	 * Les nouvelles valeurs sont plaçées à la fin
	 * 
	 * @param Temps 
	 * @param volts 
	 */
	void adc_voltsToHisto(String Temps, double volts)
	{
		int i;
		for(i=0;i<qte-1;i++){ 
			ttemps[i]= ttemps[i+1];
			tvolts[i]= tvolts[i+1];
		}
		ttemps[i]= Temps;
		tvolts[i]=volts;
		//printHistorique();
	}

	/**
	 * @brief construit l'historique des données en format json
	 * 
	 * @return String 
	 */
	String adc_voltsToHistoJSON(){
		#ifndef config_rtc
			return "{\"type\":\"noRTC\"}";
		#endif
		int i;
		String rep = "{\"type\":\"ADCHisto\",\"ts\":[\"";
		for(i=0;i<qte;i++){
			if(ttemps[i]!=""&& tvolts[i]!=0){
				rep += ttemps[i];
				if(i<qte-1)rep+="\",\"";
			}
		}
		rep+="\"],\"volts\":[";
		for(i=0;i<qte;i++){
			if(ttemps[i]!="" && tvolts[i]!=0){
				rep += tvolts[i];
				if(i<qte-1)rep+=",";
			}
		}
		rep+="]}";  
		return rep;
	}


	/** @brief		Interpréteur de commandes du module de gestionIO
	 * @param[in]	ligne de commande
	 * @returns		Retourne une String donnant le résultat de l'interprétation de la commande
	 */
	String adc_interpret(String line){
		// Permet de séparer les mots d'une line de caractères
		Vector<String> Mot;
		int qteMots = splitString(line, ' ' , Mot);

		// retourne le menu
		if(qteMots == 1)	return adc_menu();

		// retourne le menu des commandes du module gestionIO
		else if(qteMots == 2){
			if(Mot[1]== "menu") return adc_menu();
			else if(Mot[1]== "json") 	return adc_JSON()+"\n";
			else if(Mot[1]== "jsonf") 	return adc_getJSONF();
			else if(Mot[1]== "pubudp") 	return adc_getPub_udp();
			else if(Mot[1]== "pubmqtt") 	return adc_getPub_mqtt();					
			else if(Mot[1]== "lire") 	return "Alim:"+adc_lire()+" volts\n";
			else if(Mot[1]== "histo") 	return adc_voltsToHistoJSON()+"\n";
			else if (Mot[1]== "interval") return "Lecture ADC aux "+adc_getTimeUnits()+"\n";	
			else if (Mot[1]== "intervalunits") return adc_units+"\n";		
			else if (Mot[1]== "stop") return adc_timerStop();
			else if (Mot[1]== "start") return adc_timerRestart();
		}	

		else if(qteMots == 3){
			if (Mot[1]== "pubudp") return adc_setPub_udp(Mot[2]);		
			else if (Mot[1]== "pubmqtt") return adc_setPub_mqtt(Mot[2]);
			else if (Mot[1]== "interval") return adc_setADC_interval(Mot[2]);			
			else if (Mot[1]== "intervalunits") return adc_setTimeUnits(Mot[2]);
			else if(Mot[1]== "jsonf") 	return adc_setJSONF(Mot[2]);
		}

		// commande non reconnue
		return line + "  ???\n";
	}
#endif