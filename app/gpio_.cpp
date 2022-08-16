/*************************************************************************
 * @file	gpio_.cpp				 
 * @version	1.0
 * @author Jean Mercier profjmer@gmail.com
 * @brief Module de gestion des broches gpios
 * 
*/
#include "../include/application.h"
#ifdef config_gpio
	int gpio_pinList[9]; // maximum 9 gpio
	int gpioQte;

	/** @brief		Initialise les broches utilisées en sortie.
	 * @param[in]	rien
	 * @returns		rien
	 */
	void gpio_ini()
	{
		int i;
		/* construire la liste des broches permises selon le fichier pin_.h */
			#ifdef pin_GPIO0
				gpio_pinList[gpioQte++]=0;
				pinMode(0,OUTPUT);
			#endif
			#ifdef pin_GPIO1
				gpio_pinList[gpioQte++]=1;
				pinMode(1,OUTPUT);				
			#endif
			#ifdef pin_GPIO2
				gpio_pinList[gpioQte++]=2;
				pinMode(2,OUTPUT);				
			#endif
			#ifdef pin_GPIO3
				gpio_pinList[gpioQte++]=3;
				pinMode(3,OUTPUT);				
			#endif	
			#ifdef pin_GPIO4
				gpio_pinList[gpioQte++]=4;
				pinMode(4,OUTPUT);				
			#endif
			#ifdef pin_GPIO5
				gpio_pinList[gpioQte++]=5;
				pinMode(5,OUTPUT);				
			#endif
			#ifdef pin_GPIO12
				gpio_pinList[gpioQte++]=12;
				pinMode(12,OUTPUT);				
			#endif
			#ifdef pin_GPIO13
				gpio_pinList[gpioQte++]=13;
				pinMode(13,OUTPUT);				
			#endif
			#ifdef pin_GPIO114
				gpio_pinList[gpioQte++]=14;
				pinMode(14,OUTPUT);				
			#endif
			#ifdef pin_GPIO15
				gpio_pinList[gpioQte++]=15;
				pinMode(15,OUTPUT);				
			#endif
			#ifdef pin_GPIO16
				gpio_pinList[gpioQte++]=16;
				pinMode(16,OUTPUT);				
			#endif													
	}

	/** @brief		Retourne l'état des gpios
	 * @param[in]	rien
	 * @returns		Retourne une String donnant l'état des gpios dans la liste gpio_pinList
	 */
	String gpios()
	{
		int i,pin;
		String io;
		String rep="GPIO state:\n";
		for(i=0;i<gpioQte;i++){
			pin=gpio_pinList[i];
			if(digitalRead(pin)==0)io= " 0\n";
			else io = " 1\n";
			rep=rep+"gpio"+pin+io;
		}
		// envoi statut par websockets
		gpio_status();

		return rep;
	}

	/** @brief		Retourne la liste des gpios utilisés
	 * @param[in]	rien
	 * @returns		Retourne la liste gpio_pinList
	 */
	String gpio_list()
	{
		int i,pin,l;
		String rep="[";
		for(i=0;i<gpioQte;i++){
			pin=gpio_pinList[i];
			rep += String(pin)+" ";
		}
		l = rep.length()-1;
		rep = rep.substring(0,l);
		rep +="]";
		return rep;
	}

	/** @brief		Retourne le menu pour le module gestionIO.c
	 * @param[in]	rien
	 * @returns		Retourne une String donnant la liste des commandes et des broches disponibles
	 */
	String gpio_menu(){
		String menu = "\ngpio\ngpio state\ngpio write pin"+gpio_list()+" etat\n"+"gpio flip pin"+gpio_list()+"\n";
	return menu;
	}

	/**
	 * @brief mise en format JSON du pinFeedback
	 * 
	 * @param id identification de la broche
	 * @return String  {"type":"gpio", "id":x, "etat":y} x numéro pin y 0 ou 1
	 */
	String pinFeedbackToJSONstring(int id){
		bool etat = digitalRead(id);

		String rep ="{\"type\":\"gpio\", \"id\":" +(String)id + ", \"etat\":" + etat + "}";
		//Serial.println(rep);
		return (rep);
	}

	/** @brief		Retourne l'état de la broche fournie en paramètre
	 * @param[in]	numéro de la broche
	 * @returns		Retourne une String donnant l'état de la broche
	 */
	String pinFeedback(uint8_t pin)
	{	
			String id(pin);	// construire une String à partir du uint8_t
			String rep = "gpio" + id;

			// feedback format JSON
			web_wsSend(pinFeedbackToJSONstring(pin));
			
			// feedback format texte
			if(digitalRead(pin)==1)	return rep+" 1\n";
			else return rep + " 0\n";
	}

	/** @brief		Contrôle les sorties gpios
	 * activation des sorties sur réception de la commande
	 * la broche est relue et son etat est transmis au client
	 * exemple de commande: gpio 12 0	 pour mettre gpio 12 à 0
	 * @param[in]	numéro de la broche, état de la broche
	 * @returns		Retourne une String donnant l'état de la broche
	 */
	String gpio(uint8_t pin, uint8_t etat)
	{
			int i;	
			// si la pin n'est pas dans la liste -> return "pin not in gpio_pinList\n";
			bool found = false;
			for(i=0;i<gpioQte;i++){
				if(pin==gpio_pinList[i]){found=true;break;}
			}
			if(found==false)return "Erreur gpio pin\n";
			
			// modifier etat
			// n'importe quelle valeur autre que 0 met la broche à 1
			if(etat==0)digitalWrite(pin,0);
			else digitalWrite(pin,1);
		
			return pinFeedback(pin);
	}

	/** @brief		Bascule une sortie gpio
	 * exemple de commande: gpio flip 12	 pour basculer gpio12
	 * @param[in]	numéro de la broche
	 * @returns		Retourne une String donnant l'état de la broche
	 */
	// basculer l'état de la broche dont l'id est fournie en paramètre	
	String flipio(uint8_t pin)
	{
		int i;
		
		// si la pin n'est pas dans la liste -> return
		bool found = false;
		for(i=0;i<gpioQte;i++){
			if(pin==gpio_pinList[i]){found=true;break;}
		}
		if(found==false)return"Erreur gpio pin\n";

		// lire et écrire l'inverse.
		digitalWrite(pin,!digitalRead(pin));

		// feedback
		return pinFeedback(pin);
	}

	/**
	 * @brief Envoi le statut des gpios par websocket
	 * 
	 */
	void gpio_status(){
		for(int i=0;i<gpioQte;i++){
			web_wsSend(pinFeedbackToJSONstring(gpio_pinList[i]));
		}
	}

	/** @brief		Interpréteur de commandes du module de gestionIO
	 * @param[in]	ligne de commande
	 * @returns		Retourne une String donnant le résultat de l'interprétation de la commande
	 */
	String gpio_interpret(String line){
		// Permet de séparer les mots d'une line de caractères
		Vector<String> Mot;
		int qteMots = splitString(line, ' ' , Mot);

		// retourne le menu
		if(qteMots == 1)	return gpio_menu();

		// retourne le menu des commandes du module gestionIO
		if(qteMots == 2){
			if(Mot[1]== "menu") return gpio_menu();
			if(Mot[1]== "state") 	return gpios();
		}

		// exemple de commande gpio flip 2 pour basculer la broche 2
		if(qteMots == 3){
			if(Mot[1]== "flip"){
				int i;
				// extraire no de gpio_pinList et convertir en uint8
				uint8_t pin = (int8_t)atoi(Mot[2].c_str());
				return flipio(pin); // changer l'état gpio et retourne la chaine du résultat
			}
		}
		
		// exemple de commande: gpio write 12 0	mettre gpio 12 à 0
		if(qteMots == 4){
			if(Mot[1]== "write")
			{
				// extraire no de gpio_pinList et convertir en uint8
				uint8_t pin = (int8_t)atoi(Mot[2].c_str());		
				// extraire etat
				uint8_t etat=(int8_t)atoi(Mot[3].c_str());
				return gpio(pin,etat);	// changer l'état gpio qui retourne la chaine du résultat
			}
		}
			// commande non reconnue
		return line + "  ???\n";
	}
#endif