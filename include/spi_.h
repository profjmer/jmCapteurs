/**
 * @file spi_.h
 * @author Jean Mercier profjmer@gmail.com
 * @brief header du module spi_.cpp, architecture jmObjean_2023
 * @version 1.0
 */

#ifndef INCLUDE_gestionSPI_H_
#define INCLUDE_gestionSPI_H_

	// identification des dispositifs 
	#define ILI9341 0
	#define XPT2046 1
	#define SD 2
	#define MAX7219 3
	#define USER 4


	// retourne les commandes du module
	String spi_menu();
	void spi_ini();
	uint16_t spi_transfert_16(uint16_t data);
	uint8_t spi_transfert_8(uint8_t data);
	void spi_settingsFor(uint8_t device);
	bool spi_enTransaction();
	void spi_reservation();
	void spi_liberation();

	// interpréteur de la ligne de commande
	String spi_interpret(String line);
	
#endif //INCLUDE_gestionSPI_H__