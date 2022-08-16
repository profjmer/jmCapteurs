/**
 * @file mqtt_.h
 * @author Jean Mercier profjmer@gmail.com
 * @brief header du module mqtt_.cpp, architecture jmObjean_2023
 * @version 1.0
 */
#ifndef INCLUDE_gestionMQTT_H_
#define INCLUDE_gestionMQTT_H_

	void mqtt_ini();
	String mqtt_menu();
	void mqtt_pub(String topic,String msg);
	String mqtt_interpret(String line);
	String mqttpub_setTopic(String topic);
	bool mqtt_isConnected();

	String mqtt_info();
	String mqtt_getBroker();
	String mqtt_getBrokerName();
	uint16_t mqtt_getBrokerPort();
	String mqtt_getBrokerAndPort();
	String mqtt_getPassword();
	String mqtt_getClientID();
	String mqtt_getUser();
	String mqtt_getPubTopic();
	String mqtt_setPubTopic(String topic);
	String mqtt_getSubTopic();
	String mqtt_setSubTopic(String sub);
		
#endif //INCLUDE_gestionMQTT_H_