/**
 * @file param_.h
 * @author Jean Mercier profjmer@gmail.com
 * @brief header du module param_.cpp, architecture jmObjean_2023
 * @version 1.0
 * @date 2020-03-04
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#ifndef INCLUDE_param_H_
#define INCLUDE_param_H_

  #define eveil 1
  #define faibleConsommation 0

  void param_ini();
  String param_ackChipJSON();
  String param_chipJSON();
  String param_chipInfo();
  String param_info();
  String param_menu();
  uint8_t param_getCanalWiFi();   
  String param_getNomDuPA();
  String param_getBoardID();
  String param_getFirmwareID();
  String param_getChipID();
  String param_get_jmObjean_ID_JSON();
  String param_getLieu();
  String param_getModules();
  String param_getModulesJSON();
  uint8_t param_getFonctionnement();
  void param_setFonctionnement(uint8_t modeFonctionnement);
  String param_getModeFonctionnement();
  String param_wifi();


  String param_interpret(String line); 
#endif