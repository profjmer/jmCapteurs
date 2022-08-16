/**
 * @file uart_.cpp
 * @author Jean Mercier profjmer@gmail.com
 * @brief module de gestion des communications uart
 * @version 1.0
 */
#include "../include/application.h"

String line;

/**
 * @brief À faire lors de la réception uart
 * 
 * @param stream 
 * @param arrivedChar 
 * @param availableCharsCount 
 */
void serialCallBack(Stream& stream, char arrivedChar, unsigned short availableCharsCount) {
	char car = stream.read();
	line += car;
	if(car=='\n'){
		// route vers le module, interprète commande et affiche résultat
		String rep = routeur(line);
		Serial.println(rep);
		line ="";
	}
}

/**
 * @brief Initialisation du module uart_
 * 
 */
void uart_ini(){
	Serial.println("uart_ini()");
	Serial.setCallback(serialCallBack);
}
