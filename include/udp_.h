/**
 * @file udp_.h
 * @author Jean Mercier profjmer@gmail.com
 * @brief header du module udp_.cpp, architecture jmObjean_2023
 * @version 1.0 
 */
#ifndef INCLUDE_UDPSERVER_H_
#define INCLUDE_UDPSERVER_H_

	void udp_broadcastStation_ip(String msg);
	void udp_send_remoteOTA_pc(String msg);
	void udp_send(String msg);	
	String udp_interpret(String line);
	void udp_ini();
	String udp_menu();
	String udp_info();
	String udp_client();
	String udp_getClientIP();
	String udp_getClientPort();	
	String udp_getPub();	
	String udp_setPub(String active);
#endif /* INCLUDE_UDPSERVER_H_ */
