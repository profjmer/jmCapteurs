/**
 * @file ota_.h
 * @author Jean Mercier profjmer@gmail.com
 * @brief header du module ota_.cpp, architecture jmObjean_2023
 * @version 1.0
 * @date 2020-03-04
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#ifndef INCLUDE_gestionOTA_H_
#define INCLUDE_gestionOTA_H_

	//void ota_ini();
  String ota_menu();
  String ota_interpret(String line);
  String ota_getIP();
  void ota_setIP(String ip);
  void ota_mountSPIFFS();
  void ota_activeAccessPoint();
  String ota_WifiAccessPoint_name();
  void ota_systemRestart();
		
#endif //INCLUDE_gestionOTA_H_