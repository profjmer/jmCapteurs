/**
 * @file spi_.cpp
 * @author Jean Mercier profjmer@gmail.com
 * @brief module de gestion SPI
 * @version 1.0
 * Le module SPI peut communiquer avec différents composants SPI ayant des vitesses différentes et des modes différents.
 * Ce module gère le fonctionnement approprié du protocole spi selon le composant utilisé.
 * Seulement une transaction à la fois est permise.
 */

#include "../include/application.h"
#ifdef config_spi
	#include <SPISettings.h>

	// Broches communes selon le schéma jmObjean2021
	// HSPIQ MISO GPIO12
	// HSPID MOSI GPIO13
	// CLK GPIO14

	// La sélection du composant 
	// T_CS	GPIO3 	touch ou max7219 selon spi_.h
	// SD_CS GPIO1 	carte SD
	// CS	GPIO15 		écran tft



	String dispositifs[5]={"ILI9341","XPT2046","SD_CARD","MAX7219","USER"};
	uint8_t dernierDispositif;	// permet de savoir la configuration du spi
	SPISettings settings;		// pour la vitesse, l'ordre et le mode
	uint8_t chipSelect;			// sélection du dispositif
	bool spi_enUtilisation= false;

	// valeurs par défaut pour l'utilisateur
	uint8_t uChipSelect =15;		// selon l'utilisateur par ligne de commande
	int uCLK= 1000000;
	uint8_t uOrdre= 1;
	uint8_t uMode = SPI_MODE3;

	#define spiTxBufSize 8
	uint16_t spiTxBuf[spiTxBufSize];


	/**
	 * @brief Mettre les broches de sélection en sortie et au niveau élevé. Les dipositifs ne sont pas sélectionnés.
	 * 
	 */
	void spi_ini(){
		//Serial.println("spi_ini(), Attention conflit possible entre TFT et Max7219, initialiser seulement un des deux dans application.cpp");
		digitalWrite(pin_SPI_TFT_CS,1);
		pinMode(pin_SPI_TFT_CS,OUTPUT);
		#ifdef config_touch
			digitalWrite(pin_SPI_TOUCH_CS,1);
			pinMode(pin_SPI_TOUCH_CS,OUTPUT);
		#endif
		#ifdef config_max7219
			digitalWrite(pin_SPI_MAX7219_CS,1);
			pinMode(pin_SPI_MAX7219_CS,OUTPUT);	
		#endif
		chipSelect = ILI9341;	// chip select initial pour le SPI, modifiable via ILC
		SPI.begin();
		//spi_settingsFor(MAX7219);
	}

	/**
	 * @brief Menu des commandes du module spi_
	 * 
	 * @return String 
	 */
	String spi_menu(){
	return "\nspi\nspi settings\nspi settings cs[0..16] clock[Hz] ordre[lsb..msb] mode[0..3]\nspi tranfert8 data[00..ff]\nspi tranfert16 data [00..ffff]\n";
	}

	/**
	 * @brief Le dispositif est'il en utilisation ?
	 * 
	 * @return true 
	 * @return false 
	 */
	bool spi_enTransaction(){
		return spi_enUtilisation;
	}

	/**
	 * @brief Permet de réserver l'utilisation du spi
	 * 
	 */
	void spi_reservation(){
		spi_enUtilisation=true;
	}

	/**
	 * @brief Libère le spi
	 * 
	 */
	void spi_liberation(){
		spi_enUtilisation=false;
	}

	/**
	 * @brief transmet un mot de 16 bits et libère le spi
	 * 
	 * @param data 
	 * @return uint16_t Mot retourné par le dispositif
	 */
	uint16_t spi_transfert_16(uint16_t data){
		spi_enUtilisation=true;
		digitalWrite(chipSelect,0);
		uint16_t receivedVal = SPI.transfer16(data);	//hspi
		digitalWrite(chipSelect,1);
		SPI.endTransaction();
		spi_enUtilisation=false;	
		return receivedVal;
	}

	/**
	 * @brief Transfert spi bidirectionnel de 16 bits
	 * 
	 * @param data 16 bits
	 * @return String 
	 */
	String spi_transfert16(uint16_t data){
		if(spi_enTransaction())return"occupé\n";
		uint16_t receivedVal16 = spi_transfert_16(data);
		return "Transfert 16 bits TX: 0x"+ String(data,16) + "  RX: 0x"+String(receivedVal16,16)+"\n";
	}

	/**
	 * @brief Transfert spi bidirectionnel de 8 bits
	 * 
	 * @param data  8 bits
	 * @return uint8_t 
	 */
	uint8_t spi_transfert_8(uint8_t data){
		spi_enUtilisation=true;
		digitalWrite(chipSelect,0);
		uint8_t receivedVal = SPI.transfer(data);
		digitalWrite(chipSelect,1);
		SPI.endTransaction();
		spi_enUtilisation=false;
		return receivedVal;	
	}

	/**
	 * @brief Transfert spi bidirectionnel de 8bits
	 * 
	 * @param cs  chip select de l'esclave
	 * @param data 8bits
	 * @return String 
	 */
	String spi_transfert8(uint8_t data){
		if(spi_enTransaction())return"occupé\n";
		uint8_t receivedVal = spi_transfert_8(data);
		return "Transfert 8 bits TX: 0x"+ String(data,16) + "  RX: 0x"+String(receivedVal,16)+"\n";
	}

	/**
	 * @brief Permet de transmettre plusieurs mots de 16 bits. Utile pour afficher des symboles et caractères au max7219
	 * 
	 * @param buf 
	 * @param bufLen 
	 * @return String 
	 */
	String spi_transferts_16(uint16_t *buf, uint8_t bufLen){
		if(spi_enTransaction())return"occupé\n";
		spi_enUtilisation=true;	
		uint16_t rxBuf[bufLen],i;

		for(int i=0;i<bufLen;i++){
			digitalWrite(chipSelect,0);
			rxBuf[i]= SPI.transfer16(buf[i]);	//hspi
			digitalWrite(chipSelect,1);
		}

		SPI.endTransaction();
		spi_enUtilisation=false;	
		return "Transfert du Buffer fait\n";
	}

	/**
	 * @brief retourne la chaine décrivant l'ordre et le mode spi
	 * 
	 * @param ordre [0..1]
	 * @param mode [0, 0x0f, 0xf0,0xff]
	 * @return String [lsb..msb] [spi_mode(0..3)]
	 */
	String spi_ordre_mode(uint8_t ordre, uint8_t mode){
			String s_mode, s_ordre;
			switch(ordre){
			case 0: s_ordre ="lsb";break;
			case 1: s_ordre = "msb";break;
		}
		switch(mode){	// selon SPISettings.h SmingCore
			case 0: s_mode ="spi_mode0"; break;
			case 0x0f: s_mode ="spi_mode1";	break;
			case 0xf0: s_mode ="spi_mode2"; break;
			case 0xff: s_mode ="spi_mode3";	break;							
		}
		if(ordre>1 || (mode!=0 && mode != 0x0f && mode != 0xf0 && mode !=0xff))return"SPI settings erreur ordre/mode\n";

		return s_ordre+ " "+s_mode+"\n";
	}

	/**
	 * @brief détermine le mode de fonctionnement du spi
	 * 
	 * @param clk 
	 * @param ordre 
	 * @param mode 
	 */
	void spi_settings(int clk, uint8_t ordre, uint8_t mode){
		settings = SPISettings(clk, ordre, mode);
		SPI.beginTransaction(settings);
		//Serial.println("CS "+String(chipSelect)+" CLK "+String(clk)+" "+spi_ordre_mode(ordre,mode));
	}

	/**
	 * @brief détermine le mode de fonctionnement du spi selon un device ID
	 * 
	 * @param deviceID  ILI9341(40MHZ) XPT2046(2Mhz) MAX7219(8Mhz) USER(définit uChipselect et utiliser spi_settings(cs,ordre,mode))
	 */
	void spi_settingsFor(uint8_t deviceID){
		if(dernierDispositif==deviceID)return;
		dernierDispositif = deviceID;
		//Serial.println("Dispositif SPI "+dispositifs[deviceID]);
		switch(deviceID){
			#ifdef config_tft
				case ILI9341: chipSelect=pin_SPI_TFT_CS;spi_settings(40000000,MSBFIRST,SPI_MODE0);break; 	// 40MHz
			#endif
			#ifdef config_touch
				case XPT2046: chipSelect=pin_SPI_TFT_CS;spi_settings(2000000,MSBFIRST,SPI_MODE2);break;		// 2MHz
			#endif
			#ifdef config_max7219
				case MAX7219: chipSelect=pin_SPI_MAX7219_CS;spi_settings(8000000,MSBFIRST,SPI_MODE3);break;		// 8MHz
			#endif
			case USER: chipSelect=uChipSelect;spi_settings(uCLK,uOrdre,uMode);break;				// selon la commande de l'utilisateur
		}	
	}
	/**
	 * @brief Interpréteur de commandes du module spi_
	 * 
	 * @param line ligne de commande
	 * @return String feedback
	 */
	String spi_interpret(String line){
		// Permet de séparer les mots d'une line de caractères
		Vector<String> Mot;
		int qteMots = splitString(line, ' ' , Mot);

		if(qteMots == 1)return spi_menu();
		
		if(qteMots == 2){	
			if(Mot[1]== "settings"){
				return "SPI utilisateur settings: CS "+String(uChipSelect)+" Clock "+ String(uCLK)+ "Hz " +spi_ordre_mode(uOrdre,uMode); 	
			}		

		}

		if(qteMots == 3){		// on doit utiliser spi settings avant de transférer des données.
			if(Mot[1]== "transfert8"){
				spi_settingsFor(USER);
				return  spi_transfert8((uint8_t)strtol(Mot[2].c_str(),NULL,16));
			}
			if(Mot[1]== "transfert16"){
				spi_settingsFor(USER);
				return spi_transfert16((uint16_t)strtol(Mot[2].c_str(),NULL,16));	
			}				
		}

		if(qteMots >2){	
			if(Mot[1]== "transferts16"){
				spi_settingsFor(USER);
				uint8_t qte = qteMots-2;
				uint16_t buf[qte];
				for (int i=0;i<qte;i++){
					buf[i] = (uint16_t)strtol(Mot[2+i].c_str(),NULL,16);
					//Serial.printf("%x ",buf[i]);
				}
				return spi_transferts_16(buf,qte);
			}	
		}

		if(qteMots == 6){		// permet de spécifier les paramètres de communication et le dispositif
			if(Mot[1]== "settings"){
				uChipSelect = (uint)atoi(Mot[2].c_str());
				if(uChipSelect >16)uChipSelect=15;
				uCLK = (int)atoi(Mot[3].c_str());
				if(Mot[4]=="lsb")uOrdre =0;
				else uOrdre =1;
				uint8_t mode = (uint8_t)atoi(Mot[5].c_str());
				if(mode >3)return "Erreur mode\n";
				switch(mode){
					case 0: uMode=SPI_MODE0;break;
					case 1: uMode=SPI_MODE1;break;
					case 2: uMode=SPI_MODE2;break;
					case 3: uMode=SPI_MODE3;break;				
				}
				spi_settingsFor(USER);
				return "SPI userSettings: CS "+String(uChipSelect)+" Clock "+ String(uCLK)+ "Hz " +Mot[4]+ " spi_mode "+String(mode)+"\n"; 
			}	
		}

		return line + ("???\n");

	}
#endif
