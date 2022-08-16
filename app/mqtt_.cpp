/**
 * @file mqtt_.cpp
 * @author profjmer@gmail.com
 * @brief gestion du module client MQTT
 * @version 0.1
 * @date 2020-01-31
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "../include/application.h"

#ifdef config_mqtt

	// Forward declarations
	void startMqttClient();
	void onMessageReceived(String topic, String message);

	String mqttClientID = "profjmer";
	String mqttUserName = "profjmer";
	String mqttPassword = "aio_PMrP14D6XS8KoqSh1wlsPxqk4ScJ";
	String mqttBroker = "io.adafruit.com";
	uint16_t mqttPort = 1883;	// broker local
	bool mqttPub;
	String boardID;

	// MQTT client
	MqttClient *mqtt;

	/********************* PARAM MQTT ******************************/

	/**
	 * @brief Retourne si mqtt peut publier
	 * 
	 * @return String 
	 */
	String mqtt_etatPub(){
		if(mqttPub == true){
			return "MQTT pub on\n";
		}
		else {
			return "MQTT pub off\n";
		}
	}

	/**
	 * @brief Active ou inactive la publication mqtt
	 * 
	 * @param action on ou off
	 * @return String Retourne si mqtt peut publier
	 */
	String mqtt_activePub(String action){
		if(action == "on"){
			mqttPub = true;
		}
		else {
			mqttPub = false;
		}
		return mqtt_etatPub();
	}

	String mqtt_getBroker(){
		return mqttBroker;
	}


	String mqtt_setBroker(String broker){
		mqttBroker = broker;
		return "MQTT Host: "+broker+"\n";
	}

	/**
	 * @brief Get Broker Port 
	 * 
	 * @return uint16_t 
	 */
	uint16_t mqtt_getBrokerPort(){
		return mqttPort;
	}

	/**
	 * @brief Change le paramètre Broker port
	 * 
	 * @param port 
	 * @return String feedback
	 */
	String mqtt_setBrokerPort(uint16_t port){
		mqttPort = port;
		return "Broker port "+String(mqtt_getBrokerPort())+"\n"; 
	}

	/**
	 * @brief Get adresse du BrokerIP:port MQTT
	 * 
	 * @return String adresse IP:port du broker
	 */
	String mqtt_getBrokerAndPort(){
		return mqtt_getBroker()+":"+String(mqtt_getBrokerPort())+"\n";
	}

	String mqtt_getPassword(){
		return mqttPassword;
	}

	String mqtt_setPassword(String PWD){
		mqttPassword = PWD;
		return "MQTT password: "+PWD+"\n";
	}

	String mqtt_getClientID(){
		return mqttClientID; 
	}  

	String mqtt_setClientID(String client){
		mqttClientID = client;
		return "MQTT clientid: "+client+"\n";
	}

	String mqtt_getUserName(){
		return mqttUserName; 
	}  

	String mqtt_setUserName(String client){
		mqttUserName = client;
		return "MQTT username: "+client+"\n";
	}

	bool mqtt_isConnected(){
		if (mqtt->getConnectionState() == eTCS_Connected) return true;
		else return false;
	}

	#ifdef config_uart
		// Check for MQTT Disconnection
		void checkMQTTDisconnect(TcpClient& client, bool flag){
			
			// Called whenever MQTT connection is failed.
			if (flag == true)
				Serial.println("MQTT Broker Déconnecté!!");
			else
				Serial.println("MQTT Broker absent!!");
		}

		void onMessageDelivered(uint16_t msgId, int type) {
			Serial.printf("Message ayant id %d et QoS %d a été délivré avec succès.", msgId, (type==MQTT_MSG_PUBREC? 2: 1));
		}
	#endif

	
	/**
	 * @brief publie un message mqtt. Le broker doit être initialisévers le bon broker et port.
	 * 
	 * @param capteur 
	 * @param msg valeur du capteur
	 */
	void mqtt_pub(String capteur,String msg){
		String topic;
		if(mqttPub){

			if (mqtt->getConnectionState() != eTCS_Connected){
				startMqttClient(); // Auto reconnect seulement si wifi est actif 
			}

	
			// Publication selon le broker
			if(mqtt_getBroker() == "nodered"){	// publication vers broker nodered
				topic = "jmCapteurs/"+param_getLieu()+"/capteur/"+capteur;
			}
			else {	// publication vers adafruit.io
			
				topic = mqtt_getClientID() + "/feeds/"+param_getNomDuPA()+"-"+param_getLieu()+capteur;
				topic.toLowerCase();
				Serial.println(topic+" "+msg);
			}

			mqtt->publish(topic, msg); // la publication  si active

			//mqtt->publishWithQoS(PubPile, msg, 1, false, onMessageDelivered); 

		}

	}

	// Callback for messages, arrived from MQTT server
	void onMessageReceived(String topic, String line)
	{
		#ifdef config_uart
			Serial.println(topic);
			Serial.println(line);
		#endif
		String rep = routeur(line+"\n"); // les messages mqttlens et mqttbox n'ont pas de line feed
		mqtt_pub("jmCapteurs/feedback",rep);
	}

	// Run MQTT client
	
	void startMqttClient()
	{
		#ifdef config_uart		
			Serial.println("MQTT se connecte: clientID: "+mqttClientID+" username: "+mqttUserName+ " password: "+mqttPassword);
		#endif
		mqtt->connect(mqttClientID, mqttUserName, mqttPassword, true);
	
		#ifdef ENABLE_SSL
			mqtt->addSslOptions(SSL_SERVER_VERIFY_LATER);

			#include <ssl/private_key.h>
			#include <ssl/cert.h>

			mqtt->setSslClientKeyCert(default_private_key, default_private_key_len,
										default_certificate, default_certificate_len, NULL, true);
		#endif

		// Assign a disconnect callback function
		mqtt->setCompleteDelegate(checkMQTTDisconnect);
		//mqtt->subscribe("jmObjean/command");
	}

	String nop(){return "";}

	String mqtt_menu(){
		return "\nmqtt info\nmqtt broker [brokername]\nmqtt brokerport [1000-65536]\nmqtt clientid [clientID]"+nop()+
		"mqtt username [nom]\nmqtt password [string]\nmqtt pub [on/off]\nmqtt restart\n";
	}

	String mqtt_info(){
		return   "\n"+mqtt_etatPub()+
    "MQTT broker: "+mqtt_getBroker()+"\nMQTT brokerport:"+String(mqtt_getBrokerPort())+ 
    "\nMQTT username: "+mqtt_getUserName()+
		"\nMQTT clientid: "+mqtt_getClientID()+
    "\nMQTT password: "+mqtt_getPassword()+
		"\n";
	}

	void onMqttMessageReceived(String topic, String message)
	{
		#ifdef config_uart
			Serial.println("MQTT Topic:"+topic+" msg:+message");
		#endif
	}

	void mqtt_ini(){ 
		#ifdef config_uart
			Serial.println("mqtt_ini()");
			Serial.println("MQTT Broker: "+mqtt_getBroker()+" Port: "+mqttPort);
		#endif
		mqtt = new MqttClient(mqttBroker, mqttPort, onMessageReceived);
	}


	/**
	 * @brief Interpréteur du module de gestion MQTT
	 * 
	 * @param line 
	 * @return String 
	 */
	String mqtt_interpret(String line){
		// Permet de séparer les mots d'une line de caractères
		Vector<String> Mot;
		int qteMots = splitString(line, ' ' , Mot);

		if(qteMots == 1)return mqtt_menu();

		else if(qteMots == 2){
			if(Mot[1]== "menu")	return mqtt_menu();
			else if(Mot[1]== "info")	return mqtt_info()+"\n";
			else if(Mot[1] == "restart") { mqtt_ini(); return "MQTT restarted\n";	}
		}

		else if(qteMots == 3){
			if(Mot[1]== "brokerip") return ("Adresse ip du Broker MQTT : "+mqtt_setBroker(Mot[2])+"\n");
			else if(Mot[1]== "brokerport") return ("Port du Broker MQTT : " +mqtt_setBrokerPort((int16_t)atoi(Mot[2].c_str())) +"\n");
			//else if(Mot[1]== "topic") return mqtt_setPubTopic(Mot[2]);
			//else if(Mot[1]== "subtopic") return mqtt_setSubTopic(Mot[2]);
			else if(Mot[1]== "broker") return mqtt_setBroker(Mot[2]);	
			else if(Mot[1]== "username") return mqtt_setUserName(Mot[2]);
			else if(Mot[1]== "password") return mqtt_setPassword(Mot[2]);
			else if(Mot[1]== "clientid") return mqtt_setClientID(Mot[2]);	
			else if(Mot[1]== "pub")	return mqtt_activePub(Mot[2]);																			
		}
		
		return line + ("???\n");
	}
#endif
