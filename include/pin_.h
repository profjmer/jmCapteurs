/**
 * @file pin_.h
 * @author Jean Mercier profjmer@gmail.com
 * @brief Module de définitions des broches du ESP8266 pour ce projet
 * @version 1.0
 */

#include "../include/config_.h"
#ifndef INCLUDE_pin_H_
#define INCLUDE_pin_H_

    /* Définitions des broches WEMOS en valeurs GPIO */
    #define D0 16
    #define D1 5
    #define D2 4
    #define D3 0    
    #define D4 2
    #define D5 14
    #define D6 12
    #define D7 13 
    #define D8 15  
    #define TX 1
    #define RX 3 

    /* prototypes pour info sur pins */
    String pin_wemos(int gpio);
    int pin_gpio(String wemos);

 
    /* Capteurs */
    #define pin_DHT D3		// DHT22
    #define pin_1WIRE D4	// DS1820 4 capteurs maximum

       /* Module GPIO */
    #ifdef config_gpio  
        /* Décommenter les broches à utiliser comme gpio */
        //#define pin_GPIO0 0
        //#define pin_GPIO1 1
        //define pin_GPIO2 2      
        //#define pin_GPIO3 3
        //#define pin_GPIO4 4
        //#define pin_GPIO5 5 
        //#define pin_GPIO12 12
        //#define pin_GPIO13 13
        //#define pin_GPIO14 14
        #define pin_GPIO15 15
        #define pin_GPIO16 16
        
        String pin_gpio();     // liste des broches utilisées par gpio_
    #endif


    /* I2C */
    #ifdef config_i2c
        #define pin_I2C_SCL D1	// OLED, BME280
        #define pin_I2C_SDA D2

        #ifdef config_bme280
            #define pin_BME280_I2C_ADDRESS 0x76
        #endif

        #ifdef config_rtc
            #define pin_RTC_I2C_ADDRESS 0x68
        #endif

        #ifdef config_eeprom
            #define pin_EEPROM_I2C_ADDRESS 0x57     // adresse eeprom du module RTC
        #endif

        #ifdef config_oled
            #define pin_OLED_I2C_ADDRESS 0x3c	// adresse I2C de l'afficheur
        #endif  

        String pin_i2c();   // retourne les broches I2C
    #endif

    /* SPI */
    // Broches communes selon le schéma jmObjean2021

    // La sélection du composant 
    // T_CS	GPIO3 	touch ou max7219 selon spi_.h
    // SD_CS GPIO1 	carte SD
    // CS	GPIO15 		écran tft

    #ifdef config_spi 
        /* Définition des broches */
        #define pin_SPI_MOSI D7
        #define pin_SPI_MISO D6
        #define pin_SPI_CLK D5

        /* Définitions des composants spi */
        #ifdef config_tft       
            #define pin_SPI_TFT_CS D8
            #define pin_SPI_TFT_DC D4   
        #endif  

        #ifdef config_touch     //ILI9341 ou MAX7219, pas les deux !
            #define pin_SPI_TOUCH_CS GPIO3
        #endif 

        #ifdef config_max7219   //ILI9341 ou MAX7219, pas les deux !
            #define pin_SPI_MAX7219_CS GPIO3
        #endif 

        #ifdef config_sd
            #define pin_SPI_SD_CS GPIO3		//gpio1		uart-rx aussi selon SW9
        #endif 

    #endif  
        

    /* Module pulse : Définir le nombre de broches pour le module pulse_ */
    #ifdef config_pulse
        /* Commenter les broches pulse non utilisées */
        #define pin_pulse_D0 16
        //#define pin_pulse_D1 5
        // #define pin_pulse_D2 4
        //#define pin_pulse_D3 0    
        //#define pin_pulse_D4 2
        #define pin_pulse_D5 14
        #define pin_pulse_D6 12
        #define pin_pulse_D7 13 
        //#define pin_pulse_D8 15  
                 
        String pin_pulse();     // liste des broches utilisées par pulse_
    #endif

    /*  Module  power : Définir le nombre et les broches pour le module power_ 
            Il faut décommenter la même broche dans la section pulse
    */
    #ifdef config_power
         #define pin_power_D0 16
        // #define pin_power_D1 5
        //#define pin_power_D2 4
        // #define pin_power_D3 0    
         //#define pin_power_D4 2
        // #define pin_power_D5 14
        // #define pin_power_D6 12
        //#define pin_power_D7 13 
        //#define pin_power_D8 15  
      
        String pin_power();     // liste des broches utilisées par power_
    #endif
 

    /* Prototypes */
    String pin_menu();
    String pin_pins();      // liste des broches utilisées
    String pin_interpret(String line);

    void pin_ini();
#endif