/**
 * @file oled_.cpp
 * @author Jean Mercier profjmer@gmail.com
 * @brief module de gestion de l'afficheur OLED SSD1306
 * dépend de la librairie Adafruit_SSD1306
 * @version 1.0
 */

#include "../include/application.h"
#ifdef config_oled
	#include <Libraries/Adafruit_SSD1306/Adafruit_SSD1306.h>
	/* définitions pour l'affichage de différentes pages sur l'oled */
	#define bme280 0
	#define adc 1
	#define dht22 2
	#define ds1820 3
	#define udp 4
	#define mqtt 5
	
	int page, pageQte;	// no de page a afficher et quantité de pages
	int PagesOled[6];	// 6 pages max
	bool cycle; // vrai pour afficher séquentiellement les pages au rytme de interval_oled

	/**
	 * @brief retourne les pages pouvant être affichées à l'écran OLED
	 * 
	 * 
	 * @return String 
	 */
	String oled_pages(){
		int i;
		String pages;
		for(i=0;i<pageQte;i++){
			if(PagesOled[i]==0)pages+="bme280 ";
			if(PagesOled[i]==1)pages+="udp ";
			if(PagesOled[i]==2)pages+="adc ";
			if(PagesOled[i]==3)pages+="dht22 ";
			if(PagesOled[i]==4)pages+="ds1820 ";	
			if(PagesOled[i]==5)pages+="mqtt ";									
		}
		return pages;
	}


	Adafruit_SSD1306 display(-1); // reset Pin required but later ignored if set to False
	String zone1, zone2, zone3, zone4; // zone virtuelle d'affichage de l'écram

	uint8_t interval_oled;
	Timer timerOLED;

	/**
	 * @brief Intervalle de rafraichissement de l'écran OLED
	 * 
	 * @return uint8_t 
	 */
	uint8_t oled_getOLED_interval(){return interval_oled;}

	String oled_setOLED_interval(String interval){
	interval_oled = (int8_t)atoi(interval.c_str());
		timerOLED.stop();
		timerOLED.setIntervalMs(interval_oled * 1000);
		timerOLED.restart();
	return "Rafraichissement OLED aux "+String(interval_oled)+" sec\n";
	} 

	/**
	 * @brief redémarre la minuterie de rafraichissement de l'afficheur OLED
	 * 
	 * @return String 
	 */
	String oled_timerRestart(){
		timerOLED.stop();
		if(param_getFonctionnement()==faibleConsommation){
			interval_oled = 0;	
			return "Pas de rafraichissement en mode faible consommation\n";
		}
		interval_oled = oled_getOLED_interval(); 
		timerOLED.setIntervalMs(interval_oled * 1000);
		timerOLED.restart();		
		return "Redémarrage rafraichissement OLED\n";
	}

	/**
	 * @brief arrêt de la minuterie de rafraichissement de l'afficheur OLED
	 * 
	 * @return String 
	 */
	String oled_timerStop(){
		timerOLED.stop();
		return "Arrêt rafraichissement OLED\n";	
	}

	/**
	 * @brief initialisation de la minuterie de rafraichissement de l'afficheur OLED
	 * 
	 */
	void oled_timer_ini(){
		//Serial.println("oled_timer_ini");
		#ifdef config_oled
			interval_oled =4; 
			timerOLED.initializeMs(interval_oled * 1000, oled_update).start();  
		#endif

	}

	/**
	 * @brief Menu des commandes du module oled_
	 * 
	 * @return String 
	 */
	String oled_menu(){
		return "\noled\noled zone message [zone (3..4) et message est de 42 caracteres maximum]\noled invert\noled normal\noled on\noled off\noled interval [0..255]sec"\
	"\noled stop\noled start\noled page ["+oled_pages()+"]\noled cycle [0..1]\n";
	}

	/**
	 * @brief Construit la table d'identification des pages oled selon la configuration du projet dans config_.h
	 * 
	 */
	void oled_setPages()
	{
		pageQte;
			
		// inscription des pages dans la liste des pages
		#ifdef config_adc
			PagesOled[pageQte++]=adc; 
		#endif
		#ifdef config_bme280
			PagesOled[pageQte++]=bme280;
		#endif
		#ifdef config_udp
			PagesOled[pageQte++]=udp;
		#endif
		#ifdef config_dht22
			PagesOled[pageQte++]=dht22;
		#endif	
		#ifdef config_ds1820
			PagesOled[pageQte++]=ds1820;
		#endif
		#ifdef config_mqtt
			PagesOled[pageQte++]=mqtt;
		#endif									
	}


	/**
	 * @brief Initialisation du module oled
	 * 
	 */
	void oled_ini(){
		//Serial.println("oled_ini()");
		display.begin(SSD1306_SWITCHCAPVCC, pin_OLED_I2C_ADDRESS, false);
		display.setTextColor(WHITE);
		display.setTextSize(1);	
		oled_setZone1(ota_WifiAccessPoint_name());
		oled_setZone2("Zone 2");
		oled_setZone3("Zone 3");
		oled_setZone4("Zone 4");
		oled_update();
		oled_setPages(); // détermine le nombre de pages à afficher
		cycle = true;
		oled_timer_ini();
	}

	/**
	 * @brief arrêt de l'affichage OLED
	 * 
	 */
	void oled_off(){
		display.dim(true);	
	}

	/**
	 * @brief fonctions pour écrire dans une des 4 zones de l'afficheur OLED
	 * 
	 * @param s  zone (1..4)
	 */
	void oled_setZone1(String s){zone1=s;}
	void oled_setZone2(String s){zone2=s;}
	void oled_setZone3(String s){zone3=s;}
	void oled_setZone4(String s){zone4=s;}

	String oled_getZone4(){
		return zone4;
	}

	/**
	 * @brief déplacement du curseur sur une des 8 lignes de l'écran OLED SSD1306
	 * 
	 * @param ligne 
	 */
	void oled_setCursorLigne(uint8_t ligne){
		if(ligne > 8)return;
		ligne--;
		display.setCursor(0,ligne *8);
	}

	/**
	 * @brief affiche la zone1 à l'écran du OLED
	 * 
	 */
	void showZone1(){
		display.setCursor(0,0);
		display.println(zone1);
	}

	/**
	 * @brief affiche le zone 2 à l'écran du OLED
	 * 
	 */
	void showZone2(){
		display.setCursor(0,16);
		display.println(zone2);
	}

	/**
	 * @brief affiche la zone 3 à l'écran du OLED
	 * 
	 */
	void showZone3(){
		display.setCursor(0,32);
		display.println(zone3);
	}

	/**
	 * @brief affiche la zone 4 à l'écran du OLED
	 * 
	 */
	void showZone4(){
		display.setCursor(0,48);
		display.println(zone4);
	}

	#ifdef config_bme280
		/**
		 * @brief affiche la page du capteur BME280 à l'écran du OLED
		 * 
		 */
		void oled_pageBME280(){
			//Serial.println("oled_pageBME280()");
			oled_setZone1(ota_WifiAccessPoint_name());
			oled_setZone2(param_wifi());
			#ifdef config_bme280
				if(bme280_getBME280_interval()!=0)
					oled_setZone3(bme280_last());
			#endif
			#ifdef config_rtc
				oled_setZone4(rtc_ts());
			#endif
		}
	#endif

	#ifdef config_dht22
		/**
		 * @brief affiche la page du capteur DHT22 à l'écran du OLED
		 * 
		 */
		void oled_pageDHT22(){
			oled_setZone1(ota_WifiAccessPoint_name());
			oled_setZone2(param_wifi());
			#ifdef config_dht22
				if(dht22_getDHT22_interval()!=0)
					oled_setZone3(dht22_last());
			#endif
			#ifdef config_rtc
				oled_setZone4(rtc_ts());
			#endif
		}
	#endif

	#ifdef config_ds1820
		/**
		 * @brief affiche la page des capteurs ds1820 à l'écran du OLED
		 * 
		 */
		void oled_pageDS1820(){
			oled_setZone1(ota_WifiAccessPoint_name());
			oled_setZone2(param_wifi());
			#ifdef config_ds1820
				if(ds1820_getDS1820_interval()!=0)
					oled_setZone3(ds1820_last());
			#endif
			#ifdef config_rtc
				oled_setZone4(rtc_ts());
			#endif
		}
	#endif

	#ifdef config_udp
		/**
		 * @brief affiche la page de publication UDP à l'écran du OLED
		 * 
		 */
		void oled_pageUDP(){
			#ifdef config_udp
				oled_setZone1("Client WiFi UDP");
				oled_setZone2(udp_getClientIP());
				oled_setZone3(udp_getClientPort());
			#endif
			#ifdef config_rtc
					zone4 = rtc_ts();
			#endif
		}
	#endif

	#ifdef config_adc
		/**
		 * @brief affiche la page du convertisseur ADC à l'écran du OLED
		 * 
		 */
		void oled_pageADC(){
			#ifdef config_adc
				oled_setZone1("ADC volts");
				oled_setZone2(adc_last());
				oled_setZone3("Lectures aux "+String(adc_getADC_interval())+" sec");
			#endif
			#ifdef config_rtc
					zone4 = rtc_ts();
			#endif
		}
	#endif

	/**
	 * @brief Retourne le numéro d'affichage de la page oled associée au module
	 * 
	 * @param module 
	 * @return int 
	 */
	int oled_getPageNo(String module){
		int i=0;
		if(module == "bme")i=bme280;
		if(module == "adc")i=adc;
		if(module == "udp")i=udp;	
		if(module == "dht22")i=dht22;
		if(module == "ds1820")i=ds1820;
		if(module == "mqtt")i=mqtt;	
		return i;
	}

	/**
	 * @brief Fait afficher le contenu de la page du module dont le nom est donné
	 * 
	 * @param module 
	 * @return String 
	 */
	void oled_loadPageNo(int i){
		switch(i){
		#ifdef config_bme280
			case bme280 : oled_pageBME280();break;
		#endif
		#ifdef config_adc
			case adc : oled_pageADC();break;
		#endif
		#ifdef confi_udp
			case udp : oled_pageUDP();	break;
		#endif
		#ifdef config_dht22
			case dht22 : oled_pageDHT22();break;
		#endif
		#ifdef config_ds1820
			case ds1820 : oled_pageDS1820();break;	
		#endif	
		}
		//return "Chargement page OLED "	+ String(i) +"\n";
	}

	/**
	 * @brief affiche à l'écran OLED SSD1306 le module sélectionné
	 * 
	 * @param module  bme adc udp dht22 ds1820 mqtt
	 * @return String 
	 */
	String oled_showPage(String module){
		int pageid = oled_getPageNo(module);
		page=0;	// page affichée par défaut si recherche infructueuse
		int i;
		for(i=0;i<pageQte;i++){
			if(PagesOled[i]==pageid){
				page=i; break;
			}
		}
		oled_update();
		return "Chargement OLED "+module+"\n";
	}

	/**
	 * @brief mise à jour de l'afficheur
	 * 
	 */
	void oled_update(){
		oled_loadPageNo(PagesOled[page]);
		display.clearDisplay();
		showZone1();
		showZone2();
		showZone3();
		showZone4();
		display.display();
		if(cycle)				// pour afficher toutes les pages sccessivements au rytme de interval_oled
			if(page++>=pageQte-1)page=0;
	}

	/**
	 * @brief active / inactive l'affichage en boucles des pages à l'écran TFT
	 * 
	 * @param n  1 / 0   active /inactive
	 * @return String 
	 */
	String oled_setCycle(String n){
		if(n=="0"){cycle=false; return "Arrêt affichages en boucle\n";}
		else cycle = true;
		return "Affichages des pages en boucle\n";

	}

	#define lineMax 42	// correspond à deux lignes de grosseur 1
	/**
	 * @brief Interpréteur des commandes du module oled_
	 * 
	 * @param line 
	 * @return String 
	 */
	String oled_interpret(String line){
		// Permet de séparer les mots d'une line de caractères
		Vector<String> Mot;
		int qteMots = splitString(line, ' ' , Mot);

		if(qteMots == 1)return oled_menu();

		if(qteMots == 2){
			if(Mot[1]== "update")	{oled_update();	return "oled update fait\n";}	

			else if(Mot[1]== "on"){
				display.dim(false);
				return "oled on\n";
			}

			else if(Mot[1]== "off"){
				display.dim(true);
				return "oled off\n";
			}			

			else if(Mot[1]== "invert"){
			display.invertDisplay(true);
			return "oled mode invert\n";
			}

			else if(Mot[1]== "normal"){
			display.invertDisplay(false);
			return "oled mode normal\n";
			}

			else if (Mot[1]== "interval") return "Rafraichissement OLED aux "+String(oled_getOLED_interval())+" sec\n";	
			else if (Mot[1]== "stop") return oled_timerStop(); 
			else if (Mot[1]== "start") return oled_timerRestart(); 
			else if (Mot[1]== "page") return oled_pages()+"\n"; 			
			else if (Mot[1]== "cycle") return String(cycle); 		
		}	

		if(qteMots >= 3){
			if (Mot[1]== "interval") return oled_setOLED_interval(Mot[2]);
			if (Mot[1]== "page") return oled_showPage(Mot[2]);
			if (Mot[1]== "cycle") return oled_setCycle(Mot[2]);
			if(Mot[1]== "zone"){
				int noZone = strtol(Mot[2].c_str(),NULL,10); // identification du no de la zone d'affichage
				line.remove(0,12); // retire oled zone et numéro de zone du texte
				int qte = line.length(); // nombre de char	
				if(qte > lineMax)line.remove(lineMax, qte-lineMax); // tronque les longs messages
				
				bool zoneOk = false;
				// affiche dans la zone sélectionnée
				switch(noZone){
					case 3: oled_setZone3(line);zoneOk=true;break;
					case 4: oled_setZone4(line);zoneOk=true;break;
				}
				if(zoneOk){
					oled_update();
					return "transmis vers OLED\n";
				}
				else return "zone interdite\n";
			}

		}
		
		return line + ("???\n");
	}
#endif
