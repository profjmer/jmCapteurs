/**
 * @file power_.cpp
 * @author Jean Mercier profjmer@gmail.com
 * @brief Module de contrôle de la puissance 120VCa 60Hz
 * @version 1.0

 */

#include "../include/application.h"
#ifdef config_power

  #define pwrQte 9 // maximum de 9
  uint8_t power[pwrQte]; 
  uint32_t cycles_total[pwrQte];
  uint32_t cycles_low[pwrQte];
  uint32_t cycles_high[pwrQte]; // cycles alternatifs du secteur utilisés pour le contrôle.
  String power_pinList[pwrQte];  
	int powerQte;

  		/**
	 * @brief identification des broches utilsant le contrôle de la puissance CA
	 * 
	 * @return String 
	 */
	String power_pins(){
		int i,pin,l;
		String rep="[";
		for(i=0;i<powerQte;i++){
			rep += power_pinList[i]+" ";
		}
		l = rep.length()-1;
		rep = rep.substring(0,l);
		rep +="]";
		return rep;
	}

  /**
   * @brief initialisation du module power_
   * 
   */
  void power_ini(){
    int i;
    /* identification des broches utilisées en mode power */
    #ifdef pin_power_D0 
				power_pinList[powerQte++]="D0 ";
		#endif	
		#ifdef pin_power_D1
			power_pinList[powerQte++]="D1 ";			
		#endif	
		#ifdef pin_power_D2	
			power_pinList[powerQte++]="D2 ";			
		#endif	
		#ifdef pin_power_D3
			power_pinList[powerQte++]="D3 ";			
		#endif	
		#ifdef pin_power_D4
			power_pinList[powerQte++]="D4 ";			
		#endif	
		#ifdef pin_power_D5	
			power_pinList[powerQte++]="D5 ";			
		#endif	
		#ifdef pin_power_D6
			power_pinList[powerQte++]="D6 ";			
		#endif	
		#ifdef pin_power_D7
			power_pinList[powerQte++]="D7 ";			
		#endif	
		#ifdef pin_power_D8
			power_pinList[powerQte++]="D8 ";			
		#endif

    /* initialisations  pour les secteur Hydro Quebec 60Hz */
    for(i=0;i<pwrQte;i++){
      cycles_total[i] = 16660;
    }

  }

  /**
   * @brief 
   * 
   * @param pin 0..3
   * @return * u power_getPower 
   */
  int8_t power_getPower(uint8_t i){
    return power[i];
  }

  /**
   * @brief Ajuste la puissance de sortie  cycles low /cycles high pour le module pulse_
   * Le module power contrôle le fonctiontionnement des sorties du modules pulse_ en utilisant un pourcentage de la puissance maximale.
   * On peut modifier le nombre total de cycles utilisés pour le contrôle de la puissance
   * 
   * @param D_id (D0..D8) les broches pin_power_Dx et pin_pulse_Dx doivent être décommenter dans le fichier pin_.h
   * 
   * @param pourcent poucentage de la puissance maximale
   * @return String 
   */
  String power_setPower(String D_id, uint8_t pourcent){
    String id = D_id.substring(1);
    int i = atoi(id.c_str());
    if(i <0 || i >powerQte)return "Identificaion wemos pin incorrecte\n";

    if (pourcent <= 100) {
      power[i] = pourcent; 
      cycles_high[i] = cycles_total[i]*power[i]/100;
      cycles_low[i] = cycles_total[i] - cycles_high[i];
      String s = "pu hl "+D_id+ " "+String(cycles_high[i])+ " "+String(cycles_low[i])+" 65535\n";
      //Serial.println(s);
      pulse_interpret(s);
      
      return "Puissance "+D_id +" "+String(power[i])+ "%\n";}
    else return "Valeurs commande incorrectes\n";
  }

  /**
   * @brief retourne le nombre de cycles total du secteur HQ pour le contrôle de la puissance
   * 
   * @param i 
   * @return uint32_t 
   */
  uint32_t power_getCycles_total(uint8_t i){
    return cycles_total[i];
  }

  /**
   * @brief retourne le nombre de cycles du secteur HQ pour lesquels la sortie est low
   * partie low du PWM
   * @param i 
   * @return uint32_t 
   */
  uint32_t power_getCycles_low(uint8_t i){
    return cycles_low[i];
  }

  /**
   * @brief retourne le nombre de cycles du secteur HQ pour lesquels la sortie est high
   * partie high du PWM
   * @param i 
   * @return uint32_t 
   */
  uint32_t power_getCycles_high(uint8_t i){
    return cycles_high[i];
  }

  /**
   * @brief fixe le nombre total de cycles à effectuer low/high pour le PWM 
   * 
   * @param D_id identification pin wemos D0..D8
   * @param cycles 
   * @return String 
   */
  String power_setCycles_total(String D_id, uint32_t cycles){
    String id = D_id.substring(1);
    int i = atoi(id.c_str());
    if(i <0 || i >powerQte)return "Identificaion wemos pin incorrecte\n";
  
    cycles_total[i]= cycles;
    cycles_high[i] = cycles_total[i]*power[i]/100;
    cycles_low[i] = cycles_total[i] - cycles_high[i];
    return "Nombre de cycles total "+ String(cycles_total[i])+"\n";
  }

  /**
   * @brief fixe le nombre de cycles du secteur HQ pour lesquels la sortie est low
   * 
   * @param pin 
   * @param cycles 
   * @return String 
   */
  String power_setCycles_low(uint8_t pin, uint32_t cycles){
    if(pin>powerQte) return "Identificaion pin incorrecte\n";
    cycles_low[pin]= cycles;
    return "Nombre de cycles inactifs "+ String(cycles_low[pin])+"\n";
  }

  /**
   * @brief fixe le nombre de cycles du secteur HQ pour lesquels la sortie est high
   * 
   * @param pin 
   * @param cycles 
   * @return String 
   */
  String power_setCycles_high(uint8_t pin, uint32_t cycles){
    if(pin>powerQte) return "Identificaion pin incorrecte\n";
    cycles_high[pin]= cycles;
    return "Nombre de cycles actifs "+ String(cycles_high[pin])+"\n";
  }

  /**
   * @brief Informations sur les puissances programmées
   * 
   * @return String 
   */
  String power_info(){
      int i;
      String s = "Puissance :\n";
  
      #ifdef pin_power_D0
          s += "D0 "+String(power[0])+"% Cylces actifs "+String(cycles_high[0])+ "  Cycles total "+String(cycles_total[0])+"\n";
      #endif
      #ifdef pin_power_D1
          s += "D0 "+String(power[1])+"% Cylces actifs "+String(cycles_high[1])+ "  Cycles total "+String(cycles_total[1])+"\n";
      #endif
      #ifdef pin_power_D2
          s += "D2 "+String(power[2])+"% Cylces actifs "+String(cycles_high[2])+ "  Cycles total "+String(cycles_total[2])+"\n";
      #endif   
      #ifdef pin_power_D3
          s += "D3 "+String(power[3])+"% Cylces actifs "+String(cycles_high[3])+ "  Cycles total "+String(cycles_total[3])+"\n";
      #endif
      #ifdef pin_power_D4
          s += "D4 "+String(power[4])+"% Cylces actifs "+String(cycles_high[4])+ "  Cycles total "+String(cycles_total[4])+"\n";
      #endif
      #ifdef pin_power_D5
          s += "D5 "+String(power[5])+"% Cylces actifs "+String(cycles_high[5])+ "  Cycles total "+String(cycles_total[5])+"\n";
      #endif   
      #ifdef pin_power_D6
          s += "D6 "+String(power[6])+"% Cylces actifs "+String(cycles_high[6])+ "  Cycles total "+String(cycles_total[6])+"\n";
      #endif
      #ifdef pin_power_D7
          s += "D7 "+String(power[7])+"% Cylces actifs "+String(cycles_high[7])+ "  Cycles total "+String(cycles_total[7])+"\n";
      #endif
      #ifdef pin_power_D8
          s += "D8 "+String(power[8])+"% Cylces actifs "+String(cycles_high[8])+ "  Cycles total "+String(cycles_total[8])+"\n";
      #endif           

    return s;
  }


  /**
   * @brief retourne le menu des commandes du module power_
   * 
   * @return String 
   */
  String power_menu(){return "\npower\npower info\npower "+power_pins()+" % [0..100]\npower "+power_pins()+"  cyclestotal [100..60000]\n";}



  /**
   * @brief Interpréteur de commandes du module power_
   * 
   * @param line 
   * @return String 
   */
  String power_interpret(String line){
    int D_id;
    // Permet de séparer les mots d'une line de caractères
    Vector<String> Mot;
    int qteMots = splitString(line, ' ' , Mot);

    if(qteMots == 1)return power_menu();	

    if(qteMots == 2){
      if(Mot[1]== "menu")	return power_menu();
      if(Mot[1]== "info")	return power_info();                   
    }

    if(qteMots ==4){       
      /* power [D0..D8] % [0..100] */
      if(Mot[2]=="%"){
        uint8_t p = (uint8_t)atoi(Mot[3].c_str());
        if(p==0)return pulse_interpret("pu "+Mot[1]+" 0\n");
        if(p==100)return pulse_interpret("pu "+Mot[1]+" 1\n");
        return power_setPower( Mot[1], p);
      }

      /* power [0..8] cyclestotal [100..60000] */
      if(Mot[2]=="cyclestotal"){
        uint32_t cytotal= atol(Mot[3].c_str());
        return power_setCycles_total(Mot[1],cytotal);
      }       
    }

      return line + ("???\n");	    
  }
#endif