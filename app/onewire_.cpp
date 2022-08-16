/**
 * @file onewire_.cpp
 * @author Jean Mercier profjmer@gmail.com
 * @brief gestion du module onewire_
 * @version 1.0
 */
#include "../include/application.h"
#ifdef config_onewire
	// OneWire
	#define slaves 4 // nombre d'esclaves sur le bus

	#define COPYSCRATCH     0x48  // Copy EEPROM
	#define REAowCRATCH     0xBE  // Read EEPROM
	#define WRITESCRATCH    0x4E  // Write EEPROM
	#define RECALLSCRATCH   0xB8  // Reload last
	#define READPOWERSUPPLY 0xB4  // parasite power
	#define ALARMSEARCH     0xEC  // Query for alarm
	#define STARTCONVO      0x44  // temperature reading
	#define READROM         0x33  // ROM reading

	//int OneWirePin;
	OneWire ow(pin_1WIRE); // pullup 4.7k sur le bus, à la broche OmeWirePin

	/**
	 * @brief initialisation du module onewire_
	 * 
	 */
	void onewire_ini(){
		ow.begin(); //
		//Serial.println("init One Wire Pin "+String(OneWirePin));
	}

	/**
	 * @brief menu des commandes du module onewire_
	 * 
	 * @return String 
	 */
	String onewire_menu(){
		return "\now\now ?\now scan\n";
	}

	/**
	 * @brief identification de la présence de dispositifs sur le bus onewire
	 * 
	 * @return String 
	 */
	String onewire_presence(){
		if(ow.reset())
			return "Présence de dispositif(s)sur le bus OneWire\n";
		else return "Absence de dispositif sur le bus OneWire\n";  
	}

	/**
	 * @brief recherche et identification des dispositifs sur le bus onewire
	 * 
	 * @return String 
	 */
	String onewire_scan()
	{
			uint8_t counter=1;
			uint8_t a;
			uint8_t addr[8];
			String rep;

		while (counter)
		{
			if (!ow.search(addr))		{
				counter =0;
				rep+="Scan done\n";
				break;
			}
			else
			if (OneWire::crc8(addr, 7) == addr[7])
				{
					rep+=("Chip ") +String(counter++);
					switch (addr[0]) {
					case 0x10:
						rep+=(" DS18S20\n");  // or old DS1820
						break;
					case 0x28:
						rep+=(" DS18B20\n");
						break;
					case 0x22:
						rep+=(" DS1822\n");
						break;
					default:
						rep+=String(addr[0])+"\n"; // valeur trouvée mais pas dans le switch case
						break;
					}
				}
				else rep+=(" CRC invalide\n");
		}
		//Serial.println(rep);
		return rep;
	}

	/**
	 * @brief interpréteur de commandes du module onewire_
	 * 
	 * @param line 
	 * @return String 
	 */
	String onewire_interpret(String line){
		// Permet de séparer les mots d'une line de caractères
		Vector<String> Mot;
		int qteMots = splitString(line, ' ' , Mot);
		
		if(qteMots == 1) return onewire_menu();
		if(qteMots == 2){
			if(Mot[1]== "menu")	return onewire_menu();		
			else if(Mot[1]== "scan")	return onewire_scan();		
			else if(Mot[1]== "?")	return onewire_presence();		
		}

			return line + (" ???\n");		
	}
#endif