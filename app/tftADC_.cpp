/**
 * @file tftADC_.cpp
 * @author Jean Mercier profjmer@gmail.com
 * @brief Module d'affichage des valeurs du module adc_ pour l'afficheur TFT
 * nécessite les modules tft_ adc_ spi_
 * @version 1.0
 */
#include "../include/application.h"

#ifdef config_tft
	#define points 256
	static uint16_t nPoints=0;
	uint8_t traceADC[points];
	String Alim;

	#define volts "VOLTS"

	extern Adafruit_ILI9341 tft;

	// prototype
	int gain_decal(double y, double gain, int decal);

	/**
	 * @brief Mise à jour de la page ADC de l'afficheur TFT ILI9341
	 * affiche la valeur numérique et ajoute un point dans le graphe
	 * décale le graphe vers la gauche si le graphe est complet et ajoute le point à droite
	 * 
	 */
	void  tftADC_update(){
		spi_settingsFor(ILI9341);
		tft.setTextSize(2);    

			// nom de la page 
		tft.setCursor(0,0);
		tft.setTextColor(ILI9341_BLACK);
		tft.println(volts);
		tft.setCursor(0,0);
		tft.setTextColor(ILI9341_DARKGREY);
		tft.println(volts);    
		
		// efface ancienne valeur numérique et affiche nouvelle valeur
		tft.setCursor(80,0);
		tft.setTextColor(ILI9341_BLACK);
		tft.println(Alim);	

		// nouvelle valeur
		double v1 = adc_vBat();
		Alim = String(v1,1);
		tft.setCursor(80,0);
		tft.setTextColor(ILI9341_RED);
		tft.printf(Alim.c_str());  	 
		
		// ajoute un point dans le graphe
		int y1;
		uint16_t j=0;
		
		//Selon l'étendu et le décalage des valeurs d'entrées
		// les points sont calculés pour occupés une étendue de -200 et un décalage de 200
		
		// étendue sonde volts  0- 16 volts
		// gain sonde volt -200/16, 200
		y1 = gain_decal(v1, -12.5, 200);
		
		// au reset les valeurs sont entrées directement
		if(nPoints<points)
		{
			traceADC[nPoints]=y1;  		
			tft.drawPixel(nPoints,y1,ILI9341_RED);		
			nPoints++;
		}
		else{        
				tft.drawPixel(255,traceADC[255],ILI9341_BLACK);			
			// décalle les valeurs pour en ajouter une autre
			for(j=0;j<points-1;j++)
			{
				//efface les traces

				tft.drawPixel(j,traceADC[j],ILI9341_BLACK);			
				// décalle les données
				traceADC[j]=traceADC[j+1];						
				// dessine les données décalées
				tft.drawPixel(j,traceADC[j],ILI9341_RED);			
			}
				
			// ajoute les nouvelles valeurs en fin de graphe
			traceADC[points-1]=y1;		
			tft.drawPixel(points-1,y1,ILI9341_RED);		
		}
	}

	/**
	 * @brief retourne la position Y du point selon le gain et le décalage appliqués à la valeur Y fournie
	 * 
	 * @param y 
	 * @param gain 
	 * @param decal 
	 * @return int 
	 */
	int gain_decal(double y, double gain, int decal)
	{
			return(y*gain + decal);
	}


	/**
	 * @brief affiche la page ADC dans l'afficheur TFT ILI9341
	 * 
	 */
	void tftADC_page()
	{
		tft_draw_axeX();
		uint8_t rotation;
		// Axes Y Légendes 
		// portrait
		if(tft_getOrientation()==0)tft.setRotation(2);
		else tft.setRotation(0);

		tft.setCursor(35,265);
		tft.setTextColor(ILI9341_RED);
		tft.printf("0       3       6      9     12");
		
		// paysage
		if(tft_getOrientation()==0 )rotation=3;
		else rotation=1;
		tft.setRotation(rotation);
		
		tft_String(0, 0, 2, ILI9341_DARKGREY, rotation , "VOLTS");
		tft_horloge();
		tft_showTimeUnits(adc_getTimeUnits());

	}
#endif