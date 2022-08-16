/**
 * @file tftDS1820_.cpp
 * @author Jean Mercier profjmer@gmail.com
 * @brief module d'affichage du capteur BME280 pour afficheur TFT ILI9341
 * nécessite les modules tft_ onewire_ ds1820_
 * @version 1.0
 */
#include "../include/application.h"

#ifdef config_tft
	#define points 256
	extern uint8_t dsCount;
	extern float DS1820[4];
	static uint16_t nPoints=0;
	static uint8_t trace1[points];
	static uint8_t trace2[points];
	static uint8_t trace3[points];
	static uint8_t trace4[points];

	static String nom ="DS1820";

	extern Adafruit_ILI9341 tft;

	String Temperature1,Temperature2,Temperature3,Temperature4;

	// prototype
	int gain_decal(double y, double gain, int decal);

	/**
	 * @brief Mise à jour de la page DS1820 de l'afficheur TFT ILI9341
	 * affiche les valeurs numériques et ajoute les points dans le graphe, possibilité de 4 capteurs
	 * décale le graphe vers la gauche si le graphe est complet et ajoute les points à droite
	 * 
	 */
	void  tftDS1820_update(){
		int qte = dsCount;
		double v1,v2,v3,v4;
		v1=DS1820[0];
		v2=DS1820[1];
		v3=DS1820[2];
		v4=DS1820[3];
		spi_settingsFor(ILI9341);
		tft.setTextSize(2);        

		tft.setCursor(0,0);
		tft.setTextColor(ILI9341_BLACK);
		tft.println(Temperature1);
		if(v1 != -100){
			Temperature1 = "T1 "+String(v1,1);
			tft.setCursor(0,0);
			tft.setTextColor(ILI9341_GREEN);
			tft.printf(Temperature1.c_str()); 
		}

		tft.setCursor(160,0);
		tft.setTextColor(ILI9341_BLACK);
		tft.println(Temperature2);
		Temperature2 = "T2 "+String(v2,1);
		if(v2 != -100){	
			tft.setCursor(160,0);
			tft.setTextColor(ILI9341_YELLOW);
			tft.printf(Temperature2.c_str());   
		}

		tft.setCursor(0,25);
		tft.setTextColor(ILI9341_BLACK);
		tft.println(Temperature3);
		Temperature3 = "T3 "+String(v3,1);
		if(v3 != -100){	
			tft.setCursor(0,25);
			tft.setTextColor(ILI9341_BLUE);
			tft.printf(Temperature3.c_str());
		}

		tft.setCursor(160,25);
		tft.setTextColor(ILI9341_BLACK);
		tft.println(Temperature4);	
		if(v4 != -100){	
			Temperature4 = "T4 "+String(v4,1);
			tft.setCursor(160,25);
			tft.setTextColor(ILI9341_RED);
			tft.printf(Temperature4.c_str());  	 
		}
		
	/*********************************** mise à jour du graphe selon interval_grapheBME280 **************************/

		int y1,y2,y3,y4;
		uint16_t j=0;
		
		//Selon l'étendu et le décalage des valeurs d'entrées
		// les points sont calculés pour occupés une étendue de -200 et un décalage de 200
		
		// étendue température 50*C décalage 0*C (zone de 0-50*C)
		// gain temp -200/50, décalage 200
		y1 = gain_decal(v1, -4, 200); 
		y2 = gain_decal(v2, -4, 200); 
		y3 = gain_decal(v3, -4, 200); 
		y4 = gain_decal(v4, -4, 200);    	   
		

		
		// au reset les valeurs sont entrées directement
		if(nPoints<points)
		{
			trace1[nPoints]=y1;
			trace2[nPoints]=y2;   
			trace3[nPoints]=y3;  
			trace4[nPoints]=y4;  		
		
			if(v1 != -100) tft.drawPixel(nPoints,y1,ILI9341_GREEN);
			if(v2 != -100)tft.drawPixel(nPoints,y2,ILI9341_YELLOW);
			if(v3 != -100)tft.drawPixel(nPoints,y3,ILI9341_BLUE);
			if(v4 != -100)tft.drawPixel(nPoints,y4,ILI9341_RED);		
		
			nPoints++;
		}
		else{        
			
				tft.drawPixel(255,trace1[255],ILI9341_BLACK);
				tft.drawPixel(255,trace2[255],ILI9341_BLACK);
				tft.drawPixel(255,trace3[255],ILI9341_BLACK);
				tft.drawPixel(255,trace4[255],ILI9341_BLACK);			
				
			// décalle les valeurs pour en ajouter une autre
			for(j=0;j<points-1;j++)
			{
				//efface les traces
				tft.drawPixel(j,trace1[j],ILI9341_BLACK);
				tft.drawPixel(j,trace2[j],ILI9341_BLACK);
				tft.drawPixel(j,trace3[j],ILI9341_BLACK);
				tft.drawPixel(j,trace4[j],ILI9341_BLACK);			
			
				// décalle les données
				trace1[j]=trace1[j+1];
				trace2[j]=trace2[j+1];
				trace3[j]=trace3[j+1];
				trace4[j]=trace4[j+1];			
						
				// dessine les données décalées
				if(v1 != -100) tft.drawPixel(j,trace1[j],ILI9341_GREEN);
				if(v2 != -100) tft.drawPixel(j,trace2[j],ILI9341_YELLOW);
				if(v3 != -100) tft.drawPixel(j,trace3[j],ILI9341_BLUE);
				if(v4 != -100)tft.drawPixel(j,trace4[j],ILI9341_RED);			
		
			}
				
			// ajoute les nouvelles valeurs en fin de graphe
			trace1[points-1]=y1;
			trace2[points-1]=y2;
			trace3[points-1]=y3;
			trace4[points-1]=y4;		
		
			if(v1 != -100)tft.drawPixel(points-1,y1,ILI9341_GREEN);
			if(v2 != -100)tft.drawPixel(points-1,y2,ILI9341_YELLOW);
			if(v3 != -100)tft.drawPixel(points-1,y3,ILI9341_BLUE);	
			if(v4 != -100)tft.drawPixel(points-1,y4,ILI9341_RED);		
		}
	}

	/**
	 * @brief affiche la page DS1820 à l'écran du TFT ILI9341
	 * 
	 */
	void tftDS1820_page(){
		uint8_t rotation;
		tft_draw_axeX();
		
		// Axes Y Légendes 
		// portrait
		if(tft_getOrientation() ==0)tft.setRotation(2);
		else tft.setRotation(0);

		tft.setCursor(35,265);
		tft.setTextColor(ILI9341_DARKGREY);
		tft.printf("0       12       25      37     50");

		tft.setCursor(90,300);
		tft.setTextSize(2); 
		tft.setTextColor(ILI9341_DARKGREY);
		tft.printf("DS1820");
		
		// paysage
		if(tft_getOrientation() ==0 )rotation=3;
		else rotation=1;
		tft.setRotation(rotation);
		
		//tft_String(0, 200, 2, ILI9341_DARKGREY, rotation , nom);
		tft_horloge();
		tft_showTimeUnits(ds1820_getTimeUnits());
	}
#endif