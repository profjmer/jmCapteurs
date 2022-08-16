/**
 * @file pulse_.cpp
 * @author Jean Mercier profjmer@gmail.com
 * @brief module de gestion des impulsions pour les broches gpio
 * @version 1.0
 */

#include "../include/application.h"

#ifdef config_pulse
	#include "../include/jmPulseESP.h"

	uint8_t pu_id, pu_pin, pu_state ;
	uint32_t pu_cycles, pu_tLow ,pu_tHigh, pu_time;
	String pulse_pinList[8]; // maximum de 8 générateurs
	int pulseQte;

	String msg;

	Timer pulse_timer; // base de temps pour les générateurs d'impulsion

	/*	Instantiation des générateurs d'impulsions selon aux broches définies dans pin_.h
	*/
	#ifdef pin_pulse_D0 
		jmPulse puPULSE0(pin_pulse_D0); 
	#endif	
	#ifdef pin_pulse_D1
		jmPulse puPULSE1(pin_pulse_D1);
	#endif	
	#ifdef pin_pulse_D2	
		jmPulse puPULSE2(pin_pulse_D2); 
	#endif	
	#ifdef pin_pulse_D3
		jmPulse puPULSE3(pin_pulse_D3);
	#endif	
	#ifdef pin_pulse_D4
		jmPulse puPULSE4(pin_pulse_D4);
	#endif	
	#ifdef pin_pulse_D5	
		jmPulse puPULSE5(pin_pulse_D5); 
	#endif	
	#ifdef pin_pulse_D6
		jmPulse puPULSE6(pin_pulse_D6);
	#endif	
	#ifdef pin_pulse_D7
		jmPulse puPULSE7(pin_pulse_D7);	
	#endif	
	#ifdef pin_pulse_D8
		jmPulse puPULSE8(pin_pulse_D8);	
	#endif	

	/*	Prototypes des générateurs d'impulsions selon aux broches définies dans pin_.h */
	#ifdef pin_pulse_D0 
		void puPULSE0_sm();
	#endif	
	#ifdef pin_pulse_D1
		void puPULSE1_sm();
	#endif	
	#ifdef pin_pulse_D2	
		void puPULSE2_sm();
	#endif	
	#ifdef pin_pulse_D3
		void puPULSE3_sm();
	#endif	
	#ifdef pin_pulse_D4
		void puPULSE4_sm();
	#endif	
	#ifdef pin_pulse_D5	
		void puPULSE5_sm();
	#endif	
	#ifdef pin_pulse_D6
		void puPULSE6_sm();
	#endif	
	#ifdef pin_pulse_D7
		void puPULSE7_sm();	
	#endif	
	#ifdef pin_pulse_D8
		void puPULSE8_sm();	
	#endif	

	/*	Machine d'état des générateurs d'impulsions selon aux broches définies dans pin_.h */
	void systick()
	{
		#ifdef pin_pulse_D0 
			puPULSE0.sm();
		#endif	
		#ifdef pin_pulse_D1
			puPULSE1.sm();
		#endif	
		#ifdef pin_pulse_D2	
			puPULSE2.sm();
		#endif	
		#ifdef pin_pulse_D3
			puPULSE3.sm();
		#endif	
		#ifdef pin_pulse_D4
			puPULSE4.sm();
		#endif	
		#ifdef pin_pulse_D5	
			puPULSE5.sm();
		#endif	
		#ifdef pin_pulse_D6
			puPULSE6.sm();
		#endif	
		#ifdef pin_pulse_D7
			puPULSE7.sm();	
		#endif	
		#ifdef pin_pulse_D8
			puPULSE8.sm();	
		#endif	

	}

		/**
	 * @brief identification des broches utilsant les générateurs d'impulsions
	 * 
	 * @return String 
	 */
	String pulse_pins(){
		//return pin_pulse();
		int i,pin,l;
		String rep="[";
		for(i=0;i<pulseQte;i++){
			rep += pulse_pinList[i]+" ";
		}
		l = rep.length()-1;
		rep = rep.substring(0,l);
		rep +="]";
		return rep;
	}

	/**
	 * @brief retourne le menu des commandes du module pulse_
	 * 
	 * @return String 
	 */
	String pulse_menu(){return "\npulse\npu\npu pins\npu "+pulse_pins()+" [0 1 2]\npu l "+pulse_pins()+"  Low(ms)\npu h "+pulse_pins()+" High(ms)\npu lh "+pulse_pins()+" Low(ms) High(ms) cycles\npu hl "+pulse_pins()+" Low(ms) High(ms) cycles\n";}


	/**
	 * @brief arrêt de la minuterie des impulsions
	 * 
	 */
	void pulse_timerStop(){
		pulse_timer.stop();
	}

	/**
	 * @brief redémarrage de la minuterie des impulsions
	 * 
	 */
	void pulse_timerStart(){
		pulse_timer.restart();
	}

	/**
	 * @brief initialisation du module pulse_
	 * 
	 */
	void pulse_ini(){	
		//Serial.println("pulse_ini()");
		/* création des outputs pour les broches et ajout des broches utilisées dans la liste */
		#ifdef pin_pulse_D0 
			pinMode(16,OUTPUT);
			pulse_pinList[pulseQte++]="D0 ";
		#endif	
		#ifdef pin_pulse_D1
			pinMode(5,OUTPUT);
			pulse_pinList[pulseQte++]="D1 ";			
		#endif	
		#ifdef pin_pulse_D2	
			pinMode(4,OUTPUT);
			pulse_pinList[pulseQte++]="D2 ";			
		#endif	
		#ifdef pin_pulse_D3
			pinMode(0,OUTPUT);
			pulse_pinList[pulseQte++]="D3 ";			
		#endif	
		#ifdef pin_pulse_D4
			pinMode(2,OUTPUT);
			pulse_pinList[pulseQte++]="D4 ";			
		#endif	
		#ifdef pin_pulse_D5	
			pinMode(14,OUTPUT);
			pulse_pinList[pulseQte++]="D5 ";			
		#endif	
		#ifdef pin_pulse_D6
			pinMode(12,OUTPUT);
			pulse_pinList[pulseQte++]="D6 ";			
		#endif	
		#ifdef pin_pulse_D7
			pinMode(13,OUTPUT);
			pulse_pinList[pulseQte++]="D7 ";			
		#endif	
		#ifdef pin_pulse_D8
			pinMode(15,OUTPUT);
			pulse_pinList[pulseQte++]="D8 ";			
		#endif	
	
		// activation horloge pour les générateurs
		pulse_timer.initializeUs(100, systick).start();
	}

	/** @brief		Interpréteur de commandes du module pulse_
	 * @param[in]	ligne de commande
	 * @returns		Retourne une String donnant le résultat de l'interprétation de la commande
	 */
	String pulse_interpret(String line){

		// Permet de séparer les mots d'une line de caractères
		Vector<String> Mot;
		int qteMots = splitString(line, ' ' , Mot);

		// retourne le menu
		if(qteMots == 1)	return pulse_menu();

		// retourne le menu des commandes du module gestionIO
		if(qteMots == 2){
			if(Mot[1]== "menu") return pulse_menu();
		}

		if(qteMots == 2){
			if(Mot[1]== "pins") return pulse_pins();
		}

		// section générateur d'impulsions
		// set pin state[low high toggle]
		if(qteMots == 3){
			pu_pin=(uint8_t)pin_gpio(Mot[1]);
			if(pu_pin == -1)return"Erreur identification pin wemos\n";

			pu_state=(uint8_t)atoi(Mot[2].c_str());

			/*	Selon les broches définies comme générateur d'impulsions dans pin_.h	*/	
			switch(pu_pin)
			{
				#ifdef pin_pulse_D0
					case pin_pulse_D0:	puPULSE0.pinState(pu_state); break;
				#endif
				#ifdef pin_pulse_D1
					case pin_pulse_D1:	puPULSE1.pinState(pu_state); break;
				#endif
				#ifdef pin_pulse_D2
					case pin_pulse_D2:	puPULSE2.pinState(pu_state); break;
				#endif
				#ifdef pin_pulse_D3
					case pin_pulse_D3:	puPULSE3.pinState(pu_state); break;
				#endif
				#ifdef pin_pulse_D4
					case pin_pulse_D4:	puPULSE4.pinState(pu_state); break;
				#endif	
				#ifdef pin_pulse_D5
					case pin_pulse_D5:	puPULSE5.pinState(pu_state); break;
				#endif
				#ifdef pin_pulse_D6
					case pin_pulse_D6:	puPULSE6.pinState(pu_state); break;
				#endif
				#ifdef pin_pulse_D7
					case pin_pulse_D7:	puPULSE7.pinState(pu_state); break;
				#endif
				#ifdef pin_pulse_D8
					case pin_pulse_D8:	puPULSE8.pinState(pu_state); break;
				#endif																
			}
			return "set " +Mot[1] + " etat "+ Mot[2] +"\n";
		
		}

		if(qteMots == 4){
			// impulsion basse puLow pin temps
			if(Mot[1]== "l")
			{
				pu_pin=(uint8_t)pin_gpio(Mot[1]);
				if(pu_pin == -1)return"Erreur identification pin wemos\n";
				pu_time=(uint32_t)atol(Mot[3].c_str());

				/*	Selon les broches définies comme générateur d'impulsions dans pin_.h	*/				
				switch(pu_pin)
				{
					#ifdef pin_pulse_D0 
						case pin_pulse_D0:	puPULSE0.low(pu_time); break; 
					#endif
					#ifdef pin_pulse_D1 
						case pin_pulse_D1:	puPULSE1.low(pu_time); break; 
					#endif
					#ifdef pin_pulse_D2
						case pin_pulse_D2:	puPULSE2.low(pu_time); break; 
					#endif	
					#ifdef pin_pulse_D3
						case pin_pulse_D3:	puPULSE3.low(pu_time); break; 
					#endif
					#ifdef pin_pulse_D4 
						case pin_pulse_D4:	puPULSE4.low(pu_time); break; 
					#endif
					#ifdef pin_pulse_D5 
						case pin_pulse_D5:	puPULSE5.low(pu_time); break; 
					#endif
					#ifdef pin_pulse_D6 
						case pin_pulse_D6:	puPULSE6.low(pu_time); break; 
					#endif
					#ifdef pin_pulse_D7 
						case pin_pulse_D7:	puPULSE7.low(pu_time); break; 
					#endif
					#ifdef pin_pulse_D8 
						case pin_pulse_D8:	puPULSE8.low(pu_time); break; 
					#endif																			
				}
				//Serial.print("low :"); Serial.println(pu_time);
				return "impulsion basse\n";
			}
		
			// impulsion haute puHigh pin temps

			if(Mot[1]== "h")
			{
				pu_pin=(uint8_t)pin_gpio(Mot[1]);
				if(pu_pin == -1)return"Erreur identification pin wemos\n";
				pu_time=(uint32_t)atol(Mot[3].c_str());

				/*	Selon les broches définies comme générateur d'impulsions dans pin_.h	*/		
				switch(pu_pin)
				{
					#ifdef pin_pulse_D0
						case pin_pulse_D0: puPULSE0.high(pu_time); break;
					#endif
					#ifdef pin_pulse_D1
						case pin_pulse_D1: puPULSE1.high(pu_time); break;
					#endif
					#ifdef pin_pulse_D2
						case pin_pulse_D2: puPULSE2.high(pu_time); break;
					#endif
					#ifdef pin_pulse_D3				
						case pin_pulse_D3: puPULSE3.high(pu_time); break;
					#endif
					#ifdef pin_pulse_D4
						case pin_pulse_D4: puPULSE4.high(pu_time); break;
					#endif
					#ifdef pin_pulse_D5
						case pin_pulse_D5: puPULSE5.high(pu_time); break;
					#endif	
					#ifdef pin_pulse_D6				
						case pin_pulse_D6: puPULSE6.high(pu_time); break;
					#endif
					#ifdef pin_pulse_D7
						case pin_pulse_D7: puPULSE7.high(pu_time); break;
					#endif
					#ifdef pin_pulse_D8
						case pin_pulse_D8: puPULSE8.high(pu_time); break;
					#endif																			
				}
				return "impulsion haute\n";
			}
		}

		if(qteMots==6){
			if(Mot[1]== "lh")
			{
				pu_pin=(uint8_t)pin_gpio(Mot[2]);
				if(pu_pin == -1)return"Erreur identification pin wemos\n";
				pu_tLow=(uint32_t)atol(Mot[3].c_str());
				pu_tHigh=(uint32_t)atol(Mot[4].c_str());
				pu_cycles=(uint32_t)atol(Mot[5].c_str());

				/*	Selon les broches définies comme générateur d'impulsions dans pin_.h	*/	
				switch(pu_pin)
				{
					#ifdef pin_pulse_D0
						case pin_pulse_D0:	puPULSE0.lowHigh(pu_tLow, pu_tHigh, pu_cycles); break;
					#endif
					#ifdef pin_pulse_D1
						case pin_pulse_D1:	puPULSE1.lowHigh(pu_tLow, pu_tHigh, pu_cycles); break;
					#endif
					#ifdef pin_pulse_D2
						case pin_pulse_D2:	puPULSE2.lowHigh(pu_tLow, pu_tHigh, pu_cycles); break;
					#endif	
					#ifdef pin_pulse_D3
						case pin_pulse_D3:	puPULSE3.lowHigh(pu_tLow, pu_tHigh, pu_cycles); break;
					#endif
					#ifdef pin_pulse_D4
						case pin_pulse_D4:	puPULSE4.lowHigh(pu_tLow, pu_tHigh, pu_cycles); break;
					#endif
					#ifdef pin_pulse_D5
						case pin_pulse_D5:	puPULSE5.lowHigh(pu_tLow, pu_tHigh, pu_cycles); break;
					#endif
					#ifdef pin_pulse_D6
						case pin_pulse_D6:	puPULSE6.lowHigh(pu_tLow, pu_tHigh, pu_cycles); break;
					#endif
					#ifdef pin_pulse_D7
						case pin_pulse_D7:	puPULSE7.lowHigh(pu_tLow, pu_tHigh, pu_cycles); break;
					#endif
					#ifdef pin_pulse_D8
						case pin_pulse_D8:	puPULSE8.lowHigh(pu_tLow, pu_tHigh, pu_cycles); break;
					#endif																		
				}
				return "impulsion LH\n";
			}

			if(Mot[1]== "hl")
			{
				pu_pin=(uint8_t)pin_gpio(Mot[1]);
				if(pu_pin == -1)return"Erreur identification pin wemos\n";
				pu_tHigh=(uint32_t)atol(Mot[3].c_str());
				pu_tLow=(uint32_t)atol(Mot[4].c_str());
				pu_cycles=(uint32_t)atol(Mot[5].c_str());

				/*	Selon les broches définies comme générateur d'impulsions dans pin_.h	*/	
				switch(pu_pin)
				{
					#ifdef pin_pulse_D0
						case pin_pulse_D0:	puPULSE0.highLow(pu_tHigh,pu_tLow,pu_cycles); break;
					#endif
					#ifdef pin_pulse_D1
						case pin_pulse_D1:	puPULSE1.highLow(pu_tHigh,pu_tLow,pu_cycles); break;
					#endif
					#ifdef pin_pulse_D2
						case pin_pulse_D2:	puPULSE2.highLow(pu_tHigh,pu_tLow,pu_cycles); break;
					#endif
					#ifdef pin_pulse_D3
						case pin_pulse_D3:	puPULSE3.highLow(pu_tHigh,pu_tLow,pu_cycles); break;
					#endif
					#ifdef pin_pulse_D4
						case pin_pulse_D4:	puPULSE4.highLow(pu_tHigh,pu_tLow,pu_cycles); break;
					#endif
					#ifdef pin_pulse_D5
						case pin_pulse_D5:	puPULSE5.highLow(pu_tHigh,pu_tLow,pu_cycles); break;
					#endif
					#ifdef pin_pulse_D6
						case pin_pulse_D6:	puPULSE6.highLow(pu_tHigh,pu_tLow,pu_cycles); break;
					#endif
					#ifdef pin_pulse_D7
						case pin_pulse_D7:	puPULSE7.highLow(pu_tHigh,pu_tLow,pu_cycles); break;
					#endif
					#ifdef pin_pulse_D8
						case pin_pulse_D8:	puPULSE8.highLow(pu_tHigh,pu_tLow,pu_cycles); break;
					#endif											
				}
			return "impulsion HL\n";
			}
		}

		// commande non reconnue
		return line + "  ???\n";
	}
#endif