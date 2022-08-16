/**
 * @file ota_.cpp
 * @author Jean Mercier profjmer@gmail.com
 * @brief permet le téléversement des applications par WiFi
 * Le serveur jmOTA.js doit être démarré
 * @version 1.0
 */

#include "../include/application.h"
#include <SmingCore/Network/rBootHttpUpdate.h>
#include <IPAddress.h>
#include "rboot-api.h"


// Ceci sont les urls du serveur en mode Point d'accès
#define ROM_0_URL  "http://192.168.4.2:8686/out/firmware/rom0.bin"
#define ROM_1_URL  "http://192.168.4.2:8686/out/firmware/rom1.bin"
#define SPIFFS_URL "http://192.168.4.2:8686/out/firmware/spiff_rom.bin"

Timer ota_systemRestart_timer;

uint16_t delai_restart = 1000; // ms

rBootHttpUpdate* otaUpdater = 0;
String url_rom0 = ROM_0_URL;
String url_spiff = SPIFFS_URL;
String serveurOTA = "0.0.0.0";

/**
 * @brief Menu du module OTA
 * 
 * @return String 
 */
String ota_menu(){return "\nota menu\nota switch\nota restart\nota update\nota ap\nota serveur\n";}

/**
 * @brief Réassigne l'adresse IP du serveur de fichier jmOTA
 * 
 * @param ip 
 */
void ota_setIP(String ip){
	serveurOTA = ip;
}

/**
 * @brief Donne l'adresse IP du serveur jmOTA
 * 
 * @return String 
 */
String ota_getIP(){
	return (serveurOTA);
}

String ota_WifiAccessPoint_name()
{
 return String(version_app)+"-"+WifiAccessPoint.getMAC().substring(8,12); // version et 4 derniers chiffres de l'adresse MAC
}

/**
 * @brief Démarre le point d'accès WiFi
 * 
 */
void ota_activeAccessPoint(){
	String nomPA = ota_WifiAccessPoint_name(); // nom du PA par défaut
	if(fileExist("config.ini")){
		//Serial.println("Nom du PA selon config.ini");
		nomPA = param_getNomDuPA();
		WifiAccessPoint.config(nomPA, "", AUTH_OPEN, false, param_getCanalWiFi(), 200);
	} 
	else {
		//Serial.println("Nom du PA par défaut");
		WifiAccessPoint.config(nomPA, "", AUTH_OPEN, false, 5, 200);
	}

	WifiAccessPoint.enable(true);	
}

/**
 * @brief Delai avant de restarter le système
 * 
 */
void ota_delaiSystemRestart (){
		System.restart();
}

/**
 * @brief fin de l'update
 * 
 * @param result 
 */
//void OtaUpdate_CallBack(rBootHttpUpdate& client, bool result){
void otaUpdate_CallBack(bool result) {
	
	//Serial.println("In callback...");
	if(result == true) {
		// success
		uint8 slot;
		slot = rboot_get_current_rom();
		if (slot == 0) slot = 1; else slot = 0;
		// set to boot new rom and then reboot
		rboot_set_current_rom(slot);

		// messages
		String msg ="Succès!,  Redémarrage sur le rom "+String(slot) +"\n";		
		//Serial.print(msg);
		web_wsSend(msg);
		udp_broadcastStation_ip(msg);

		// retarder le system restart quelques millisondes
		ota_systemRestart_timer.initializeMs(delai_restart, ota_delaiSystemRestart).startOnce();	

	} else {
		// fail
		String erreur = "Erreur d'update\n";
		//Serial.print(erreur);
		web_wsSend(erreur);
		udp_broadcastStation_ip(erreur);
	}
}

/**
 * @brief Début de l'update
 * 
 */
void ota_update() {
	
	uint8 slot;
	rboot_config bootconf;
	
	timers_stop();					// arrêt des timers de publications.

	String msg = "Arrêt des timers et mise en téléchargement...\n";
	
	//Serial.print(msg);
	web_wsSend(msg);
	udp_broadcastStation_ip(msg);
	
	// need a clean object, otherwise if run before and failed will not run again
	if (otaUpdater) delete otaUpdater;
	otaUpdater = new rBootHttpUpdate();
	
	// select rom slot to flash
	bootconf = rboot_get_config();
	slot = bootconf.current_rom;
	if (slot == 0) slot = 1; else slot = 0;

	#ifndef RBOOT_TWO_ROMS
		// flash rom to position indicated in the rBoot config rom table
		otaUpdater->addItem(bootconf.roms[slot], url_rom0);
	#else
		// flash appropriate rom
		if (slot == 0) {
			otaUpdater->addItem(bootconf.roms[slot], ROM_0_URL);
		} else {
			otaUpdater->addItem(bootconf.roms[slot], ROM_1_URL);
		}
	#endif
		
	#ifndef DISABLE_SPIFFS
		// use user supplied values (defaults for 4mb flash in makefile)
		if (slot == 0) {
			otaUpdater->addItem(RBOOT_SPIFFS_0, url_spiff);
		} else {
			otaUpdater->addItem(RBOOT_SPIFFS_1, url_spiff);
		}
	#endif

	// request switch and reboot on success
	//otaUpdater->switchToRom(slot);
	// and/or set a callback (called on failure or success without switching requested)
	otaUpdater->setCallback(otaUpdate_CallBack);

	// start update
	otaUpdater->start();
}

/**
 * @brief Lance l'autre application résidente en SPIFFS
 * 
 */
void ota_switch() {
	uint8 before, after;
	before = rboot_get_current_rom();
	if (before == 0) after = 1; else after = 0;
	String msg ="Rom actif rom "+String(before) + "Redémarrage sur le rom "+String(after)+"\n";
	//Serial.print(msg);
	web_wsSend(msg);
	udp_broadcastStation_ip(msg);

	rboot_set_current_rom(after);

	// retarder le system restart quelques millisondes
	ota_systemRestart_timer.initializeMs(delai_restart, ota_delaiSystemRestart).startOnce();	// attendre la transmission des messages web de faire un system restart
}

/**
 * @brief 
 * 
 * @return String 
 */
String ota_showInfo() {
	return "SDK: "+ String(system_get_sdk_version())+ "\nFree Heap: "+String(system_get_free_heap_size())+"\nFrequence CPU: "+String(system_get_cpu_freq())
	+"\nChip id: "+String(system_get_chip_id())+	"\nSPI Flash id: "+ String(spi_flash_get_id())+"\n";
}

/**
 * @brief Charge le système de fichiers de l'application
 * 
 */
void ota_mountSPIFFS()
{
	// mount spiffs
	int slot = rboot_get_current_rom();
	#ifndef DISABLE_SPIFFS
		if (slot == 0) {
	#ifdef RBOOT_SPIFFS_0
			debugf("trying to mount spiffs at %x, length %d", RBOOT_SPIFFS_0 + 0x40200000, SPIFF_SIZE);
			spiffs_mount_manual(RBOOT_SPIFFS_0 + 0x40200000, SPIFF_SIZE);
	#else
			debugf("trying to mount spiffs at %x, length %d", 0x40300000, SPIFF_SIZE);
			spiffs_mount_manual(0x40300000, SPIFF_SIZE);
	#endif
		} else {
	#ifdef RBOOT_SPIFFS_1
			debugf("trying to mount spiffs at %x, length %d", RBOOT_SPIFFS_1 + 0x40200000, SPIFF_SIZE);
			spiffs_mount_manual(RBOOT_SPIFFS_1 + 0x40200000, SPIFF_SIZE);
	#else
			debugf("trying to mount spiffs at %x, length %d", 0x40500000, SPIFF_SIZE);
			spiffs_mount_manual(0x40500000, SPIFF_SIZE);
	#endif
		}
	#else
		debugf("spiffs disabled");
	#endif
}

/** @brief		Interpréteur de commandes du module
 * @param[in]	ligne de commande
 * @returns		Retourne une String donnant le résultat de l'interprétation de la commande
 */
String ota_interpret(String line){
	// Permet de séparer les mots d'une line de caractères
	Vector<String> Mot;
	int qteMots = splitString(line, ' ' , Mot);

	if(qteMots == 1)return ota_showInfo() +ota_menu();

	// retourne le menu des commandes du module gestionIO
	if(qteMots == 2){
		if(Mot[1]== "menu") return ota_menu();

		if(Mot[1]== "connect"){
			WifiStation.enable(true);
			return "Activation de la station\n";
		}

		if(Mot[1]== "update"){
			// selon le mode station ou point d'accès
			if(WifiStation.isConnected()){
				url_rom0 = "http://" + serveurOTA+":8686/out/firmware/rom0.bin";
				url_spiff = "http://" + serveurOTA+":8686/out/firmware/spiff_rom.bin";
			}
			else {
				url_rom0 = ROM_0_URL;
				url_spiff = SPIFFS_URL;
			}
			//Serial.println("urls de téléchargement");
			//Serial.println(url_rom0);
			//Serial.println(url_spiff);
			ota_update();
			return "Téléchargement...\nAncienne station IP:"+WifiStation.getIP().toString()+"\n";
		}

		if(Mot[1]== "switch"){
			ota_switch();
			return "fin switch\n";
		}

		if(Mot[1]== "restart"){
			System.restart();
			return "restart\n";
		}

		if(Mot[1]== "ap"){
			ota_activeAccessPoint();
			return "Activation de "+ String(version_app) +"\n";		
		}

		if(Mot[1]== "serveur"){
			return ota_getIP()+"\n";		
		}
	}

	// permet d'assigner l'adresse IP du serveur de fichiers OTA
	if(qteMots == 3){
		if(Mot[1]== "serveur"){
			serveurOTA = Mot[2];	//réassignation IP
			return ota_getIP()+"\n";
		}
	}	

  return line + ("???\n");			
}
