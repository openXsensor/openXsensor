/*
*************************************************************************************
*                                  Information importante:
*  Ne pas modifier ce fichier a mois de vouloir y a jouter vos propre commentaire.
*  Pour configurer votre OXS, vous devez modifier au moins le fichier oXs_config_basic.h
*  Dans certains cas, vous devrez également modifier le fichier oXs_config_advanced.h
*************************************************************************************

Voir le github OpenXsensor https://github.com/openXsensor/
Ecrit par par Rainer Schlosshan traduction Thierry ZINK

************************************************************************************************************************
************************************* description général de toute les options *****************************************
************************************************************************************************************************
*  le fichier OXS_config.h permet aux utilisateurs de configurer différentes options. Voici la liste des différentes options.
*  
*  1 - Choix du protocole de télémétrie
*    1.1 - Sélection de la sortie vers le récepteur
*    1.2 - SPORT_SENSOR_ID (utilisé unique pour le protocol FrSky Sport)
*  2 - Données a transmettre a l'émetteur
*  3 - réglage de la sortie PPM(optionnel)
*  4 - Réglage Vario (le vario est optionnel)
*   4.1 - Connections de 1 ou 2 capteurs barométriques 
*   4.2 - Type de vitesse vertical envoyer
*   4.3 - Sensibilité predefinie par programation
*   4.4 - Sensibilité ajuster depuis l'emmeteur
*   4.5 - Parametre hysteresis & compensation de l'altitude a partir du capteur de temperature 
*   4.6 - Calcule de la vitesse vertical baser sur le signal PPM (option)
*   4.7 - Vitesse vertical analogique (optionnel)
*   4.8 - Calcul des finesse du planeur, calcul moyen des tôt de monter/descente, calcul moyen de l’altitude gagner/perdu (optionel)
*  5 – Réglage de la sonde de Pitot (optionnel)
*  6 – Réglage capteur de tension et de courant (optionnel)
*   6.1 - Tension de référence pour mesure des tensions et des courants
*   6.2 - Paramètres des tensions
*   6.3 - Maximum de cellule LiPo mesurer (et envoyer a l’émetteur)
*   6.4 - Conversion de tension en température (° Celcius)     
*   6.5 - Paramètre des capteurs de mesure de courant.
*   6.6 - Paramètres de Ads1115
*  7 - Réglage capteur RPM (tour par minute) (optionnel)
*  8 - Réglage de la mémoire non volatile (optionnel)
*  9 - GPS (optionnel)
*  10 - IMU 6050 (capteur accéléromètre/gyroscope) (optionnel)  et HMC5883 (magnetometre)
*  11 - Capteur de débit 
*  20 - Séquenceur  (ON/OFF) pour certaine sortie digitale (Exemple : contrôle de lumière)
*  xx – Réserver au développeur
*
* Note: Les paramètres activé commence par  "#define",  suivi du nom du paramètre et dans la majorité des cas d’une valeur pour ce paramètre
*        Pour activé ou désactivé les paramètres il faut supprimer ou ajouté les "//" qui précède "#define" Example “// #define ParameterToto Bonjour“ le ParametreToto ne sera pas pris en compte car pour Arduino c’est considérer comme un commentaire. 
************************************************************************************************************************
comment

**** 1 - Choix du protocole de télémétrie **********************************************************************
* Actuellement OXS supports 4 protocoles de télémétrie: Multiplex , Frsky, Jeti and Hott (=Graupner)
* Un et uniquement un protocole peut-être sélection; Exemple si vous active le protocole Multiplex, les protocoles Frsky, Jeti et Hott sont désactivé (et vice et versa) 
* FrSky utilise 2 protocoles différents :
*       - SPORT utilisé par les récepteurs de la série X (comme le X8R ou X6R)
*       - HUB utilisé par les récepteurs de la série D (comme le D4R-II)
*      OXS peut supporter les deux protocoles FrSky en même temps (en utilisant l’option FRSKY_SPORT_HUB). OXS peut automatiquement détecter le type de récepteur auquel il est connecté
*      Si vous utilisez cette option vous n’aurez pas besoin de reprogrammer votre OXS si vous passez d’un récepteur série X à série D.
*      l’auto détection ne semble pas marcher a 100% et demande plus de mémoire…
*      Dans le cas de l’auto détection ne fonctionne pas ou si vous avez besoin de plus de mémoire (car vous utilisé beaucoup de capteur connecté à votre OXS), vous pouvez forcer sur le protocole FRSKY spécifique que vous utilisez
*  Pour sélectionné le protocole utilise par OXS remplir la ligne #define PROTOCOL avec le nom du protocole sélectionné avec l’une des valeurs suivante: FRSKY_SPORT , FRSKY_HUB , FRSKY_SPORT_HUB , MULTIPLEX , HOTT; JETI
************************************************************************************************************************
#define PROTOCOL FRSKY_SPORT    // select between FRSKY_SPORT , FRSKY_HUB , FRSKY_SPORT_HUB , MULTIPLEX , HOTT, JETI


*  1.1 - Sélection de la sortie vers le récepteur ***********************************************************************************************
*   OXS doit être connecté a récepteur afin de transmettre les informations.
*   Pour cela, une sortie DIGITAL de l’arduino doit être connectée au récepteur.
*   Vous devez spécifier la PIN arduino utilisé pour cette fonction.
*   Valeur par défaut: 4 ; Les valeurs autorisées sont 2 ou 4 mais faite attention de ne pas utiliser la même PIN pour 2 fonction. 
************************************************************************************************************************
#define PIN_SERIALTX  4

*  1.2 ****** SPORT_SENSOR_ID (utilisé unique pour le protocol FrSky Sport) **********************    
*  
*      Pour le protocole SPORT, il peut y avoir plusieurs capteur connecté sur le même bus  mais chaque capteur doit avoir un identifiant « SPORT_SENSOR_ID » diffèrent.
*           Pour le protocole SPORT, OXS peu utilisé jusqu’à 6 identifiant. Les 6 SPORT_SENSOR_ID utilisé par OXS sont :    
*               DATA_ID_VARIO  	0x00  	// = sensor 0 utilisé pour altitude et la vitesse vertical
*               DATA_ID_FLVSS  	0xA1 	//		    1 utilisé pour la mesure de élément de baterie
*               DATA_ID_FAS		0x22  	//         	2 utilisé pour vfas, courant et carburant
*               DATA_ID_GPS		0x83  	//          3 utilisé pour les données GPS
*               DATA_ID_RPM		0xE4  	//          4 utilisé pour les rpm, T1, T2, et la sonde de pitot
*               DATA_ID_ACC		0x67  	//          7 utilisé poue Acceleration sur X, Y, Z
*               Si besoin (par exemple pour connecter 2 OXS envoyant deux donné identique), vous pouvez changer les valeur de SPORT_SENSOR_ID mais vous devez uniquement selectioner des adresse parmie les valeur suivante:
*                  0x00,0xA1,0x22,0x83,0xE4,0x45,0xC6,0x67,0x48,0xE9,0x6A,0xCB,0xAC,0x0D,0x8E,0x2F,0xD0,0x71,0xF2,0x53,0x34,0x95,0x16,0xB7,0x98,0x39,0xBA,0x1B
*               les valeurs du paramétre SPORT_SENSOR_ID sont défini dans le fichier OXS_config_advanced.h (section 1.1)
************************************************************************************************************************



**** 2 - Data to transmit **********************************************************************************************
*   selon les paramètres selection dans le fichié config.h (et les capteur connecté à OXS), OXS peux calculer plusieurs messures 
*   Pour le protocole Multiplex, vous devez spécifier quelle mesures doit être renvoyer a l'emmeteur (et sur quelle ligne de l'ecran de l'émmeteur il doit etre afficher) (voir ci dessous) 
*   Pour les protocoles SPORT, HUB, JETI et HOTT, les mesure sont automatique transmise dans le champs adapté et certain protocol autorise envoyer certain mesure en reutilisant certain champs.
*   Une de mesure effectuer est la "Vitesse vertical principale".
**** 2.1 Protocole FrSKY (SPORT et HUB ) ************************************************************************************ 
*      les meusure transmise automatiquement sont : 
*         - Altitude relative(cm), Vitesse vertical principale(cm/sec) (si au minimum un capteur barométrique est connecté)
*         - Tensions des cellules (si NUMBER_OF_CELLS > 0) 
*         - Courant (quand un capteur de courant est connecté)
*         - GPS (long, lat, vitesse, altitude , course) (quand un GPS est connecté)
*         - RPM (hz) (quand un capteur de vitesse de rotation est connecté)
*      En suplement de ces mesure, vous pouvez sepcifié les parametères des messures suivante Vfas, Fuel, A3, A4, T1, T2, AccX, AccY, AccZ (voir les option disponible ci-dessous)
*      Commenter toutes les lignes qui ne doive pas être transmise.
*    ex :  
*      #define VFAS_SOURCE VOLT_4                 // selectionné une valeur parmi: VOLT_1, VOLT_2, VOLT_3 , VOLT_4, VOLT_5 , VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
*      #define FUEL_SOURCE VOLT_4                 // selectionné une valeur parmi: VOLT_1, VOLT_2, VOLT_3 , VOLT_4, VOLT_5 , VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
*      #define A3_SOURCE VOLT_4                   // selectionné une valeur parmi: VOLT_1, VOLT_2, VOLT_3 , VOLT_4, VOLT_5 , VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
*      #define A4_SOURCE VOLT_4                   // selectionné une valeur parmi: VOLT_1, VOLT_2, VOLT_3 , VOLT_4, VOLT_5 , VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
*      #define ACCX_SOURCE TEST_1                 // selectionné une valeur parmi: TEST_1, TEST_2, TEST_3, GLIDER_RATIO , SECONDS_SINCE_T0 ,AVERAGE_VSPEED_SINCE_TO , VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, PITCH, ROLL, YAW, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4 
*      #define ACCY_SOURCE TEST_2                 // selectionné une valeur parmi: TEST_1, TEST_2, TEST_3, GLIDER_RATIO , SECONDS_SINCE_T0 ,AVERAGE_VSPEED_SINCE_TO , VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, PITCH, ROLL, YAW, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4 
*      #define ACCZ_SOURCE TEST_3                 // selectionné une valeur parmi: TEST_1, TEST_2, TEST_3, GLIDER_RATIO , SECONDS_SINCE_T0 ,AVERAGE_VSPEED_SINCE_TO , VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, PITCH, ROLL, YAW, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4 
*      #define T1_SOURCE GLIDER_RATIO             // selectionné une valeur parmi: TEST_1, TEST_2, TEST_3, GLIDER_RATIO , SECONDS_SINCE_T0 ,AVERAGE_VSPEED_SINCE_TO , SENSITIVITY , PPM, VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4 
*      #define T2_SOURCE SENSITIVITY              // selectionné une valeur parmi: TEST_1, TEST_2, TEST_3, GLIDER_RATIO , SECONDS_SINCE_T0 ,AVERAGE_VSPEED_SINCE_TO , SENSITIVITY , PPM, VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
**** 2.2 Hott protocol ************************************************************************************ 
*      les meusure transmise automatiquement sont : 
*         - Altitude relative(cm), Vitesse vertical principale(cm/sec) (si au minimum un capteur barométrique est connecté)
*         - Tensions des cellules (si NUMBER_OF_CELLS > 0)
*         - jusqu'à trois mesure de tension de battery (le setupe specifie quelle tension doive être mesureret transmise - Exemple VOLT_1,VOLT_2, ...)  
*         - Courant (quand un capteur de courant est connecté)
*         - GPS (long, lat, vitesse, altitude , course) (quand un GPS est connecté)
*         - RPM (hz) (quand un capteur de vitesse de rotation est connecté)
*      Lorsque vous mesurez des tensions de bateries, une alarme peux etre regler pour indiqué les tensions inférieur a un niveau reglé.   
*      Pour activé cette alarme, décommenter la ligne ci-dessous (en milliVolt) pour regler le seuille d'alarme
*         //#define CELL_UNDERVOLTAGE_WARNING 3300            // Attention c'est en mV;
*         
*      Vous pouvez aussi selectioné comment les valeurs temperature1 et temperature2 seront remplie 
*      note : ces deux champs peuvent selement retourner des valeurs allant de -20 à 235; pour le mode PPM une valeurs de -100 sera affiché 0 et +100 sera affiché 200      
*      Mettre les lignes ci-dessous en commentaire quand aucune mesures doivent être transmise par télémetry      
*     ex:  
*       #define BATTERY_1_SOURCE          VOLT_4                // selectionné une valeur parmi: VOLT_1, VOLT_2, VOLT_3 , VOLT_4, VOLT_5 , VOLT_6
*       #define BATTERY_2_SOURCE          VOLT_2                // selectionné une valeur parmi: VOLT_1, VOLT_2, VOLT_3 , VOLT_4, VOLT_5 , VOLT_6
*       #define MAIN_BATTERY_SOURCE       VOLT_5                // selectionné une valeur parmi: VOLT_1, VOLT_2, VOLT_3 , VOLT_4, VOLT_5 , VOLT_6
*       #define TEMPERATURE_1_SOURCE      TEST_1          		// selectionné une valeur parmi: TEST_1, TEST_2, TEST_3 , GLIDER_RATIO , SENSITIVITY , PPM
*       #define TEMPERATURE_2_SOURCE      SENSITIVITY          	// selectionné une valeur parmi: TEST_1, TEST_2, TEST_3 , GLIDER_RATIO , SENSITIVITY , PPM
**** 2.3 Multiplex  ************************************************************************************ 
*      Pour le protocole Multiplex, vous avez a définire quelle mesures de OXS sont transmise sur quelle ligne de l'emeteur.
*      Vous devez aussi spécifié les echelles qui doivent être appliqué par OXS et le range de valeur afin de definir le on/off des alarmes.
*      Vous DEVEZ specifie une ligne pour chaque mesure a envoyer a l'emmeteur. Chaque ligne doit contenir :
*        -  1 : le numero de ligne de l'ecran lde l'emmeteur ou doit etre afficher la mesure. Les valeur autorisées vont de 2 à 15, attention de ne pas utilisé deux foix la même valeur.
*        -  2 : une virgule
*        -  3 : les nom du parameter OXS qui doit être transmit sur cette ligne (exemple "VOLT_1")  (voir note (2))
*        -  4 : une virgule
*        -  5 : un coeficient multiplicateur a être appliqué sur la valeur transmise (mettre "1" pour conserver la mesure original, 10 pour multiplier la messure par 10 , ...) (voir note (3))
*        -  6 : une virgule
*        -  7 : un coeficient diviseur à être appliqué sur la valeur transmise (mettre "1" pour conserver la mesure original, 10 pour multiplier la messure par 10 , ...)  (voir note (3))
*        -  8 : une virgule
*        -  9 : offset appliquer sur la valeur apres avoir appliquer le coeficient multiplicateur et diviseur ((mettre "0" pour conserver la mesure d'origine, "100" pour ajouter 100, ...)
*        - 10 : une virgule
*        - 11 : une alarme pour la valeur minimum (voir note (4))
*        - 12 : une virgule
*        - 13 : une valeur pour la valeur haute (voir note (4))
*        - 14 : une virgule et une "\" si il y a une mesure apres celle ci (,\ a ne pas mettre pour la dernière mesure);
*                  /!\ ATTENTION que "\" doit être le dernier character de la ligne (aucun espace et tolleré après)
*                  /!\ ATTENTION que aucune ligne de commentaire ("*...") soit glisser entre les differentes lignes
* Note (2) : Une mesure OXS (exemple VOLT_1) ne peux pas etre déclarer dans plusieur limits.
* Note (3) : Les coeficient multiplicateur, diviseur et l'offset doit etre une valeur entière (pas de chiffre à virgule); ils peuvent être négatif (exemple "-100").
*            Les coeificien multiplicateur et diviseur peuvent etre utile dans las cas suivant:
*              - pour convertire d'un unité de mesure a une autre (meter <> pied)
*              - pour convertir en pourcentage (example multiplier par 100 et diviser par 4000 afin d'optenir un % de consomation pour un accu de 4000 mAmph)
*              - pour ajusté le nombre de digit afficher sur l'emmeteur.
*              - pour avoir par exemple une valeur de quantité de fuel commencent a 100 (en pourcent) et diminuent a 0 quand la comsomation augmente (dans ce cas vous devez utiliser un coeficient mutliplicateur négatif et un offset de 100%).
*            Multipicateur/diviseur/offset doivent être defini mais ne sont pas toujours utilisé par OXS. C'est le cas quand:
*              - CELLS_1_2, CELLS_3_4, CELLS_5_6 sont transmit (car ces champ requiere un formatage spéciaux requi par l'émmeteur)
* Note (4) : les limite d'alarme doivent etre des nombres entiés (pas de chiffre à virgule); ils peuvent être négatif (exemple "-100"). 
*                Afin de definir les alarmes demander dans le protocole Multiplex, OXS vas proceder de la facon suivante:
*                 - adapté le nombre de decimal de la mesure faite par OXS (la plupart des mesures de OXS ont plus de décimal que prévur par le protocol Multiplex)
*                 - appliqué les coeificents multiplicateur, diviseur et l'offset
*                 - comparé les résultats avec les niveaux haut et bas des alarmes
*               LEs limite doive etre fixé en tennant compte tous le digit visible a l'ecrant.
*                  Exemple: Les temsion sont afficher mVolt. Si vous voulez un alarme quand la tension est inferieur à 11.2 Volt, regler la valeur d'alarme minimum a 112.
*              afin d'inibé les alarme mettre la valeur minimum à -16384 et/ou la valeur maximal a 16383 (valeur minimal et maximal pour le protocole de type Multiplex sur 15 bits)
*   
* ci-dessous un exemple de reglage pour transmetre sur le protocole Multiplex
*     - sur la ligne 3, l'altitude relative mesurer (sans alarme)
*     - sur la ligne 6, la vitesse vertical measuer (avec une alarme si la valeur excede 50m/s)
*            #define SETUP_MULTIPLEX_DATA_TO_SEND    \
*                       3 , REL_ALTIMETER , 1 , 1 , 0 , -16384 , 16383,\
*                       6 , VERTICAL_SPEED , 1 , 1 , -500 , 500
*  /!\ IMPORTANT : toujours conservé la ligne "#define SETUP_DATA_TO_SEND    \"  ; ne pas inserer de ligne avant ou apres les lignes utiliser pour le setup.
*   
* Ici vous trouverez la liste des mesure que peux envoyer OXS.
*    Code a utilisez        Unité       Signification 
*    ALTIMETER           	m      		Absolute altitude (from first baro sensor)
*    REL_ALTIMETER       	m      		Relative altitude (from first baro sensor)  
*    ALTIMETER_MAX       	m      		Max relative altitude
*    VERTICAL_SPEED     	cm/s    	Main vertical speed (as set up in VSPEED_SOURCE)
*    SENSITIVITY        	none    	Vario sensitivity 
*    VOLT_1            		0.1V(5)     Value read on first PIN_VOLTAGE     
*    VOLT_2            		0.1V(5)     Value read on 2d PIN_VOLTAGE        
*    VOLT_3            		0.1V(5)     Value read on 3d PIN_VOLTAGE        
*    VOLT_4            		0.1V(5)     Value read on 4th PIN_VOLTAGE       
*    VOLT_5            		0.1V(5)     Value read on 5th PIN_VOLTAGE       
*    VOLT_6            		0.1V(5)     Value read on 6th PIN_VOLTAGE       
*    CURRENTMA         		0.1A        MilliAmp measured by current sensor  
*    MILLIAH           		mAh        	Consumed milli Amp heure             
*    RPM              		Hz         	Rotation per min                     
*    AIR_SPEED        		0.1 km/h   	Air speed   
*    CELL_1             	0.1V(5)    	Value based on first PIN_VOLTAGE         
*    CELL_2             	0.1V(5)    	Value based on first and 2d PIN_VOLTAGE  
*    CELL_3             	0.1V(5)    	Value based on 2d and 3d PIN_VOLTAGE     
*    CELL_4             	0.1V(5)    	Value based on 3d and 4th PIN_VOLTAGE    
*    CELL_5             	0.1V(5)    	Value based on 4th and 5th PIN_VOLTAGE   
*    CELL_6             	0.1V(5)    	Value based on 5th and 6th PIN_VOLTAGE   
*    CELL_MIN           	0.1V(5)    	Value based on CELL_1 ... CELL_6         
*    CELL_TOT           	0.1V(5)    	Value based on VOLT1...VOLT6             
*    PPM                         		Value set by Tx to control some functions (sensitivity, ...) (range is normally -100 / +100)
*    GPS_COURSE        		0.1 deg   	Orientation of plane       
*    GPS_SPEED         		0.1 km/h  	Ground speed (2D or 3D)       
*    GPS_ALTITUDE      		m         	Absolute Altitude       
*    GPS_DISTANCE      		m         	Distance from home
*    GPS_BEARING       		0.1 deg   	Direction from home  
*    TEST_1                      		Value used for test
*    TEST_2                      		Value used for test
*    TEST_3                      		Value used for test
*   (5) Les unité dependes des parametres utiliser (exemple quand une tension est fournit par un capteur de temperature, l'unité peux etre en degrées)
*       Quand mesuré des cellule d'accu, vous devez vous sassuré que la calibration doit etre en milliVoltc(OXS vas covertir les mVolt en 0.1Volt quand c'est transmi en protocole Multiplex).
***** 2.4 - données Jeti ************************************************************************************************************************
*     Les mesure transmise automatiquement sont : 
*         - Altitude relative(cm), Vitesse vertical principale(cm/sec) (si au minimum un capteur barométrique est connecté) et l'altitude maximum
*         - Tensions des cellules (si NUMBER_OF_CELLS > 0)
*         - Courant (quand un capteur de courant est connecté)
*         - GPS (long, lat, vitesse, altitude , course) (quand un GPS est connecté)
*      A ceci vous pouvez aussi specifie l'envoie de:
*         - une des six tension (si la tension est mesurer); quand un capteur NTS est utilise pour mesurer une température, il faux definir aussi un VOLTAGE_SOURCE (voir la section a propot des capteurs NTC)
*         - une température: vous pouver sélèctioner la température fourni par le MS5611 ou une sonde NTC.
*      d'autre champs peuve etre ajouté a la demande plus tard.
//#define VOLTAGE_SOURCE   VOLT_1                 // selectionné une valeur parmi: VOLT_1, VOLT_2, VOLT_3 , VOLT_4, VOLT_5 , VOLT_6
//#define TEMPERATURE_SOURCE MS5611               // selectionné une valeur parmi: MS5611 and NTC
****** 2.5 - comment renseigné TEST_1, TEST_2, TEST_3  ******    
*   OXS peux calculer plus de mesure que prévu par les protocole.
*   Quelque une d'entre elle peuve etre directement configurer voir dans les section 2.1 à 2.4 (exemple VOLT_3 peux être transmit dans T1 (pour le protocol SPORT)
*   Certaine autre sont raremenr utiliser et requiere de procédé en deux étapes;
*   - Première étape, décidé quelle mesure vonts remplir les champs TEST_1, TEST_2, TEST_3 (dans la section 2.5)
*   - Second étape, decidé dans quelle parametre de télémetry ces mesure seront envoyer (dans la section 2.1 to 2.4)
*   Pour choisir cooment ces paramètre TEST_1, TEST_2, TEST_3 doivent être remplie décomenter un ou plusieur ligne ci-dessous.
*   Si vous décommenter plusieur ligne, prenez garde a ne pas utilisé plusieur fois le même parametre.
**************************************************************************************************************************   
//#define FILL_TEST_3_WITH_EXPECTED_ALT                        	// décommenter cette ligne si OXS doit calculer une altitude souhaité basé sur l'altitude actuel, Vitesse vertical et acceleration verticale
//#define EXPECTED_ALT_AT_SEC 0.2                               // temps (en sec) pour l'altitude souhaité (Cette ligne doit etre décommenter si la précedente est décommenter)
//#define FILL_TEST_1_2_3_WITH_LINEAR_ACC                     	// décommenter cette ligne si OXS doit replire les parametres TEST_1, TEST_2, TEST_3 avec l'acceleration lineaire
//#define FILL_TEST_1_2_WITH_VSPEED_AND_ALT_FROM_SECOND_VARIO  	// décommenter pour activé cette option
//#define FILL_TEST_1_WITH_DTE                                 	// décommenter pour activé cette option
//#define FILL_TEST_2_WITH_PPM_AIRSPEED_COMPENSATION           	// décommenter pour activé cette option
//#define FILL_TEST_1_WITH_YAWRATE                             	// décommenter pour activé cette option
//#define FILL_TEST1_WITH_HEADING_FROM_MAGNETOMETER             // décommenter pour activé cette option
//#define FILL_TEST_3_WITH_FLOW_SENSOR_CONSUMPTION             	// décommenter pour activé cette option
************************************************************************************************************************



**** 3 - PPM settings **************************************************************************************************
*   OXS peux (option) changer certain parameter et/ou rester certain donner a partir d'action sur l'émmeteur
*   Pour tous les protocole, cela peux etre réaliser a partir du sortie servo du recepteur pour recevoir les informations de l'émetteur.
*   Cette fonction permet depuis l’émetteur:
*      - de changer la sensibilité du vario en utilisant ou un potard ou un interrupteur de votre émetteur.
*      - de changer entre un vario compenser et non compenser (quand on utilisant 2 capteurs barométrique avec un connecté sur une sonde TEK)
*      - de réinitialiser le capteur de vitesse (quand la valeur décale avec la température)
*      - de changer le facteur de compensation du vario (quand le vario compensé utilise le capteur de vitesse) en utilisant un potentiomètre ou un interrupteur de l’émetteur.
*   Ces différentes fonctions demandent des mixages dans l’émetteur et certaine configuration dans le fichier OXS-config.h (voir ci-dessous).
*
*   Si vous voulez ces fonctions vous devez;
*      - vous devez décommenter (en suppriment les // devant la ligne) les 3 lignes suivantes
*      - Spécifié à la ligne PIN_PPM, la PIN Arduino qui sera connecté à la sortie servo du récepteur
*            Par défaut: 2 ; vous pouvez aussi connecter la PIN 3 pour lire le signal PPM.
*            Fait bien attention d’utiliser la pine sélectionné (2 ou 3) uniquement pour le signal PPM.
*            Garder cette ligne commentée si vous ne voulez pas utilisé cette fonction.
*      - spécifié a la ligne PPM_MIN_100 la largeur d’impulsion (en micro seconde) quand la voie envoie la valeur “logique” = -100.
*              La valeur par défaut est 988.
*      - spécifié a la ligne PPM_PLUS_100 la largeur d’impulsion (en micro seconde) quand la voie envoie la valeur “logique” = +100.
*              La valeur par défaut est 2012.
*   Les deux dernières valeurs sont utilisées afin de calibrer OXS avec l’émetteur quand la fonction PPM est utilisée
*   Il est probable que si vous utilisez les valeurs par défaut cela fonctionne.
*   Il est plus sûr pour le bon fonctionnement de calibrer ces paramètres (uniquement si vous utilisé la fonction PPM).
*   /!\ TRADUCTION A REVOIR :To do so, ensure that you transmit the OXS measurement field "PPM" to the TX (e.g. as T1).
*   /!\ TRADUCTION A REVOIR :Note the values on TX (e.g. in field T1) when the TX send respectively -100 and +100 on the PPM channel (use a switch or a pot).
*   /!\ TRADUCTION A REVOIR :Fill in those values in those parameters and reload your program in the Arduino.
*
*   Si vous utilisé le protocol FRSKY SPORT (uniquement pour récepteur X) avec openTX 2.2(ou dessus) vous pouvez envoyer le signale PPM sans avoir a connecté une sortie servo du récèpteur.
*   OpenTx 2.2 vous permet de lancé un script LUA qui vous permet d'envoyer des valeur depuis votre émeteur vers OXS
*   Pour activé cette option vous devez:
*     - décommenter la ligne #define PPM_VIA_SPORT
*     - lancer le script LUA qui vous perment d'envoyer via le protocol SPORT le même type de valeur qu une voie due recepteur. (cette valeur est comprise entre -100 and 100)
*       Ce script LUA doit utilisé une commande comme ceci: 
*            local ret = sportTelemetryPush( 0x0D , 0x10 , 0x0010 , -52 )
*       ou    
*          - 0xOD : est le numero d'identification utilisé (defini dans les fichier OXS_config_advanced.h)
*          - 0x10 : est une valuer non utilisé
*          - 0x0010 : est le code utiliser pour identifier qui s'agit d'une valeur PPM
*          - -52 : est la valeur PPM transmise (-52 est juste un exemple; elle doit etre adapter selon la valeur que on veux transmetre) 
*      vous pouvez trouvé un exemple de script LUA dans ce package (dans le dossier "lua scripts"). le nom du fichié est Name is OXSppm.lua et doit etre instaler dans le répertoire SCRIPTS/FUNCTIONS de la carte SD.
************************************************************************************************************************
#define PIN_PPM             2 
#define PPM_MIN_100       988   
#define PPM_PLUS_100      2012   
#define PPM_VIA_SPORT             // décommenter cette ligne pour recevoir un signal PPM par le protocol SPORT a la place d'une sortie servo du récepteur (Cela nécésite un émeteur equiper d'openTX et contenant le script en LUA)
************************************************************************************************************************



**** 4 - Reglage du vario ************************************************************************************************
*
* 4.1 - Connecting 1 or 2 barometric sensor(s) (optional feature) *********************************************
*     OXS peux etre connecter avec different capteur barometrique.
*     les meilleur resultat sont obtenu avec un capteur barometrique MS5611. Ce type de capteur est generalement monter sur une platine (comme la GY-63 ou GY-86) avec un régulateur de tension de 3.3 volt et un bus I2C.
*     Il est possible d’utiliser des platines avec des capteurs BMP085 ou BMP180 ou BMP280 (qui sont plus économiques mais mois précis). Il est à noter que les capteurs BMP085, BMP180 ou BMP280 ne peuvent être utilisé pour le premier capteur.
 Il est possible de connecter 1 ou 2 capteur barométrique ; le premier peux être un capteur de type BMP085 ou BMP180 ou BMP280 ou encore un MS5611; le second capteur doit etre un MS5611 (car les capteurs de type BMPxxx ont qu’une adresse I2C).
*         Note : Toujours utilisé un capteur qui possède un régulateur de tension 3,3Volt et un bus I2C quand on utilise le 5 Volt Vcc de l’Arduino.
*     Chaque capteur peut calculer l’altitude absolue ou relative (en mètre avec 1 décimal) et la vitesse verticale (en mètre/seconde avec 2 décimal)
*     Un second capteur peur être utile associé à une sonde TEK et si l’option PPM est active, il est possible de sélectionner depuis l’émetteur le capteur qui vas générer le sont du vario.
*     Ceci permet de basculer entre un vario compensé pneumatique ou un vario non compensé.
*     Quand on utilise deux capteur barométrique, OXS peux transmettre une valeur moyen de vitesse vertical des deux capteurs. Ceci ne semble pas très utile.
*     Vous devez specifier quelle type de capteur est utiliser pour le premier vario avec la ligne #define FIRST_BARO_SENSOR_USE     MS5611         
*        La valeur a renseigner doit etre une valeur de la liste suivante: NO_BARO , MS5611, GY86 , BMP085 , BMP180 , BMP280  
*     Vous devez spécifier quelle type de capteur est utiliser pour le second vario avec la ligne #define SECOND_BARO_SENSOR_USE     NO_BARO         
*        La valeur a renseigner doit etre une valeur de la liste suivante: NO_BARO , MS5611  
*     Les deux capteurs sont connecté en parallèle (utilise les pines A4 et A5 Arduino).
*     Le premier capteur (MS5611 ou BMPxxx) répond sur l’adresse 0x77 du bus I2C.
*			/!\ ATTENTION ne pas oublier de connecté la PIN SDO à VDD sur le BMP280 pour obtenir l'adresse 0x77
*     Si il est utilisé, le second capteur (un MS5611) doit avoir la PIN CE connecter à VDD. Comme cela l’adresse I2C sera 0x76. (Note: par défaut la pine CE du MS5611 doit être connecter a GND ainsi son adresse I2C est 0x77)
*     Note : Quand un seul capteur est connecté il doit être connecté comme premier capteur.
***********************************************************************************************************************
* 4.2 - Type de vitesse verticale transmise  *****                                    
*     Selon vos option d'implementation, OXS est capable de caluler different type de vitesse vertical et choisier la quelle doit etre transmise.
*     Selon le contenu de la ligne #define VSPEED_SOURCE, la vitesse verticale ser basé sur 
*      - premier capteur barométrique (= cas normal)
*      - deuxième capteur barométrique
*      - moyenne des deux capteur barométrique
*      - premier capteur barométrique et compensation par le tube de place (= dtE)
*      - premier capteur barométrique + IMU (accelerometer + gyroscope)
*      - premier ou second capteur barometrique selectioner par le signal PPM
***********************************************************************************************************************     
#define VSPEED_SOURCE  FIRST_BARO     // select between FIRST_BARO, SECOND_BARO , AVERAGE_FIRST_SECOND, AIRSPEED_COMPENSATED , BARO_AND_IMU or PPM_SELECTION
***********************************************************************************************************************
* 4.3 - Définition de la sensibilité par programmation *****************************
*     Quand un vario est utilisé, OXS peux prendre certain paramètres en compte pour ajuster la sensibilité 
*     La sensibilité peux être régler entre les valeurs:
*         - 20 (réglage conservatif, temps de réaction = plusieurs second)                   
*         - 150 (rapide mais beaucoup d’erreur, temps de réaction = bien inferieur a la second)       
*        50 est une valeur de réglage pour mesurer de petite vitesse vertical (recherché d’ascendance en planeur); ce réglage donne une réaction autour de la seconde.
*     la sensibilité peux être régler par programmation et/ou par l’émetteur.
*     
*     Si vous voulez une réponse très rapide du vario la meilleur solution est d’utilisé un capteur MPU6050 (accéléromètre/gyroscope) en complément du capteur barométrique (voir ci-dessous).
*     Note: Quand le vario est utilisé (#define VARIO décommenter), vous avez a spécifié (dans la section 9) comment la vitesse vertical est calculer.
*
*     Le réglage de la sensibilité utilise 4 paramètres:
*        SENSIVITY_MIN = Cette sensibilité est la sensibilité basic du vario.
*                        Ce paramètres est donc le paramètres principale du control de sensibilité du vario.
*                        Cette valeur est utilisé par OXS quand la différence entre la valeur de la Vitesse vertical actuel (calculer toute les 20 ms) et la précédente valeur de la vitesse vertical est inferieur a la valeur définit par SENSITIVITY_MIN_AT ; la valeur typique est 40.
*        SENSIVITY_MAX = c’est la sensibilité maximal utiliser quand la vitesse vertical change très rapidement. 
*                        Cette valeur est utilisé par OXS quand la différence entre la valeur de la Vitesse vertical actuel et la précédente valeur de la vitesse vertical est supérieur a la valeur définit par SENSITIVITY_MAX_AT ; la valeur typique est 300.
*        SENSITIVITY_MIN_AT = définie jusqu’à quelle valeur de SENSITIVITY_MIN est applicable (voir ci-dessus) (cm/s); la valeur typique est 100 (cm/s).
*        SENSITIVITY_MAX_AT = définie jusqu’à quelle valeur de SENSITIVITY_MAX est applicable (voir ci-dessus) (cm/s); la valeur typique est 1000 (cm/s).
*           La sensibilité du vario est une interpolation automatique entre SENSITIVITY_MIN et SENSITIVITY_MAX.
*     Ces paramètres permettent à OXS d’avoir un temps de réaction rapide quand la vitesse verticale change rapidement et d’avoir un vario silencieux dans des condition calme.
*     Note: SENSITIVITY_MAX peut-être égale au paramètre SENSITIVITY_MIN si aucun adaptation automatique est désiré en cas de changement rapide. 
************************************************************************************************************************
#define SENSITIVITY_MIN 50
#define SENSITIVITY_MAX 300
#define SENSITIVITY_MIN_AT 100
#define SENSITIVITY_MAX_AT 1000
************************************************************************************************************************
* 4.4 - Sensitivity adjusted from the TX *******************************************************************************
*     la sensibilité peux être ajusté depuis l’émetteur grâce a un interrupteur ou un potentiomètre et quelque mixage sur un sorti servo du récepteur.    
*     Pour utilizer cette option, une sortie servo doit etre connecter à OXS (voir « Définition du retour émetteur » dans la section 3)
*     Quand OXS reçoit un signal de l’émetteur, la valeur SENSIVITY_MIN définie dans la section 4.3 est supprimer et remplacer par la valeur calculer basée sur la valeur défini dans cette section et le signal PPM de l’émetteur.
*
*     Le réglage utilise 4 parametres:
*         SENSITIVITY_MIN_AT_PPM = quand l’émetteur envoi cette valeur sur la sortie PPM, le paramètre SENSITIVITY_MIN (voir section 4.2) vas être remplacé par la valeur du paramètre SENSITIVITY_PPM_MIN;  Une valeur typique peux être 10.
*         SENSITIVITY_MAX_AT_PPM = quand l’émetteur envoi cette valeur sur la sortie PPM, le paramètre SENSITIVITY_MIN (voir section 4.2) vas être remplacé par la valeur du paramètre SENSITIVITY_PPM_MAX;  Une valeur typique peux être 40.
*              Le paramètre SENSIVITY_PPM_MIN+MAX définie la plage dans laquelle vous aller pouvoir ajuster la sensibilité:
*         SENSITIVITY_PPM_MIN     =  valeur minimume pour le parametre SENSITIVITY_MIN qui peu lui être asigné par PPM; Une valeur typique peux être 20.
*         SENSITIVITY_PPM_MAX     =  valeur maximum pour le parameter SENSITIVITY_MIN qui peu lui être asigné par PPM; Une valeur typique peux être 100.
*             SENSITIVITY_MIN iest automatiquement interpoler entre le parametre SENSITIVITY_PPM_MIN et le parametre SENSITIVITY_PPM_MAX selon le signal PPM recu.
*     la sensibiliter ser changer seleument si la valeur du signal PMM est comprise entre SENSITIVITY_MIN_AT_PPM - 5 et SENSITIVITY_MAX_AT_PPM + 5.
*     /!\ note importante: la sensibilier est changer baser sur la valeur absolue du signal PPM; donc PPM = -25 a le même effet que PPM = 25.
*     Ceci est important pour combiner le chamgement de semcibiliter et une selection du capteur barometrique 1 et 2 (ceci permet de passer d'un vario non compensé a un vario compensé.
************************************************************************************************************************
#define SENSITIVITY_MIN_AT_PPM 10
#define SENSITIVITY_MAX_AT_PPM 40
#define SENSITIVITY_PPM_MIN  20
#define SENSITIVITY_PPM_MAX 100
************************************************************************************************************************
* 4.5 - parametre hysteresys et compensation de l'altitude baser sur un capteur de temperature ***************************************************
*     OXS peux appliquer un hysteresis sur les changement de vitesse vertical.
*     Ceci permet d'envoyer tros souvent un changement de vitesse vertical (ce qui peu en decouler de fausse alerte sonor). C'est un autre moyen de reduire la sensibilité.
*     VARIOHYSTERESIS veux dire que la vitesse vertical change si la vitesse vertical mesurer (après filtrage) differe the la valeur précédament transmise the plus de la valeur de ce paramètre.
*     Une valeur typique peux être 5  (= 5 cm/s); 0 veux dire pas de correction hysteresis.
*
*     OXS permet aussi d'avoir une compensation depuis le capteur MS5611 (pas depuis les capteur BMPxxx) afin de compenser la dirive de l'altitude due à l'augmentation du capteur en lui même apres la mise en route.
*     La températue interne du composant MS5611 augemente de plusieur degrées lors de ca lecture par l'Arduino.
*     Dans le principe de fonctionelent le capteur MS5611 inclus des paramètre de compensation pour palié à l'augmentation de ca temperature interne.
*     Pourtant il semble que ces parametres de correction ne soit pas optimum.
*     OXS permet d'ajouter une compensation suplementaire pour palier à ce problème. Pour cela vous devez remplir le paramètre ALT_TEMP_COMPENSATION.
*     Afin de definir la meilleur valeur pour votre capteur(cela peux varier d'un MS5611 à un autre MS5611), la facon la plus facil est de tester plusieur valeur.
*     Les informations suivante peuve vous être utile:
*        - Il est preferable de faire les essais dans des conditions atmospherique calme (journé calme sans vent)
*        - Assurer vous que l'altitude est afficher sur votre emmeteur
*        - Assurez vous que le capteur MS5611 est protegé de la lumiere (le capteur est photosensible)
*        - Premierement avec le parametre ALT_TEMP_COMPENSATION mis à zero (ou commenté la ligne), demaré votre émetteur et votre récepteur. Ne bouger pas votre OXS (l'altitude afficher sur votre émetteur doit etre 0).
*        - Apès 5 minutes noter la difference d'altitude.
*        - Eteigner votre récepteur et votre emmeteur pour 5 minute minimum afin de faire redecendre la temperature interne du capteur.
*        - Repeter cette operation une ou deux foix.
*        - Si la deviation d'altitude est environ la meme entre chaque essais et dépasse les 1 metre. il sera possible de reduire cette derive avec une valeur dans le paramètre ALT_TEMP_COMPENSATION
*        - Si la dervie est positive, alors augemter le parametre ALT_TEMP_COMPENSATION autrement reduisé le; ALT_TEMP_COMPENSATION peux être négatif (cela peux etre le cas si la derive est négative).
*        - Il est très compliquié de calculer la valeur de correction. Sur mon capteur j'ai une valeur = 1000 pour compenser une variation de 3 mètre mais cela peux varier sur votre capteur.
*        - Mettre une valeur au peremètre ALT_TEMP_COMPENSATION, repeter les testes précédent (allumer votre émeteur et votre récepteur, attendre 5 min, noter la dervie d'altitude) et augmenter ou diminuer la valeur si besoin.
*    Note importante: donner une valeur au paramètre ALT_TEMP_COMPENSATION ne vas pas élimine totalement la derive d'altitude car:
*         - cela peux etre le résultat d'une variation de pression atmospherique (ceci ne peu pas etre compensé)
*         - la dervie peux venir d'autre facteurs 
*         - la compenstion n'est pas lineaire sur toute les valeur de pression et de température
************************************************************************************************************************
#define VARIOHYSTERESIS 5
************************************************************************************************************************
* 4.6 - changement entre different type de calcul de vitesse vertical (optionel) **************************************************************
*     Quand vous utiliser deuc capteur barometrique ou un capteur barometrique et un capteur de vitesse pitot (4525D0 - voir section 5) ou un cpateur barometrique et un IMU, OXS peux calculer different vitesse vertical (ou dTE).
*     Quand l'option PMM est activé, OXS autorise quelle calcul (vitesse vertical du premier ou second capteur ou compensé par le pitot, etc...) doit être envoyer pour le vario.
*     Ceci permet de changer entre vario compenser et non compenser.
*     Bien que OXS puisse calculer jusqu'à type 5 vitesse vertical ( FIRST_BARO, SECOND_BARO , AVERAGE_FIRST_SECOND, AIRSPEED_COMPENSATED , BARO_AND_IMU ), il est uniquement possible de changer entre 2 vitesse vertical.
*     Pour activé cette option, plusieur parametre sont nécésaire,
*       1) Specifier quelle calcul de vitesse vertical sera le primaire et lequel sera le secondraire en utilisant les ligne ci-dessous:
*                  #define VARIO_PRIMARY       XXXXXXX  
*                  #define VARIO_SECONDARY     YYYYYYY
*                 Ou XXXXXX et YYYYYY est un valeur selectionné dans la liste suivante: FIRST_BARO, SECOND_BARO , AVERAGE_FIRST_SECOND, AIRSPEED_COMPENSATED , BARO_AND_IMU 
*       2) Specifé une plage de valeur PPM que OXS doit verifié pour envoyer la vitesse vertical primaire ou secondair en utilisant les lignes ci-dessous.
*                 #define SWITCH_VARIO_MIN_AT_PPM 10 
*                 #define SWITCH_VARIO_MAX_AT_PPM 90 
*             Quans la valeur absolue entre le parametre SWITCH_VARIO_MIN_AT_PPM (valeur typique = 10) et le parametre SWITCH_VARIO_MAX_AT_PPM (valeur typique = 90),
*              - OXS vas switcher sur la vitesser vertical primaire si le resultat est positif
*              - OXS vas switcher sur la vitesser vertical secondaire si le resultat est négatif
*            Note: Quqnd la valeur absolue the PPM est hors de la plage de valeur, OXS ne vas pas changer de type de calcul de vitesse vertical et rester avec la valeur actuelle.
*                  Ce principe permet d'utilisé un interupteur en même temps qu'un potentiometre pour controlé la sensibilité.
*                  Le passage de positif a négatife sur openTx peu être réaliser a l'aide d'un mixage "MULTIPLY by -100%".
*                  l'envoie d'une vlauer PPM au de cette plage de valeur permet une autre fonction (exemple le reste de offset de la vitesse aire) sans changer le calcul de la vitesse vertical.
*       3) sepecifié dans la section 4.2 que la selection de la source de vitesse vertical est "PPM_SELECTION"             
****************************************************************************************************************************
#define VARIO_PRIMARY       AIRSPEED_COMPENSATED  // selectionné une valeur parmi: FIRST_BARO, SECOND_BARO , AVERAGE_FIRST_SECOND, AIRSPEED_COMPENSATED , BARO_AND_IMU
#define VARIO_SECONDARY     FIRST_BARO            // selectionné une valeur parmi: FIRST_BARO, SECOND_BARO , AVERAGE_FIRST_SECOND, AIRSPEED_COMPENSATED , BARO_AND_IMU
#define SWITCH_VARIO_MIN_AT_PPM 10
#define SWITCH_VARIO_MAX_AT_PPM 90
*************************************************************************************************************************
* 4.7 - vitesse vertical analogique (optional) ***********************************************************************************
*     OXS peux aussi donné la vitesse vertical (uniquement du depuis le capteur 1) sous forme de signal analogique uniquement sur les sortie A1 ou A2 sur le recpteur FrSky:
*     Ceci peu etre util si votre recepteur ne possède pas de communication digital (ou si celle ci est déjà utilisé par un autre capteur)
*     Des composent suplementaire (1 resitance et 1 condensateur) sont nécésaire! Lire le Wiki si vous voulez utilisé cette option.
*     Pour activé cette option:
*     - décommenter les trois prametres suivant
*     - definir dans le peramètre PIN_ANALOG_VSPEED la sortie Arduino DIGITAL qui vas être utilisé (voir ci-dessous)
*         Par defaut 3; les PIN utilisé ne peuvent etre que les PIN 3 et 11 car la fonction Arduino timer 2 doit être utilisé. (la fonction timer 2 bloc certaine PIN et force a utilisé d'autre)
*     - definir le limite min et max de la vitesse vertical (en meter/sec)
*        - ANALOG_VSPEED_MIN Valeur inferieur ou égale vas envoyer 0Volt au recepteur
*        - ANALOG_VSPEED_MAX Valeur supérieur ou égale vas envoyer 3.2Volt au recepteur
*************************************************************************************************************************
#define PIN_ANALOG_VSPEED   3
#define ANALOG_VSPEED_MIN -3
#define ANALOG_VSPEED_MAX  3
*************************************************************************************************************************
* 4.8  - Calcul des finesse du planeur, calcul moyen des tôt de monter/descente, calcul moyen de l’altitude gagner/perdu ***************************************************************
*     OXS peux calculer et envoyer le calcul moyen des tôt de monté/descente quand la vitesse air et la vitesse vertical rest stable dans une certaine tolerence plus de X seconde (exemple entre 5 à 10 sec)
*     Les valeur calculé sont:
*        - temps écoulé pour calculé les finesse du planeur et le calcul moyen des tôt de monté/descente.
*        - moyen des tôt de monté/descente  ( = difference d'altitude / temps écoulé )
*        - finesse planeur (= distance / difference d'altitude) (en faite = vitesse aire * temps écoulé / difference d'altitude )
*     le calcul de la finesse planeur est un parametre qui permet d'affiner les réglages du planeur. Cela a de sens que si la vitesse est aproximativement regulière.
*     OXS effectue ces calcul uniquement quand:
*         - la vitesse air ne change pas d'un certain pourcentage(compré a la vitesse au début du temps écoulé). Ce pourcentage peux être défini par l'utilisateur dans le paramètre SPEED_TOLERANCE.
*         - la vitesse vertical reste dans un certain pourcentage defini par l'utilisateur dans les parameters VSPEED_MIN_TOLERANCE et VSPEED_MAX_TOLERANCE
*     Toutes les 0.5sec, OXS véréfi que les mesure instantané reste dans les tolérences défini. 
*     Si les mesures, effectué par OXS, sont hors des tolérence defini, il arrete le calcul en cour, et reprent avec les nouvelles valeur mesuré. PErformance planeur et moyenne des tôt de monté/descente sont remise a 0.
*     Si OXS calcul une valeur dans le toleranc defini par l'utilisateur dans le temps écoulé (defini par le paramètre GLIDER_RATIO_CALCULATED_AFTER_X_SEC), il procèdera au calcul
*     Note: Dans cette version de OXS, si vous voulez envoyé les parameters calculé, vous devez definir d'envoyer les paramètres de la facon suivante:
*         - TEST1 pour le temps écoulé (en 1/10 de sec) 
*         - TEST2 pour la vitesse vertical moyen (en cm/sec comme al vitesse vertical)
*         - TEST3 pour la finesse du planeur (en 1/10 d'unité)
*     Le calcul de finesse du planeur nécésite au tube de pitot et un vario;
*     Moyenne du tot de monté/descente peux etre calculer sans sonde de pitot( mais la  correction par la vitesse ne pourat pas ce faire)
*     Afin d'activer la fonction vous devez décommenter et remplire les quatres lignes suivante.
*     Pour désactivé la fonction il vous suffit de commenté la ligne suivante: GLIDER_RATIO_CALCULATED_AFTER_X_SEC
*************************************************************************************************************************
#define GLIDER_RATIO_CALCULATED_AFTER_X_SEC       10        // Temps écoulé pour effectué le calculer et le transmetre(en sec)
#define SPEED_TOLERANCE              5                      // En % de la vitesse
#define VSPEED_MIN_TOLERANCE -200                           // Hors de la tolerence quand la vitesse vertical est inferieur a cette valeur (cm/sec)
#define VSPEED_MAX_TOLERANCE  -10                           // Hors de la tolerence quand la vitesse vertical est supperieur a cette valeur (cm/sec)
************************************************************************************************************************
* Note : Il n'est pas util de commenter les paramètre de sensibilier, hysteresis etc quand un vario n'est pas utilisé (ces seron juste eliminer a la compilation)
*        Les paramètres de sensibiliter , hysteresis son partager par les deux capreur barométrique
************************************************************************************************************************



**** 5 - Réglage de la sonde de Pitot (optionnel) ***************************************************************************
* OXS peut mesurer la difference de pression entre une pression statique et une pression dynamique grasse a une sonde de Prandtl (ou plus connus sous le nom de sonde de pitot)
* Les meilleur resultat de mesure sont obtenu avec un capteur 4525DO-DS5AI001DP.
* Il est a noter que ce capteur fonctionne à Vcc = 5 Volt et qu'il peux mesuré des vitesse jusqu'à 360 km/h.
* Il existe d'autre versiob du capteur 4525DO fonctionant en Vcc = 3.3 Volt ou permetant de mesuer de plus forte pressions donc de plus haute vitesse.
* Le Capteur 4525DO est connecté a L'Arduino en parallel avec le capteur barométrique MS5611 (en utilisant le bus I2C pin A4 et A5 en plud de Vcc et Ground)
* Il est recommendé d'ajouter un condensateur (100nf)le plus pres possible du 5424DO entre Ground et Vcc.
* Note: A faible vitesse air (example sous 10 km/h), la difference de pression a mesurer est tres faible ce qui la rend moi fiable.
* Le capteur 4525DO-DS5AI001DP est assez difficile a ce proquuré et peux être chaire.
* Un alternative peux etre un capteur MPXV7002 combiné a un ADC ADS1115. Voir la section ADS1115 ci dessous.
* 
* Dans la ligne #define AIRSPEED_SENSOR_USE vous devez definir le type de capteur que vous utilisé     
*    Sélèctioner un valeur parmi: NO_AIRSPEED , MS4525 , MPXV7002
* 
* Le capteur de pression nécésite une calibration afin de retouner la valeur "0" quand la vitesse air est nul.
* OXS calibvre automatiquement le capteur quand il est mis sous tension (considerent que la valeur lu a ce moment la correspond a vitesse nul)
* Pour cela, il est important de ne pas exposer au vent la sond de Prandtl lors du démarage de OXSK.
* Maglrès cela, un dérive peux çetre observer lors de l'échauffement du composant.
* OXS permet de recalibrer le capteur depuis l émetteur via la fonction PPM (voir section 3).
* Le parametre AIRSPEED_RESET_AT_PPM défini la valeur que doit envoyé l émetteur pour forcé la recalibration du capteur.
* La valeur par défaut est 100.
* Le plus simple est de configuré une impulsion sur l'émmeteur par exemple de 1 seconde avec cette valeur.
* /!\ Note que lors de la recalibration doit etre envoyer uniquement quand la vitesse air est vraiment nul sinon cela génère de fausse mesure.
*
* OXS peux calculer deux type de vitesse air:
*  - la première est une vitesse air baser sur la densiter de l'air à 15°C et 11325 hPa (au niveau de la mer). C'est la vitessse air est celle utiliser dans l'aeronautique.
*    Exemple la vitesse de decrochage ne varie pas avec l'altitude
*  - La second tien compte la pression (fourni par le capteur barométrique) et la tèmperature interne du capteur afin de calculer a "vrai" vitesse (a comparer avec la vitesse GPS par vent nul).
* La vitesse air normaliser est calculer quand la ligne #define AIRSPEED_AT_SEA_LEVEL_AND_15C est décommenter. Pour avoir la vrai vitesse aire, metter cette ligne en commentaire.
* 
* OXS peux envoyer la vitesse air en (1/10) de knot/h ou en (1/10) km/h. Pour OpenTx, vous dever utilisé l'option knot/h (selement certaine vielle version on besoin de l'option km/h).
* Pour activé l'option km/h, il faut décommenter cette ligne #define AIRSPEED_IN_KMH
* Sinon meter cette ligne en commentaire pour activer l'option knot/h.
*
* OXS peux aussi utiliser la mesure de vitesse air pour calculer le varimetre compenser appeller PRANDTL_DTE (=delta total energy).
* Aller voir sur le web pour plus d'information a propos dTE (= electronically compensated vario).
* Le proincip)e est d'essayer de detectè uniquement les vrai assencdance  en neutralisant les effait de la gouverne de profondeur.
* Normalement, DTE doit être transmis par le parametre vitesse vertival (vitesse verticale = Valeur par défaut) car OpenTX ne supporte pas encore un champ spécifique pour celui-ci.
*
* Lorsque l'option PPM est implémentée, il est également possible d'ajuster le facteur de compensation entre 2 valeurs.
* Cela nécessite de décommenter 4 paramètres:
*     COMPENSATION_MIN_AT_PPM  Spécifiez la valeur PPM afin de définir la compensation à la valeur spécifiée par COMPENSATION_PPM_MIN; Défaut = 60
*     COMPENSATION_MAX_AT_PPM  Spécifiez la valeur PPM afin de définir la compensation à la valeur spécifiée par COMPENSATION_PPM_MAX; Défaut = 90
*     COMPENSATION_PPM_MIN     Compensation minimale; en % ; Par défaut 80
*     COMPENSATION_PPM_MAX     120 Compensation maximal; en % ; Par défaut 120
************************************************************************************************************************
#define AIRSPEED_SENSOR_USE  MS4525
#define AIRSPEED_AT_SEA_LEVEL_AND_15C 	// Si cette ligne est commentée, la vitesse est calculée à l'aide de la pression et de la température du baro (donc une vitesse "vraie" au lieu de la vitesse normale).      
//#define AIRSPEED_IN_KMH  				// Décommentez cette ligne si la vitesse doit être en km / h au lieu de nœud / h (sauf certaines anciennes versions, OpenTx s'attend à nœud / h) 
#define AIRSPEED_RESET_AT_PPM   100
#define COMPENSATION_MIN_AT_PPM 60
#define COMPENSATION_MAX_AT_PPM 90
#define COMPENSATION_PPM_MIN  80
#define COMPENSATION_PPM_MAX 120
************************************************************************************************************************




**** 6 - Réglage capteur de tension et de courant (optionnel) ***************************************************
*
* 6.1 - Tension de référence pour mesure des tensions et des courants **************************************************************
*     Le courant et les tensions peuvent être mesurés en (1023)pas ou à partir de VCC, ou à partir d'une référence interne 1.1 ou d'une référence externe.
*     Si VCC est très stable, il est probablement plus précis et plus facile de mesurer le courant et les tensions basées sur VCC (donc en 1023 pas de VCC).
*     Pourtant, cela exige que la tension appliquée sur la broche "RAW" d'Arduino soit régulée ou au moins toujours supérieure à 5,5 volts (afin de laisser la tension de la carte Arduino la réguler à 5 volts)
*     Si la tension sur la broche "Raw" est inférieure à 5,5 volts et change (par exemple en raison de la consommation d'asservissement ou de la batterie faible), les mesures de courant et de tension seront fausses.
*     Si VCC ne peut être très stable (par exemple, Arduino est alimenté par le RX sous 4.8 Volt NiMh) et vous n'avez besoin que des tensions (pas besoin des mesures de courant), il est possible de mesurer en fonction de la référence de tension interne Arduino (1.1 volt).
*     Si vous avez besoin également d'une mesure de courant, l'utilisation de la tension interne (1.1 volt) n'est pas la solution, car le capteur de courant a besoin d'une tension stable aussi.
*     Veillez à ce que les diviseurs de tension (voir ci-dessous) soient configurés pour que la tension de la broche analogique Arduino ne dépasse pas VCC ou 1,1 volt ou la tension externe en fonction de l'option que vous choisissez.
*     - Décommentez la "#define USE_INTERNAL_REFERENCE" pour activer la référence à la tension interne 1.1 volt (sinon, les mesures seront basées sur VCC).
*     - Décommentez la "#define USE_EXTERNAL_REFERENCE" pour activer la référence de tension externe (sinon, les mesures seront basées sur VCC).
*        Note: pour utiliser une référence de tension externe, vous devez utiliser un périphérique de référence de tension à votre Arduino. Ceci est assez difficile car Arduino pro mini la PIN AREF n'est pas disponible.
*     - Pour obtenir les meilleures mesures, vous pouvez configurer la référence de tension en utilisant la ligne #define REFERENCE_VOLTAGE.
*       La valeur doit être définie en millivolt (par exemple 5100 pour 5,1 volts) et ne pas pas contenir de décimales;
*       Si omise, les OXS appliquera automatiquement 1100 (lorsque USE_INTERNAL_REFERENCE est défini) et 5000 dans les autre cas
*       Lorsqu'une référence externe est utilisée, REFERENCE_VOLTAGE doit être spécifié
************************************************************************************************************************
//#define USE_INTERNAL_REFERENCE
//#define USE_EXTERNAL_REFERENCE  //Décommentez cette ligne si vous utilisez une référence externe au lieu de Vcc
#define REFERENCE_VOLTAGE 5000
************************************************************************************************************************
* 6.2 - Paramètres des tensions *******************************************************************************************
*     OXS peut mesurer jusqu'à 6 tensions d'entrée (veuillez noter que selon le fabricant, certains Arduino pro mini ont moins de broches analogiques disponibles).
*     Pour mesurer les tensions, vous:
*       - Devez spécifier les broches analogiques (A0 à A7) connectées à une batterie (par exemple un lipo multicellulaire) ou à un capteur analogique (par exemple, un capteur de température transforme la température en une tension qui peut être mesurée)
*       - Devez spécifier les valeurs des résistances utilisées pour les pont séparateurs de tension (voir ci-dessous)
*       - Pouvez spécifier un décalage et/ou une mise à l'échelle à être appliquer
* 
*     Note : Une broche analogique peut également être utilisée pour mesurer un courant à l'aide d'un capteur de courant; La configuration d'un capteur de courant est décrite dans la section 6.4 (voir ci-dessous);
*          N'utilisez pas la même broche analogique pour mesurer une tension et un courant.
*     /!\ Attention: ne pas utiliser les broches A4 et A5 si vous utilisez un vario ou un capteur de vitesse (ces broches sont réservées aux capteurs barométriques et de pression).
*     
*    Les broches utilisées pour mesurer certaines tensions sont définies dans la ligne #define PIN_VOLTAGE. Il est préférable de mettre cette ligne en tant que commentaire (ayant "//" à l'avant) si aucune tension ne doit être mesurée.
*     Lorsqu'il est utilisé, cette ligne doit contenir 6 valeurs (séparées par des virgules); La première valeur est utilisée pour mesurer VOLT1, le deuxième VOLT2, ... jusqu'à VOLT6
*     Chaque valeur doit être un nombre de 0 à 7 (0 signifie A0 = analogique 0, 1 signifie A1, ... 7 signifie A7) ou la valeur "8" (quand une tension ne doit pas être mesurée)
*     Note: les mêmes valeurs de broches analogiques peuvent être utilisées dans plusieurs tensions (par exemple pour VOLT1 et VOLT6).
*     
*     Note: Si la tension d'un LiPo doit être mesurée, elle DOIT commencer à partir de VOLT1 pour 1s, VOLT2 pour 2s ....., d'autres tensions peuvent être mesurées mais doivent être après le nombre de cellules à mesurer
*         Et le nombre d'ellement LiPo DOIT être spécifié dans "numberofcells" (voir la section suivante)
*         Le numéro de broche Ax peut être au hasard selon la broche connectée pour chaque cellule
*         
*  /!\ Veillez à ce que la tension appliquée à la broche Arduino ne dépasse pas Vcc (normalement 5 volts) ou 1,1 volt (si la tension de référence interne est utilisée).
*     Il se peut que vous devez utiliser le diviseur de tension afin de réduire la tension appliquée sur la broche Arduino par rapport à la tension que vous souhaitez mesurer.
*     Pour chaque tension à abbaiser au borne Ax de OXS, procédez comme suit:
*      - Faire un diviseur avec 2 résistances
*
*                 ------>  Point final = connexion à l'appareil à mesurer (batterie, capteur de courant, ...) 
*                 |
*               __|__   
*              |     |
*              |     |   R2 (Resistor_To_Voltage)          
*              |     |              
*              |_____|          
*                 |
*                 |------> Point milieu = connecter à la broche Arduino A0, A1, A2, A3, A6 ou A7
*               __|__   
*              |     |
*              |     |   R1 (Resistor_To_Ground)          
*              |     |              
*              |_____|          
*                 |
*                 ------>  Connecté à Ground
*
*        Note: Un condensateur (par exemple 100nf) pourrait également être ajouté entre le ground et le point milieux afin d'obtenir des valeurs plus stables
*         
*      - R1 et R2 sont choisis pour s'assurer que la tension s'applique à Arduino est proche de (mais ne dépasse jamais) VCC ou 1.1 volt selon votre choix en ce qui concerne la mesure de courant et de tension (voir ci-dessus)
*      - Volt sur Arduino pin = VCC (ou 1.1 volt) = "tension maximale à mesurer à partir de ce capteur" * R1 / (R1 + R2)
*      - R1 pourrait être de 10 kOhm; Donc R2 = R1 * (("tension maximale à mesurer à partir de ce capteur" / VCC [ou 1.1 volt selon la référence]) - 1)
*         par exemple. En utilisant 1.1 volt comme référence de tension on peux mesurer max 6 volts avec R1 = 10000, alors R2 = 10000 * ((6 / 1.1) - 1) = 45545 Ohm; Mieux vos arrondi à la valeur haut disponible en valeur de resitance, par exemple 47000 ohm
*
*    Les résistances utilisées dans les diviseurs de tension doivent être spécifiées dans les lignes #define RESISTOR_TO_GROUND et #define RESISTOR_TO_VOLTAGE.
*    Chaque ligne, doit contenir 6 valeurs (une pour chaque tension); Chaque valeur représente la résistance en Ohm ou kOhm mais utilisé la même unité pour les deux lignes. 
*    Si aucun diviseur n'est utilisé pour une certaine tension, réglez les deux résistances sur 0 (zéro)
*    Si aucun diviseur n'est utilisé, les lignes peuvent être commentées (en ajoutant "//" à l'avant)
*     
*     Afin d'obtenir les meilleures mesures de tension, l'OXS peuvent être calibrés. Ce processus facultatif permet de compenser les tolérances sur les résistances et sur Arduino ADC (convertisseur analogique / numérique).
*     Pour étalonner chaque mesure de tension, procédez comme suit afin de trouver les meilleures valeurs à configurer dans les lignes #define OFFSET_VOLTAGE et #define SCALE_VOLTAGE
*      - Reglé les paramètres dans OXS_config.h
*            - Laisser OFFSET_VOLTAGE = 0 et SCALE_VOLTAGE = 1 (ce reglage ajout aucune compensation)
*            - Sélectionner un champ pour transmettre la tension souhaitée (par exemple T1 pour VOLT3) et remplir la ligne "#define SETUP_DATA_TO_SEND" en conséquence
*      - Charger le programme dans Arduino
*      - Appliquer plusieurs tensions différentes sur le point final ( /!\ ne dépassé pas la tension maximale autorisée en fonction de R1 et R2)
*      - Pour chaque tension appliquée, mesurer la tension appliquée avec un voltmètre et lire la valeur reçue sur le panneau de télémétrie sur l'émetteur
*      - mettre ces valeurs dans excel (ou sur un graphique) et calculez les meilleures valeurs OFFSET_VOLTAGE et SCALE_VOLTAGE (à l'aide d'une régression linéaire)
*     Si cela semble trop complexe, utilisez simplement 2 tensions aussi différentes que possible (mais dans la gamme des valeurs normales que vous souhaitez mesurer)
*     Et appliquer un calcul basé sur l'exemple suivant:       .
*      Je m'attends à ce que la tension soit normalement entre 4 volts et 6 volts, donc j'applique 2 tensions proches de ces valeurs au point final
*        - Pour la première tension, le voltmètre donne 3510 millivolt et la télémétrie donne 3622
*        - Pour la deuxième tension, le voltmètre donne 5900 millivolt et la télémétrie donne 6013
*      Alors SCALE_VOLTAGE = (5900-3510) / (6013-3622) = 0.99958
*      et OFFSET_VOLTAGE = -3510 + (3622 * 0.99958 ) = 110
*  Note: Vous pouvez appliquer le même principe si vous mesurez autre chose qu'une tension.
*         Par exemple. Vous pouvez calibrer un capteur de température et régler le paramètre d'étalonnage afin d'obtenir un degré sur l'affichage de l'émetteur.
*     
*     Les lignes #define OFFSET_VOLTAGE et #define SCALE_VOLTAGE sont facultatifs (elle peuvent être mis en commentaire ajoutant "//" à l'avant))
*     Si elle est définie, chaque ligne doit contenir 6 valeurs. Utilisez 0 (pour le décalage => OFFSET) et 1 (pour l'échelle => SCALE) si aucun étalonnage n'est effectué pour une certaine tension.
*     
************************************************************************************************************************
                           VOLT1  VOLT2  VOLT3  VOLT4  VOLT5  VOLT6 
#define PIN_VOLTAGE         2    , 0    , 2   ,  3 ,     8 ,    8             
#define RESISTOR_TO_GROUND 12   , 20 ,   30 ,   40 ,    50 ,   60           // Définissez la valeur à 0 lorsqu'aucun diviseur n'est utilisé pour une de ces tensions
#define RESISTOR_TO_VOLTAGE 50, 100.1 , 200,   300 ,   500 ,  600           // Définissez la valeur à 0 lorsqu'aucun diviseur n'est utilisé pour une de ces tensions
#define OFFSET_VOLTAGE      0 ,    0 ,    0 ,    0 ,     0 ,    0           // Peut être négatif, doit être un nombre entier
#define SCALE_VOLTAGE       1 ,    1 ,    1 ,    1 ,     1 ,    1           // Peut être négatif, peut avoir des décimale
************************************************************************************************************************
* 6.3 - Maximum de cellule LiPo mesurer (et envoyer a l’émetteur) ***********************************************************
*     Les différentes tensions mesurées dans ce paragraphe sont toute referencé a ground.
*     OXS peuvent utiliser certains d'entre eux pour calculer la tension de certaines cellules lipo.
*     Définissez ici le nombre maximal de tension par celule d'un lipo que vous souhaitez transmettre; La valeur peut être 0 (sans cellules), 1,2,3,4,5,6
*     Si une valeur supérieure à 1 est définie, l'OXS calculera la tension de chaque cellule en fonction de la différence entre 2 tensions successives à partir de la tension1.
*     La tension totale de toutes les cellules sera calculée sur le côté emmetteur, en additionnant toutes les tensions de chaque cellule
*     L'emmeteur affichera la tension totale dans un champ de télémétrie nommé "Cells".
*     L'emmeteur identifiera également la cellule avec la tension la plus basse et l'affichera dans un champ nommé "Cell".
*     L'emmeteur à également un écran spécial où toutes les tensions s'affichent (voir le manuel Taranis).
*     Par exemple. Si nombre de cellules = 3, 
*           La tension sur la cellule 1 sera la tension mesurée sur la première broche définie dans PIN_Voltage
*           La tension sur la cellule 2 sera la différence entre la mesure des tensions sur la deuxième broche et la première broche (donc VOLT2 - VOLT1)
*           La tension sur la cellule 3 sera la différence entre la mesure des tensions sur la troisième broche et la deuxième broche (donc VOLT3 - VOLT2)
*           etc.
*     Lors de la transmission de tensions de la cellule, vous ne devez pas oublier de configurer PIN_VOLTAGE, RESISTOR_TO_GROUND, RESISTOR_TO_VOLTAGE (et optionnellement les paramètres d'étalonnage).
*     Les broches DOIVENT commencer et séquencer de VOLT1,2,3,4 .. pour 1s, 2s, ..
*     Les PIN non utilisées peuvent être utilisées pour transmettre d'autres tensions (par exemple: un capteur de température)
*     Par exemple. Si NUMBEROFCELLS = 3, la première broche (dans la liste de 6) doit être connectée à la cellule 1 (via un diviseur de tension calculé pour environ 4,5 volts
*                                La deuxième broche doit être connectée à la cellule 2 (via un diviseur de tension calculé pour environ 9 volts
*                                La troisième broche doit être connectée à la cellule 3 (via un diviseur de tension calculé pour environ 13 volts
*                                D'autres broches peuvent encore être utilisées pour d'autres données (température, courant, ...)
*     Remarques: Vous devez utiliser des diviseurs de tension pour réduire les tensions sur chaque broche de la boucle d'équilibrage lipo
*            Si vous utilisez la référence interne 1.1, vous pouvez définir tout R1 = 10 kOhm. Alors R2 pourrait être le mieux
*                   33 kOhm pour la tension1, 68 kOhm pour la tension2, 120 kOhm pour la tension 3 et 150 kOhm pour la tension4
*            Veuillez noter que plus vous disposez de cellules, plus les mesures sont imprécises si vous ne calibrez pas les tensions.
*            Probablement, il n'est pas logique de mesurer plus que 3 ou 4 cellules individuellement.
*            Si vous ne souhaitez pas transmettre la tension de la cellule, réglez la valeur sur 0 (zéro) ou commentez la ligne.
*            Ce paramètre définit le nombre maximal de cellules que vous attendez à transmettre.
*            Si OXS est connecté à un lipo ayant moins de cellules que defini, OXS réduira automatiquement le nombre de cellules qui laissent toujours le émmetteur calculer la tension totale et la tension de cellule la plus basse 
************************************************************************************************************************
#define NUMBEROFCELLS    3 
************************************************************************************************************************
// 6.4 - Conversion de tension en température (° Celcius)  ********************************************************************     
*     Pour mesurer une température, il est possible d'utiliser un composant électronique spécial (comme lm35) qui génère une tension proportionnelle à la teperature
*     Dans ce cas, vous pouvez simplement connecter la sortie de ce composant à une broche analogique arduino et configurer OXS comme décrit dans la section 6.2 afin de mesurer la tension
*     En ajoutant un décalage "OFFSET" et une mise à l'échelle "SCALE" (voir la section 6.2) en fonction de la caractéristique de votre composant, OXS peut calculer directement la température dans la tension de mesure définie. 
*     Vous pouvez ensuite demander à OXS de transmettre cette tension au champ de télémétrie souhaité (par exemple, T1) en remplissant la configuration comme expliqué dans la section 2.
*     
*     Ce type de composant n'est pas prévu pour mesurer une température élevée (par exemple supérieure à 150 ° C)
*     Vous pouvez alors utiliser des thermistances (= NTC). 
*     L'inconvénient des NTC est que la tension générée n'est pas proportionnelle à la température et nécessite des calculs supplémentaires.
*     OXS vous permettent d'utiliser un ou plusieurs NTC (par exemple un par cylindre) afin de mesurer de haute température(s).
*     Pour chaque NTC, vous devez ajouter une résistance en série suivant le schéma ci dessous:
*
*                 ------>  < Arduino Vcc > 
*                 |
*               __|__   
*              |     |
*              |     |   [serie resistor]        
*              |     |              
*              |_____|          
*                 |
*                 |------> <Arduino analog pin>
*               __|__   
*              |     |
*              |     |   [NTC]          
*              |     |              
*              |_____|          
*                 |
*                 ------>  <ground>
*
*     Si vous utilisez plusieurs NTC, tous les NTC et les résistances doivent être identiques et doivent être connectés à une broche analogique Arduino consécutive (telle que définie dans PIN_VOLTAGE)
*     
*     OXS réutilise le mVolt calculé par OXS. La configuration doit être adaptée de telle sorte que ce mVolt soit égal à la valeur brute renvoyée par l'ADC * 1000 (pour une meilleure précision)
*     Par conséquent, aux sections 6.1 et 6.2,
*          USE_INTERNAL_REFERENCE doit être commentée (donc avec // en avant)
*          USE_EXTERNAL_REFERENCE doit être commentée (donc avec // en avant)
*          REFERENCE_VOLTAGE doit être commentée (donc avec // en avant)
*          RESISTOR_TO_GROUND doit être mis à 0 (pour l'index utilisé)
*          OFFSET_VOLTAGE doit (normalement) doit être mis à 0 (pour l'index utilisé)
*          SCALE_VOLTAGE doit être configuré sur 204.6 (= 1000 * 1023/5000) (pour l'index utilisé)     
*     Ensuite, vous devez définir certains paramètres dans la section 6.4    
*        FIRST_NTC_ON_VOLT_NR spécifie l'indice de la première tension utilisée pour la conversion en température (par exemple, 3 signifie VOLT_3) (donc ce n'est pas le code de la broche analogique, celui-ci est défini dans la section 6.2)
*        LAST_NTC_ON_VOLT_NR spécifie l'indice de la dernière tension utilisée pour la conversion en température (par exemple, 5 signifie VOLT_5)
*          Dans cet exemple, cela signifie que vous prévoyez de mesurer 3 températures basées sur NTC connectées aux broches utilisées pour VOLT_3, VOLT_4 et VOLT_5
*          Note: si vous utilisez un seul NTC, vous devez spécifier la même valeur pour FIRST_NTC_ON_VOLT_NR et pour LAST_NTC_ON_VOLT_NR
*                Si vous n'utilisez pas NTC, conservez cette ligne en tant que commentaire
*        SERIE_RESISTOR spécifie le resitance (en Ohm) connecté entre Arduino Vcc et NTC (et la broche analogique); Sélectionnez une valeur presque égale à la résistance de NTC dans la gamme de température que vous attendez la meilleure précision 
*        Les 3 paramètres suivants sont spécifiques au NTC que vous utilisez. Les valeurs entre parenthèses sont la valeur nominale pour un NTC bon marché disponible sur aliexpress.com avec 100k ohms à 25 ° C et un coefficient bêta de 3950 pour la gamme 25/50.
*        STEINHART_A          (e.g. 7.00111E-4 )   
*        STEINHART_B          (e.g. 2.1644E-4 )
*        STEINHART_C          (e.g. 1.0619E-07 )
*        Si vous ne connaissez pas ces 3 paramètres, vous pouvez les calculer en mesurant la résistance NTC à 3 températures différentes. Utiliser, par exemple, La formule donnée sur wikipedia pour la thermistance
*     Lorsque vous utilisez 1 ou 2 NTC, la température (s) sera enregistrée dans VOLT_X et VOLT_Y où X est la valeur de FIRST_NTC_ON_VOLT_NR et Y la valeur de LAST_NTC_ON_VOLT_NR
*     Si vous utilisez plus de 2 NTC, OXS s'inscrit dans:
*         VOLT_X la température la plus basse
*         VOLT_X + 1 l'indice de la température la plus élevée (1 étant l'indice du premier NTC) 
*         VOLT_Y la température la plus élevée
*     Vous pouvez alors définir comment transmettre ces données dans la section 2
************************************************************************************************************************
//#define FIRST_NTC_ON_VOLT_NR 5 	// Décommentez cette ligne lorsque la thermistance est utilisée; Spécifiez l'indice de la première tension utilisée pour la conversion en température (par exemple, 5 signifie VOLT_5)
#define LAST_NTC_ON_VOLT_NR 6 		// Spécifiez l'indice de la dernière tension utilisée pour la conversion en température (par exemple 6 signifie VOLT_6)
#define SERIE_RESISTOR 4700 		// Resitance connectée à Arduino Vcc (en Ohm)
#define STEINHART_A 7.00111E-4   
#define STEINHART_B 2.1644E-4
#define STEINHART_C 1.0619E-07
************************************************************************************************************************
* 6.5 - Paramètre des capteurs de mesure de courant.   *****************************************************************
*     Il est possible de mesurer un courant (et une consommation de courant) si un capteur de courant est connecté.
*     La connexion d'un capteur de courant est une optionel.
*     Il nécessite un matériel supplémentaire. Il peut s'agir d'un IC comme ACS712 (pour 5, 20, 30 ampères) ou ACS758 (pour 50, 100, 150, 200 ampères).
*     La plupart des capteurs peuvent lire des courants bidirectionnels, mais ACS758 qui est de types "U" ne peut lire que un courant unidirectionnel (fournissant alors une sensibilité plus élevée).
*     Ces capteurs de courant sont assez bon marché (voir par exemple ebay) et renvoyer une tension qui dépend du courant. Cette tension est mesurée par OXS via une broche analogique.
*     La valeur de la PIN à remplir dans OXS_config.h est un nombre de 0 à 7 (0 signifie A0, 1 signifie A1, ... 7 signifie A7).
*     Si un capteur de courant est utilisé, ne pas utiliser une broche déjà utilisée par une tension.
* /!\ Veillez à ce que la tension appliquée à la broche Arduino ne dépasse pas Vcc (normalement 5 volts) ou 1,1 volt (si la tension de référence interne est utilisée)
*     Il se peut que vous devez utiliser un diviseur de tension afin de réduire la tension appliquée sur la broche Arduino.
*     Voir l'explication ci-dessus (paragraphe 6.2) sur le diviseur de tension.
*     Attention: ne pas utiliser les broches A4 et A5 si vous utilisez un vario ou un capteur de vitesse (ces broches sont réservées aux capteurs barométriques et de pression).
* Remarque: Le capteur de courant est normalement alimenté par le VCC 5 volts d'OXS (identique au capteur barométriques).
*       Il existe un capteur bidirectionnel et un capteur unidirectionnel.
*       Pour les capteur bidirectionnel, la sortie est normalement égale à VCC / 2 lorsque le courant = 0 Amp et, pour unidirectionnel, la sortie est normalement de 0,6 volt à 0 Amp.
*       Si OXS est connecté à une batterie qui ne dépasse pas 5,2 volts, la tension d'alimentation du capteur de courant varie avec la tension d'alimentation OXS.
*       Par conséquent, VCC / 2 (= O amp) varie avec VCC.
*       Ceci est un problème si l'Arduino ADC est configuré pour utiliser la référence interne de 1.1 volts.
*       Donc, dans ce cas, il est préférable de configurer l'ADC afin d'utiliser VCC comme référence pour la conversion.
*       Pour utiliser un capteur de courant, vous devez décommenter la ligne //#define PIN_CURRENTSENSOR et spécifier la broche Arduino connectée au capteur actuel. 
*       Vous devez également définir 2 paramètres en fonction du type de capteur utilisé; Ces paramètres sont donnés dans la fiche technique du capteur).
*         - MVOLT_AT_ZERO_AMP  =  MilliVolt généré par le capteur lorsque le courant est 0 Amp: la valeur normale est:
*                                       - Pour un capteur bidirectionnel: Vcc du capteur de courant / 2 (donc = 2500 si le capteur est connecté à Arduino Vcc et Arduino Vcc est de 5 Volt).
*                                       - 600 pour capteur unidirectionnel 
*         - MVOLT_PER_AMP       = MilliVolt par Amp. La valeur dépend de la sensibilité du capteur (par exemple, un ACS712ELCTR-30A-T a une sensibilité de 66 mvolt/Amp, un ACS758LCB-050U a une sensibilité de 60 mvolt/Amp)
*        
*        Si vous utilisez la référence interne 1.1 volt pour mesurer le voltage et le courant, vous devez également utiliser un diviseur de tension pour réduire la tension produite par le capteur actuel.
*        Voir la section 6.2 ci-dessus sur le diviseur de tension. Le principe est tout simplement le même, mais les noms des 2 paraamètres sont:
*          - RESISTOR_TO_GROUND_FOR_CURRENT
*          - RESISTOR_TO_CURRENT_SENSOR 
*  Remarque: ces paramètres sont automatiquement pas prit en compte lorsque PIN-CURRENTSENSOR n'est pas défini (= configuré comme commentaire)
*  Remarque: Lorsque le capteur de courant est utilisé, OXS peut également calculer et transmettre la consommation de courant (milliAh) et le carburant (en% descendant de 100% à 0%).
*        Si vous voulez ce dernier, utilisez une configuration comme "Fuel , MILLIAH , -100 , 4000 ,0" dans la section "données à transmettre" (et remplacez 4000 par la capacité - en milliAmph - de votre batterie) (voir ci-dessous) .
*        Toujours, avecles émetteur utilisant le logiciel OpenTx ou Ersky9x, il est préférable de laisser l'emmeteur calculer ces valeurs par elle-même en fonction du courant.
*               Cela garantit que les valeurs sont cohérentes; Il permet de réinitialiser les valeurs du côté emmeteur; Il permet de changer la valeur de la capacité de la batterie du côté de emmeteur (donc sans devoir recharger une autre configuration OXS dans Arduino).
*               Par exemple. Sur Ersky9x, dans le menu de télémétrie configuré "courant source" défini "FAS"; Dans «Alarme mAh», réglez le mah que vous désirez pour que l'alarme sonne et sélectionnez le son d'avertissement / la voix,
*               C'est-à-dire 70% de 2200 mAh lipo, utilisez 1540. alors le pourcentage de FUEL commencera de 100% à 10% en compte jusqu'à 0% lorsque 1540 sont consommés.
************************************************************************************************************************
//#define PIN_CURRENTSENSOR      		2
#define MVOLT_AT_ZERO_AMP        		600
#define MVOLT_PER_AMP            		60
#define RESISTOR_TO_GROUND_FOR_CURRENT  10
#define RESISTOR_TO_CURRENT_SENSOR      40
************************************************************************************************************************
***** 6.6 - Paramètres de Ads1115  *************************************************************************************
*    Il est possible de connecter un ADC externe du type ads1115. 
*    Cet appareil est très bon marché (environ 2 $) et peut fournir plus de précision que l'Arduino ADC interne
*    Il a une résolution de 16 bits, une référence de tension interne précise, un amplificateur de gain programmable et la capacité de mesurer directement la tension entre 2 broches 
*    Voir la fiche technique de ADS1115 pour plus de détails
*    OXS peut être connecté à l'un de ces appareils via le bus I2C. Il peut alors fournir jusqu'à 4 mesures de tension appelées ADS_VOLT_1 ... ADS_VOLT_4
*    OXS permet de convertir une mesure de tension d'annonces en courant et consommation (lorsque ads1115 est connecté à un capteur de courant)
*    OXS permet également de convertir une mesure de tension de ads1115 en vitesse et vario compensé (lorsque ads1115 est connecté à un capteur baro différentiel comme le MPXV7002)
*          Le capteur MPXV7002 est une alternative moins coûteuse et plus facile a trouver que le capteur MS4525 (mais moins précise)
*    Pour utiliser un ads1115 ADC, vous devez décommenter ACD_MEASURE et spécifier plusieurs ensembles de 4 paramètres.
*    /!\ Attention: laissez ADC_MEASURE comme commentaire si ads1115 n'est pas utilisé (afin d'éviter les erreurs I2C et ralentir les OX)
*        Dans ACD_MEASURE, vous spécifiez les broches ads115 utilisées pour la mesure de tension
*              Remplissez toujours les 4 valeurs. Pourtant, si vous n'avez pas besoin de toutes les 4 mesures, utilisez la valeur ADS_OFF pour les mesures non requises
*              Note : Plus vous demandez de mesure, plus de temps sera nécessaire pour obtenir chacune d'elles car elles ne sont envoyé qu'une a la fois.
*              Sélectionnez 4 valeurs parmi: A0_TO_A1, A0_TO_A3, A1_TO_A3, A2_TO_A3, A0_TO_GND, A1_TO_GND, A2_TO_GND, A3_TO_GND, ADS_OFF
*        Dans ADC_FULL_SCALE_VOLT, vous spécifiez le paramètre de gain ads1115 pour chacune des 4 mesures.
*              Remplissez toujours 4 valeurs même si vous n'avez pas besoin des 4 mesures
*              Cela permet d'amplifier une faible tension appliquée sur les broches d'entrée avant qu'il ne soit converti par l'ADC. La précision de la conversion est donc optimale.
*              Sélectionnez entre MV6144 MV4096 MV2048 MV1024 MV512 MV256 où les chiffres donnent le mvolt maximal appliqué sur la broche (par exemple pour A0_TO_GND) ou entre 2 broches (par exemple pour A0_TO_A1)
*        Dans ADS_SCALE, vous spécifiez un facteur d'échelle à appliquer sur chaque mesure afin d'obtenir une valeur conforme à la mesure attendue.
*              Remplissez toujours 4 valeurs même si vous n'avez pas besoin des 4 mesures
*              Lorsque le paramètre de mise à l'échelle = 1, OXS renvoie une valeur = 1 lorsque la tension appliquée sur la/les PIN de ads1115 est la tension maximale définie par ADC_FULL_SCALE_VOLT. 
*              Donc, si ADC_FULL_SCALE_VOLT est réglé sur MV512, lorsque la tension d'entrée sera de 512mv (ou plus), OXS renvera 1 si ADS_SCALE = 1
*              Si vous ne disposez pas d'un diviseur de tension sur ads1115, vous vous attendez probablement à ce que les OX renvoient 512; définissez alors ADS_SCALE à 512.
*              Si vous avez un diviseur de tension, la tension que vous souhaitez mesurer est supérieure à la tension appliquée sur ads1115 broches et vous devez augmenter ADS_SCALE.
*              par exemple. Si votre diviseur de tension divise votre tension par 10, vous devez définir ADS_SCALE à 5120 (= 512 * 10)
*              Note: ADS_SCALE peut avoir des décimales (par exemple, 100.5). Cela peut être positif ou négatif; Il ne peut pas être 0
*        Dans ADS_OFFSET, vous spécifiez un décalage à appliquer. 
*              Remplissez toujours 4 valeurs même si vous n'avez pas besoin des 4 mesures
*              Lorsqu'aucun décalage ne doit être appliqué, réglez la valeur sur 0
*              Le décalage spécifié est ajouté à la valeur calculée après la mise à l'échelle  
*              Chaque valeur doit être un nombre entier (positif ou négatif); Il peut être 0
*        Dans ADS_RATE, vous spécifiez le nombre de milli sec que Ads1115 prend pour convertir une tension.     
*              Remplissez toujours 4 valeurs même si vous n'avez pas besoin des 4 mesures
*              L'utilisation d'une valeur élevée réduit la consommation d'énergie, mais elle réduit le nombre de mesures pouvant être effectuées / transmises par seconde 
*              Sélectionnez les valeurs entre MS137, MS69, MS35, MS18, MS9, MS5, MS3, MS2; Les chiffres correspondent au nombre de milli sec (par exemple MS18 signifie 18 ms)
*              Remarque: OXS attendra au moins le délai spécifié, mais il se peut que le délai soit plus élevé en raison d'autres tâches devant être exécutées par OXS
*        Dans ADS_AVERAGING_ON, vous spécifiez le nombre de tensions à collecter afin de calculer une moyenne.
*              Remplissez toujours 4 valeurs, même si vous n'avez pas besoin des 4 mesures. Si vous ne désirez pas une moyenne, définissez la valeur sur 1
*              Cela doit être un nombre entier, positif et différent de 0.
*              Note : L'augmentation de la valeur est un moyen de réduire le bruit, mais il augmente le délai entre 2 transmissions
*        Dans ADS_CURRENT_BASED_ON, vous spécifiez quelle mesure de tension (le cas échéant) est utilisée pour le courant (et la consommation actuelle)
*              Décommentez cette ligne uniquement lorsqu'une conversion est demandée
*              Gardez cette ligne en tant que commentaire s'il n'y a pas de capteur de courant ou si le capteur est connecté à une broche Arduino comme expliqué à la section 6.4
*              Remplissez une seule valeur; Sélectionnez une valeur parmi ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
*              Note : Lorsque OXS calcule un curent basé sur une tension, il utilise également 2 paramètres de la section 6.4: MVOLT_AT_ZERO_AMP ​​et MVOLT_PER_AMP
*        Dans ADS_AIRSPEED_BASED_ON, vous spécifiez quelle mesure de tension (le cas échéant) est utilisée pour la vitesse air
*              Décommentez cette ligne uniquement lorsqu'un capteur de vitesse analogique est connecté à ads1115 
*              Gardez cette ligne comme un commentaire s'il n'y a pas de capteur de vitesse connecté à ads1115
*              Remplissez une seule valeur; Sélectionnez une valeur entre ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
*              Note : Un capteur de vitesse typique est le MPXV7002DP qui est disponible sur ebay ou aliexpress.com.
*                     Lorsque ce capteur est utilisé, vous devez configurer l ads1115 de la manière suivante
*                     - Demandez q'une mesure ads1115 et celci doit être une tension différentielle: utilisez donc cette configuration: ADS_MEASURE A0_to_A1, ADS_OFF, ADS_OFF, ADS_OFF
*                     - Connectez une résistance d'environ 10kohm entre l ads1115 Vcc et ads1115 A1 et une autre de la même valeur entre l'ads1115 Ground et l'ads1115 A1; Donc A1 broche obtenir Vcc/2 volt
*                     - Définissez le gain de ads1115 pour obtenir 2048 mvolt à pleine échelle: utilisez donc: ADC_FULL_SCALE_VOLT MV2048, MV2048, MV2048, MV2048  
*                     - Définir le taux de rafraichisement de ads afin de faire la conversion ADC le plus rapidement possible; utilisez donc: ADS_RATE MS2, MS2, MS2, MS2
*                     - Les autres paramètres ne sont pas critiques (pas utilisé pour la vitesse air)
************************************************************************************************************************
#define ADS_MEASURE A1_TO_GND ,   ADS_OFF , ADS_OFF , ADS_OFF 	// décomenter lorsque ADS1115 est utilisé; Sélectionnez 4 valeurs entre A0_TO_A1, A0_TO_A3, A1_TO_A3, A2_TO_A3, A0_TO_GND, A1_TO_GND, A2_TO_GND, A3_TO_GND, ADS_OFF
#define ADS_FULL_SCALE_VOLT  MV4096, MV4096, MV4096, MV4096 	// Sélectionnez entre MV6144 MV4096 MV2048 MV1024 MV512 MV256
#define ADS_OFFSET 0, 0 , 0 , 0 								// Doit être un nombre entier (positif ou négatif)
#define ADS_SCALE 2, 10, 1, 1 									// peux est un float
#define ADS_RATE  MS137 , MS5, MS3 , MS2 						// Sélectionnez entre MS137, MS69, MS35, MS18, MS9, MS5, MS3, MS2
#define ADS_AVERAGING_ON 1 , 10, 50, 50 						// Nombre de valeurs utilisées pour la moyenne (doit être compris entre 1 et 254)
#define ADS_CURRENT_BASED_ON ADS_VOLT_1  						// décomenter si le courant et la comsomation doivent être calculés en fonction de l'une des mesures de tension ADS; Sélectionnez ensuite la tension à utiliser entre ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
#define ADS_AIRSPEED_BASED_ON ADS_VOLT1  						// Décommenter si la vitesse (et dte) doit être calculée en fonction de l'une des mesures de tension ADS; Sélectionnez ensuite la tension à utiliser entre ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
************************************************************************************************************************



**** 7 - Réglage capteur RPM (tour par minute) (optionnel) ***************************************************************************
*      Il est possible de mesurer les RPM à l'aide d'un capteur connecté sur le PIN ICP (= PB0, = broche 8) d'OXS
*      Ce capteur doit fournir un changement de niveau (0 - Vcc) sur cette broche chaque fois qu'une lame passe devant elle.
*      Le nombre de pales est un paramètre important à configurer, mais cela se fait sur le côté émetteur
*      Il est également possible de construire une petite carte PCB qui fournira des impulsions lorsqu'elle est connectée sur un fil sans balais.
*      Pour activer cette fonction, mettez YES au lieu de NO dans la ligne #define CALCULATE_RPM  .
*      Note: La broche numérique 8 (PB0 / ICP) est la seule à être utilisée pour mesurer les RPM.
*            La valeur calculée par OXS est en Hertz (et non en roration par minute)
************************************************************************************************************************
#define CALCULATE_RPM     NO 
************************************************************************************************************************



**** 8 - Réglage de la mémoire non volatile (optionnel) ************************************************************************************
*      Fonction optionnelle.
*      Si la mémoire persistante est activée, la consommation de courant et la consommation de carburant (+ paramètres de flux) seront stockées dans EEProm toutes les 30 secondes.
*      Cette valeur sera restaurée chaque mise sous tension.
*      Ainsi, vous obtiendrez une consommation continue même si vous éteignez le modèle entre les vols.
*      Si vous souhaitez enregistrer ces données, vous devez dire OUI dans la ligne "#define SAVE_TO_EEPROM"
*      
*      Il est également possible de réinitialiser:  
*       - la consommation de courant et de carburant à zéro appuyant sur un bouton-poussoir connecté à OXS.
*         pour cela une broche DIGITAL Arduino doit être connectée à un bouton-poussoir, l'autre broche du bouton-poussoir étant connectée à Gnd (terre).
*         Pour utiliser cette fonction, vous devez spécifier la broche DIDITAL Arduino utilisée.
*               Par défaut: 10; Une autre broche numérique peut être utilisée; N'utilisez pas une broche déjà utilisée dans un autre but.
*       - La consommation de carburant à zéro à partir de l'émmeteur en utilisant un canal sur l'émmetteur connecté à l'Aduino à l'aide de la fonction PPM (voir la section 3 PPM et 11 capteur de débit)         
************************************************************************************************************************
#define SAVE_TO_EEPROM    NO
#define PIN_PUSHBUTTON    10   
************************************************************************************************************************



**** 9 - GPS (optionel)  *********************************************************************************************
* Il est possible de connecter un module GPS à Arduino. Dans ce cas, OXS transmettra a l'emetteur certaines données générées par le module GPS.
* Note: les données sont envoyées au destinataire uniquement lorsque le GPS est prês (connecter au satelite).
*     Ainsi, il peut prendre plusieurs minutes avant que OXS ne commence à envoyer des données GPS.
*     Si le GPS a perdu sa connexcion avec les satelit, OXS arrêtera d'envoyer des données GPS jusqu'à ce que le GPS ait obtenu une nouvelle solution.
* Lorsque le récepteur Frsky (SPORT ou HUB) est utilisé, OXS enverra toujours la longitude, la latitude, l'altitude, la vitesse du sol et le parcours. 
*   Cela ne nécessite aucune ligne supplémentaire dans la section "Données à transmettre" (voir 9 ci-dessus) 
* Lorsque le récepteur multiplex est utilisé, l'utilisateur doit bien définir sous la ligne "#define SETUP_MULTIPLEX_DATA_TO_SEND", les données sont envoyées et le numéro de ligne où les données doivent apparaître à l'écran.
* Points a faire attention sur le type de matériels.
*   Modules GPS pris en charge: OXS prend en charge les modules basés sur UBLOX GPS (facilement disponible sur ebay ou aliexpress) comme Neo6M, Neo7M et Neo8M. 
*   La plupart des modules sont équipé d'un régulateur de tension afin de ramener la tension à 3,3 volts, soit environ le maximum autorisé par le GPS
*   Ils ont 4 broches disponibles qui doivent être connectés à Arduino
*     - GPS Ground est connecté à Arduino Ground
*     - GPS Vcc est normalement connecté à la broche Raw Arduino (qui est normalement connectée à Vcc du récepteur)
*          Toujours prendre soin que le régulateur de tension sur le module GPS soit, pour la plupart du temps, prévu pour une tension maximale de 6 volts alors que la broche RAW arduino accepte plus. 
*          Donc, si votre broche RAW recoi plus de 6 volts, il est prudent d'ajouter un autre régulateur de tension pour faire tomber la tension GPS Vcc. 
*    - La broche GPS Tx est connectée à la broche Arduino Rx      
*    - La broche GPS Rx est connectée à une résistance (par exemple 10k) et l'autre broche de la résistance est connectée à la broche 6 d'Arduino (broche numérique 6).
*          Cette résistance est ajoutée (au moins pour un Arduino 5 volts) afin de protéger la broche GPS. 
*          Ceci est demandé car Arduino généreré un signal avec un niveau élevé égal à Arduino Vcc (normalement 5 volts) alors que le module GPS ne devrait normalement pas accepter plus de 3.3 Volt.
*          Pour être plus sûr, vous pouvez même ajouter une deuxième résistance entre GPS Rx pin et Ground (valeur = 22k) mais, dans mon cas, cela a fonctionné sans cette deuxième résistance. 
*          Note: Il serait possible d'utiliser une autre broche que la broche 6 d'Arduino, mais il faut modifier certains paramètres dans le fichier OXS_gps.cpp (voir "Configuration du capteur GPS").
*  /!\ Note importante : 
*      La broche Arduino Rx est utilisée pour 2 besoin: obtenir les données du GPS et télécharger le programme dans l'Arduino (normalement fait une fois) à l'aide d'un adaptateur USB vers série.
*              Vous devez éviter d'utiliser le GPS et l'USB en même temps, car lors du téléchargement d'un programme dans Arduino, il y aura des conflits entre les signaux envoyés par les 2 appareils (le PC et le GPC) et la programmation échouera.
*              Il existe un autre risque: si votre adaptateur USB vers série génère un signal de 5 volts, cela pourrait endommager le module GPS.
*              Donc, lorsque vous connectez l'usb à l'adaptateur série à l'Arduino, vous devez déconnecter au moins la broche GPS TX de la broche Arduino Rx.  
*              Personnellement, j'utilise un connecteur entre Arduino et le module GPS et je peux donc déconnecter complètement le module GPS.
* Points a faire d'attention sur le logiciel
*    Le module GPS UBLOX est normalement livré avec une configuration par défaut (générant automatiquement, par exemple, des messages NMEA à 9600 bauds à un taux de rafraîchissement de 1 hz).
*    OXS suppose que, au démarrage, le GPS fonctionne à 9600 bauds. OXS a envoyé alors quelques commandes pour
*       - Désactiver tous les messages NMEA
*       - Active certains messages UBX
*       - Augmenter la fréquence de calcul (à 5 Hz au lieu de 1 heure))
*       - Configurez le débit en bauds à 38400 au lieu de 9600.
*    Ces paramètres ne sont pas enregistrés dans le GPS (car certains modules GPS ne le permettent pas). Ainsi, OXS enverra ces commandes à chaque mise sous tension.  
*    Si OXS n'émet pas de données GPS, vérifiez que votre module GPS a encore la configuration par défaut (le plus important est qu'il soit configuré pour recevoir des messages de commande UBX à 9600 bauds). 
*       Un moyen simple de vérifier la configuration GPS est de connecter le module GPS à un FTDI de 3,3 volts (ou un adaptateur USB à série) et d'utiliser un logiciel gratuit nommé "u-centre". 
*       Ce logiciel est disponible sur le site officiel de UBLOX. Plus d'informations sont facilement disponibles sur le Web. 
*  OXS permet de modifier certains paramètres dans le fichier config.h:
*     -   #define A_GPS_IS_CONNECTED      NO       	: Remplacez NO par YES si un GPS est connecté et doit transmettre ses données
*     -   #define GPS_SPEED_IN_KMH   				: Décommentez cette ligne si la vitesse GPS doit être envoyée en km / h au lieu de nœud / h
*     -   #define GPS_SPEED_3D       				: Décommentez cette ligne si la vitesse GPS doit être la vitesse 3d au lieu de la vitesse 2d (note: 3d est probablement moins précis - à tester)
*     
************************************************************************************************************************
#define A_GPS_IS_CONNECTED      NO    	// Remplacez NO par YES si un GPS est connecté et doit transmettre ses données
//#define GPS_SPEED_IN_KMH				// Décommentez cette ligne si la vitesse GPS doit être envoyée en km / h au lieu de nœud / h
#define GPS_SPEED_3D  					// Décommentez cette ligne si la vitesse GPS doit être la vitesse 3d au lieu de la vitesse 2d (note: 3d est probablement moins précis - à tester)
************************************************************************************************************************



****** 10 - IMU 6050 (capteur accéléromètre/gyroscope) (optionnel) et HMC5883 (magnetometre)) ********************************************************
// ***** 10.1 - IMU 6050 *****
*  Il est possible de connecter un capteur IMU (= accéléromètre / gyro) à l'arduino; C'est optionnel.
*  Il permet :
*      - Pour réduire le temps de réaction du vario d'environ 0,5 sec (note: un capteur baro doit également être connecté parce que OXS fusionne les données des deux capteurs)
*      - Transmettre des données sur les accélérations et / ou l'orientation (pitch / roll); Dans ce cas, il est important que le dispositif OXS soit monté dans une position correcte et soit aligné avec l'axe du plan avion. 
*  Un seul capteur IMU qui est pris en charge est le: mpu6050. 
*  Ce capteur est facilement disponible sur ebay, ... sous différents modules. Le meilleur module à utiliser est probablement le GY-86 car il dispose également d'un régulateur de tension (3.3 volts), d'un convertisseur de niveau I2C et d'un capteur baro (MS5611)    
*  5 broches de la mpu6050 doivent être connectées à Arduino:
*       - MP6050 ground  <--->  Arduino ground
*       - MP6050 Vcc     <--->  Arduino Vcc
*       - MP6050 SDA     <--->  Arduino SDA = Arduino A4
*       - MP6050 SCL     <--->  Arduino SCL = Arduino A5
*       - MP6050 INT     <--->  Arduino INT0 = Arduino 2 OR Arduino INT1 = Arduino 3 (n'utilisez pas la même broche pour un autre but, comme PPM!)
* Pour activer l'IMU, définissez YES dans la ligne #define A_MPU6050_IS_CONNECTED       
* Lorsque IMU est activé, cette version d'OXS calcule une vitesse verticale de manière différente, fusionnant l'altitude du capteur baro avec l'accélération verticale de l'IMU (en référence Terre).
* Cet autre type de vitesse verticale peut être envoyé comme vitesse vertical en configurant la valeur de BARO_AND_IMU à #define VSPEED_SOURCE (voir section 4.2)
* Il est également possible de l'affecter à "VARIO_PRIMARY" ou "VARIO_SECONDARY" et donc de basculer entre 2 sources vario du Tx (en utilisant un canal ppm)
* Afin d'obtenir les meilleurs résultats d'IMU, il est nécessaire de calibrer les décalages de l'accéléromètre. Pour ce faire, il vous faut:
*    - Téléchargez une version du microprogramme OXS avec la ligne #define DISPLAY_ACC_OFFSET sans commentaire
*    - Lancez OXS en cours de connexion au PC (via l'interface série USB = FTDI)
*    - Ouvrir le terminal IDE Arduino (appuyez simultanément sur CTRL + MAJ + M)
*    - Veillez à régler le débit en bauds à 115200 (ou 38400 si le GPS est activé aussi)
*    - Après le démarrage, le terminal doit, tous les 2 ou 3 s, afficher Acc suivi par 3 nombres respectivement AccX, AccY et AccZ. Notez que ces chiffres changent lorsque mpu6050 se déplace.
*    - Assurez-vous que le mpu6050 (GY86) est parfaitement horizontal et ne se déplace pas (par exemple, mettre sur une table) 
*    - Notez les 2 premiers numéros (= AccX et AccY); Ne prenez pas soin du 3ème numéro car lorsque le capteur est dans cette position, il reflètera la gravité et sera autour de 16384. 
*    - Tournez le mpu6050 afin d'obtenir des axes X ou Y parfaitement verticaux et ne vous déplacez pas. Maintenant, le 3ème nombre deviendrait beaucoup plus bas (car il ne mesure plus la gravité)
*    - Notez le 3ème numéro (= Accz)
*    - Mettez à jour le fichier OXS_config.h remplissant les 3 nombres dans les lignes #define ACC_OFFSET_X, #define ACC_OFFSET_Y et #define ACC_OFFSET_Z
*    - Définissez ligne #define DISPLAY_ACC_OFFSET comme commentaire (ajoutant "//" à l'avant)
*    - Télécharger à nouveau le firmware OXS dans arduino
************************************************************************************************************************ 
#define A_MPU6050_IS_CONNECTED      NO     	// Replace NO by YES if a IMU6050 is connected and has to transmit his data
#define PIN_INT_6050 3    					// Interrupt from 6050 has to be connected to Arduino pin 2 or pin 3 (do not use here the same pin as PPM) 
#define DISPLAY_ACC_OFFSET
#define ACC_OFFSET_X 0 						// fill here the value reported when DISPLAY_ACC_OFFSET is activated (eg. -160)
#define ACC_OFFSET_Y 0 						// fill here the value reported when DISPLAY_ACC_OFFSET is activated (eg. -150)
#define ACC_OFFSET_Z 0 						// fill here the value reported when DISPLAY_ACC_OFFSET is activated (eg. -1100)
************************************************************************************************************************ 
// ***** 10.2 - HMC5883 *****
* Si vous utilisez un module comme GY-86 et si IMU6050 est connecté à arduino (voir 10.1), OXS peut accéder à un magetomètre HMC5883 afin d'obtenir une mesure Yaw.
* Cela ne nécessite pas de câblage supplémentaire.  
* Pour activer le HMC5883, réglez YES dans la ligne CALCULATE_YAW_WITH_HMC5883      
* Veillez à obtenir des valeurs fiables du magnétomètre, il est obligatoire de le calibrer.
* Il s'agit d'un processus non trivial.
* Je recommande de suivre le processus décrit dans ce lien: http://www.physi.cz/hmc5883l-magnetometer-calibration.html
* Cela signifie que vous devez:
* 1) Collecter des données d'OXS: 
*    Cela exige que vous: 
*      - Décommentez la ligne #define GENERATE_MAG_CALIBRATION_DATA
*      - Télécharger le programme dans l'Arduino
*      - Ouvrez le terminal PC Arduino
*      - Dans le terminal PC, vous obtiendrez quelques lignes de données générales suivies d'une liste avec 3 valeurs par ligne (il s'agit des lectures X, Y et Z du magnétomètre)
*      - Tournez le capteur lentement sur 360 ° le long de tous les axes afin de collecter plus de 1000 lignes de données (cela peut prendre plusieurs minutes)
*  2)Copiez et collez les lignes du terminal PC vers un fichier TXT (en utilisant un éditeur de texte comme bloc-notes). Supprimez les premières lignes de données générales (avant la liste) et enregistrez ce fichier en tant que fichier TXT
*  3)Téléchargez le programme Windows dans le lien ci-dessus (voir "Téléchargement exécutable")
*      Exécutez ce programme, réglez la norme en 1.0, cliquez sur "ouvrir" et sélectionnez le fichier TXT avec vos données d'échantillons, puis cliquez sur "Calibrer"
*  4) Notez les 3 valeurs "biais combiné" et copiez-les dans XMAG_OFFSET, YMAG_OFFSET et ZMAG_OFFSET (dans OXS_config_advanced.h)
*  5) Notez les 9 "facteurs d'échelle combinés ..." et ouvrez maintenant un lien vers un site Web qui vous permettra de trouver l'inverse de la matrice des 9 valeurs:
*      https://www.wolframalpha.com/input/?i=%7B%7B591.0437,-13.1628,-15.0294%7D,%7B-13.1628,596.1147,30.5314%7D,%7B-15.0294,30.5314,552.0759%7D%7D%5E-1
*      - Remplissez les 9 valeurs dans le premier champ de saisie (respecter le format spécial avec {{,,}, {,,}, {,,}} et cliquez sur le bouton à droite de la ligne d'entrée.
*  6) Vous obtiendrez une matrice "résultat". Notez les 9 valeurs du résultat et copiez-les dans XXMAG_CORRECTION, XYMAG_CORRECTION, XZMAG_CORRECTION ... ZZMAG_CORRECTION (dans OXS_config_advanced.h)
*  7) Définissez la ligne #define GENERATE_MAG_CALIBRATION_DATA comme commentaire et téléchargez encore le programme à Arduino
************************************************************************************************************************ 
#define CALCULATE_YAW_WITH_HMC5883   NO    	// Sélectionnez entre YES, NO; YES demande a ce que A_MPU6050_IS_CONNECTED soit à YES ici ci-dessus
//#define GENERATE_MAG_CALIBRATION_DATA   	// Décommentez cette ligne lorsque l'étalonnage HMC5883 doit être effectué. Rétablir en tant que commentaire une fois que les paramètres d'étalonnage ont été introduits
#define    XMAG_OFFSET 2     				// Doit etre un interger
#define    YMAG_OFFSET -1     				// Doit etre un interger
#define    ZMAG_OFFSET 139     				// Doit etre un interger
#define    XXMAG_CORRECTION  0.122082   	// peux avoir des décimals
#define    XYMAG_CORRECTION  -0.00204026
#define    XZMAG_CORRECTION  0.00377534 
#define    YXMAG_CORRECTION  -0.00204026
#define    YYMAG_CORRECTION  0.130413
#define    YZMAG_CORRECTION  -0.00491189
#define    ZXMAG_CORRECTION  0.00377534
#define    ZYMAG_CORRECTION  -0.00491189
#define    ZZMAG_CORRECTION  0.138038
************************************************************************************************************************ 



****** 11 - Flow sensor ******************
* Si vous utilisez un moteur à essence, vous pouvez connecter un débitmètre à OXS
* Ce capteur génère une impulsion chaque fois que quelques milli litres passent à travers le capteur
* OXS peut compter le nombre d'impulsions et calcule 3 données: la consomation courante milli litres / min et, en prenant soin de la capacité du réservoir, le carburant restant en ml et en%.
* Il est recommandé d'utiliser le type de débit suivant, car il est probablement plus précis que d'autres lorsque le débit est faible
* http://www.conrad.be/ce/nl/product/155374/BIO-TECH-eK-FCH-M-Doorstroomsensor-1-stuks-Voedingsspanning-bereik-5-24-VDC-Meetbereik-08-0015-lmin-l-x;jsessionid=EED7B26A7F28BA3F20F0060807E20FD1.ASTPCEN22?ref=searchDetail
* Il est prévu pour mesurer de 0,015 à 0,8 ml / min.
* Le signal de sortie du capteur du débitmètre doit être connecté à la broche 9 d'arduino (et aux 2 autres broches à 5 volts VCC et au sol).
* Il existe d'autres capteurs de débit bon marché sur ebay ou aliexpress, mais je m'attends à ce qu'il n'y ait pas assez de précision lorsque le débit est faible.
* 
* Pour activer un tel capteur de débit, vous devez: 
*   - Affectez YES au paraamètres A_FLOW_SENSOR_IS_CONNECTED (dans le fichier OXS_config_basic.h)
*   - Spécifiez dans PULSES_PER_ML le nombre d'impulsions générées par le capteur lorsque 1 milli litre de liquide s'écoule à travers
*   - Spécifiez dans TANK_CAPACITY la capacité maximale en milli litre
*   - Spécifiez 8 valeurs utilisées pour calibrer votre capteur dans INIT_FLOW_PARAM
*         Ces paramètres sont utilisés afin de s'assurer que le nombre d'impulsions générées par le capteur lorsque 1 milli-litre de liquide s'écoule varie en fonction du flux lui-même.
*         Pour 4 valeurs d'écoulement (les 4 premiers paramètres), OXS vous permet de définir une correction (en%) dans les 4 derniers paramètres.  
*         Le processus d'étalonnage du capteur devrait être le suivant: 
*              Définir les 4 derniers paramètres sur 0 (donc 0% de correction)
*              Faite fonctioné votre moteur à 4 vitesses différentes (de très bas, vers un moyen et enfin à une vitesse élevée) pendant quelques minutes. 
*              Pour chaque fonctionement, 
*                - Notez le carburant restant (en ml) rapporté par OXS au démarrage (par exemple 1200) et à la fin du vol (par exemple 1090)
*                - Mesurer le carburant restant (en ml) dans le réservoir au démarrage (par exemple 1500) et à la fin du vol (par exemple 1380)
*                - Notez le temps de fonctionement (en min) entre le début et la fin du vol (par exemple, 2 minutes).
*                - Comparez le carburant en ml consommé (différence entre le carburant restant au début et à la fin) rapporté par OXS et la realité (par exemple, rapporté = 1200 - 1090 = 110 ml; réel = 1500 - 1380 = 120 ml)
*                - Calculer le facteur de correction à appliquer (par exemple (120 - 110) / 110 = 9%); La correction des notes pourrait être négative)
*                - Calculer le débit où cette correction s'applique (= ml consommé rapporté par OXS / temps en conflit = 110 ml / 2 min = 55 ml / min)
*              Remplissez les 4 premiers paramètres avec les flux calculés (par exemple, 55) et les 4 derniers paramètres avec le pourcentage de correction (par exemple, 9). 
*              Veillez à ce que les 4 premiers paramètres soient en ordre croissant (donc de faible vitesse à grande vitesse).
*         Note: Lorsque OXS calcule la consommation, il appliquera une interpolation linéaire pour la gamme de valeurs associée.
*
*   Si vous déplacez un périphérique OXS d'un avion à un autre, vous devrez probablement modifier les valeurs définies dans TANK_CAPACITY et / ou INIT_FLOW_PARAM. 
*   Cela nécessite de télécharger un nouveau firmware dans votre OXS, sauf si vous utilisez le protocole JETI ou le protocole SPORT avec openTx 2.2.x (ou ci-dessus). 
*   Pour le protocole JETI, vous pouvez simplement entrer dans la boîte de dialogue JETIBOX, appuyer sur la touche DOWN pour passer à l'élément à modifier et appuyer sur "<" or ">" pour augmenter / diminuer la valeur.
*       N'oubliez pas d'activer l'option SAVE_TO_EEPROM dans la section 8 afin d'enregistrer les paramètres et de les réutiliser à la prochaine mise sous tension. 
*   Pour le protocole SPORT, vous devez exécuter certains scripts LUA. Dans ce paquet (dans le dossier "scripts lua"), vous pouvez trouver 3 scripts lua dans le dossier SCRIPTS / FUNCTIONS de la carte SD:
*        - Un (rstful.lua) pour réinitialiser le carburant consommé à 0 (à utiliser lorsque le réservoir est rempli)
*        - Un (tank.lua) pour définir la capacité maximale du réservoir (avant l'exécution du script, la capacité du réservoir doit être entrée dans GVAR9 pour la phase 9
*                La valeur dans GVAR9 est le nombre de 50 ml dans le reservoir. Cela signifie, par exemple, Que pour un reservoir de 1000 ml, GVAR9 doit être réglé sur 20 (= 1000/50)
*        - Un (calful.lua) pour définir les paramètres d'étalonnage 8 (4 X 2) (pior pour exécuter le script, les paramètres doivent être entrés dans GVAR9 (pour la phase 0 à 8)
*            Les premières 4 valeurs sont le débit en ml / min (min = 30, max = 800); les valeurs doivent être en ordre croissant
*            Les 4 dernières valeurs sont la correction en pourcentage à appliquer (min = -100, max = 100) pour chaque flux
*   Note: Les paramètres enregistrés dans l'eeprom ou chargés par un script LUA reprennent les paramètres définis dans la configuration.
*
* Veuillez noter que la consommation de carburant peut être sauvegardée toutes les 30 secondes dans une mémoire non volatile.
* Pour activer cette option, vous devez dire YES dans la ligne #define SAVE_TO_EEPROM de la section 8
* Si cette option est activée, à la mise sous tension OXS, il commence à compter du dernier carburant restant lorsque l'alimentation à été couper
* Sinon, OXS réinitialise la consommation de carburant et assume un réservoir de 100%.
* 
* Une réinitialisation de la consommation de carburant peut être demandée à la TX. Ceci est vraiment nécessaire lorsque SAVE_TO_EEPROM est activé
* Cela peut se faire de plusieurs façons en fonction également du protocole utilisé:
* Pour tous les protocoles, il peut être demandé à l'aide d'un signal PPM
*    Cela nécessite d'activer l'option OXS PPM (voir la section 3) et de connecter un canal Rx à OXS.
*    La réinitialisation se produira lorsque la valeur absolue du signal PPM dépasse la valeur spécifiée dans FLOW_SENSOR_RESET_AT_PPM (section 11)
* En outre:
*  - Pour le protocole JETI, il peut être demandé au JETIBOX de presser simultanément les touches "<" et ">" lorsque JETIBOX décompte le carburant restant en pourcentage
*  - Pour le protocole FRSKY SPORT, depuis OpenTX version 2.2.x, il est possible d'activer un script de fonction LUA qui enverra une commande de réinitialisation à OXS
*  
* Pour les protocoles JETI, OXS transmet automatiquement le débit actuel, le carburant restant dans ml et en% 
* Pour d'autres protocoles, vous devez demander à OXS de transmettre les données; Donc vous avez:
*   - à décommenter la ligne #define FILL_TEST_1_2_3_WITH_FLOW_SENSOR_CONSUMPTION (dans la section 2.5)
*   - à spécifier dans lequel les champs de télémétrie, TEST_1 (flux de courant en ml / min), TEST_2 (carburant restant en ml) et TEST_3 (carburant restant en%) sont envoyés (voir section 2.1 / 2.4)
*       Notez que si vous transmettez TEST_1 ... TEST_3 dans AccX ... AccZ dans le protocole FRSKY, les valeurs sont divisées par 100 par OpenTx. Vous pouvez récupérer les valeurs d'origine si vous configurez une échelle = 255 sur le côté Tx dans les champs de télémétrie
**************************************************************************************************************************************
#define A_FLOW_SENSOR_IS_CONNECTED      NO                    	// Sélectionnez entre YES , NO
#define PULSES_PER_ML                    10.0                 	// Nombre d'impulsions par milli litre (dépend du capteur); Peut avoir des décimales
#define TANK_CAPACITY                    1000                 	// Capacité du réservoir en ml
#define INIT_FLOW_PARAM  30 , 100 , 500 , 700 , 20 , 10, -5, 15 // Définir à 4 niveaux d'écoulement (en ml / min) (par exemple 30, 100, 500, 700) 4 paramètres de correction (en%; par exemple 20, 10, -5, 15); Les niveaux d'écoulement doivent être triés de bas en haut
#define FLOW_SENSOR_RESET_AT_PPM         95                   	// Lorsque la valeur absolue de ppm est supérieure à celle-ci, le compteur de débit est réinitialisé.
**************************************************************************************************************************************



****** 20 - Sequencer (ON/OFF) for several digital outputs **************************************************************************************
* OXS allows you to control (HIGH/LOW) up to 6 digitals Arduino outputs in different sequences.
* Each sequence is composed of one or several steps; each step defines for how long (= a duration) which outputs are HIGH and which outputs are LOW.
* OXS determines normally the sequence to be played based on the signal received on a PPM channel (see section 3 in order to set up a PPM signal). 
* Still there are 2 exceptions:
*    At power on or when no PPM channel is configured/received, OXS will generate as default the sequence defined in the line #define SEQUENCE_m100 (see below)
*    When a low voltage alarm is configured (see below) and if the voltage becomes low, OXS will generate the sequence defined in the line #define SEQUENCE_LOW (see below) as long as the voltage remains low   
* When a sequence has been played, OXS can or repeat it or just wait for a new sequence. The difference is made in the set up of the sequence.
* Each time a new (= different) valid PPM signal is received, OXS start immediately the corresponding sequence (even if the current sequence is not completely played)
* - In order to use the sequencer functionality, you first have to define which Arduino digital pin have to be controlled by the sequencer.
*     The arduino pins that can be controlled are the pin 13, 12, 11, 10 , 9 and 8.
*     This set up is achived by a line like : #define SEQUENCE_OUTPUTS 0b100100
*     Each bit (1 or 0 after the "b") represent an output; the least significant bit correspond to pin 8, the bit on the left pin 9 etc... up to pin 13  
*     Put 1 when the pin has to be controlled by the sequencer, 0 otherwise; In this example, it means that only pins 13 and 10 would be controlled.
*     Note: if the line #define SEQUENCE_OUTPUTS xxxx is omitted or put as comment, then the sequencer is not active at all.
*           Take care not to use the same pin for the sequencer and for another OXS funtionallity (e.g. as Tx pin, for push button, for PPM, for RPM, ...)
*           If a pin is set up with 0 (= not controlled by OXS), it will never be forced to HIGH or LOW by the sequencer even if a 1 or 0 is set up in a sequence.
*           When sequencer is activated ( SEQUENCE_OUTPUTS is defined) PPM signal is automatically used ONLY to control the sequence (so PPM can't control any more vario sensitivity, ...)
*           Current passing through the digital pins of Arduino should not exceed 40mA per pin (and 200 mA for all pins).
*               In case you want to have a higher current (which is the case for most high-power LEDs and LED strips), you need to add a transistor. Connection diagram could easily be found in Google.
* - Then you can (optionnal) specify the units used for defining the durations
*       By default, the durations are expressed in 10 milli second. A parameter allow you to increase the unit in multiple of 10 milli second;
*       E.g with a line like #define SEQUENCE_UNIT 50, the durations will become multiple of 500 milli second (= 50 * 10).
*       Note; this parameter should be an integer between 1 and 1000. So, take care that it is not possible to get a duration less than 10 msec.
*             If this line is ommitted (or as comment), the default value (1 = 10 msec) will be applied.
* - Then you have to define the sequences being used for each value of the PPM channel
*       You can define up to 9 different sequences.
*       A sequence is defined by a line like : #define SEQUENCE_m50    200 , 0b100000 , 300 , 0b000000 , 100 , 0b100100
*       Each sequence is identified by the value of the ppm signal that will activate it; suffix m100 (m = minus) means that it should be activated when ppm is about -100, m75 is for ppm = -75, 75 is for ppm = +75, etc...
*       Sequence suffix are in multiple of 25; so valid sequence suffix are only : m100, m75, m50, m25, 0, 25, 50, 75 and 100
*       Each sequence is composed of several steps (from 1 up to 126 steps or even more) (separated by ",")
*          Each step is composed of 2 parameters (also separated by ",") : a duration and a combination (LOW/HIGH) of outputs
*             - A duration can be any value between 0 and 255.
*               The value fix the minimum duration that a combination of outputs has to be applied. Duration (in msec) = value * SEQUENCE_UNIT * 10
*               So a value = 2 means a duration of 1 sec (if SEQUENCE_UNIT = 50).
*               Value = 0 has a special meaning. When OXS reachs a duration = 0, it applies the corresponding combination of outputs and keeps it for an unlimitted time.
*               This allows to force the outputs to stay with a specific combination after having played the sequence.
*               If duration = 0 is used, it should be in the last step of the sequence (because OXS will never apply the following steps).
*               If duration is set to 0 in the first step, OXS will apply directly the specific combination of outputs and keep it. 
*               If duration = 0 is not used in a sequence, OXS will automatically repeat the whole sequence after reaching the last step.
*               Note: if you need a duration that is longer than the max duration (= 255 * SEQUENCE_UNIT * 10 msec), you can put several steps with the same combination of outputs.   
*             - A combination (LOW/HIGH) of outputs defines which pins have to be set to LOW and which one to HIGH
*               A combination can be defined in binary format so setting six 1 (HIGH) and/or 0 (LOW) just after "0b" (e.g. 0b100100)
*               The least significant bit correspond to pin 8, the bit on the left pin 9 etc... up to pin 13.
*               So if SEQUENCE_OUTPUTS = 0b110111, then 0b100100 means that:
*                   - pins 13 and 10 have to be HIGH,
*                   - pins 9 and 8  have to be LOW
*                   - the others (pin 12 and 11) are not controlled by sequence because of the value assigned to SEQUENCE_OUTPUTS = 0b100111 
*      So #define SEQUENCE_m50    2 , 0b100000 , 3 , 0b000000 , 1 , 0b100100 means (assuming that SEQUENCE_OUTPUTS = 0b100100 and SEQUENCE_UNIT = 50, ):
*            - set pin 13 to HIGH and  pin 10 to 0 (= 0b100000) when PPM signal becomes -50
*            - then wait at least for 2 * 50 * 10 = 1000 ms = 1 sec before changing the outputs
*            - after 1 sec, set pin 13 ( and pin 10) to LOW (=0b000000) for a duration of 1.5 sec (3 * 50 * 10)
*            - after 1.5 sec, set pin 13 and 10 to HIGH for a duration of 0.5 sec (1 * 50 * 10)
*            - after 0.5 sec repeat first step (pin 13 HIGH for 1 sec)
*            - continue with next steps
*      Note: when a sequence name is not defined, OXS handles it like it would be defined with 0 , 0b000000 (so no repeat, all outputs LOW)
* - Finally you can (but it is not mandatory) set up the condition(s) for a low voltage detection. When a voltage becomes too low, OXS starts automatically SEQUENCE_LOW (and discard PPM channel)
*     A low voltage condition can be set up based on 1 or 2 voltage(s):
*         - the voltage on the Arduino pin defined by the 6th parameter PIN_VOLTAGE; this set up is achived by a line like : #define SEQUENCE_MIN_VOLT_6 6000 where 6000 is the "low" voltage in mVolt.
*           Note: if you use this option, do not forget assign a pin number to the 6th parameter in #define PIN_VOLTAGE and to fill (if requested) the 6th parameter of other voltage parameters.        
*                 The pin defined in the 6th parameter of PIN_VOLTAGE can be the same as another parameter in PIN_VOLTAGE; this can be useful if you want to set up low voltage parameters too.
*         - the lowest lipo cell voltage; this set up is achived by a line like : #define SEQUENCE_MIN_CELL 3000 where 3000 is the "low" voltage in mVolt.
*           Note: if you use this option, do not forget to define the other voltage parameters PIN_VOLTAGE , etc ... and NUMBEROFCELLS        
*     Note:  when no one low voltage parameter is defined, OXS will not automatically start SEQUENCE_LOW.
*            when both voltage parameters are defined, OXS will automatically start SEQUENCE_LOW as soon as one of the 2 voltages becomes low.
*            If you want that OXS notifies a low voltage detection do not forget to also define SEQUENCE_LOW (otherwise, OXS will just set all output pins to LOW)
*            If you have telemetry, you can also make a set up on Tx side in order to detect a low voltage and then send a specific value on the ppm channel.
*               In this case you do not have to define the set up in OXS and the same device can be used on several model.
************************************************************************************************************************
 
**** xx - Reserved for developer. **************************************************************************************
* DEBUG must be activated here when you want to debug one or several functions in some other files.
* Enabling DEBUG will allow to use Arduino Serial Monitor at 115200 baud (or 38600 when GPS is activated) to see init data and some live sensor values
* You can then select the parts that you want to debug by uncommenting the specifics DEBUG parameters you want in each file
* Note: OXS allows to transmit 3 fields named TEST1, TEST2, TEST3. You can fill those fields with whatever you want where you want if you want to transmit additional data to the Tx.
* Just fill in test1.value (or 2, 3) with an int32_t and test1.available (or 2, 3) with true and add those OXS measurements in the data to be sent section. 
************************************************************************************************************************
//#define DEBUG