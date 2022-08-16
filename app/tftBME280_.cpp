/**
 * @file tftBME280_.cpp
 * @author Jean Mercier profjmer@gmail.com
 * @brief module d'affichage du capteur BME280 pour afficheur TFT ILI9341
 * nécessite les modules tft_ bme280_ i2c_
 * @version 1.0 
 */
#include "../include/application.h"

#ifdef config_tft
	#define couleurPression ILI9341_YELLOW
	#define couleurTemperature ILI9341_RED
	#define couleurHumidity 0x001F

	#define points 256
	static uint16_t nPoints=0;
	static uint8_t trace1[points];
	static uint8_t trace2[points];
	static uint8_t trace3[points];

	static String Temperature;
	static String Humidity;
	static String Pression;
	static String nom ="BME280";

	extern Adafruit_ILI9341 tft;

	// prototype
	int gain_decal(double y, double gain, int decal);

	/**
	 * @brief Mise à jour de la page BME280 de l'afficheur TFT ILI9341
	 * affiche la valeur numérique et ajoute un point dans le graphe
	 * décale le graphe vers la gauche si le graphe est complet et ajoute le point à droite
	 */
	void tftBME280_update(){
		/********************************** mise à jour des valeurs numériques selon interval_tft ****************/
		// récupération des valeurs du capteur
		double v1 = bme280_temperature_last();
		double v2 = bme280_humidity_last();
		double v3 = bme280_pression_last();

		spi_settingsFor(ILI9341);
		tft.setTextSize(2);        

		// nom de la page 
		tft.setCursor(0,0);
		tft.setTextColor(ILI9341_BLACK);
		tft.println(nom);
		tft.setCursor(0,0);
		tft.setTextColor(ILI9341_DARKGREY);
		tft.println(nom);		

		
		// efface anciennes valeurs numériques et affiche nouvelles valeurs
		tft.setCursor(120,0);
		tft.setTextColor(ILI9341_BLACK);
		tft.println(Temperature);
		Temperature = "T "+String(v1,1)+"C";
		tft.setCursor(120,0);
		tft.setTextColor(couleurTemperature);
		tft.printf(Temperature.c_str()); 

		tft.setCursor(120,30);
		tft.setTextColor(ILI9341_BLACK);
		tft.println(Humidity);
		Humidity = "HR "+String(v2,0)+ "%%";
		tft.setCursor(120,30);
		tft.setTextColor(couleurHumidity);
		tft.printf(Humidity.c_str());   


		tft.setCursor(0,30);
		tft.setTextColor(ILI9341_BLACK);
		tft.println(Pression);
		Pression = "P "+String(v3,1);
		tft.setCursor(0,30);
		tft.setTextColor(couleurPression);
		tft.printf(Pression.c_str());

		
		/*********************************** mise à jour du graphe selon interval_grapheBME280 **************************/

		int y1,y2,y3;
		uint16_t j=0;
		
		//Selon l'étendu et le décalage des valeurs d'entrées
		// les points sont calculés pour occupés une étendue de -200 et un décalage de 200
		
		// étendue température 50*C décalage 0*C (zone de 0-50*C)
		// gain temp -200/50, décalage 200
		y1 = gain_decal(v1, -4, 200); 
		
		// étendue sonde RH 0-100%, décalage 0%
		// gain sonde RH -200/100, 200
		y2 = gain_decal(v2, -2, 200);
		
		
		// étendue sonde pression 95 - 105 kpa
		// gain sonde pression -200/10, 105*20 de décalage
		y3 = gain_decal(v3, -20, 2100);
		
		// au reset les valeurs sont entrées directement
		if(nPoints<points)
		{
			trace1[nPoints]=y1;
			trace2[nPoints]=y2;   
			trace3[nPoints]=y3;  
		
			tft.drawPixel(nPoints,y1,couleurTemperature);
			tft.drawPixel(nPoints,y2,couleurHumidity);
			tft.drawPixel(nPoints,y3,couleurPression);
		
			nPoints++;
		}
		else{        
			
				tft.drawPixel(255,trace1[255],ILI9341_BLACK);
				tft.drawPixel(255,trace2[255],ILI9341_BLACK);
				tft.drawPixel(255,trace3[255],ILI9341_BLACK);
				
			// décalle les valeurs pour en ajouter une autre
			for(j=0;j<points-1;j++)
			{
				//efface les traces
				tft.drawPixel(j,trace1[j],ILI9341_BLACK);
				tft.drawPixel(j,trace2[j],ILI9341_BLACK);
				tft.drawPixel(j,trace3[j],ILI9341_BLACK);
			
				// décalle les données
				trace1[j]=trace1[j+1];
				trace2[j]=trace2[j+1];
				trace3[j]=trace3[j+1];
						
				// dessine les données décalées
				tft.drawPixel(j,trace1[j],couleurTemperature);
				tft.drawPixel(j,trace2[j],couleurHumidity);
				tft.drawPixel(j,trace3[j],couleurPression);
		
			}
				
			// ajoute les nouvelles valeurs en fin de graphe
			trace1[points-1]=y1;
			trace2[points-1]=y2;
			trace3[points-1]=y3;
		
			tft.drawPixel(points-1,y1,couleurTemperature);
			tft.drawPixel(points-1,y2,couleurHumidity);
			tft.drawPixel(points-1,y3,couleurPression);	
		}
	}

	/**
	 * @brief affiche la page BME280 dans l'afficheur TFT ILI9341
	 * 
	 */
	void tftBME280_page()
	{
		uint8_t rotation;
		tft_draw_axeX();
		
		// Axes Y Légendes 
		// portrait
		if(tft_getOrientation()==0)tft.setRotation(2);
		else tft.setRotation(0);

		tft.setCursor(35,265);
		tft.setTextColor(couleurTemperature);
		tft.printf("0       12       25      37     50");
		
		tft.setCursor(35,275);
		tft.setTextColor(couleurHumidity);
		tft.printf("0       25       50      75    100");

		tft.setCursor(35,285);
		tft.setTextColor(couleurPression);
		tft.printf("95     97.5     100    102.5   105");

		// paysage
		if(tft_getOrientation()==0 )rotation=3;
		else rotation=1;
		tft.setRotation(rotation);
		
		// affiche nom de la page 
		tft_String(0, 0, 2, ILI9341_DARKGREY, rotation , nom);
		tft_horloge();
		tft_showTimeUnits(bme280_getTimeUnits());
	}
#endif