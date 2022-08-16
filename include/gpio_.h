/**
 * @file gpio_.h
 * @author Jean Mercier profjmer@gmail.com
 * @brief header du module gpio_.cpp, architecture jmObjean_2023
 * @version 1.0
 */
#ifdef config_gpio
	#ifndef INCLUDE_gestionIO_H_
	#define INCLUDE_gestionIO_H_

		#include <WString.h>
		void gpio_ini();
		String gpio_menu();
		String gpio_interpret(String line);
		void gpio_status();
			
	#endif //INCLUDE_gestionIO_H_
#endif




