/**
 * @file station_.h
 * @author Jean Mercier profjmer@gmail.com
 * @brief header du module station_.cpp, architecture jmObjean_2023
 * @version 1.0
 */
#ifndef INCLUDE_STATION_H_
#define INCLUDE_STATION_H_
	void station_networkScanCompleted(bool succeeded, BssList list);
	void station_connectOk(IPAddress ip, IPAddress mask, IPAddress gateway);
	void station_connectFail(String ssid, uint8_t ssidLength, uint8_t* bssid, uint8_t reason);
	void station_ini();
	String station_menu();
	String station_ssid();
	String station_interpret(String line);	

#endif //INCLUDE_STATION_H_