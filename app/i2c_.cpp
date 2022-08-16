/**
 * @file i2c_.cpp
 * @author Jean Mercier profjmer@gmail.com
 * @brief module de gestion I2C
 * @version 1.0
 */

#include "../include/application.h"
#ifdef config_i2c
	/* broches par défaut pour I2C */
	int SCL_PIN = pin_I2C_SCL;
	int SDA_PIN = pin_I2C_SDA;

	/**
	 * @brief initialise les broches I2C
	 * 
	 */
	void i2c_ini(){
		//Serial.println("i2c_ini()");
		Wire.pins(SCL_PIN, SDA_PIN); //SCL SDA
		Wire.begin();
	}

	/**
	 * @brief Menu des commandes du module
	 * 
	 * @return String 
	 */
	String i2c_menu(){
	return "\ni2c\ni2c scan\nPour les commandes I2C, les valeurs sont hexa,\
	sauf pour qty qui est decimale\ni2c read adresseI2C adresseReg qty\ni2c write adresseI2C adresseReg octet\ni2c writebuf adresseI2C adresseReg b1 b2 ...\n";
	}

	/**
	 * @brief retourne une String comprenant les esclaves trouvés
	 * 
	 * @return String contenant la liste des esclaves actifs sur le bus
	 */
	String i2c_scanBus(){
		uint8_t error, address;
		int nDevices;
		char rep[250];
		String res;

		nDevices = 0;
		for(address = 1; address < 127; address++ )
		{
			// The i2c_scanner uses the return value of
			// the Write.endTransmisstion to see if
			// a device did acknowledge to the address.
			Wire.beginTransmission(address);
			error = Wire.endTransmission();

			WDT.alive(); // Second option: notify Watch Dog what you are alive (feed it)

			if (error == 0)
			{
				//Serial.printf("I2C device found at address 0x%#x\n",address);
				sprintf(rep,"I2C esclave  0x%#x\n",address);
				res += rep;

				nDevices++;
			}
			else if (error==4)
			{
				//Serial.println("erreur 4");
				return "erreur 4";
			}
		}
		//Serial.println("Scan done");
		if (nDevices == 0)
		// Serial.println("no I2C device found");
		return "Aucun esclave I2C disponible\n";
		else{
		//Serial.println("done");
		
		return res +"Fin de la Recherche\n";
		}
	}

	/**
	 * @brief Lit les octets d'un esclave
	 * 
	 * @param line contient les paramètres
	 * @return String contenant les octets
	 */
	String i2c_readString(String line)
	{	Vector<String> Mot;
		//Serial.println(line);
		int qteMots = splitString(line, ' ' , Mot);
		uint8_t I2CdeviceAddress = strtol(Mot[0].c_str(),NULL,16);
		uint8_t registerID = strtol(Mot[1].c_str(),NULL,16);
		uint8_t qty = strtol(Mot[2].c_str(),NULL,10);
		String rep = "Lire I2C 0x"+Mot[0] +"  Début 0x"+Mot[1]+"  Qté "+Mot[2]+"  HEX->";
		rep += i2c_read(I2CdeviceAddress, registerID,qty)+"\n";
		return rep;
	}

	/**
	 * @brief Lit un octet
	 * 
	 * @param I2CdeviceAddress 
	 * @param address 
	 * @return uint8_t 
	 */
	uint8_t i2c_readB(uint8_t I2CdeviceAddress, uint8_t address)
	{
		uint8_t b;

		Wire.beginTransmission(I2CdeviceAddress);
		Wire.write(address);
		Wire.endTransmission();
		
		Wire.beginTransmission(I2CdeviceAddress);
		Wire.requestFrom(I2CdeviceAddress, address);
		b= Wire.read();
		Wire.endTransmission();
		return b;
	}


	/**
	 * @brief lit le contenu des registres débutant à address du slave I2CdeviceAddress, la quantité à lire est length
	 * 
	 * @param I2CdeviceAddress 
	 * @param address 
	 * @param length 
	 * @return String contenant les octets
	 */
	String i2c_read(uint8_t I2CdeviceAddress, uint8_t address, uint8_t length)
	{
		uint8_t RxBufferI2C[64];
		int i;
		String res;
		
		if(length > 32)return  "Erreur qté > 32\n";
		Wire.beginTransmission(I2CdeviceAddress);
		Wire.write(address);
		Wire.endTransmission();
		
		Wire.beginTransmission(I2CdeviceAddress);
		Wire.requestFrom(I2CdeviceAddress, length);

		while(Wire.available())
		{
			for(i = 0; i < length; i++)
			{
				RxBufferI2C[i] = Wire.read();
			}
		}
		Wire.endTransmission();
	
		// tableau de char en string
		for(uint8_t i=0;i<length;i++){
		res += " " +String(RxBufferI2C[i],16); // ff
		}
		res += "\n";
		res.toUpperCase();
		return res;
	}

	/**
	 * @brief Écrit des octets dans un esclave
	 * 
	 * @param line contenant les octets
	 * @return String feedback
	 */
	String i2c_writeString(String line)
	{	Vector<String> Mot;
		int qteMots = splitString(line, ' ' , Mot);
		uint8_t I2CdeviceAddress = strtol(Mot[0].c_str(),NULL,16);
		uint8_t registerID = strtol(Mot[1].c_str(),NULL,16);
		uint8_t data = strtol(Mot[2].c_str(),NULL,16);
		String rep = i2c_write(I2CdeviceAddress,registerID, data)+"\n";
		return rep;
	}


	/**
	 * @brief écrit au slave à I2CdeviceAddress au registre address l'octet de data fourni
	 * 
	 * @param I2CdeviceAddress 
	 * @param registerID 
	 * @param data 
	 * @return String 
	 */
	String i2c_write(uint8_t I2CdeviceAddress, uint8_t registerID, uint8_t data)
	{
		Wire.beginTransmission(I2CdeviceAddress);
		Wire.write(registerID);
		Wire.write(data);
		Wire.endTransmission();
			String rep = ("Écrit en esclave I2C 0x"+ String(I2CdeviceAddress,16) +" adresse 0x" + String(registerID,16) + " valeur 0x" + String(data,16)+"\n");
		return rep;	
	}

	/**
	 * @brief Écris un tableau d'octet dans l'esclave I2C
	 * 
	 * @param I2CdeviceAddress 
	 * @param registerID 
	 * @param p pointeur du tableau
	 * @param qty quatité à écrire
	 * @return String feedback
	 */
	String i2c_writeArray(uint8_t I2CdeviceAddress,uint8_t registerID,uint8_t *p, uint8_t qty)
	{
		int i;
		Wire.beginTransmission(I2CdeviceAddress);
		Wire.write((uint8_t) registerID);
		for(i=0; i<qty;i++){
			Wire.write(*p++);
		}
		Wire.endTransmission();
		String rep = ("Écrit "+String(qty,10) +" bytes, adresse 0x" + String(registerID,16) + " esclave 0x" + String(I2CdeviceAddress,16) +"\n");
		return rep;		
	}

	/**
	 * @brief Écris un tableau d"octets dans un esclave I2C
	 * 
	 * @param line  (i2c writebuf i2cADresse ofset dd dd dd ...)	les octets sont séparés par des espaces
	 * @return String 
	 */
	String i2c_writeBuf(String line)
	{	Vector<String> Mot;
		int i;
		int qteMots = splitString(line, ' ' , Mot);
		uint8_t I2CdeviceAddress = strtol(Mot[2].c_str(),NULL,16);
		uint8_t registerID = strtol(Mot[3].c_str(),NULL,16);
		int qteOctets = qteMots - 4;
		uint8_t buf[qteOctets];

		for(i=0; i< qteOctets;i++){
			buf[i] = strtol(Mot[4+i].c_str(),NULL,16);
		}
		return i2c_writeArray(I2CdeviceAddress,registerID,buf,qteOctets);
	}


	/**
	 * @brief Interpréteur du module de gestion I2C
	 * 
	 * @param line ligne de commande
	 * @return String feedback
	 */
	String i2c_interpret(String line){
		// Permet de séparer les mots d'une line de caractères
		Vector<String> Mot;
		int qteMots = splitString(line, ' ' , Mot);

		if(qteMots == 1)return i2c_menu();
		
		if(qteMots == 2){
			if(Mot[1]== "scan")	return i2c_scanBus();		
			if(Mot[1]== "menu")	return i2c_menu();		
		}
		
		// lire une qty d'octets à partir de l'adresse registerID du slave I2CdeviceAddress
		if(qteMots == 5){
			String param = Mot[2]+" "+Mot[3]+" "+Mot[4];
			if(Mot[1]== "read") return i2c_readString(param);
			if(Mot[1]== "write") return i2c_writeString(param);
		}

		if(qteMots > 5){
			if(Mot[1]=="writebuf") return i2c_writeBuf(line);
		}

		return line + ("???\n");

	}
#endif
