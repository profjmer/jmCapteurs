/**
 * @file udp_.cpp
 * @author Jean Mercier profjmer@gmail.com
 * @brief module de gestion des communications udp
 * @version 1.0
 * 
 * Un PC de développement dans le réseau peut servir une nouvelle application. La requête est effectuée par UDP.
 * Au démarrage, l'ESP8266 broadcaste son identification dans le réseau.
 * Si il y a une application jmOTA.js qui est active dans le réseau, alors sur réception de ce broadcast, le PC envoi une requête de modifier l'adresse du serveur pour la sienne.
 * Par la suite, à partir du ESP8266, on peut activer un update de l'application par la commande ota update
 * 
 * L'ESP8266 peut recevoir d'autres requêtes par UDP et les exécuter.
 * 
 * L'ESP8266 peut transmettre en UDP à différentes adresse:port Le message doit être compatible avec l'application de réception.
 * La station ou le point d'accès doit être activé pour transmettre en UDP
 */

#include "../include/application.h"


// paramètres modifiables selon la configuration de jmOTA.js
// L'ESP8266 transmet vers ce port, le PC écoute sur ce port qui est partagé avec le conteneur
uint16_t UDP_Port_PC = 54321;

// Serveur UDP du ESP8266, il écoute sur ce port les communications du PC/conteneur
uint16_t UDP_Port_ESP = 8266;

IPAddress UDP_CLIENT_IP = IPAddress("192.168.4.2");
uint16_t UDP_CLIENT_PORT = 54321;

uint16_t remoteOTA_port;
IPAddress remoteOTA_ip;
bool udp_pub;

/**
 * @brief menu des commandes du module udp_
 * 
 * @return String 
 */
String udp_menu(){return "\nudp\nudp info\nudp esp [port]\nudp pc [port]\nudp pub [off on]\nudp client\nudp send message\nudp clientip [ip]\nudp clientport [port]\nudp broadcast message\n";}

// prototypes
void onReceive(UdpConnection& connection, char *data, int size, IPAddress remoteIP, uint16_t remotePort); // Declaration

UdpConnection udp(onReceive);

/**
 * @brief retourne les informations de la communication UDP
 * 
 * @return String 
 */
String udp_info(){
	return 	"\nUDP Port ESP "+String(UDP_Port_ESP) +"\nUDP Port PC "+	String(UDP_Port_PC)+"\nUDP Client "+UDP_CLIENT_IP.toString().c_str()+":"+String(UDP_CLIENT_PORT)+" "+udp_getPub();
}

/**
 * @brief retourne l'état de publication UDP
 * 
 * @return String 
 */
String udp_getPub(){
	if(udp_pub ==true)return "Publication UDP on\n";
	else return "Publication UDP off\n";
}

/**
 * @brief active / inactive la publication UDP
 * 
 * @param active on /off
 * @return String 
 */
String udp_setPub(String active){
	if(active == "on")udp_pub = true;
	else udp_pub = false;
	return udp_getPub();
}

/**
 * @brief Initialise l'écoute UDP et broadcaste l'identification du projet
 * 
 */
void udp_ini(){
	//Serial.println("udp_ini()");
	udp.listen(UDP_Port_ESP);
	udp_broadcastStation_ip(param_chipJSON());
}

/**
 * @brief retourne l'identification du projet en format influxDB
 * 
 * @return String 
 */
String udp_influxDB(){
	//Serial.println("udp_influxDB()");
	String json= "[  { \"name\" : \"capteurs\", \"columns\" : [\"Temperature\", \"Capteur\", \"lieux\"], \"points\" : [ [23.2, \"BME280\", \"maison\"] ] }]";
//Serial.println(json);
return "test influxdb \n";
}

/**
 * @brief Retourne le port UDP du ESP
 * 
 * @return String feedback
 */
String udp_espPort(){return "{\"type\":\"udp\",\"Port\":\""+String(UDP_Port_ESP)+"\"}\n";}

/**
 * @brief Retourne le port UDP du PC
 * 
 * @return String feedback
 */
String udp_pcPort(){return "{\"type\":\"udp\",\"Port\":\""+String(UDP_Port_PC)+"\"}\n";}

/**
 * @brief retourne IP:Port du client UDP pour transmettre les messages
 * 
 * @return String 
 */
String udp_client(){
	return "UDP"+UDP_CLIENT_IP.toString()+":"+String(UDP_CLIENT_PORT);
}

/**
 * @brief retourne l'IP du client UDP pour transmettre les messages
 * 
 * @return String 
 */
String udp_getClientIP(){
	return "IP "+UDP_CLIENT_IP.toString();
}

/**
 * @brief retourne le Port du client UDP pour transmettre les messages
 * 
 * @return String 
 */
String udp_getClientPort(){
	return "Port "+String(UDP_CLIENT_PORT);
}

/**
 * @brief Actions effectuées lors d'une réception udp
 * 
 * @param connection 
 * @param data 
 * @param size 
 * @param remoteIP 
 * @param remotePort 
 */
void onReceive(UdpConnection& connection, char *data, int size, IPAddress remoteIP, uint16_t remotePort)
{	
	String line(data);
	
	// cas 1
	// réassignation de l'adresse IP du serveur jmOTA automatiquement 
	// lors du lancement de jmOTA.js
  	if(line == "jmOTA\n"){
		#ifdef config_rtc
			rtc_espDontSleep();		// gestionRTC
		#endif
		//Serial.println("Requete jmOTA\n");
		remoteOTA_port=remotePort;
		remoteOTA_ip=remoteIP;
		ota_setIP(remoteIP.toString());	//réassignation adresse IP de jmOTA
		//Serial.println("Réponse à "+remoteIP.toString()+":"+remotePort+" -> "+param_ackChipJSON());
		udp.sendStringTo(remoteIP,remotePort, param_ackChipJSON()+"\n"); // retourne IP
		return;
	}
	
	// cas 2
	// commande transmise automatiquement par le client UDP de jmSerialCom 
	// pour fin d'identification unique, l'adresse MAC du ESP8266 est retournée
	if(line == "ESP:ServerID?\n"){
		//Serial.println("jmSerialCom UDP "+remoteIP.toString()+":"+remotePort);
		udp.sendStringTo(remoteIP,remotePort, param_chipInfo()+"\n");
		return;
	} 

	// autres cas
	// transmet la ligne de commande à l'interpréteur
	String rep = routeur(line);
	udp.sendStringTo(remoteIP,remotePort, rep);
}

/**
 * @brief envoi un message au client UDP
 * 
 * @param msg 
 */
void udp_send(String msg){
	//Serial.println("udp send "+UDP_CLIENT_IP.toString()+":"+UDP_CLIENT_PORT+"-> "+msg);
	if(udp_pub==true)udp.sendStringTo(IPAddress(UDP_CLIENT_IP),UDP_CLIENT_PORT, msg);
}

/**
 * @brief Broadcast un message à tous en UDP
 * 
 * 
 * @param msg 
 */
void udp_broadcastStation_ip(String msg)
{
	IPAddress broadcastIP = WifiStation.getIP();				// adresse de la station
	IPAddress UDPbroadcastIP;
	if(broadcastIP[0] == 0 && broadcastIP[1] == 0  && broadcastIP[2] == 0 )	UDPbroadcastIP =IPAddress(192,168,4,255);	// en mode point d'accèa
	else { UDPbroadcastIP = IPAddress(broadcastIP[0],broadcastIP[1],broadcastIP[2],255);}	

	udp.sendStringTo(UDPbroadcastIP,UDP_Port_PC,msg);
	//Serial.println("Broadcast "+ UDPbroadcastIP.toString()+":"+UDP_Port_PC+" -> "+msg);
}

/**
 * @brief envoi un message au PC ayant le service jmOTA
 * 
 * @param msg 
 */
void udp_send_remoteOTA_pc(String msg){
	//Serial.println("udp send :"+msg);
	udp.sendStringTo(remoteOTA_ip,remoteOTA_port, msg);
}

/**
 * @brief Interpréteur de commandes du module udp_
 * 
 * @param line 
 * @return String 
 */
String udp_interpret(String line){
	// Permet de séparer les mots d'une line de caractères
	Vector<String> Mot;
	int qteMots = splitString(line, ' ' , Mot);

	if(qteMots == 1){
		return udp_menu();
	}

	if(qteMots == 2){
		if(Mot[1]== "menu")	return udp_menu();
		else if(Mot[1]== "info")	return udp_info();		
		else if(Mot[1]== "esp")	return udp_espPort();		
		else if(Mot[1]== "pc")	return udp_pcPort();		
		else if(Mot[1]== "pub")	return udp_getPub();			
		else if(Mot[1]== "client") return udp_client()+"\n";		
	}	

  
	if(qteMots ==3){
		if(Mot[1]== "pub")return udp_setPub(Mot[2]);

		else if(Mot[1]== "espport"){
			UDP_Port_ESP = atol(Mot[2].c_str()); 
			if(UDP_Port_ESP >1023 && UDP_Port_ESP <65536){
				udp.listen(UDP_Port_ESP);
				return "Modification du Port UDP du ESP\n";
			}
		}

		else if(Mot[1]== "pcport"){
      		UDP_Port_PC = atol(Mot[2].c_str()); 
			if(UDP_Port_PC >1023 && UDP_Port_PC <65536){
				return "Modification du Port UDP du PC\n";
			}
    	}

		else if(Mot[1]== "clientport"){
     		 UDP_CLIENT_PORT = atol(Mot[2].c_str()); 
			if(UDP_CLIENT_PORT >1023 && UDP_CLIENT_PORT<65536){
				return "Modification PORT du Client:"+Mot[2]+"\n";
			}
  		}

		else if(Mot[1]== "clientip"){
      		UDP_CLIENT_IP = IPAddress(Mot[2]); 
			return "Modification IP du Client:"+Mot[2]+"\n";
    	}

		else if(Mot[1]== "broadcast"){
			String msg;
			int i;
			for(i=2;i<qteMots;i++)msg+=Mot[i];
			udp_broadcastStation_ip(msg);
			return "Broadcast "+ WifiStation.getIP().toString()+":"+UDP_Port_PC+" -> "+msg;
		}	

		else if(Mot[1]== "send"){
			String msg;
			int i;
			for(i=2;i<qteMots;i++)msg+=Mot[i];
			udp_send(msg +"\n");
			return "Envoi UDP " +UDP_CLIENT_IP.toString() +":"+UDP_CLIENT_PORT +" -> "+msg +"\n";
		}	
	}			
  
  return line + (" ???\n");	    
}