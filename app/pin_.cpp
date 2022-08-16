/**
 * @file pins_.cpp
 * @author Jean Mercier profjmer@gmail.com
 * @brief Affiche les définitions des broches du ESP8266 pour ce projet
 * @version 1.0
 */

/**
 * GPIO vs Dx
 * GPIO0-D3	
 * GPIO1-TX		
 * GRPIO2-D4	
 * GPIO3-RX		
 * GPIO4-D2		
 * GPIO5-D1		
 
 * GPIO12-D6	
 * GPIO13-D7	
 * GPIO14-D5	
 * GPIO15-D8	
 * GPIO16-D0
 * */

#include "../include/application.h"

/**
 * @brief Retourne la notation WEMOS pour une broche GPIO
 * 
 * @param gpio numéro de la broche
 * @return String nom de la broche wemos
 */
String pin_wemos(int gpio){
	switch(gpio){
		case 0: return "D3";break;
		case 1: return "TX";break;
		case 2: return "D4";break;
		case 3: return "RX";break;	
		case 4: return "D2";break;
		case 5: return "D1";break;
		case 12: return "D6";break;
		case 13: return "D7";break;	
		case 14: return "D5";break;
		case 15: return "D8";break;
		case 16: return "D0";break;								
	}
	return "";
}

/**
 * @brief Retourne le numéro gpio d'une broche wemos
 * 
 * @param wemos nom de la broche wemos D0..D8
 * @return int numéro gpio de la broche
 */
int pin_gpio(String wemos){
	if(wemos == "D0")return 16;
	if(wemos == "D1")return 5;
	if(wemos == "D2")return 4;
	if(wemos == "D3")return 0;
	if(wemos == "D4")return 2;
	if(wemos == "D5")return 14;
	if(wemos == "D6")return 12;
	if(wemos == "D7")return 13;
	if(wemos == "D8")return 15;
	if(wemos == "TX")return 1;
	if(wemos == "RX")return 3;		
		
	return -1;

}

/**
 * @brief Retourne les identifications des broches WEMOS et leur numéro GPIOS associé
 * 
 */
String pin_WEMOS_GPIO()
{
	return "Équivalent WEMOS - GPIO\nD0 GPIO16\nD1 GPIO5\nD2 GPIO4\nD3 GPIO0\nD4 GPIO2\nD5 GPIO14\nD6 GPIO12\nD7 GPIO13\nD8 GPIO15\n";
}

/**
 * @brief Retourne le numéros des broches GPIO et leur identification WEMOS
 * 
 */
String pin_GPIO_WEMOS()
{
	return "Équivalent GPIO WEMOS\nGPIO0 D3\nGPIO1 TX\nGPIO2 D4\nGPIO3 RX\nGPIO4 D2\nGPIO5 D1\nGPIO12 D6\nGPIO13 D7\nGPIO14 D5\nGPIO15 D8\nGPIO16 D0\n";
}

/**
 * @brief Retourne les broches pour les modules utilisés
 * 
 * @return String 
 */
String pin_modules()
{ 
	String modules ;
	#ifdef config_dht22
		modules += "DHT22 " + pin_wemos(pin_DHT)+"\n";
	#endif

	#ifdef config_onewire  
		modules += "OneWire " + pin_wemos(pin_1WIRE)+"\n";
	#endif	

	#ifdef config_i2c
		modules += "I2C SCL "+pin_wemos(pin_I2C_SCL)+ "  SDA " +pin_wemos(pin_I2C_SDA)+"\n";
	#endif

	#ifdef config_bme280  
		modules += "BME280 adresse 0x"+String(pin_BME280_I2C_ADDRESS,16)+"\n";
	#endif

	#ifdef config_oled 
		modules += "OLED adresse 0x"+String(pin_OLED_I2C_ADDRESS,16)+"\n";
	#endif

	#ifdef config_rtc
		modules += "RTC adresse 0x"+String(pin_RTC_I2C_ADDRESS,16)+"\n";
	#endif

	#ifdef config_spi
		modules += "SPI MOSI "+pin_wemos(pin_SPI_MOSI)+ "  MISO " +pin_wemos(pin_SPI_MISO)+"\n";
	#endif

	#ifdef config_tft
		modules += "SPI TFT_CS "+pin_wemos(pin_SPI_TFT_CS)+"\n";
	#endif

	#ifdef config_max7219
		modules += " SPI MAX7219_CS "+pin_wemos(pin_SPI_MAX7219_CS)+"\n";
	#endif

	#ifdef config_xpt2406
		modules += " SPI TOUCH_CS "+pin_wemos(pin_SPI_TOUCH_CS)+"\n";
	#endif

	#ifdef config_sd
		modules += " SPI SD_CS "+pin_wemos(pin_SPI_SD_CS)+"\n";
	#endif

	return modules;
}



#ifdef config_gpio
	/**
	 * @brief Retourne la liste des broches GPIO utilisées par le projet
	 * 
	 * @return String 
	 */
	String pin_gpio()
	{
		String rep = "Broches GPIO : ";
		#ifdef pin_GPIO0 
			rep += "0 ";
		#endif
		#ifdef pin_GPIO1 
			rep += "1 ";
		#endif
		#ifdef pin_GPIO2 
			rep += "2 ";
		#endif
		#ifdef pin_GPIO3 
			rep += "3 ";
		#endif
		#ifdef pin_GPIO4 
			rep += "4 ";
		#endif
		#ifdef pin_GPIO5 
			rep += "5 ";
		#endif
		#ifdef pin_GPIO12 
			rep += "12 ";
		#endif
		#ifdef pin_GPIO13 
			rep += "13 ";
		#endif
		#ifdef pin_GPIO14 
			rep += "14 ";
		#endif
		#ifdef pin_GPIO15 
			rep += "15 ";
		#endif
		#ifdef pin_GPIO16 
			rep += "16 ";
		#endif					
		return rep +"\n";
	}
#endif

#ifdef config_pulse
	/**
	 * @brief Retourne la liste des broches utilisées par le module pulse_
	 * 
	 * @return String  selon pin_.h
	 */
	String pin_pulse(){
		String rep = "Broches Pulse : ";
		#ifdef pin_pulse_D0 
			rep += "D0 ";
		#endif
		#ifdef pin_pulse_D1 
			rep += "D1 ";
		#endif
		#ifdef pin_pulse_D2 
			rep += "D2 ";
		#endif
		#ifdef pin_pulse_D3 
			rep += "D3 ";
		#endif
		#ifdef pin_pulse_D4 
			rep += "D4 ";
		#endif
		#ifdef pin_pulse_D5 
			rep += "D5 ";
		#endif
		#ifdef pin_pulse_D6 
			rep += "D6 ";
		#endif
		#ifdef pin_pulse_D7 
			rep += "D7 ";
		#endif
		#ifdef pin_pulse_D8 
			rep += "D8 ";
		#endif	
		#ifdef pin_pulse_TX 
			rep += "TX ";
		#endif	
		#ifdef pin_pulse_RX 
			rep += "RX ";
		#endif								
		return rep +"\n";
	}
	#endif

	#ifdef config_power
	// L'identification des broches utilisées par le module pulse est faite dans pin_.h
		String pin_power(){
			String rep = "Broches Power : ";
			#ifdef pin_power_D0 
				rep += "D0 ";
			#endif
			#ifdef pin_power_D1 
				rep += "D1 ";
			#endif
			#ifdef pin_power_D2 
				rep += "D2 ";
			#endif
			#ifdef pin_power_D3 
				rep += "D3 ";
			#endif
			#ifdef pin_power_D4 
				rep += "D4 ";
			#endif
			#ifdef pin_power_D5 
				rep += "D5 ";
			#endif
			#ifdef pin_power_D6 
				rep += "D6 ";
			#endif
			#ifdef pin_power_D7 
				rep += "D7 ";
			#endif
			#ifdef pin_power_D8 
				rep += "D8 ";
			#endif	
			#ifdef pin_power_TX
				rep += "TX ";
			#endif
			#ifdef pin_power_RX
				rep += "RX ";
			#endif						
			return rep +"\n";
		}
#endif

/**
 * @brief à modifier pour tenir compte des config
 * 
 * @return String 
 */
String pin_pins()
{
	String pinout ="Broches ESP8266 utilisées :\n"+ pin_modules();
	#ifdef config_gpio
		pinout+= pin_gpio();
	#endif
	#ifdef config_pulse
		pinout+=pin_pulse();
	#endif
	#ifdef config_power
		pinout+=pin_power();
	#endif
	
	return pinout;
}

/**
 * @brief Retourne le menu du module pin_
 * 
 * @return String 
 */
String pin_menu(){return "pin\npin pins\npin wemos\npin gpio\n";}

void pin_ini(){
	
}

/* ajouter les nouvelles commandes pins capteur i2c spi onewire dht wemos:gpio gpio:wemos

/** @brief		Interpréteur de commandes du module pins_
 * @param[in]	ligne de commande
 * @returns		Retourne une String donnant le résultat de l'interprétation de la commande
 */
String pin_interpret(String line){
	// Permet de séparer les mots d'une line de caractères
	Vector<String> Mot;
	int qteMots = splitString(line, ' ' , Mot);

	// retourne le menu
	if(qteMots == 1)	return pin_menu();

	if(qteMots == 2){
		if(Mot[1]== "pins")return pin_pins();		
		if(Mot[1]== "wemos")return pin_WEMOS_GPIO();
		if(Mot[1]== "gpio")return pin_GPIO_WEMOS();		
	}	

	// commande non reconnue
	return line + "  ???\n";
}

