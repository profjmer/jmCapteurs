/**
 * @file tft_.cpp
 * @author Jean Mercier profjmer@gmail,com
 * @brief module de gestion de l'afficheur TFT ILI9341
 * utilise la librairie Adafruit_ILI9341.cpp, à changer/modifier pour pouvoir choisir le CS
 * @version 1.0
 */
#include "../include/application.h"

#ifdef config_tft
	#include "tftADC.h"
	#include "tftBME280.h"
	#include "tftDHT22.h"
	#include "tftDS1820.h"

	int tftpageQte;
	int PagesTFT[4];	// tableau des pages d'affichage TFT pour maximum de 4 pages
	int tftpage;	// no de page a afficher
	bool tftcycle; // vrai pour afficher séquentiellement les pages au rytme de interval_tft
	bool logoTime;

	Adafruit_ILI9341 tft;

	Timer timer_logo;

	uint8_t orientation = 0;	// orientation de l'écran  2 mode  paysages
	uint8_t rotation=0;			// 4 orientations possibles 2 paysages et deux portraits

	String horloge;				// mémoire du texte affiché pour l'horloge, utilisé pour effacer ancienne valeur affichée
	String timeUnits;
	bool voir_horloge;
	uint8_t horloge_x;
	uint8_t horloge_y;

	/**
	 * @brief retourne les pages pouvant être affichées à l'écran du TFT ILI9341
	 * 
	 * @return String 
	 */
	String tft_pages(){
		int i;
		String pages ="";
		for(i=0;i<tftpageQte;i++){
			if(PagesTFT[i]==0)pages+="bme280 ";
			if(PagesTFT[i]==1)pages+="adc ";
			if(PagesTFT[i]==2)pages+="dht22 ";
			if(PagesTFT[i]==3)pages+="ds1820 ";									
		}
		return pages;
	}

	/**
	 * @brief Retourne le numéro d'identifiant de la page tft associé au nom du capteur
	 * 
	 * @param module 
	 * @return int 
	 */
	int tft_getPageNo(String module){
		if(module == "bme280")return bme280;
		else if(module == "adc")return adc;
		else if(module == "dht22")return dht22;
		else if(module == "ds1820")return ds1820;
		return 0;
	}

	/**
	 * @brief Fait afficher le contenu de la page du capteur dont le numéro d'identifiant est donné
	 * 
	 * @param module 
	 * @return String 
	 */
	void tft_loadPageNo(int i){
		switch(i){
			case bme280 : tftBME280_page();break;
			case adc : tftADC_page();break;
			case dht22 : tftDHT22_page();break;
			case ds1820 : tftDS1820_page();break;		
		}
		tft_update(i);
	}

	/**
	 * @brief Affiche la page à partir du nom du capteur
	 * 
	 * @param module nom du capteur
	 * @return String 
	 */
	String tft_showPage(String module){
		int pageid = tft_getPageNo(module);
		tftpage=0;	// page affichée par défaut si recherche infructueuse
		int i;
		for(i=0;i<tftpageQte;i++){		// Pour tous les identifiants numériques des pages dans le tableau
			if(PagesTFT[i]==pageid){	// si l'identifiant du module en paramètre est aussi celui à la position i du tableau
				tftpage=i;				// alors l'indice de la page à afficher est i
				tft_loadPageNo(i);	
				return "Chargement page fait\n";
			}
		}

		return "Chargement page pas fait\n";
	}

	/**
	 * @brief Mise à jour de la page active et de l'horloge selon l'intervalle interval_tft
	 * 
	 */
	void tft_update(uint8_t capteur){
		if(logoTime)return;
		// il faut mettre à jour la page active avec les dernières valeurs du capteur, selon la page de capteur active
		if(capteur == tftpage){
			switch(tftpage){
				case bme280: tftBME280_update();break;
				case adc:	tftADC_update();	break;		
				case dht22:	 tftDHT22_update();break;
				case ds1820: tftDS1820_update(); break;		
			}
		}
		// mettre à jour l'horloge
		if(voir_horloge){
			tft_horloge();
		}
	}



	/** @brief		Retourne le menu
	 * @param[in]	rien
	 * @returns		menu
	 */
	String tft_menu(){
		return "\ntft\ntft fond r g b [0..255]" \
		"\ntft horloge [off on]\ntft horlogeXY [0..150] [0..220]\ntft orientation [0..1]\ntft inverse [0..1]\n" \
		"tft logo\ntft print x y size[1..4] couleur[0..65535] rotation[0..3] texte\ntft bmp nomfichier x y rotation[0..3]\n" \
		"\ntft page ["+tft_pages()+"]\n";
	}

	/**
	 * @brief fixe la position de l'affichage de l'horloge à l'écran
	 * 
	 * @param x 
	 * @param y 
	 * @return String 
	 */
	String tft_horloge_xy(uint8_t x, uint8_t y){
		horloge_x = x;
		horloge_y = y;
	}

	/**
	 * @brief affiche l'horloge à l'écran du TFT
	 * 
	 */
	void tft_horloge(){
		//Serial.println("tft_horloge()");
		String ts = rtc_ts() ;
		spi_settingsFor(ILI9341);
		tft.setTextSize(1); 

		// efface ancienne valeur numérique et affiche nouvelle valeur
		tft.setCursor(horloge_x,horloge_y);
		tft.setTextColor(ILI9341_BLACK);
		tft.println(horloge);

		// nouvelle valeur d'horloge
		horloge = ts;		
		tft.setCursor(horloge_x,horloge_y);
		tft.setTextColor(ILI9341_DARKGREY);
		tft.println(horloge); 

	}

	/**
	 * @brief  horloge est la string pour afficher l'horloge. On écrit cette string avec la couleur de fond 0 pour effacer.
	 * Ne prend pas en charge différente couleur de fond
	 */
	void tft_effaceHorloge(){
		spi_settingsFor(ILI9341);
		tft.setTextSize(1); 
			// efface ancienne string de l'horloge
		tft.setCursor(horloge_x,horloge_y);
		tft.setTextColor(ILI9341_BLACK);
		tft.println(horloge);
		voir_horloge =false;
	}

	/**
	 * @brief retourne si l'affichage l'horloge est actif
	 * 
	 * @return String 
	 */
	String tft_getVoirHorloge(){
		if(voir_horloge)return "Horloge TFT on\n";
		else "Horloge TFT off\n";
	}

	/**
	 * @brief Affiche le logo à l'écran
	 * 
	 */
	void tft_logo() {
		logoTime=true;
		spi_settingsFor(ILI9341);	
		// paysage
		if(orientation ==0 )tft.setRotation(3);
		else tft.setRotation(1);
		tft.fillScreen(ILI9341_WHITE); // Clear display
		bmpDraw(tft, "jmer.bmp", 0, 0);
		bmpDraw(tft, "profjmer.bmp", 100, 50);
		tft.setCursor(110,190);
		tft.setTextColor(ILI9341_BLACK);
		tft.setTextSize(1);
		tft.printf("profjmer@gmail.com");
		tft.setCursor(70,200);
		tft.printf("youtube.com/user/AsTuVuComment");
		tft.setCursor(70,220);
		tft.setTextColor(ILI9341_RED);	
		tft.println(param_getFirmwareID());
	}



	/**
	 * @brief Termine l'initialisation après l'affichage du logo
	 * 
	 */
	void continue_ini(){
		voir_horloge=true;
		horloge_x = 0;	// centrer l'horlege dans le bas de l'écran
		horloge_y =230;
		tftpageQte=0;
		logoTime =false;
		timeUnits = "secondes";

		//Construction du tableau des pages selon la configuration du projet
		#ifdef config_bme280
		PagesTFT[tftpageQte]=bme280;
			tftpageQte++;
		#endif
		#ifdef config_adc
			PagesTFT[tftpageQte]=adc;
			tftpageQte++;
		#endif		
		#ifdef config_dht22
			PagesTFT[tftpageQte]=dht22;	
			tftpageQte++;
		#endif
		#ifdef config_ds1820
			PagesTFT[tftpageQte]=ds1820;
			tftpageQte++;
		#endif	
		
		tft_loadPageNo(0);	// page initiale après le logo
		tft_update(0); 		// dernière mesure du capteur au démarrage
	}

	/**
	 * @brief Affiche le logo du projet pendant une courte période
	 * 
	 */
	void tft_ini()
	{			
		//Serial.println("tft_ini()");
		spi_settingsFor(ILI9341);  
		tft.begin();
		tft_logo();

		timer_logo.initializeMs(5000, continue_ini).startOnce(); // Affichage quelques secondes
	}

	/**
	 * @brief retourne l'int associé à la couleur RGB de fond de l'écran
	 * modifie la cpouleur de fond de l'écran TFT
	 * @param r 
	 * @param g 
	 * @param b 
	 * @return uint16_t 
	 */
	uint16_t tft_couleur_rgb(uint8_t r, uint8_t g, uint8_t b)
	{
			return tft.color565(r,g,b);
	}

	/**
	 * @brief changement de couleur de fond de l'écran TFT
	 * 
	 * @param r 0..255
	 * @param g 0..255
	 * @param b 0..255
	 * @return String 
	 */
	String tft_fond_rgb(uint8_t r, uint8_t g, uint8_t b){
		spi_settingsFor(ILI9341);
		tft.fillScreen(tft_couleur_rgb(r,g,b));	
		return "Changement couleur de fond tft\n";
	}

	/**
	 * @brief Affiche un texte à l'écran TFT
	 * 
	 * @param x 
	 * @param y 
	 * @param size 
	 * @param couleur 
	 * @param rotation 
	 * @param texte 
	 */
	void tft_String(uint16_t x, uint16_t y, uint8_t size, uint16_t couleur, uint8_t rotation, String texte )
	{
		spi_settingsFor(ILI9341);
		tft.setTextSize(size);
		tft.setCursor(x,y);
		tft.setTextColor(couleur);	
		tft.setRotation(rotation);
		tft.println(texte);	
		tft.setRotation(rotation);
	}

	/**
	 * @brief Affiche un texte à l'écran TFT et retourne un feedback
	 * 
	 * @param x 
	 * @param y 
	 * @param size 
	 * @param couleur 
	 * @param rotation 
	 * @param texte 
	 * @return String 
	 */
	String tft_print(uint16_t x, uint16_t y, uint8_t size, uint16_t couleur, uint8_t rotation, String texte ){
		tft_String(x,y,size,couleur,rotation,texte);
		return "Affichage fait\n";
	}

	/**
	 * @brief retourne l'orientation de l'écran pour l'affichage
	 * 
	 * @return uint8_t 
	 */
	uint8_t tft_getOrientation(){
		return orientation;
	}

	/**
	 * @brief Afficheur en mode paysage ou paysage inverse
	 * angles de rotation 0,1,2,3 pour 0,90,180,270°, ici seulement modes paysage utilisés
	 * @param value  0 1 	paysage 90°, paysage 270°
	 */
	void tft_setOrientation(uint8_t value){
		if(value==0){
			orientation = value;
			tft.setRotation(1);
		}
		else{
			orientation = 1;
			tft.setRotation(1);
		}
	}

	/**
	 * @brief affiche l'unité d'intervalle pour les points d'échantillons à l'écran
	 * 
	 * @param echTime 
	 */
	void tft_showTimeUnits(String echTime){
		spi_settingsFor(ILI9341);
		tft.setTextSize(1); 

		// efface ancienne valeur numérique et affiche nouvelle valeur
		tft.setCursor(200,230);
		tft.setTextColor(ILI9341_BLACK);
		tft.println("ech.."+timeUnits); 

		// nouvelle valeur d'horloge
		timeUnits = echTime;		
		tft.setCursor(200,230);
		tft.setTextColor(ILI9341_DARKGREY);
		tft.println("ech.."+timeUnits); 
	}

	/**
	 * @brief dessine l'axe X pour les échantillons à l'écran du TFT
	 * 
	 */
	void tft_draw_axeX(){
		spi_settingsFor(ILI9341);
		tft.fillScreen(0);
		int i;
		// Axe X Divisions

		// paysage
		if(orientation ==0 )rotation=3;
		else rotation=1;
		tft.setRotation(rotation);

		tft.drawFastHLine(0,205,260,ILI9341_DARKGREY);
		for(i=260;i>=60;i-=60)   tft.drawFastVLine(i,201,4,ILI9341_DARKGREY);
		for(i=260;i>30;i-=30)    tft.drawFastVLine(i,203,2,ILI9341_DARKGREY);
		
		// Axe X unit�s
		tft.setCursor(18,210);
		tft.setTextColor(ILI9341_DARKGREY);
		tft.setTextSize(1);
		tft.printf("200      150       100       50    25   ech");
		
		// Axe Y divisions
		tft.drawFastVLine(260,0,200,ILI9341_DARKGREY);
		for(i=0;i<200;i+=50)    tft.drawFastHLine(256,i,4,ILI9341_DARKGREY);
		for(i=0;i<200;i+=25)    tft.drawFastHLine(258,i,2,ILI9341_DARKGREY);
	}


	/** @brief		Interpréteur de commandes du module tft_
	 * @param[in]	ligne de commande
	 * @returns		Retourne une String donnant le résultat de l'interprétation de la commande
	 */
	String tft_interpret(String line){
		// Permet de séparer les mots d'une line de caractères
		Vector<String> Mot;
		int qteMots = splitString(line, ' ' , Mot);

		// retourne le menu
		if(qteMots == 1)	return tft_menu();

		// retourne le menu des commandes du module gestionIO
		if(qteMots == 2){
			if(Mot[1]== "menu") return tft_menu();
			else if(Mot[1]== "logo") { tft_logo(); return "Affichage du logo\n";}		
			else if (Mot[1]== "page") return tft_pages()+"\n"; 		
			else if (Mot[1]== "orientation") return "Orientation TFT "+String(tft_getOrientation())+"\n"; 					
			//else if (Mot[1]== "horloge")return tft_getVoirHorloge();
		}	

		else if(qteMots == 3){
			if(Mot[1]== "horloge") {
				if(Mot[2]=="off")tft_effaceHorloge();
				else {voir_horloge = true;tft_horloge();}
				return tft_getVoirHorloge();
			}
			if(Mot[1]== "orientation") {
				if(Mot[2]=="0")orientation =0; 
				else orientation =1; 
				tft_loadPageNo(tftpage);
				return "Orientation TFT fait\n";
			}
			if(Mot[1]== "inverse") {
				if(Mot[2]=="0"){tft.invertDisplay(0); return "TFT mode normal\n";}
				else {tft.invertDisplay(1); return "TFT mode inverse\n";}
			}	

			if (Mot[1]== "page") return tft_showPage(Mot[2]);
		}	

		if(qteMots == 4){
			if(Mot[1]== "horlogeXY") {	
				horloge_x = (uint8_t)atoi(Mot[2].c_str());
				horloge_y = (uint8_t)atoi(Mot[3].c_str());	
				return "Position horloge TFT fait\n";	
			}
		}

		if(qteMots == 5){
			if(Mot[1]== "fond") {	
				uint8_t r = (uint8_t)atoi(Mot[2].c_str());
				uint8_t g = (uint8_t)atoi(Mot[3].c_str());	
				uint8_t b = (uint8_t)atoi(Mot[4].c_str());			
				tft_fond_rgb(r, g, b);				
				return "Couleur de fond RGB fait\n";	
			}
		}

		if(qteMots >= 8){
			if(Mot[1]== "print") {	
				uint16_t x =atoi(Mot[2].c_str());
				uint16_t y =atoi(Mot[3].c_str());
				uint8_t size =atoi(Mot[4].c_str());		
				uint16_t coul =atoi(Mot[5].c_str());	
				uint8_t rotation = atoi(Mot[6].c_str());	
				uint8_t debutTexte = 15 + Mot[2].length()+Mot[3].length()+Mot[4].length()+Mot[5].length()+Mot[6].length();		
				String texte = 	line.substring(debutTexte);				
				return tft_print(x, y, size, coul, rotation, texte);
			}
		}

		if(qteMots == 6 ){
			if(Mot[1]== "bmp") {	
				uint16_t x =atoi(Mot[3].c_str());
				uint16_t y =atoi(Mot[4].c_str());
				uint8_t rotation = atoi(Mot[5].c_str());	
				spi_settingsFor(ILI9341);  
				tft.setRotation(rotation);
				bmpDraw(tft, Mot[2], x, y);		
				tft.setRotation(orientation);
				return "Dessin BMP fait\n";
			}
		}

		// commande non reconnue
		return line + "  ???\n";
	}
#endif	