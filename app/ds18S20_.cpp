/**
 * @file ds1820_.cpp
 * @author Jean Mercier profjmer@gmail.com
 * @brief Module pour les capteurs DS1820 utilisant le bus one wire
 * @version 1.0
 */
#include "../include/application.h"
#ifdef config_ds1820
	#define qte 24
	float DSTemperature[4][qte];

	DS18S20 ReadTemp;
	String ts_ds;
	float DS1820[4];
	uint8_t dsCount;

	uint32_t interval_ds1820;
	String ds1820_units;
	Timer timerDS1820;
	bool ds1820_jsonf;		// off: format simple  on: format influx
	bool ds1820_UDPpub;
	bool ds1820_MQTTpub;	


	// L'union permet de récupérer les 8 octets dans une opération
	// par la suite on extrait les champs désirés
	union {
		uint64_t rom;
		struct {
			uint8_t crc;
			uint8_t serial[6];
			uint8_t type;
		};
	}Rom;

	/**
	 * @brief Retourne le numéro de série de la sonde ds1820
	 * 
	 * @param a numéro de la sonde dans la liste des sondes
	 * @return String 
	 */
	String ds1820_serial(int a){
		int i;
		String id;
		for(i=0;i<6;i++){
			id+=String(Rom.serial[i],16);
		}
		id.toLowerCase();
		return id;
	}

	/**
	 * @brief retourne l'intervalle de lecture des capteurs DS1820
	 * 
	 * @return String 
	 */
	String ds1820_getTimeUnits(){
		return String(interval_ds1820)+" "+ds1820_units;
	}

	/**
	 * @brief définit l'unité des intervalles de lecture des capteurs ds1820.
	 * Effectue une première lecture par la fonction ds1820_T() et
	 * si l'afficheur TFT est défini, met à jour l'intervalle de lecture à l'écran TFT
	 * 
	 * @param units secondes, minutes ou heures
	 * @return String 
	 */
	String ds1820_setTimeUnits(String units){
		if(units == "secondes")  ds1820_units = units ;
		else if(units == "minutes")  ds1820_units = units ;
		else if(units == "heures")  ds1820_units = units ;	
		else return "Mauvais units\n";

		timerDS1820.stop();
		timerDS1820.setIntervalMs(timers_interval_ms(interval_ds1820,ds1820_units));
		timerDS1820.restart();	

		ds1820_T();
		#ifdef config_tft
			tft_showTimeUnits(ds1820_getTimeUnits());
		#endif
		return ds1820_getTimeUnits();
	}

	/**
	 * @brief retourne la valeur numérique de l'intervalle de lecture des ds1820
	 * 
	 * @return uint32_t 
	 */
	uint32_t ds1820_getDS1820_interval(){return interval_ds1820;}

	/**
	 * @brief fixe la valeur numérique des intervalles de lecture des capteurs DS1820.
	 * Effectur une lecture par la fonction ds1820_T() et
	 * si le module TFT est défini, met à jour l'intervalle de lecture des capteurs ds180 à l'écran du TFT
	 * 
	 * @param interval  string de la valeur numérique
	 * @return String 
	 */
	String ds1820_setDS1820_interval(String interval){
		interval_ds1820 = atol(interval.c_str());
		timerDS1820.stop();
		timerDS1820.setIntervalMs(timers_interval_ms(interval_ds1820,ds1820_units));
		timerDS1820.restart();
		ds1820_T();	
		#ifdef config_tft
			tft_showTimeUnits(ds1820_getTimeUnits());
		#endif
		return "Lecture DS1820 aux "+ds1820_getTimeUnits()+"\n";
	} 

	/**
	 * @brief redémarre la minuterie des lecture des capteurs ds1820.
	 * Effectue une lecture par la fonction ds1820_T() et
	 * si le module TFT est défini, met à jour l'intervalle de lecture des capteurs de1820 à l'écran TFT
	 * 
	 * @return String 
	 */
	String ds1820_timerRestart(){
		timerDS1820.stop();
		if(param_getFonctionnement()==faibleConsommation){
			interval_ds1820 = 0;	
			return "";
		}
		interval_ds1820 = ds1820_getDS1820_interval(); 
		timerDS1820.setIntervalMs(timers_interval_ms(interval_ds1820,ds1820_units) );
		timerDS1820.restart();	
		ds1820_T();		
		#ifdef config_tft
			tft_showTimeUnits(ds1820_getTimeUnits());
		#endif
		return "Lecture DS1820 aux "+ds1820_getTimeUnits()+"\n";
	}

	/**
	 * @brief Arrêt de la minuterie de lecture des capteurs ds1820
	 * 
	 * @return String 
	 */
	String ds1820_timerStop(){
		timerDS1820.stop();
		return "Arrêt lecture DS1820\n";	
	}

	/**
	 * @brief retourne l'état de la publication UDP des lectures des capteurs ds1820
	 * 
	 * @return String 
	 */
	String ds1820_getPub_udp(){
		if(ds1820_UDPpub ==true)return "Publication DS1820-UDP on\n";
		else return "Publication DS1820-UDP off\n";
	}

	/**
	 * @brief Active ou inactive la publication des mesures du ds1820 par UDP
	 * 
	 * @param active 
	 * @return String 
	 */
	String ds1820_setPub_udp(String active){
		if(active == "on"){ds1820_UDPpub = true;udp_setPub("on");}
		else ds1820_UDPpub = false;
		return ds1820_getPub_udp();
	}

	String ds1820_getPub_mqtt(){
		if(ds1820_MQTTpub ==true)return "Publication DS1820-MQTT on\n";
		else return "Publication DS1820-MQTT off\n";
	}

	String ds1820_setPub_mqtt(String active){
		if(active == "on"){ds1820_MQTTpub = true;}
		else ds1820_MQTTpub = false;
		return ds1820_getPub_mqtt();
	}	

	/**
	 * @brief initialise la minuterie des capteurs ds1820
	 * 
	 */
	void ds1820_timer_ini(){
		//Serial.println("ds1820_timer_ini()");
		#ifdef config_ds1820
			interval_ds1820 =10;
			ds1820_units="secondes";
			timerDS1820.initializeMs(timers_interval_ms(interval_ds1820,ds1820_units), ds1820_T).start();  
		#endif
	}

	/**
	 * @brief retourne les valeurs de la dernière lecture des capteurs ds1820
	 * 
	 * @return String 
	 */
	String ds1820_last(){
		int i;
		String rep = "DS";
		for(i=0; i<dsCount; i++){
			if(i==2)rep+=" ";
			rep += " T"+String(i+1)+" " + String(DS1820[i],1)+"  ";
		}
		return rep +"\n";
	}

	/**
	 * @brief retourne les valeurs de la dernière lecture des capteurs ds1820 en format json
	 * 
	 * @return String 
	 */
	String ds1820_last_JSON(){
		int i;
		String rep = "{\"type\":\"DS1820\",\"ts\":\""+ts_ds+"\",\"T\":[";
		for(i=0; i<dsCount; i++){
			rep += String(DS1820[i],1)+",";
		}
		rep.remove(rep.length()-1,1);
		return rep +"]}\n";
	}

	/**
	 * @brief retourne le champ du format influxDB des valeurs de la dernière lecture des capteurs ds1820
	 * 
	 * @return String 
	 */
	String ds1820_field(){
		int i;
		String rep;
		for(i=0; i<dsCount; i++){
			rep += String(DS1820[i],1)+",";
		}
		rep.remove(rep.length()-1,1);
		return "\"field\":{\"Temperature\":["+ rep+"]},"+"\"ts\":\""+ts_ds+"\"}";		
	}

	/**
	 * @brief retourne le format json actif
	 * 
	 * @return String 
	 */
	String ds1820_getJSONF(){
		if(ds1820_jsonf)return "Format JSON INFLUX\n";
		else return "Format JSON simple\n";
	}

	/**
	 * @brief active / inactive le format json influxDB
	 * 
	 * @param format on / off
	 * @return String 
	 */
	String ds1820_setJSONF(String format){
		if(format=="false")ds1820_jsonf = false;
		else ds1820_jsonf =true;
		return "JSONF fait\n";
	}

	/**
	 * @brief retourne la dernière lecture des valeurs des capteurs ds1820 selon le format json actif
	 * 
	 * @return String 
	 */
	String ds1820_JSONF(){
		if(ds1820_jsonf)return param_get_jmObjean_ID_JSON()+"\"Capteur\":\"DS1820\"},"+ds1820_field();
		else return ds1820_last_JSON();
	return ds1820_JSON();
	}



	//**********************************************************
	// DS18S20 example, reading
	// You can connect multiple sensors to a single port
	// (At the moment 4 pcs - it depends on the definition in the library)
	// Measuring time: 1.2 seconds * number of sensors
	// The main difference with the previous version of the demo:
	//  - Do not use the Delay function () which discourages by manufacturer of ESP8266
	//  - We can read several sensors
	// Usage:
	//  Call Init to setup pin eg. ReadTemp.Init(2);   //pin 2 selected
	//  Call ReadTemp.StartMeasure();
	//   if ReadTemp.MeasureStatus() false read sensors
	//   You can recognize sensors by the ID or index.
	//***********************************************************

	/**
	 * @brief retourne le menu des commandes du module ds1820_
	 * 
	 * @return String 
	 */
	String ds1820_menu(){
		return "\nds\nds scan\nds lire\nds last\nds json\nds jsonf [off on]\nds pubudp [off on]\nds pubmqtt [off on]\nds interval [0..59]\nds intervalunits [secondes minutes heures]\nds stop\nds start\n";
	}
	
	/**
	 * @brief utile pour démarrer à distance les lectures si la minuterie est inactive
	 * 
	 * @return String 
	 */
	String ds1820_start(){
		ts_ds = "";
		#ifdef config_rtc
			ts_ds = rtc_ts();
		#endif
		ReadTemp.StartMeasure(); 
		return "Démarre la conversion pour les DS1820\n";
	}

	/**
	 * @brief Lecture des DS1820 sur le bus one wire par intervalles.
	 * Les valeurs sont disponibles à la fin de la conversion dans le tableau DS1820[]
	 * Lorsque la minuterie de lecture est active, les valeurs retournées sont celles de la conversion précédente
	 */
	void ds1820_T(){
		uint8_t a;
		uint64_t info;
		ts_ds = "";
		#ifdef config_rtc
			ts_ds = rtc_ts();
		#endif

		if (!ReadTemp.MeasureStatus())  // the last measurement completed
		{
			for(a=0;a<4;a++)DS1820[a]=-100; // écrase ancienne mesure

			dsCount = ReadTemp.GetSensorsCount(); 
			if (dsCount){
				for(a=0;a<dsCount;a++)   // prints for all sensors
				{
					if (ReadTemp.IsValidTemperature(a))   // temperature read correctly ?
					{
						DS1820[a]=ReadTemp.GetCelsius(a);
						Rom.rom = ReadTemp.GetSensorID(a); // récupération des 8 octets
						#ifdef config_mqtt
							if(ds1820_MQTTpub){
								String idds= ds1820_serial(a);
								//Serial.println("id:"+idds);
								mqtt_pub(".ds1820-"+idds, String(DS1820[a],1));														
							}
						#endif
					}
					else{
						//Serial.println("Valeur DS1820 non valide");
					}
				}
				#ifdef config_udp
					if(ds1820_UDPpub)udp_send(ds1820_JSONF());
				#endif
				web_wsSend(ds1820_last_JSON());
				#ifdef config_tft
					tft_update(ds1820);
				#endif
			}   // is minimum 1 sensor detected ?
			
			ReadTemp.StartMeasure();  // start next measure
		} 
		
	}

	/**
	 * @brief Comme la fonction retourne les valeurs de la dernière conversion et en démarre une nouvelle, il faut faire deux lectures pour obtenir
	 * des valeurs récentes. La conversion prend 1.2 sec par sonde. Alors pour 4 sondes il faut compter un minimum de 5 secondes entre deux conversions.
	 * @return String Les dernières valeurs de conversions sont retournées et une nouvelle conversion est débutée.
	 */
	String ds1820_lire()
	{
		uint8_t a;
		uint64_t info;
		String rep;
		//Serial.println("Lecture DS1820");

		if (!ReadTemp.MeasureStatus())  // the last measurement completed
		{
			for(a=0;a<4;a++)DS1820[a]=-100; // écrase ancienne mesure
				dsCount = ReadTemp.GetSensorsCount(); 
				if (dsCount){
					for(a=0;a<dsCount;a++)   // for all sensors
					{
						if(a==2)rep+=" ";
						rep += " T"+String(a+1)+" ";
						if (ReadTemp.IsValidTemperature(a))   // temperature read correctly ?
						{
							DS1820[a]=ReadTemp.GetCelsius(a);
						}
						else{
							rep = ("Température non valide\n");
						}
					}

					ReadTemp.StartMeasure();  // start next measure

				}   // is minimum 1 sensor detected ?
		}
		else{
			rep=("Attendre la fin de conversion\n");
		}
		return rep += "\n";
	}

	/**
	 * @brief initialise le module ds1820_
	 * Effectue une première lecture
	 * 
	 */
	void ds1820_ini(){

		ReadTemp.Init(pin_1WIRE);  			// select PIN It's required for one-wire initialization!
		//ReadTemp.StartMeasure(); // first measure start,result after 1.2 seconds * number of sensors
		ds1820_timer_ini();
		ds1820_T();
	}

	/**
	 * @brief retourne les capteurs identifiés sur le bus one-wire
	 * 
	 * @return String 
	 */
	String ds1820_scan(){
		int a;
		int count = ReadTemp.GetSensorsCount();
		char buf[40];

		String rep;

		rep =String(count) + " dispositif(s) sur le bus\n";
		if (count){   // is minimum 1 sensor detected ?
			for(a=0;a<count;a++) {  // prints for all sensors
				//rom[a] = ReadTempGetSensorID(a);
				//Serial.printf("ROM:%lx\n",rom[a]);
		
				Rom.rom = ReadTemp.GetSensorID(a); // récupération des 8 octets
				// séparation de l'info 
				sprintf(buf,"ROM %d T:%x Ser:%x %x %x %x %x %x CRC:%x\n",a,Rom.type,Rom.serial[5],Rom.serial[4],Rom.serial[3],Rom.serial[2],Rom.serial[1],Rom.serial[0],Rom.crc);
				rep+= String(buf);
			}
			rep.toUpperCase();
			//Serial.println(rep);
			return rep;
		}
	}

	/**
	 * @brief À faire ...
	 * 
	 * @return String 
	 */
	String ds1820_histo(){
		// À faire	
			return "{\"type\":\"noRTC\"}";

	}

	/**
	 * @brief Interpréteur de commandes du module ds1820_
	 * 
	 * @param line 
	 * @return String 
	 */
	String ds1820_interpret(String line){
		// Permet de séparer les mots d'une line de caractères
		Vector<String> Mot;
		int qteMots = splitString(line, ' ' , Mot);

		if(qteMots == 1)return ds1820_menu();	

		if(qteMots == 2){
			if(Mot[1]== "menu")	return ds1820_menu();		
			else if (Mot[1]== "interval") return "Lecture DS1820 aux "+ds1820_getTimeUnits()+"\n";
			else if (Mot[1]== "intervalunits") return ds1820_units+"\n";		
			else if(Mot[1]== "json")	return ds1820_last_JSON();	
			else if(Mot[1]== "jsonf") 	return ds1820_getJSONF();		
			else if(Mot[1]== "udppub") 	return ds1820_getPub_udp();	
			else if(Mot[1]== "pubmqtt") 	return ds1820_getPub_mqtt();						
			else if(Mot[1]== "histo")	return ds1820_histo();			
			else if(Mot[1]== "last")	return ds1820_last();		
			else if(Mot[1]== "lire")	return ds1820_lire();			
			else if (Mot[1]== "start") return ds1820_timerRestart();
			else if (Mot[1]== "stop") return ds1820_timerStop(); 		
			else if(Mot[1]== "scan")	return ds1820_scan();					
		}	

		if(qteMots == 3){
			if (Mot[1]== "pubudp") return ds1820_setPub_udp(Mot[2]);
			else if (Mot[1]== "pubmqtt") return ds1820_setPub_mqtt(Mot[2]);				
			else if (Mot[1]== "interval") return ds1820_setDS1820_interval(Mot[2]);
			else if (Mot[1]== "intervalunits") return ds1820_setTimeUnits(Mot[2]);		
			else if(Mot[1]== "jsonf") 	return ds1820_setJSONF(Mot[2]);
		}

		
			return line + (" ???\n");	
	}
#endif
