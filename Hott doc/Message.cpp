// 
// LiPoMetre pour télémesure Graupner Hott
// 
// Gestion messagerie Hott
//
// Auteur : www.thyzoon.fr 
//

#include <EEPROM.h>  
#include "Message.h"
#include "Tension_Lipo.h"


#define VERSION         "0.60"

#define LEDPIN_OFF              PORTB &= ~(1<<5);
#define LEDPIN_ON               PORTB |= (1<<5);
// -------------------------------------
// Valeurs par défaut stockées en EEPROM
//
#define SEUIL_TENSION_ALARME 340      // 350 = 3.40V
#define SON_ALARME            9       // Bip
#define REPETITION_ALARME    15       // 15 s

#define ALARME_OFF 0

// buffer message
static uint8_t _hott_serial_buffer[173];   //création d'une variable tampon pour stocker les struct

// pointer des structures vers le tampon "_hott_serial_buffer"
struct HOTT_GAM_MSG             *hott_gam_msg = (struct HOTT_GAM_MSG *)&_hott_serial_buffer[0];
struct HOTT_TEXTMODE_MSG	*hott_txt_msg =	(struct HOTT_TEXTMODE_MSG *)&_hott_serial_buffer[0];

word seuil_tension_alarme;       // seuil tension alarme (1 element)
word son_alarme;                 // son alarme
word repetition;                 // Repetition alarme

// For communication
static uint8_t octet1 = 0;  // reception
static uint8_t octet2 = 0;  // reception

// Variables
static uint8_t nb_Lipo = 3; // Nb d'éléments accu Lipo
float  Lipo_total = 0;   // Tension totale mesurée
float  Lipo_Total_Mini = 26;   // Tension totale mini mesurée 
float lipo1 = 0.0; // Tension sur chacun des éléments
float lipo2 = 0.0;
float lipo3 = 0.0;
float lipo4 = 0.0; 
float lipo5 = 0.0; 
float lipo6 = 0.0; 
float lipo_mini = 0.0; // Valeur tension mini element
uint8_t Jauge = 0;  // Jauge carburant
uint8_t Alarme = false; // Alarme tension basse
uint8_t Alarme_On = false; // Alarme en marche

unsigned long temps = 0; // Gestion du temps depuis mise sous tension en ms
unsigned long temps_alarme = 0; // date de l'alarme de tension basse


// Sauvegardedes parametres en EEPROM 
/*
 !WARNING!
 Writing takes 3.3ms.
 Maximum life of the EEPROM is 100000 writings/readings.
 */
// Adresses des parametres en EEPROM, tous les parametres sont stockes sur 16 bits.  
#define adr_eprom_init                 0     // For the test for 1st time init of the Arduino (First power on)
#define adr_eprom_seuil_tension_alarme 2     // Tension seuil alarme
#define adr_eprom_son_alarme           4     // Son alarme
#define adr_eprom_repetition           6     // Repetition


extern TensionLipo tension;

GMessage::GMessage(){

}
void GMessage::init(){
  
// Initialisation COM 19200 bauds pour recepteur HOTT GRxx
  Serial.begin (SERIAL_COM_SPEED); 
  
  // ******************************************************
  // Test for 1st time init of the Arduino (First power on)
  int test = read_eprom(adr_eprom_init);
  if (test != 120)
  {
    write_eprom(adr_eprom_init,120);
    write_eprom(adr_eprom_seuil_tension_alarme,SEUIL_TENSION_ALARME);  // Seuil par défaut
    write_eprom(adr_eprom_son_alarme,SON_ALARME);                      // Son par défaut	
	write_eprom(adr_eprom_repetition,REPETITION_ALARME);
  }
  // Read saved values from EEPROM
  seuil_tension_alarme = read_eprom(adr_eprom_seuil_tension_alarme);        // sens gaz Normal / Inversé  
  son_alarme = read_eprom(adr_eprom_son_alarme);        // son alarme
  repetition = read_eprom(adr_eprom_repetition);        // 
  
  // Init. pour certaines données  
  setLipo (0,1);
  setLipo (0,2);
  setLipo (0,3);      
  setLipo (0,4);  
  setLipo (0,5);  
  setLipo (0,6);  
  setAltitudeInMeters (0);
  setClimbrate (0);
  setClimbrate_M3 (0);
  setTemp (0,2);
  setVoltage2(0); 
  
  nb_Lipo = tension.Detect (); // detection du nombre d'elements 2S a 6S  
  setTemp (nb_Lipo,1); // affichage nb element a la place de Temp1  
}

// Lecture d'un mot de 16 bits en EEPROM
uint16_t GMessage::read_eprom(int address){
  return  (uint16_t) EEPROM.read(address) * 256 + EEPROM.read(address+1) ;
}

// Ecriture d'un mot de 16 bits en EEPROM
void GMessage::write_eprom(int address,uint16_t val){
  EEPROM.write(address, val  / 256);
  EEPROM.write(address+1,val % 256 );
}

// Initialisation message GAM (General Air Module)
void GMessage::init_gam_msg(){
  //met tous à Zero, puis on modifie les constantes
  memset(hott_gam_msg, 0, sizeof(struct HOTT_GAM_MSG));   
  hott_gam_msg->start_byte = 0x7c;
  hott_gam_msg->gam_sensor_id = HOTT_TELEMETRY_GAM_SENSOR_ID;
  hott_gam_msg->sensor_id = 0xd0;
  hott_gam_msg->stop_byte = 0x7d;
}

// Emission de la trame
void GMessage::send(int lenght){ 
  uint8_t sum = 0;
  delay(5);
  for(int i = 0; i < lenght-1; i++){
    sum = sum + _hott_serial_buffer[i];
    Serial.write (_hott_serial_buffer[i]);
    delayMicroseconds(HOTTV4_TX_DELAY);

    Serial.read(); // A cause du rebouclage TX <--> RX Lecture de l'octet émis 
  }  
  //Emision du checksum
  Serial.write (sum);
  delayMicroseconds(HOTTV4_TX_DELAY);
  Serial.read(); // A cause du rebouclage TX <--> RX Lecture de l'octet émis 
}

void GMessage::main(){ 
  // 
  static byte page_settings = 1;//numéro de la page de settings à afficher 
    
  if (Serial.available() >= 2) {
    uint8_t octet1 = Serial.read();
    switch (octet1) {
    case HOTT_BINARY_MODE_REQUEST_ID:
      { 
        uint8_t  octet2 = Serial.read();        
        // Demande RX Module =	$80 $XX
        switch (octet2) {
        case HOTT_TELEMETRY_GAM_SENSOR_ID: //0x8D
        {
            // init structure
            init_gam_msg();        
            
            // ******************************************************
            LEDPIN_ON	   

            // Mesure des tensions
            lipo1 = tension.Tension_Lipo1();  
            lipo2 = tension.Tension_Lipo2();  
            lipo3 = tension.Tension_Lipo3();  
            lipo4 = tension.Tension_Lipo4();
            lipo5 = tension.Tension_Lipo5();
            lipo6 = tension.Tension_Lipo6();
  
            // Calcul de la valeur de chaque elements
            switch (nb_Lipo){
             case 2 :
             Lipo_total = lipo2;
             lipo2 = lipo2 - lipo1;
             break;  

             case 3 :
             Lipo_total = lipo3;
             lipo3 = lipo3 - lipo2;
             lipo2 = lipo2 - lipo1;       
             setLipo (lipo3,3);   
             break;        

             case 4 :
             Lipo_total = lipo4;
             lipo4 = lipo4 - lipo3;
             lipo3 = lipo3 - lipo2;
             lipo2 = lipo2 - lipo1;
             setLipo (lipo3,3);      
             setLipo (lipo4,4);  
             break;  
        
            case 5 :
            Lipo_total = lipo5;
            lipo5 = lipo5 - lipo4;
            lipo4 = lipo4 - lipo3;
            lipo3 = lipo3 - lipo2;
            lipo2 = lipo2 - lipo1;
            setLipo (lipo3,3);    
            setLipo (lipo4,4);      
            setLipo (lipo5,5);  
            break;          
        
            case 6 :
            Lipo_total = lipo6;
            lipo6 = lipo6 - lipo5;
            lipo5 = lipo5 - lipo4;
            lipo4 = lipo4 - lipo3;
            lipo3 = lipo3 - lipo2;
            lipo2 = lipo2 - lipo1;
            setLipo (lipo3,3);    
            setLipo (lipo4,4);      
            setLipo (lipo5,5);  
            setLipo (lipo6,6);  
            break;         
        
            default : ;
          }     
         
         // par défaut 1 et 2      
          setLipo (lipo1,1);
          setLipo (lipo2,2);

          setVoltage1(Lipo_total);    // Affichage sur Batt 1
          setMainVoltage(Lipo_total); // Affichage sur principal
            
          if (Lipo_total < Lipo_Total_Mini)  {
            Lipo_Total_Mini = Lipo_total; // mémorise la valeur mini
            setVoltage2(Lipo_Total_Mini); 
          }
       
         // 
         // Recherche element le plus faible
         // pour affichage jauge
         // et gestion alarme radio
         lipo_mini = tension.Lipo_Mini(nb_Lipo,lipo1,lipo2,lipo3,lipo4,lipo5,lipo6);
       
         if ((lipo_mini >= 3.6)&&(lipo_mini <= 4.2))
            Jauge = 100 - (4.2 - lipo_mini) / 0.0066667;           
         else if ((lipo_mini < 3.6) && (lipo_mini >= 3.0)) 
            Jauge = (lipo_mini - 3.0) / 0.06 ; 
         else if (lipo_mini < 3.0)
            Jauge = 0;               
       
         // Alarme
         if (lipo_mini < (float(seuil_tension_alarme)/100.0))        
           Alarme = true;
         else
           Alarme = false;
  
         if (Alarme_On == true) {
           alarme (son_alarme); // Bip alarme
           temps_alarme = millis ();             // temps courant
           Alarme_On = false;                    // Flag faux
         }
         else {
           alarme (ALARME_OFF);}          // fin d'alarme
        
        // Affichage jauge carburant
        setFuelPercent (Jauge);

        LEDPIN_OFF
  
        if (Alarme == true) {
          temps = millis ();
          if (temps > temps_alarme + (repetition * 1000)) { // repetition  entre alarmes
            Alarme_On = true;
          }  
      }            
                        
      send(sizeof(struct HOTT_GAM_MSG));
            break;
       } //end case GAM*/
       } //end case octet 2
        break;
    }

    case HOTT_TEXT_MODE_REQUEST_ID:
      {      
        uint8_t  octet3 = Serial.read();
        byte id_sensor = (octet3 >> 4);
        byte id_key = octet3 & 0x0f;
        static byte ligne_select = 1 ; // Ligne 1 selectionnée par défaut
        static int8_t ligne_edit = -1 ;
        hott_txt_msg->start_byte = 0x7b;
        hott_txt_msg->esc = 0;
        hott_txt_msg->warning_beeps = 0;
        
        memset((char *)&hott_txt_msg->text, 0x20, HOTT_TEXTMODE_MSG_TEXT_LEN);
        hott_txt_msg->stop_byte = 0x7d;
        
          if (id_sensor == (HOTT_TELEMETRY_GAM_SENSOR_ID & 0x0f)) 
          {
            
            switch (page_settings) { // Pages de parametres LiPoMetre
              
              case 1:   //PAGE 1 
              {          
                    if (ligne_edit == -1)
                    {
                      switch (id_key) {
                        case HOTT_KEY_UP : 
                          ligne_select = min(3,ligne_select + 1); // Ligne 2 maxi                 
                          break;                
                          
                        case HOTT_KEY_DOWN :
                          ligne_select = max(1,ligne_select - 1); //Ligne 1 mini
                          break; 
                          
                        case HOTT_KEY_SET :                      // Sélection de la ligne courante
                          ligne_edit = ligne_select ;
                          break;
                          
                        case HOTT_KEY_LEFT :        // Sortie
                          hott_txt_msg->esc = 0x01;
                          ligne_edit = -1 ;
                          ligne_select = 1;
                          break;  
                                           
                        case HOTT_KEY_RIGHT :                   
                          page_settings = 1;
                          ligne_edit = -1 ;     
                          ligne_select = 1;                     
                          break;    
                        
                        default : break;
                      }
                    }   
                      
                    //*************************************************
                    //LIGNE 2 SELECTIONNEE = Seuil alarme
                    else if (ligne_select == 1)
                    {
                      switch (id_key) {
                        case HOTT_KEY_UP :
                          seuil_tension_alarme = seuil_tension_alarme +5;  
                          if (seuil_tension_alarme >380)
                            seuil_tension_alarme = 380;
                          break;
                          
                        case HOTT_KEY_DOWN :
                          seuil_tension_alarme = seuil_tension_alarme -5;  
                          if (seuil_tension_alarme <300)
   			    seuil_tension_alarme = 300;    
                          break;
                          
                        case HOTT_KEY_SET :  
                          ligne_edit = -1 ;
                          write_eprom(adr_eprom_seuil_tension_alarme,seuil_tension_alarme); // Ecriture en memoire
                          break;
                          
                        default : break;
                      }
                    }          
                    
                    //*************************************************
                    //LIGNE 3 SELECTIONNEE = Son alarme
                    else if (ligne_select == 2)
                    {
                      switch (id_key) {
                        case HOTT_KEY_UP :
                          son_alarme = son_alarme + 1;
                          if (son_alarme > 26) 
                            son_alarme = 26;                     
                          break;
                          
                        case HOTT_KEY_DOWN :
                          son_alarme = son_alarme - 1;
                          if (son_alarme < 1) 
                            son_alarme = 1; 
                          break;
                          
                        case HOTT_KEY_SET :  
                          ligne_edit = -1 ;
                          write_eprom(adr_eprom_son_alarme,son_alarme); // Ecriture en memoire
                          break;                     
                          
                        default : break;
                      }
                    }                          
                                   
                    //*************************************************
                    //LIGNE 4 SELECTIONNEE = Repetition alarme
                    else if (ligne_select == 3)
                    {
                      switch (id_key) {
                        case HOTT_KEY_UP :
                          repetition = repetition + 5;
                          if (repetition > 30) 
                            repetition = 30;                     
                          break;
                          
                        case HOTT_KEY_DOWN :
                          repetition = repetition - 5;
                          if (repetition < 10) 
                            repetition = 10; 
                          break;
                          
                        case HOTT_KEY_SET :  
                          ligne_edit = -1 ;
                          write_eprom(adr_eprom_repetition,repetition); // Ecriture en memoire
                          break;                     
                          
                        default : break;
                      }
                    }                    
                 

                    //******************************************************************************************
                    // Affichage de la page 1
                    //ligne 0:
                    snprintf((char *)&hott_txt_msg->text[0],21," * LiPoMetre V%s *",VERSION);
                    //ligne 1:
                    snprintf((char *)&hott_txt_msg->text[1],21," Seuil alarme  %i.%iV",int(seuil_tension_alarme/100),int(seuil_tension_alarme%100)) ;   //float(seuil_tension_alarme)/100.0);
                    //ligne 2:
                    snprintf((char *)&hott_txt_msg->text[2],21," Son alarme       %2i",son_alarme);
                    //ligne 3:
                    snprintf((char *)&hott_txt_msg->text[3],21," Repet. alarme   %2is",repetition);
                    //ligne 4:
                    snprintf((char *)&hott_txt_msg->text[4],21," ------------------- ");
                    //ligne 5:
                    snprintf((char *)&hott_txt_msg->text[5],21," LIPO             %iS", nb_Lipo);
                    //ligne 6:
                    snprintf((char *)&hott_txt_msg->text[6],21," ------------------- ");
                    //ligne 7:
                    snprintf((char *)&hott_txt_msg->text[7],21," thyzoon.fr     2015"); 
                    
                    hott_txt_msg->text[ligne_select][0] = '>';
                    _hott_invert_ligne(ligne_edit);
                    break;                    
              }//END PAGE 1
                            
                  default://PAGE 
                  {
                    break;
                  }            
                 
            }//END SETTINGS 
       
          } // END IF 
          
          //********************************************************************************************************************

          else if(id_sensor == (HOTT_TELEMETRY_GEA_SENSOR_ID & 0x0f)) {
            snprintf((char *)&hott_txt_msg->text[0],21,"Module EAM");
            snprintf((char *)&hott_txt_msg->text[1],21,"----------");
            snprintf((char *)&hott_txt_msg->text[2],21,"Selectionner :");
            snprintf((char *)&hott_txt_msg->text[3],21," GENERAL");
          }
          else if(id_sensor == (HOTT_TELEMETRY_VARIO_SENSOR_ID & 0x0f)) {         
            snprintf((char *)&hott_txt_msg->text[0],21,"Module VARIO");
            snprintf((char *)&hott_txt_msg->text[1],21,"------------");
            snprintf((char *)&hott_txt_msg->text[2],21,"Selectionner :");
            snprintf((char *)&hott_txt_msg->text[3],21," GENERAL");
          }
          else {
            snprintf((char *)&hott_txt_msg->text[0],21,"Module inconnu");
            snprintf((char *)&hott_txt_msg->text[1],21,"--------------");
            snprintf((char *)&hott_txt_msg->text[2],21,"Selectionner :");
            snprintf((char *)&hott_txt_msg->text[3],21," GENERAL");
          }
        //}
        // Emission en mode texte
        _hott_send_text_msg();
        break;
      }
    }	
  }
}

//////////////////////////////////////////////////
// Tension principale (resolution 0.1V)
void GMessage::setMainVoltage(float tension){
  hott_gam_msg->main_voltage = (uint16_t) (tension * 10);
}

// Batterie 1 (resolution 0.1V)
void GMessage::setVoltage1(float volt){
  hott_gam_msg->Battery1 =  (uint16_t) (volt * 10); 
}

// Batterie 2 (resolution 0.1V)
void GMessage::setVoltage2(float volt){
  hott_gam_msg->Battery2 = (uint16_t) (volt * 10); 
}

// Element Lipo n (resolution 0.02V)
void GMessage::setLipo(float volt, int lipo){

  if (lipo >= 1 and lipo <= 6)
  {
    lipo--;
    hott_gam_msg->cell[lipo] =  (uint16_t) 100 * volt / 2 ; 
    
    if (volt/2 <= hott_gam_msg->min_cell_volt || hott_gam_msg->min_cell_volt ==0)
    {
      hott_gam_msg->min_cell_volt = (uint16_t) 100 * volt/2 ;
      hott_gam_msg->min_cell_volt_num = lipo;
    }
  }
}

// Altitude relative en metres : -500 / +9999 [m]
void GMessage::setAltitudeInMeters(uint16_t alti){
  hott_gam_msg->altitude = alti + 500;
}

// Temperature : -20 a +235° C 
// 
void GMessage::setTemp(int temp, int capteur){
  if(temp < -20)
    temp = -20;
  else if(temp > 234)
    temp = 235;
  if (capteur < 1)
    capteur = 1;
  if (capteur > 2)
    capteur = 2; 
  if (capteur == 1)
    hott_gam_msg->temperature1 = temp + 20;
  else if (capteur == 2)
    hott_gam_msg->temperature2 = temp + 20;
}

// Niveau reservoir : 0%, 25%, 50%, 75%, 100%  
// 
void GMessage::setFuelPercent(uint8_t pourcent){
  if(pourcent > 100)
    pourcent = 100;
  else if(pourcent < 0)
    pourcent = 0;
  hott_gam_msg->fuel_procent = pourcent ;
}

// niveau reservoir en ml : 0 a 65535 ml
// 
void GMessage::setFuelMilliliters(uint16_t ml){
  hott_gam_msg->fuel_ml = ml ;
}

// Rotation : 0 a 655350 tr/min
// 

void GMessage::setRPM(uint16_t rpm){
  hott_gam_msg->rpm = rpm ;
} 

// Set climbrates
void GMessage::setClimbrate(uint16_t climb_L){
  hott_gam_msg->climbrate_L =  climb_L+30000;
}

void GMessage::setClimbrate_M3(int climb3s){
  hott_gam_msg->climbrate3s =  climb3s+120;
}

// Set speed
void GMessage::setSpeed(float speed){
  hott_gam_msg->climbrate3s =  (int) speed;
}


//
// Alarme sonore sur radio
//
void GMessage::alarme (uint8_t son){
  hott_gam_msg->warning_beeps = son;
}

void GMessage::_hott_send_text_msg() {
  for(byte *_hott_msg_ptr = hott_txt_msg->text[0]; _hott_msg_ptr < &hott_txt_msg->stop_byte ; _hott_msg_ptr++){
    if (*_hott_msg_ptr == 0)
      *_hott_msg_ptr = 0x20;
  }  
  //_hott_send_msg(_hott_serial_buffer, sizeof(struct HOTT_TEXTMODE_MSG));
  send(sizeof(struct HOTT_TEXTMODE_MSG));
}

char * GMessage::_hott_invert_all_chars(char *str) {
  return _hott_invert_chars(str, 0);
}

char * GMessage::_hott_invert_chars(char *str, int cnt) {
  if(str == 0) return str;
  int len = strlen(str);
  if((len < cnt)  && cnt > 0) len = cnt;
  for(int i=0; i< len; i++) {
    str[i] = (byte)(0x80 + (byte)str[i]);
  }
  return str;
}

void GMessage::_hott_invert_ligne(int ligne) {
  if (ligne>= 0 && ligne<= 7)
    for(int i=0; i< 21; i++) {
      if (hott_txt_msg->text[ligne][i] == 0)   //inversion du caratère null (fin de string)
        hott_txt_msg->text[ligne][i] = (byte)(0x80 + 0x20);
      else
        hott_txt_msg->text[ligne][i] = (byte)(0x80 + (byte)hott_txt_msg->text[ligne][i]);
    }
}



