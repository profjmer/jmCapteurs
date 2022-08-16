/**
 * @file web_.h
 * @author Jean Mercier profjmer@gmail.com
 * @brief header du module web_.cpp, architecture jmObjean_2023
 * @version 1.0
 */
#ifndef INCLUDE_WEBSERVER_H_
#define INCLUDE_WEBSERVER_H_

	void web_wsSend(String msg);
	void web_ini();
	void web_ftp_ini();
	uint8_t web_getActiveClients();
	
#endif /* INCLUDE_WEBSERVER_H_ */
