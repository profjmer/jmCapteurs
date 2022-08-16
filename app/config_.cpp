/**
 * @file config_.cpp
 * @author Jean Mercier profjmer@gmail.com
 * @brief Plateforme jmObjean 2023, module de configuration initiale, utilise le fichier de commande config.ini dans la flash
 * et pouvant être modifié avec un client FTP
 * Le fichier config.h permet une initialisation conditionnelle des modules. Les modules non commentés initialisés et leur menu est ajouté au projet.
 * @version 1.0
 */

#include "../include/application.h"

/**
 * @brief Chargement du fichier configg.ini de commandes situé dans la flash, permettant une reconfiguration initiale
 * 
 * @param file 
 * @return String (les lignes de commandes du fichier)
 */
String config_charge(String file){
  String s ="\nprofjmer@gmail.com";
  //Serial.println(s);

	if(fileExist(file)){
		//Serial.println(file+" existe");
		return fileGetContent(file);
  }
	else {
		//Serial.println(file +" introuvable, valeurs par défaut");
    return "";
	}
}

/**
 * @brief Execute des lignes de commandes
 * 
 * @param commandes 
 */
void config_execute(String commandes){
    int i;
    Vector<String> lignes;
		int qteLignes = splitString(commandes, '\n' , lignes); // sépare les commandes en lignes de commandes
    //Serial.println("Qté de lignes de commande: "+String(qteLignes));
    //Serial.println("Exécution des commandes\n");
		for(i=0;i<qteLignes;i++){
      String rep = routeur(lignes[i]);
      //Serial.println(rep);
		}
}