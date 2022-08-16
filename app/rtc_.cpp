/**
 * @file rtc_.cpp
 * @author Jean Mercier profjmer@gmail.com
 * @brief Module de gestion du temps et du mode faible consommation
 * @version 1.0
 */
#include "../include/application.h"

#ifdef config_rtc
	#include <Libraries/DS3232RTC/DS3232RTC.h>
	//#include "param_.h"

	#define modeJour 0
	#define modeJourSemaine 1
	#define modeJourMois 2

	String adrRTC = String(adresseRTC);	 

	uint8_t sleepHre = 3;
	uint8_t sleepMin = 3;
	String timezone = "-5";
	uint8_t runTime;

	Timer timerDeepSleep, timerRunTime;

	/**
	 * @brief Menu du module
	 * 
	 * @return String 
	 */
	String rtc_menu(){
		return "\nrtc\nrtc time\nrtc time JJ MM AA HH MM SS\nrtc control\nrtc control [octet]\nrtc status\nrtc alarm2\nrtc alarm2 [Hre Min repHre repMin]\
		\nrtc sleepHre [0..24]\nrtc sleepMin [0..59]\nrtc timezone [string]\n";
	}

	/*
	void onNtpReceive(NtpClient& client, time_t timestamp);

	NtpClient ntpClient ("ntp.cegepsherbrooke.qc.ca", 30, onNtpReceive);

	void onNtpReceive(NtpClient& client, time_t timestamp) {
		Serial.printf("Synchro avec serveur NTP\n");
		SystemClock.setTime(timestamp, eTZ_UTC); // selon fuseau horaire
		DSRTC.set(timestamp); 
	}

	// Requête NTP
	String rtc_reqNTP(){
		// Affiche les valeurs locales de la date et du temps
		SystemClock.setTimeZone(-5); // Montréal ?
		ntpClient.requestTime();
		return "Requete NTP\n";
	}
	*/

	/**
	 * @brief redémarre la minuterie pour la durée de l'éevil en mode faible consommation
	 * 
	 * @return String 
	 */
	String rtc_RunTimeRestart(){
		timerRunTime.setIntervalMs(runTime*1000); 
		timerRunTime.restart();
		return "Redémarrage minuterie ESP runTime\n";
	}

	/**
	 * @brief arrêt de la minuterie pour la durée de l'éveil en mode faible consommation
	 * 
	 * @return String 
	 */
	String rtc_RunTimeStop(){
		timerRunTime.stop();
		return "Arrêt minuterie runTime\n";
	}
	/*
	if(runTime >=30  && runTime !=0xffff && param_getFonctionnement()==faibleConsommation) {
			timerRunTime.setIntervalMs(runTime*1000); 
			timerRunTime.restart();
		}
			if(runTime >=30  && runTime !=0xff  && param_getFonctionnement()==faibleConsommation)
			timerRunTime.initializeMs(runTime * 1000, timer_espDeepSleep).start();

			if(Mot[1]== "eveil"){
			return "Eveil " +String(timer_getEveilRunTime_interval())+" sec\n";
			} 		

			if(Mot[1]=="eveil") {
			return timer_setEveilRunTime_interval(Mot[2]); 
			}  		

		if(qteMots == 4){
			if(Mot[1]=="sleep") {
				return rtc_setSleep_interval(Mot[2], Mot[3]); 
			}  
		}		
	*/
	/************************************************************* Activation du Deep Sleep ************************************************/

	/**
	 * @brief Delai avant de restarter le système
	 * 
	 */
	void rtc_delaiSystemDeepSleep (){
		system_deep_sleep(0);
	}


	/**
	 * @brief Active le mode Dormance "DeepSleep"
	 * met l'écran oled au noir
	 */
	void rtc_espDeepSleep(){   

		//Serial.println("\nEntre en mode faible consommation");
		oled_off();

		//system_deep_sleep_set_option(2);
		rtc_setNextWakeUpAlarm2(); // ajuste et active alarm2 RTC

		// retarder le system deep sleep pour retourner le message
		timerDeepSleep.initializeMs(1000, rtc_delaiSystemDeepSleep).startOnce();	
		return;   
	}

	/**
	 * @brief time stamp: le temps du RTC est plus juste alors on l'utilise
	 * 
	 * @return String représentant le temps sans \n 
	 */
	String rtc_ts(){
		// on lit directement les registres du RTC
		String temps = i2c_read(adresseRTC,0,7);

		// Permet de séparer les mots d'une line de caractères
		Vector<String> Mot;
		int qteMots = splitString(temps, ' ' , Mot);
		int i;
		// ajoute leading 0 pour nombre 0-9
		for(i=0;i<7;i++){
			if(i==3)continue;
			else if(Mot[i].length()==1)Mot[i]="0"+Mot[i];
		}
		
		return "20"+Mot[6]+"-"+Mot[5]+"-"+Mot[4]+" "+Mot[2]+":"+Mot[1]+":"+Mot[0]+rtc_getTimeZone();
	}

	// le temps du RTC est plus juste
	// alors on l'utilise
	String rtc_read(){
		// on lit directement les registres du RTC
		String temps = i2c_read(adresseRTC,0,7);

		// Permet de séparer les mots d'une line de caractères
		Vector<String> Mot;
		int qteMots = splitString(temps, ' ' , Mot);
		int i;
		// ajoute leading 0 pour nombre 0-9
		for(i=0;i<7;i++){
			if(i==3)continue;
			else if(Mot[i].length()==1)Mot[i]="0"+Mot[i];
		}
			
		return "20"+Mot[6]+"."+Mot[5]+"."+Mot[4]+"  "+Mot[2]+":"+Mot[1]+":"+Mot[0]+"\n";
	}

	/**
	 * @brief Retourne les octets de programmation des alarmes
	 * 
	 * @param id de l'alarme (1 ou 2)
	 * @return String 
	 */
	String rtc_alarmRead(uint8_t id){
		uint8_t qte=3;
		uint8_t ofset=0xb;
		// alarm1 vs alarm2
		if(id==1){ qte=4;ofset=7;}

		// on lit directement les registres du RTC
		String alarm = i2c_read(adresseRTC,ofset,qte);
		// Permet de séparer les mots d'une line de caractères
		Vector<String> Mot;
		int qteMots = splitString(alarm, ' ' , Mot);

		if(id==1)	return "Alarm 1 S:"+Mot[0] +" M:"+Mot[1]+ " H:"+Mot[2] +" D:"+Mot[3]+"\n";
		return "Alarm 2 M:"+Mot[0] +" H:"+Mot[1]+ " D:"+Mot[2] +"\n";
	}

	/**
	 * @brief Permet d'écrire dans les registres du RTC
	 * 
	 * @param line rtc write j m a h m s
	 * @return String 
	 */
	String rtc_write(String line){
		// Permet de séparer les mots d'une line de caractères
		Vector<String> Mot;
		int qteMots = splitString(line, ' ' , Mot);

		uint8_t jour = strtol(Mot[2].c_str(),NULL,16);		
		uint8_t mois = strtol(Mot[3].c_str(),NULL,16);			
		uint8_t an = strtol(Mot[4].c_str(),NULL,16);
		uint8_t H = strtol(Mot[5].c_str(),NULL,16);		
		uint8_t M = strtol(Mot[6].c_str(),NULL,16);		
		uint8_t S = strtol(Mot[7].c_str(),NULL,16);	

		uint8_t octets[7]={S,M,H,0,jour,mois,an};
		uint8_t *pOctets = octets;

		// écrit dans le RTC adresseI2C, 0, le tableau
		String rep = i2c_writeArray(adresseRTC,0, pOctets, 7);
		return rep;
	}

	/**
	 * @brief Écrit dans les registres de l'alarme 1
	 * 
	 * @param line rtc alarm1 d h m s
	 * @return String 
	 */
	String rtc_alarm1Write(String line){
		uint8_t ofset =0x7;
		uint8_t qte =4;

		// Permet de séparer les mots d'une line de caractères
		Vector<String> Mot;
		int qteMots = splitString(line, ' ' , Mot);

		uint8_t D = strtol(Mot[0].c_str(),NULL,16);
		uint8_t H = strtol(Mot[1].c_str(),NULL,16);		
		uint8_t M = strtol(Mot[2].c_str(),NULL,16);	
		uint8_t S = strtol(Mot[3].c_str(),NULL,16);		
		uint8_t octets[4]={M,H,D,S};
		uint8_t *pOctets = octets;		
		// écrit dans le RTC adresseI2C, avec un ofset, le tableau
		String rep = i2c_writeArray(adresseRTC,ofset, pOctets, qte);
		return rep;
	}

	/**
	 * @brief Écrit dans les registres de l'alarme 2
	 * 
	 * @param line rtc alarm2 d h m
	 * @return String 
	 */
	String rtc_alarm2Write(String line){
		uint8_t ofset =0x0b;
		uint8_t qte =3;

		// Permet de séparer les mots d'une line de caractères
		Vector<String> Mot;
		int qteMots = splitString(line, ' ' , Mot);

		uint8_t D = strtol(Mot[0].c_str(),NULL,16);
		uint8_t H = strtol(Mot[1].c_str(),NULL,16);		
		uint8_t M = strtol(Mot[2].c_str(),NULL,16);		
		uint8_t octets[3]={M,H,D};
		uint8_t *pOctets = octets;		
		// écrit dans le RTC adresseI2C, avec un ofset, le tableau
		String rep = i2c_writeArray(adresseRTC,ofset, pOctets, qte);
		return rep;
	}

	/**
	 * @brief retourne la quantité d'heures en mode faible consommation
	 * 
	 * @return uint8_t 
	 */
	uint8_t rtc_getSleepHre(){
		return sleepHre;
	}

	/**
	 * @brief retourne la quantité de minutes en mode faible consommation
	 * 
	 * @return uint8_t 
	 */
	uint8_t rtc_getSleepMin(){
		return sleepMin;
	}

	/**
	 * @brief retourne la valeur du fuseau horaire
	 * 
	 * @return String 
	 */
	String rtc_getTimeZone(){
		return timezone;
	}

	/**
	 * @brief fixe le nombre d'heures en mode de faible consommation
	 * 
	 * @param hre 
	 */
	void rtc_setSleepHre(uint8_t hre){
		sleepHre = hre;
		return;
	}

	/**
	 * @brief fixe le nombre de minutes en mode faible consommation
	 * 
	 * @param min 
	 */
	void rtc_setSleepMin(uint8_t min){
		sleepMin = min;
		return;
	}

	/**
	 * @brief fixe la valeur du fuseau horaire
	 * 
	 * @param zone 
	 */
	void rtc_setTimeZone(String zone){
		timezone = zone;
		return;
	}

	/**
	 * @brief Désactive les interruptions des alarmes pour le mode faible consommation
	 * 
	 */
	void rtc_espDontSleep(){
		// stop la minuterie pour entrer en sleep
		rtc_RunTimeStop();
		i2c_write(adresseRTC,0xf,0);		// reset int flags
		i2c_write(adresseRTC,0x0e,4); // alarm 2 interrupt disable
		//Serial.println("Mode Éveil Permanent");
	}
	
	/**
	 * @brief Set the Alarm2 Time, une fois par jour a telle heure telle minute
	 * 				Pin en mode INTerrupt, Reset le flag A2F pertmet interrupt A2IE
	 * @param heure heure de l'alarme
	 * @param minute minute de l'alarme
	 * @return String contenant le résultat de l'écriture
	 */
	String rtc_setAlarm2Time(uint8_t heure, uint8_t minute){

		uint8_t ofset =0x0b;
		uint8_t qte =5;

		uint8_t octets[5]={minute, heure, 0x80,6,0}; // mode 4
		uint8_t *pOctets = octets;		
		// écrit dans le RTC adresseI2C, avec un ofset, le tableau
		String rep = i2c_writeArray(adresseRTC,ofset, pOctets, qte);
		return rep;
	}

	/**
	 * @brief Permet d'ajuster l'alarme 2
	 * 
	 * @param jour  1-7 pour jourSem 1-31 pour jourMois
	 * @param heure 0-23
	 * @param minute 0-59
	 * @param mode 		jour/jourSem/jourMois
	 * @return String 
	 */
	String rtc_setAlarm2(uint8_t minute, uint8_t heure, uint8_t jour, uint8_t mode){

		uint8_t ofset =0x0b;
		uint8_t qte =5;
		// le mode est définit par les bits qui sont répartis dans 3 registre
		uint8_t a2m2;
		uint8_t a2m3;
		uint8_t a2m4;
		uint8_t dy;
			
		// Conversion DEC a BCD
		uint8_t min = (minute/10)<<4 | (minute%10); 
		uint8_t hre = (heure/10)<<4 | (heure%10);
		uint8_t jr = (jour/10)<<4 | (jour%10);

		// traitement des bits pour le mode
		if(mode == modeJour){a2m2=0;a2m3=0;a2m4=0x80;}
		if(mode == modeJourSemaine){a2m2=0;a2m3=0;a2m4=0x0;dy=0x40;}
		if(mode == modeJourMois){a2m2=0;a2m3=0;a2m4=0x80;dy=0;}

		//Serial.println("Valeurs BCD "+String(min) +" min et "+ String(hre) +" hre");
		uint8_t octets[5]={uint8_t(a2m2 | min), uint8_t(a2m3 | hre), uint8_t(a2m4 | dy | jr),6,0}; // 
		uint8_t *pOctets = octets;
		// écrit dans le RTC adresseI2C, avec un ofset, le tableau
		String rep = i2c_writeArray(adresseRTC,ofset, pOctets, qte);

		return rep;
	}

	/**
	 * @brief Set the Alarm2 Repetition Interval object
	 * 
	 * @param heure 
	 * @param minute
	 * @return String 
	 */
	String rtc_setWakeUpAlarm2(uint8_t heure, uint8_t minute){
		uint8_t ofset =0x0b;
		uint8_t qte =5;
		uint8_t hreNext;
		uint8_t minNext;

		if(heure==0 && minute ==0){
			rtc_espDontSleep();
			return "Arrêt Dormance\n";
		}
		
		// une fois par jour à l'heure et minute des registre
		if(heure >=24){
			return "Alarme 2 aux 24 heures\n";
		}

		// heure et minutes actuelles
		uint8_t minNowBCD =  i2c_readB(adresseRTC,0x01); //BCD des minutes du RTC
		uint8_t hreNowBCD =  i2c_readB(adresseRTC,0x02); //BCD des minutes du RTC
		// conversion BCD a DEC
		uint8_t minNow = ((minNowBCD & 0x70)>>4)*10 + (minNowBCD & 0x0f);
		uint8_t hreNow = ((hreNowBCD & 0x70)>>4)*10 + (hreNowBCD & 0x0f);
		
		//Serial.println("Now "+String(hreNow) +" hre et "+ String(minNow) +" min");
		// prochaine alarme
		minNext = minNow + minute;
		hreNext = hreNow + heure;
		if(minNext >=60){
			minNext -=60;
			hreNext++;
		}
		if(hreNext >=24)hreNext -= 24;

		//Serial.println("Éveil a "+String(hreNext) +" hre et "+ String(minNext) +" min");
		// Conversion DEC a BCD
		uint8_t min = (minNext/10)<<4 | (minNext%10); 
		uint8_t hre = (hreNext/10)<<4 | (hreNext%10);
		//Serial.println("Valeurs BCD "+String(min) +" min et "+ String(hre) +" hre");
		uint8_t octets[5]={min,hre,0x80,6,0}; // mode 4
		uint8_t *pOctets = octets;
		// écrit dans le RTC adresseI2C, avec un ofset, le tableau
		String rep = i2c_writeArray(adresseRTC,ofset, pOctets, qte);
		return rep;

	}


	/**
	 * @brief Set the Next Wake Up Alarm2
	 * 
	 */
	void rtc_setNextWakeUpAlarm2(){
		rtc_setWakeUpAlarm2(rtc_getSleepHre(),rtc_getSleepMin());
		//Serial.println("Dormance "+String(rtc_getSleepHre())+" heures et "+String(rtc_getSleepMin())+" minutes");
	}

	/**
	 * @brief Initialisation du module, pas d'alarme du RTC
	 * 
	 */
	void rtc_ini(){
		// clear interrupt flags
		i2c_write(adresseRTC,0x0e,4);	// sortie INT, no interrupt enable
		i2c_write(adresseRTC,0x0f,0);	// clears int flags et  32khz off
	}

	/**
	 * @brief Interpréteur de commandes du module
	 * 
	 * @param line 
	 * @return String 
	 */
	String rtc_interpret(String line){
		// Permet de séparer les mots d'une ligne de caractères
		Vector<String> Mot;
		int qteMots = splitString(line, ' ' , Mot);

			if(qteMots == 1)	return rtc_menu();	  

			if(qteMots == 2){
			if(Mot[1]== "menu")	return rtc_menu();		
			if(Mot[1]== "time")	return rtc_read();	
			if(Mot[1]== "alarm1")	return rtc_alarmRead(1);	
			if(Mot[1]== "alarm2")	return rtc_alarmRead(2);
			if(Mot[1]== "control")	return ("RTC Control "+i2c_readString(adrRTC + " e 1\n"));
			if(Mot[1]== "status")	return "RTC Status "+i2c_readString(adrRTC + " f 1\n");	
			if(Mot[1]== "sleepHre")	return "Heures de dormance "+ String(rtc_getSleepHre()) + "\n";	
			if(Mot[1]== "sleepMin")	return "Minutes de dormance "+String(rtc_getSleepMin()) + "\n";					
		}
			
		if(qteMots == 3){
			if(Mot[1]== "control"){
				return "RTC Control " +i2c_writeString(adrRTC+" e "+Mot[2]);
			}
			if(Mot[1]== "sleepHre"){
				rtc_setSleepHre((uint8_t)atoi(Mot[2].c_str()));
				return "Heures de dormance "+ String(rtc_getSleepHre()) + "\n";
			}
			if(Mot[1]== "sleepMin"){
				rtc_setSleepMin((uint8_t)atoi(Mot[2].c_str()));			
				return "Minutes de dormance "+String(rtc_getSleepMin()) + "\n";	
			}
			if(Mot[1]== "timezone"){
				rtc_setTimeZone(Mot[2]);
				return "Fuseau horaire "+rtc_getTimeZone() + "\n";	
			}
		}

		if(qteMots == 6){
			if(Mot[1]== "setAlarm2"){
				uint8_t minute = (int8_t)atoi(Mot[2].c_str());	
				uint8_t heure = (int8_t)atoi(Mot[3].c_str());
				uint8_t jour = (int8_t)atoi(Mot[4].c_str());
				uint8_t mode = (int8_t)atoi(Mot[5].c_str());
									
				return "RTC setAlarm2 " +rtc_setAlarm2(minute,heure,jour,mode);
			}
		}

		if(qteMots == 8){
			if(Mot[1]== "time")	return rtc_write(line);							
		}

		return line + "???\n";

	}
#endif