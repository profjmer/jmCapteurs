/**
 * @file tftDHT22_.cpp
 * @author Jean Mercier profjmer@gmail.com
 * @brief module d'affichage du capteur DHT22 pour afficheur TFT ILI9341
 * nécessite les modules tft_ dht_ 
 * @version 1.0
 */
#include "../include/application.h"

#ifdef config_tft
	#define points 256
	static uint16_t nPoints=0;
	static uint8_t trace1[points];
	static uint8_t trace2[points];


	static String Temperature;
	static String Humidity;

	static String nom ="DHT22";

	extern Adafruit_ILI9341 tft;

	// prototype
	int gain_decal(double y, double gain, int decal);

	/**
	 * @brief Mise à jour de la page DHT22 de l'afficheur TFT ILI9341
	 * affiche la valeur numérique et ajoute un point dans le graphe
	 * décale le graphe vers la gauche si le graphe est complet et ajoute le point à droite
	 * 
	 */
	void tftDHT22_update(){
		/********************************** mise à jour des valeurs numériques selon interval_tft ****************/
		// récupération des valeurs du capteur
		double v1 = dht22_temperature_last();
		double v2 = dht22_humidity_last();


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
		tft.setTextColor(ILI9341_GREEN);
		tft.printf(Temperature.c_str()); 

		tft.setCursor(120,30);
		tft.setTextColor(ILI9341_BLACK);
		tft.println(Humidity);
		Humidity = "HR "+String(v2,0)+ "%%";
		tft.setCursor(120,30);
		tft.setTextColor(ILI9341_YELLOW);
		tft.printf(Humidity.c_str());   

		
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
		
		
		// au reset les valeurs sont entrées directement
		if(nPoints<points)
		{
			trace1[nPoints]=y1;
			trace2[nPoints]=y2;   
		
			tft.drawPixel(nPoints,y1,ILI9341_GREEN);
			tft.drawPixel(nPoints,y2,ILI9341_YELLOW);
		
			nPoints++;
		}
		else{        
			
				tft.drawPixel(255,trace1[255],ILI9341_BLACK);
				tft.drawPixel(255,trace2[255],ILI9341_BLACK);
				
			// décalle les valeurs pour en ajouter une autre
			for(j=0;j<points-1;j++)
			{
				//efface les traces
				tft.drawPixel(j,trace1[j],ILI9341_BLACK);
				tft.drawPixel(j,trace2[j],ILI9341_BLACK);
			
				// décalle les données
				trace1[j]=trace1[j+1];
				trace2[j]=trace2[j+1];
						
				// dessine les données décalées
				tft.drawPixel(j,trace1[j],ILI9341_GREEN);
				tft.drawPixel(j,trace2[j],ILI9341_YELLOW);
		
			}
				
			// ajoute les nouvelles valeurs en fin de graphe
			trace1[points-1]=y1;
			trace2[points-1]=y2;
		
			tft.drawPixel(points-1,y1,ILI9341_GREEN);
			tft.drawPixel(points-1,y2,ILI9341_YELLOW);
		}
	}

	/**
	 * @brief affiche la page DHT22 dans l'afficheur TFT ILI9341
	 * 
	 */
	void tftDHT22_page()
	{
		uint8_t rotation;
		tft_draw_axeX();
		
		// Axes Y Légendes 
		// portrait
		if(tft_getOrientation()==0)tft.setRotation(2);
		else tft.setRotation(0);

		tft.setCursor(35,265);
		tft.setTextColor(ILI9341_GREEN);
		tft.printf("0       12       25      37     50");
		
		tft.setCursor(35,275);
		tft.setTextColor(ILI9341_YELLOW);
		tft.printf("0       25       50      75    100");


		// paysage
		if(tft_getOrientation()==0 )rotation=3;
		else rotation=1;
		tft.setRotation(rotation);
		
		// affiche nom de la page 
		tft_String(0, 0, 2, ILI9341_DARKGREY, rotation , nom);
		tft_horloge();
		tft_showTimeUnits(dht22_getTimeUnits());

	}
#endif