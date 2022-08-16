/**
 * @file onewire_.h
 * @author Jean Mercier profjmer@gmail.com
 * @brief header du module onewire_.cpp, architecture jmObjean_2023
 * @version 1.0
 * @date 2020-03-04
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#ifndef INCLUDE_OneWire_H_
#define INCLUDE_OneWire_H_

  void onewire_ini();
  String onewire_menu();
  String onewire_presence();
  String onewire_scan();
  String onewire_interpret(String line);
  int onewire_getPin();

#endif //INCLUDE_OneWire_H_

