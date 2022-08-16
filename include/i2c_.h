/**
 * @file i2c_.h
 * @author Jean Mercier profjmer@gmail.com
 * @brief header du module i2c_.cpp, architecture jmObjean_2023
 * @version 1.0
 */

#ifndef INCLUDE_gestionI2C_H_
#define INCLUDE_gestionI2C_H_
	
	// détermine les broches I2C et initialise le bus
	void i2c_ini();

	// retourne les commandes du module
	String i2c_menu();
	
	// retourne une String comprenant les esclaves trouvés
	String i2c_scanBus();

	String i2c_writeString(String line);
	String i2c_readString(String line);

	// écrire à l'adresseI2C dans le registre à l'adresseReg un octet de data
	String i2c_write(uint8_t I2CdeviceAddress, uint8_t address, uint8_t data);

	// écrire le contenu d'un tableau d'octets à partir de registerID
	String i2c_writeArray(uint8_t I2CdeviceAddress,uint8_t registerID,uint8_t *p, uint8_t qty);

	// lire le contenu des registres débutant à address du slave I2CdeviceAddress, la quantité à lire est length
	String i2c_read(uint8_t I2CdeviceAddress, uint8_t address, uint8_t length);

	// lire un octet
	uint8_t i2c_readB(uint8_t I2CdeviceAddress, uint8_t address);

	// interpréteur de la ligne de commande
	String i2c_interpret(String line);
	
#endif //INCLUDE_gestionI2C_H__