/*
*************************************************************************************
*                                  Information importante:
*  Ne pas modifier ce fichier a mois de vouloir y a jouter vos propre commentaire.
*  Ce fichier a pour but de fournir des explications pour le fichier oXs_config.h.
*  La configuration doit être faite dans le fichier oXs_config.h
*************************************************************************************

Voir le github OpenXsensor https://github.com/openXsensor/
Ecrit par par Rainer Schlosshan traduction Thierry ZINK

************************************************************************************************************************
************************************* description général de toute les options *****************************************
************************************************************************************************************************
*  le fichier oXs_config.h permet aux utilisateurs de configurer différentes options. Voici la liste des différentes options.
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
*  7 - Réglage capteur RPM (tour par minute) (optionnel))
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



**** 1 - Choix du protocole de télémétrie **********************************************************************
* Actuellement oXs supports 4 protocoles de télémétrie: Multiplex , Frsky, Jeti and Hott (=Graupner)
* Un et uniquement un protocole peut-être sélection; Exemple si vous active le protocole Multiplex, les protocoles Frsky, Jeti et Hott sont désactivé (et vice et versa) 
* FrSky utilise 2 protocoles différents :
*       - SPORT utilisé par les récepteurs de la série X (comme le X8R ou X6R)
*       - HUB utilisé par les récepteurs de la série D (comme le D4R-II)
*      oXs peut supporter les deux protocoles FrSky en même temps (en utilisant l’option FRSKY_SPORT_HUB). oXs peut automatiquement détecter le type de récepteur auquel il est connecté
*      Si vous utilisez cette option vous n’aurez pas besoin de reprogrammer votre oXs si vous passez d’un récepteur série X à série D.
*      l’auto détection ne semble pas marcher a 100% et demande plus de mémoire…
*      Dans le cas de l’auto détection ne fonctionne pas ou si vous avez besoin de plus de mémoire (car vous utilisé beaucoup de capteur connecté à votre oXs), vous pouvez forcer sur le protocole FRSKY spécifique que vous utilisez
*  Pour sélectionné le protocole utilise par oXs remplir la ligne #define PROTOCOL avec le nom du protocole sélectionné avec l’une des valeurs suivante: FRSKY_SPORT , FRSKY_HUB , FRSKY_SPORT_HUB , MULTIPLEX , HOTT; JETI
************************************************************************************************************************
#define PROTOCOL FRSKY_SPORT    // select between FRSKY_SPORT , FRSKY_HUB , FRSKY_SPORT_HUB , MULTIPLEX , HOTT, JETI


*  1.1 - Sélection de la sortie vers le récepteur ***********************************************************************************************
*   oXs doit être connecté a récepteur afin de transmettre les informations.
*   Pour cela, une sortie DIGITAL de l’arduino doit être connectée au récepteur.
*   Vous devez spécifier la PIN arduino utilisé pour cette fonction.
*   Valeur par défaut: 4 ; Les valeurs autorisées sont 2 ou 4 mais faite attention de ne pas utiliser la même PIN pour 2 fonction. 
************************************************************************************************************************
#define PIN_SERIALTX  4

*  1.2 ****** SPORT_SENSOR_ID (utilisé unique pour le protocol FrSky Sport) **********************    
*  
*      Pour le protocole SPORT, il peut y avoir plusieurs capteur connecté sur le même bus  mais chaque capteur doit avoir un identifiant « SPORT_SENSOR_ID » diffèrent.
*           Pour le protocole SPORT, oXs peu utilisé jusqu’à 6 identifiant. Les 6 SPORT_SENSOR_ID utilisé par oXs sont :    
*               DATA_ID_VARIO  	0x00  	// = sensor 0 utilisé pour altitude et la vitesse vertical
*               DATA_ID_FLVSS  	0xA1 	//		    1 utilisé pour la mesure de élément de baterie
*               DATA_ID_FAS		0x22  	//         	2 utilisé pour vfas, courant et carburant
*               DATA_ID_GPS		0x83  	//          3 utilisé pour les données GPS
*               DATA_ID_RPM		0xE4  	//          4 utilisé pour les rpm, T1, T2, et la sonde de pitot
*               DATA_ID_ACC		0x67  	//          7 utilisé poue Acceleration sur X, Y, Z
*               Si besoin (par exemple pour connecter 2 oXs envoyant deux donné identique), vous pouvez changer les valeur de SPORT_SENSOR_ID mais vous devez uniquement selectioner des adresse parmie les valeur suivante:
*                  0x00,0xA1,0x22,0x83,0xE4,0x45,0xC6,0x67,0x48,0xE9,0x6A,0xCB,0xAC,0x0D,0x8E,0x2F,0xD0,0x71,0xF2,0x53,0x34,0x95,0x16,0xB7,0x98,0x39,0xBA,0x1B
*               les valeurs du paramétre SPORT_SENSOR_ID sont défini dans le fichier oXs_config_advanced.h (section 1.1)
************************************************************************************************************************



**** 2 - Data to transmit **********************************************************************************************
*   selon les paramètres selection dans le fichié config.h (et les capteur connecté à oXs), oXs peux calculer plusieurs messures 
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
*      Pour activé cette alarme, dé commenter la ligne ci-dessous (en milliVolt) pour regler le seuille d'alarme
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
*   Si vous dé commenté plusieur ligne, prenez garde a ne pas utilisé plusieur fois le même parametre.
**************************************************************************************************************************   
//#define FILL_TEST_3_WITH_EXPECTED_ALT                        	// Dé commenté cette ligne si OXS doit calculer une altitude souhaité basé sur l'altitude actuel, Vitesse vertical et acceleration verticale
//#define EXPECTED_ALT_AT_SEC 0.2                               // temps (en sec) pour l'altitude souhaité (Cette ligne doit etre dé commenter si la précedente est dé commenté)
//#define FILL_TEST_1_2_3_WITH_LINEAR_ACC                     	// Dé commenté cette ligne si OXS doit replire les parametres TEST_1, TEST_2, TEST_3 avec l'acceleration lineaire
//#define FILL_TEST_1_2_WITH_VSPEED_AND_ALT_FROM_SECOND_VARIO  	// Dé commenté pour activé cette option
//#define FILL_TEST_1_WITH_DTE                                 	// Dé commenté pour activé cette option
//#define FILL_TEST_2_WITH_PPM_AIRSPEED_COMPENSATION           	// Dé commenté pour activé cette option
//#define FILL_TEST_1_WITH_YAWRATE                             	// Dé commenté pour activé cette option
//#define FILL_TEST1_WITH_HEADING_FROM_MAGNETOMETER             // Dé commenté pour activé cette option
//#define FILL_TEST_3_WITH_FLOW_SENSOR_CONSUMPTION             	// Dé commenté pour activé cette option
************************************************************************************************************************



**** 3 - PPM settings **************************************************************************************************
*   OXS peux (option) changer certain parameter et/ou rester certain donner a partir d'action sur l'émmeteur
*   Pour tous les protocole, cela peux etre réaliser a partir du sortie servo du recepteur pour recevoir les informations de l'émetteur.
*   Cette fonction permet depuis l’émetteur:
*      - de changer la sensibilité du vario en utilisant ou un potard ou un interrupteur de votre émetteur.
*      - de changer entre un vario compenser et non compenser (quand on utilisant 2 capteurs barométrique avec un connecté sur une sonde TEK)
*      - de réinitialiser le capteur de vitesse (quand la valeur décale avec la température)
*      - de changer le facteur de compensation du vario (quand le vario compensé utilise le capteur de vitesse) en utilisant un potentiomètre ou un interrupteur de l’émetteur.
*   Ces différentes fonctions demandent des mixages dans l’émetteur et certaine configuration dans le fichier oXs-config.h (voir ci-dessous).
*
*   Si vous voulez ces fonctions vous devez;
*      - vous devez dé commenter (en suppriment les // devant la ligne) les 3 lignes suivantes
*      - Spécifié à la ligne PIN_PPM, la PIN Arduino qui sera connecté à la sortie servo du récepteur
*            Par défaut: 2 ; vous pouvez aussi connecter la PIN 3 pour lire le signal PPM.
*            Fait bien attention d’utiliser la pine sélectionné (2 ou 3) uniquement pour le signal PPM.
*            Garder cette ligne commentée si vous ne voulez pas utilisé cette fonction.
*      - spécifié a la ligne PPM_MIN_100 la largeur d’impulsion (en micro seconde) quand la voie envoie la valeur “logique” = -100.
*              La valeur par défaut est 988.
*      - spécifié a la ligne PPM_PLUS_100 la largeur d’impulsion (en micro seconde) quand la voie envoie la valeur “logique” = +100.
*              La valeur par défaut est 2012.
*   Les deux dernières valeurs sont utilisées afin de calibrer oXs avec l’émetteur quand la fonction PPM est utilisée
*   Il est probable que si vous utilisez les valeurs par défaut cela fonctionne.
*   Il est plus sûr pour le bon fonctionnement de calibrer ces paramètres (uniquement si vous utilisé la fonction PPM).
*   /!\ TRADUCTION A REVOIR :To do so, ensure that you transmit the OXS measurement field "PPM" to the TX (e.g. as T1).
*   /!\ TRADUCTION A REVOIR :Note the values on TX (e.g. in field T1) when the TX send respectively -100 and +100 on the PPM channel (use a switch or a pot).
*   /!\ TRADUCTION A REVOIR :Fill in those values in those parameters and reload your program in the Arduino.
*
*   Si vous utilisé le protocol FRSKY SPORT (uniquement pour récepteur X) avec openTX 2.2(ou dessus) vous pouvez envoyer le signale PPM sans avoir a connecté une sortie servo du récèpteur.
*   OpenTx 2.2 vous permet de lancé un script LUA qui vous permet d'envoyer des valeur depuis votre émeteur vers OXS
*   Pour activé cette option vous devez:
*     - dé commenté la ligne #define PPM_VIA_SPORT
*     - lancer le script LUA qui vous perment d'envoyer via le protocol SPORT le même type de valeur qu une voie due recepteur. (cette valeur est comprise entre -100 and 100)
*       Ce script LUA doit utilisé une commande comme ceci: 
*            local ret = sportTelemetryPush( 0x0D , 0x10 , 0x0010 , -52 )
*       ou    
*          - 0xOD : est le numero d'identification utilisé (defini dans les fichier oXs_config_advanced.h)
*          - 0x10 : est une valuer non utilisé
*          - 0x0010 : est le code utiliser pour identifier qui s'agit d'une valeur PPM
*          - -52 : est la valeur PPM transmise (-52 est juste un exemple; elle doit etre adapter selon la valeur que on veux transmetre) 
*      vous pouvez trouvé un exemple de script LUA dans ce package (dans le dossier "lua scripts"). le nom du fichié est Name is oxsppm.lua et doit etre instaler dans le répertoire SCRIPTS/FUNCTIONS de la carte SD.
************************************************************************************************************************
#define PIN_PPM             2 
#define PPM_MIN_100       988   
#define PPM_PLUS_100      2012   
#define PPM_VIA_SPORT             // dé commenté cette ligne pour recevoir un signal PPM par le protocol SPORT a la place d'une sortie servo du récepteur (Cela nécésite un émeteur equiper d'openTX et contenant le script en LUA)
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
*     Quand on utilise deux capteur barométrique, oXs peux transmettre une valeur moyen de vitesse vertical des deux capteurs. Ceci ne semble pas très utile.
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
*     Quand un vario est utilisé, oXs peux prendre certain paramètres en compte pour ajuster la sensibilité 
*     La sensibilité peux être régler entre les valeurs:
*         - 20 (réglage conservatif, temps de réaction = plusieurs second)                   
*         - 150 (rapide mais beaucoup d’erreur, temps de réaction = bien inferieur a la second)       
*        50 est une valeur de réglage pour mesurer de petite vitesse vertical (recherché d’ascendance en planeur); ce réglage donne une réaction autour de la seconde.
*     la sensibilité peux être régler par programmation et/ou par l’émetteur.
*     
*     Si vous voulez une réponse très rapide du vario la meilleur solution est d’utilisé un capteur MPU6050 (accéléromètre/gyroscope) en complément du capteur barométrique (voir ci-dessous).
*     Note: Quand le vario est utilisé (#define VARIO dé commenter), vous avez a spécifié (dans la section 9) comment la vitesse vertical est calculer.
*
*     Le réglage de la sensibilité utilise 4 paramètres:
*        SENSIVITY_MIN = Cette sensibilité est la sensibilité basic du vario.
*                        Ce paramètres est donc le paramètres principale du control de sensibilité du vario.
*                        Cette valeur est utilisé par oXs quand la différence entre la valeur de la Vitesse vertical actuel (calculer toute les 20 ms) et la précédente valeur de la vitesse vertical est inferieur a la valeur définit par SENSITIVITY_MIN_AT ; la valeur typique est 40.
*        SENSIVITY_MAX = c’est la sensibilité maximal utiliser quand la vitesse vertical change très rapidement. 
*                        Cette valeur est utilisé par oXs quand la différence entre la valeur de la Vitesse vertical actuel et la précédente valeur de la vitesse vertical est supérieur a la valeur définit par SENSITIVITY_MAX_AT ; la valeur typique est 300.
*        SENSITIVITY_MIN_AT = définie jusqu’à quelle valeur de SENSITIVITY_MIN est applicable (voir ci-dessus) (cm/s); la valeur typique est 100 (cm/s).
*        SENSITIVITY_MAX_AT = définie jusqu’à quelle valeur de SENSITIVITY_MAX est applicable (voir ci-dessus) (cm/s); la valeur typique est 1000 (cm/s).
*           La sensibilité du vario est une interpolation automatique entre SENSITIVITY_MIN et SENSITIVITY_MAX.
*     Ces paramètres permettent à oXs d’avoir un temps de réaction rapide quand la vitesse verticale change rapidement et d’avoir un vario silencieux dans des condition calme.
*     Note: SENSITIVITY_MAX peut-être égale au paramètre SENSITIVITY_MIN si aucun adaptation automatique est désiré en cas de changement rapide. 
************************************************************************************************************************
#define SENSITIVITY_MIN 50
#define SENSITIVITY_MAX 300
#define SENSITIVITY_MIN_AT 100
#define SENSITIVITY_MAX_AT 1000
************************************************************************************************************************
* 4.4 - Sensitivity adjusted from the TX *******************************************************************************
*     la sensibilité peux être ajusté depuis l’émetteur grâce a un interrupteur ou un potentiomètre et quelque mixage sur un sorti servo du récepteur.    
*     Pour utilizer cette option, une sortie servo doit etre connecter à oXs (voir « Définition du retour émetteur » dans la section 3)
*     Quand oXs reçoit un signal de l’émetteur, la valeur SENSIVITY_MIN définie dans la section 4.3 est supprimer et remplacer par la valeur calculer basée sur la valeur défini dans cette section et le signal PPM de l’émetteur.
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
*     - dé commenté les trois prametres suivant
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
*     Note: Dans cette version de oXs, si vous voulez envoyé les parameters calculé, vous devez definir d'envoyer les paramètres de la facon suivante:
*         - TEST1 pour le temps écoulé (en 1/10 de sec) 
*         - TEST2 pour la vitesse vertical moyen (en cm/sec comme al vitesse vertical)
*         - TEST3 pour la finesse du planeur (en 1/10 d'unité)
*     Le calcul de finesse du planeur nécésite au tube de pitot et un vario;
*     Moyenne du tot de monté/descente peux etre calculer sans sonde de pitot( mais la  correction par la vitesse ne pourat pas ce faire)
*     Afin d'activer la fonction vous devez decomenté et remplire les quatres lignes suivante.
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
* The easiest is to configure a TX mixer that will generate a pulse of e.g. 1 sec with this value (e.g. 100).
* Please note that the request for recalibration should be send only when the airspeed (on the probe) is really zero otherwise it would generate wrong measurements.
*
* oXs can calculate 2 types of airspeed:
*  - the first one is an airspeed based on air density at 15 Celcius degree and 1013 hPa (normalised sea level). It is a normalised airspeed (indicated airspeed) as normally used on aircraft.
*    So e.g. the stall speed does not change with altitude
*  - the second one take into account the pressure (provided by a baro sensor) and the initial airspeed sensor temperature in order to calculate a "true" airspeed (to be compared with a GPS speed when wind is null)  
* The normalised airspeed is calculated when line #define AIRSPEED_AT_SEA_LEVEL_AND_15C is uncommented. To get the "true" airspeed, put this line as comment
* 
* oXs can send the airspeed in (1/10) of knot/h or in (1/10) km/h. For openTx, you normally must use the knot/h option (only some old 2.1.x versions requires km/h option).
* To activate the km/h option, activate this line #define AIRSPEED_IN_KMH
* Put this line as comment to activate the knot/h option
*
* oXs can also use the airspeed measurements in order to calculate a compensated vario named PRANDTL_DTE (=delta total energy).
* See the web for more informations about dTE (= electronically compensated vario).
* The principle is to try to detect only true air movement by neutralising the up and down resulting from elevator changes.
* Normally, dTE has to be transmitted as Vspeed (vertical speed = defaultfield) because OpenTX does not yet support a specific field for it.
*
* When the option PPM is implemented, it is also possible to adjust the compensation factor between 2 values.
* This requires to uncomment 4 parameters:
*     COMPENSATION_MIN_AT_PPM  specify the PPM value in order to set the compensation to the value specified by COMPENSATION_PPM_MIN; default = 60.
*     COMPENSATION_MAX_AT_PPM  specify the PPM value in order to set the compensation to the value specified by COMPENSATION_PPM_MAX; default = 90.
*     COMPENSATION_PPM_MIN     minimum compensation; in % ; default 80 
*     COMPENSATION_PPM_MAX     maximum compensation; in % ; default 120
************************************************************************************************************************
#define AIRSPEED_SENSOR_USE  MS4525
#define AIRSPEED_AT_SEA_LEVEL_AND_15C // if this line is commented, airspeed is calculated using baro pressure and temperature (so being "true" airspeed instead of normalised airspeed)      
//#define AIRSPEED_IN_KMH  // uncomment this line if airspeed has to be in km/h instead of knot/h ( except some old versions, openTx expects knot/h) 

#define AIRSPEED_RESET_AT_PPM   100

#define COMPENSATION_MIN_AT_PPM 60
#define COMPENSATION_MAX_AT_PPM 90
#define COMPENSATION_PPM_MIN  80
#define COMPENSATION_PPM_MAX 120


**** 6 - Voltage measurements and current sensor settings (optional) ***************************************************
*
* 6.1 - Voltage Reference to measure voltages and current **************************************************************
*     Current and Voltages can be measured in (1023) steps or from VCC , or from an 1.1 internal reference or from an external reference.
*     If VCC is very stable, it is probably more accurate and easier to measure current and voltages based on VCC (so in 1023 steps from VCC).
*     Still this requires that the voltage applied on Arduino "RAW" pin is regulated or at least always more than 5.5 volt (in order to let the Arduino board voltage regulates it at 5 volt).
*     If voltage on "Raw" pin is less than 5.5 volt and changes (e.g. due to servo consumption or low battery) then current and voltage measurements will become wrong.
*     If VCC can't be very stable (e.g. Arduino is powered via RX by a 4.8 NiMh) and you need only the voltages (no need for the current measurement), then it is possible to measure based on the Arduino 1.1 internal voltage reference.
*     If you need current measurement too, using internal voltage is not "the" solution because current sensor needs stable voltage too.
*     Take care that voltage dividers (see below) must be setup in order that Arduino analog pin voltage do not exceed VCC or 1.1 volt or the external voltage depending on the option you choose.
*     - Uncomment the "#define USE_INTERNAL_REFERENCE" to activate the 1.1 internal voltage reference (otherwise, measurements will be based on VCC).
*     - Uncomment the "#define USE_EXTERNAL_REFERENCE" to activate the external voltage reference (otherwise, measurements will be based on VCC).
*        Note: in order to use an external reference, you have to use add a voltage reference device to your Arduino. This is quite difficult on Arduino pro mimi because the AREF pin is not available on the pin headers
*     - In order to get best measurements, you can setup the voltage reference being used with line #define REFERENCE_VOLTAGE.
*       Value must be defined in millivolt (e.g. 5100 for 5.1 Volt) and may not contains decimals;
*       If ommitted, oXs will automatically apply 1100 (when USE_INTERNAL_REFERENCE is defined) and 5000 (othewise)
*       When external reference is used, REFERENCE_VOLTAGE must be specified
************************************************************************************************************************
//#define USE_INTERNAL_REFERENCE
//#define USE_EXTERNAL_REFERENCE  // uncomment this line if you use an external reference instead of Vcc
#define REFERENCE_VOLTAGE 5000

* 6.2 - Voltages parameters *******************************************************************************************
*     oXs can measure up to 6 input voltages (please note that depending on manufacturer, some Arduino pro mini have less analog pins available)
*     In order to measure voltages, you :
*       - must specify the Analog pins (A0 to A7) connected to a battery (e.g. a multicell lipo) or to a sensor (e.g. a temperature sensor convert the temperature in a voltage that can be measured)
*       - must specify the values of resistors being used for the voltage dividers (see below)
*       - can specify offset and/or scaling to apply
* 
*     Note : one analog pin can also be used to measure a current using a current sensor; the set up for a current sensor is described in section 6.4 (see below);
*          Do not use the same analog pin to measure a voltage and a current.
*     Take care : do NOT use pins A4 and A5 if you use a vario or an airspeed (those pins are reserved for the barometric and pressure sensors).
*     
*    The pins being used to measure some voltages are defined in line #define PIN_VOLTAGE. It is better to put this line as comment (having "//" in front) if no one voltage has to be measured.  
*     When used, this line must contains 6 values (separated by comma); the first value is used to measure VOLT1, the second VOLT2, ...up to VOLT6 
*     Each value must be a number from 0 up to 7 (0 means A0 = analog 0, 1 means A1, ...7 means A7) or the value "8" (when a voltage does not have to be measured).
*     Note: the same alalog pin values can be used in several voltages (e.g. for VOLT1 and VOLT6).
*     
*     Note: If lipo voltage to be measured, it MUST start from VOLT1 for 1s, VOLT2 for 2s....., other voltages may be measured but need to be after number of cells to be measured
*         and the cel number MUST be specified in "numberofcells" (see next section)
*         The Ax pin number can be at random depending on which pin is connected for each cell.
*         
*  !! Take care that the voltage applied to Arduino pin may not exceed Vcc (normally 5 volt) or 1.1 volt (if internal reference voltage is used).
*     It can be that you have to use voltage divider in order to reduce the voltage applied on Arduino pin compared to the voltage you want to measure.
*     For each voltage to scale down, proceed as follow:
*      - make a divider with 2 resistors 
*
*                 ------>  End point  = connect to the device to measure (battery, Current sensor, ...) 
*                 |
*               __|__   
*              |     |
*              |     |   R2 (Resistor_To_Voltage)          
*              |     |              
*              |_____|          
*                 |
*                 |------> mid point = connect to Arduino pin A0,A1,A2,A3, A6 or A7
*               __|__   
*              |     |
*              |     |   R1 (Resistor_To_Ground)          
*              |     |              
*              |_____|          
*                 |
*                 ------>  connect to Ground
*        Note: a capacitor (e.g. 100nf) could be added too between ground and Arduino Analog pin in order to get more stable values.
*         
*      - R1 and R2 are chosen to make sure that voltage apply to Arduino is quiet close to ( but never exceed) VCC or 1.1 volt depending on your choice regarding the current & voltage measurement (see here above)
*      - Volt on Arduino pin = VCC (or 1.1 volt) = "max voltage to measure from this sensor" * R1 / (R1 + R2)
*      - R1 could be 10 kOhm; so R2 = R1 * ( ( "max voltage to measure from this sensor"  / VCC [or 1.1 depending on the reference] ) - 1 )
*         e.g. using 1.1 internal voltage reference and in order to measure max 6 volt with R1 = 10000, then R2 = 10000 * (( 6 / 1.1 ) - 1) = 45545 Ohm; best rounded up to high available value e.g 47000 ohm
*
*    The resistors used in voltage dividers have to be specified in lines #define RESISTOR_TO_GROUND and #define RESISTOR_TO_VOLTAGE.
*    Eeach line, must contains 6 values (one for each voltage); each value represent the resistance in Ohm or kOhm but use the same unit for both lines. 
*    If no divider is used for some voltage, set both resistors to 0 (zero)
*    If no divider at all is used, lines can be commented (adding "//" in front)
*     
*     In order to get best voltage measurements, oXs can be calibrated. This optional process allows to compensate tolerances on resistors and on Arduino ADC (analog to digital converter).
*     To calibrate each voltage measurement proceed as follow in order to find the best values to be set up in lines #define OFFSET_VOLTAGE  and #define SCALE_VOLTAGE
*      - set parameters in oXs_config.h  
*            - letting  OFFSET_VOLTAGE = 0 and SCALE_VOLTAGE = 1 (so adding no compensation)
*            - selecting a field to transmit the desired voltage (e.g. T1 for VOLT3) and fill the line  "#define SETUP_DATA_TO_SEND" accordingly 
*      - load the program in Arduino 
*      - apply several different voltages on End point (not exceeding the max voltage allowed based on R1 and R2)
*      - for each applied voltage, measure the applied voltage with a voltmeter and read the value received on telemetry panel on Tx  
*      - set the values in excel (or on a graphic) and calculate the best values OFFSET_VOLTAGE and SCALE_VOLTAGE (using a linear regression)
*     If this seems too complex, just use 2 voltages as different as possible (but in the range of the normal values you want to measure)
*     and apply calculation based on following example:        .
*      I expect voltage to be normally between 4 volt and 6 volt, so I apply 2 voltages close to those values to End point
*        - for first voltage, voltmeter gives 3510 millivolt and telemetry gives 3622
*        - for second voltage, voltmeter gives 5900 millivolt and telemetry gives 6013
*      Then SCALE_VOLTAGE = (5900-3510) / (6013-3622) = 0.99958
*      and OFFSET_VOLTAGE = -3510 + (3622 * 0.99958 ) = 110
*  Note: You can apply the same principle if you are measuring something else than a voltage.
*         E.g. You can calibrate a temperature sensor and set the calibration parameter in order to get degree on the TX display.
*     
*     Line #define OFFSET_VOLTAGE  and #define SCALE_VOLTAGE are optional (can be put as comment adding "//" in front)
*     If defined, each line must contains 6 values. Use 0 (for offset) and 1 (for scale) if no calibration is done for some voltage.
*     
************************************************************************************************************************
                           VOLT1  VOLT2  VOLT3  VOLT4  VOLT5  VOLT6 
#define PIN_VOLTAGE         2    , 0    , 2   ,  3 ,     8 ,    8             
#define RESISTOR_TO_GROUND 12   , 20 ,   30 ,   40 ,    50 ,   60           // set value to 0 when no divider is used for one of this voltage
#define RESISTOR_TO_VOLTAGE 50, 100.1 , 200,   300 ,   500 ,  600           // set value to 0 when no divider is used for one of this voltage
#define OFFSET_VOLTAGE      0 ,    0 ,    0 ,    0 ,     0 ,    0           // can be negative, must be integer
#define SCALE_VOLTAGE       1 ,    1 ,    1 ,    1 ,     1 ,    1           // can be negative, can have decimals


* 6.3 - Max number of Lipo cell to measure  (and transmit to Tx) ***********************************************************
*     The different voltages measured under 6.3 are all related to the ground.
*     oXs can use some of them to calculate the voltage of some lipo cells.
*     Define here the max number of cell voltages of a lipo you want to transmit; value can be 0 (no cells),1,2,3,4,5,6.
*     If a value greater than 1 is defined, then the OXS will calculate the voltage of each cell based on the difference between 2 successive voltages starting from Voltage1.
*     The total voltage of all cells will be calculated on TX side summing all cell voltages again.
*     TX will display the total voltage in a telemetry field named "Cells".
*     TX will also identify the cell with the lowest voltage and display it in a field named "Cell".
*     TX has also a special screen where all voltages will be displayed (see Taranis manual).
*     E.g. if number of cells = 3, 
*           voltage on cell 1 will be voltage measured on the first pin defined in PIN_Voltage
*           voltage on cell 2 will be the difference between voltages measure on second pin and first pin (so VOLT2 - VOLT1)
*           voltage on cell 3 will be the difference between voltages measure on third pin and second pin (so VOLT3 - VOLT2)
*           etc.
*     When transmitting cell voltages, you may NOT FORGET to configure the PIN_VOLTAGE, RESISTOR_TO_GROUND, RESISTOR_TO_VOLTAGE (and optionaly the calibration parameters too) .
*     The pins MUST start and sequenced from VOLT1,2,3,4.. for 1s, 2s,....
*     Pins voltage in excess may be used in order to transmit other voltages (e.g. from a temperature sensor)
*     E.g. if NUMBEROFCELLS = 3, First pin (in the list of 6) must be connected to cell 1 (via a voltage divider calculated for about 4.5 volt
*                                Second pin must be connected to cell 2 (via a voltage divider calculated for about 9 volt
*                                Third pin  must be connected to cell 3 (via a voltage divider calculated for about 13 volt
*                                Other pins may still be used for other data (temperature, current, ...)
*     Notes: You must use voltage dividers to scale down the voltages on each pin of the lipo balance plug
*            If you use the 1.1 internal reference, you can set all R1 = 10 kOhm. Then R2 could best be
*                   33 kOhm for Voltage1, 68 kOhm for Voltage2, 120 kOhm for Voltage3 and 150 kOhm for voltage4
*            Please note that the more cells you have the more inaccurate the measurements become specially if you do not calibrate the voltages.
*            Probably, it make no sense to measure more that 3 or 4 cells individually
*            If you don't want to transmit cell voltage, set value to 0 (zero) or comment the line.
*            This parameter defines the max number of cells you expect to transmit. 
*            If OXS is connected to a lipo having less cells, OXS will automatically reduce the number of cells which still let the TX calculate the total voltage and the lowest cell voltage 
************************************************************************************************************************
#define NUMBEROFCELLS    3 

// 6.4 - Convert voltage to temperature (° Celcius) ********************************************************************     
*     In order to measure a temperature it is possible to use special electronic component (like lm35) that generates a voltage proportional to the teperature.
*     In this case, you can just connect the output of this component to an arduino analog pin and configure oXs in section 6.2 in order to measure the voltage.
*     Adding an offset and a scaling (see section 6.2) accordingly with the characteristic of you component, oXs can directly calculate the temperature in the defined measurement Voltage. 
*     You can then ask oXs to transmit this voltage to the desired telemetry field (e.g. T1) filling a set up as explained in section 2.
*     
*     Still this kind of component is not foreseen to measure high temperature (e.g. higher than 150 °C).
*     You can then use thermistors (= NTC). 
*     The drawback of NTC is that the generated voltage is not proportional to the temperature and it requires some extra calculations.
*     oXs let you use one or several NTC (e.g. one per cylinder) in order to measure high temperature(s).
*     For each NTC, you have to add a resistor in serie accordingly to this schema 
*           < Arduino Vcc > --[serie resistor]-- <Arduino analog pin>  --[NTC]-- <ground>
*     If you use several NTC, all NTC and resistors have to be identical and they have to be connected to Arduino analog pin that are consecutive (as defined in PIN_VOLTAGE)
*     
*     oXs reuses the mVolt calculated by oXs. The config must be adapted in a such a way that this mVolt is equal to the raw value returned by the ADC * 1000 (for better accuracy)
*     Therefore, in section 6.1 and 6.2 ,
*          USE_INTERNAL_REFERENCE must be as comment (so with // in front off)
*          USE_EXTERNAL_REFERENCE must be as comment (so with // in front off)
*          REFERENCE_VOLTAGE must be as comment (so with // in front off)
*          RESISTOR_TO_GROUND must be set on 0 (for the index being used)
*          OFFSET_VOLTAGE must (normally) be set on 0 (for the index being used)
*          SCALE_VOLTAGE  must be set on 204.6 (=1000 * 1023/5000) (for the index being used)     
*     Then you have to define some parameters in section 6.4     
*        FIRST_NTC_ON_VOLT_NR specify index of first voltage being used for conversion to temperature (e.g. 3 means VOLT_3) (so it is not the code of analog pin; this one is define in section 6.2)
*        LAST_NTC_ON_VOLT_NR  specify index of last voltage being used for conversion to temperature (e.g. 5 means VOLT_5)
*          In this expample, it means that you expect to measure 3 temperatures based on NTC connected to the pins used for VOLT_3, VOLT_4 and VOLT_5
*          note: if you use only one NTC, you must specify the same value for FIRST_NTC_ON_VOLT_NR and for LAST_NTC_ON_VOLT_NR
*                if you do not use NTC, keep this line as comment
*        SERIE_RESISTOR       specify the resitance (in Ohm)  connected between Arduino Vcc and NTC (and analog pin); select a value nearly equal to the resistance of NTC in the range of temperature you expect best accuracy 
*        Next 3 parameters are specific to the NTC you use. Values between brackets are nominal value for a cheap NTC available on aliexpress.com with 100k ohm at 25°C and a Beta coefficient of 3950 for range 25/50
*        STEINHART_A          (e.g. 7.00111E-4 )   
*        STEINHART_B          (e.g. 2.1644E-4 )
*        STEINHART_C          (e.g. 1.0619E-07 )
*        If you do not know those 3 parameters, you can caluclate them measuring the NTC resistance at 3 different temperatures. Use then e.g. the formula given on wikipedia for thermistor
*     When you use 1 or 2 NTC, the temperature(s) will be registered in VOLT_X and VOLT_Y where X is the value of FIRST_NTC_ON_VOLT_NR and Y the value of LAST_NTC_ON_VOLT_NR
*     If you use more than 2 NTC, oXS will register in :
*         VOLT_X the lowest temperature 
*         VOLT_X+1 the index of the highest temperature (1 being the index of the first NTC) 
*         VOLT_Y the highest temperature
*     You can then define how to transmit those data in section 2
************************************************************************************************************************
//#define FIRST_NTC_ON_VOLT_NR 5 // uncomment this line when thermistor are used; specify index of first voltage being used for conversion to temperature (e.g. 5 means VOLT_5)
#define LAST_NTC_ON_VOLT_NR 6 // specify index of last voltage being used for conversion to temperature (e.g. 6 means VOLT_6)
#define SERIE_RESISTOR 4700 // resitance connected to Arduino Vcc (in Ohm)
#define STEINHART_A 7.00111E-4   
#define STEINHART_B 2.1644E-4
#define STEINHART_C 1.0619E-07




* 6.5 - Current sensor  parameters   **************************************************************************************
*     It is possible to measure a current (and current consumption) if a current sensor is connected.
*     Connecting a current sensor is an optional feature.
*     It requires some additional hardware. It can be an IC like ACS712 (for 5, 20, 30 amp) or ACS758 (for 50, 100, 150, 200 amp).
*     Most sensors can read bidirectional currents but ACS758 has "U" types that read only unidirectional current (providing then an higher sensitivity).
*     Those current sensors are quite cheap (see e.g. ebay) and return a voltage that depends on the current. This voltage is measured by OXS via an analog pin.
*     The Pin value to enter in the oXs_config.h is a number from 0 up to 7 (0 means A0, 1 means A1, ...7 means A7).
*     If a current sensor is used, do not to use a pin that is already used by a voltage.
*  !! Take care that the voltage applied to Arduino pin may not exceed Vcc (normally 5 volt) or 1.1 volt (if internal reference voltage is used).
*     It can be that you have to use a voltage divider in order to reduce the voltage applied on Arduino pin.
*     See explanation above (6.2) about voltage divider.
*     Take care : do NOT use pins A4 and A5 if you use a vario or an airspeed (those pins are reserved for the barometric and pressure sensors).
* Note: The current sensor is normally powered by the 5 volt VCC from OXS (same as the vario sensor).
*       There are bidirectional sensor and unidirectional sensor.
*       For bidirectional, output is normally equal to VCC/2 when current = 0 Amp and, for unidirectional, output is normally 0,6 volt at 0 Amp.
*       If OXS is connected to a battery giving less than 5.2 volt, the supply voltage for the current sensor will vary with the OXS supply voltage.
*       Therefore VCC/2 ( = O amp) will varies with VCC.
*       This is an issue if the Arduino ADC is configured to use the 1.1 volt internal reference.
*       So, in this case it is better to configure the ADC in order to use VCC as reference for conversion.
*       In order to use a current sensor, you have to uncomment the line //#define PIN_CURRENTSENSOR and specify the Arduino pin connected to the current sensor. 
*       You must also define 2 parameters depending of the type of sensor being use; those parameters are given in the datasheet of the sensor).
*         - MVOLT_AT_ZERO_AMP  =  milliVolt generated by the sensor when current is 0 Amp: normal value is :
*                                       - for a bidirectional sensor  : Vcc from current sensor / 2 (so = 2500 if sensor is connected to Arduino Vcc and Arduino Vcc is 5 Volt).
*                                       - 600 for unidirectional sensor 
*         - MVOLT_PER_AMP       =  milliVolt per Amp. The value depend on the sensitivity of the sensor (e.g. an ACS712ELCTR-30A-T has a sensitivity of 66 mvolt/A, a ACS758LCB-050U has a sensitivity of 60 mv/Amp)
*        
*        If you use the 1.1 internal reference to measure voltagse and current, you must also use a voltage divider in order to scale down the voltage produced by the current sensor.
*        See the section 6.2 above about voltage divider. The principle are just the same but the names of the 2 paraameters are:
*          - RESISTOR_TO_GROUND_FOR_CURRENT
*          - RESISTOR_TO_CURRENT_SENSOR 
*  Note: those parameters are automatically discarded when PIN-CURRENTSENSOR is not defined (= set as comment).
*  Note: When current sensor is used, oXs can also calculate and transmit current consumption (milliAh) and Fuel (in % going down from 100% to 0%).
*        If you want the last one, then use a setup like "Fuel , MILLIAH , -100 , 4000 ,0" in "data to transmit section" (and replace 4000 by the capacity - in milliAmph - of your battery) (see below).
*        Still, with Tx using openTx or Ersky9x software, it is better to let the Tx calculates those values by it self based on the current. 
*               This ensure that values are consistent; it allows to reset the values on Tx side; it allows to change the value of the battery capacity on Tx side (so without having to reload another set up in Arduino oXs).    
*               E.g on Ersky9x, in Telemetry menu set up "current source"  set "FAS"; in "mAh Alarm", set the mah you want for alarm to sound and select warning sound/voice, 
*               ie 70% of 2200 mAh lipo, use 1540. then the FUEL percentage will start from 100% count down to 0% when 1540 is consumed.
************************************************************************************************************************
//#define PIN_CURRENTSENSOR      2
#define MVOLT_AT_ZERO_AMP        600
#define MVOLT_PER_AMP            60
#define RESISTOR_TO_GROUND_FOR_CURRENT  10
#define RESISTOR_TO_CURRENT_SENSOR      40

***** 6.6 - Ads1115 parameters  *************************************************************************************
*    It is possible to connect an external ADC of the type ads1115. 
*    This device is very cheap (about 2 $) and can provide more accuracy than internal Arduino ADC
*    It has 16 bits resolution, accurate internal voltage reference, a programmable gain amplifier and the ability to measure directly the voltage between 2 pins 
*    See datatsheet of ads1115 for more details
*    oXs can be connected to one of those device via I2C bus. It can then provide up to 4 voltage measurements named ADS_VOLT_1 ...ADS_VOLT_4
*    oXs allows to convert one ads voltage measurement into current and consumption (when ads1115 is connected to a current sensor) 
*    oXs also allows to convert one ads voltage measurement into airspeed and compensated vario (when ads1115 is connected to a differential baro sensor like the MPXV7002) 
*          MPXV7002 sensor is a cheapier and easier to find alternative to the MS4525 sensor (but is less accurate)
*    In order to use an ads1115 ADC, you must uncomment ACD_MEASURE and specify several sets of 4 parameters. 
*    Attention : let line ADC_MEASURE as comment if ads1115 is not used (in order to avoid I2C errors and slow down oXs)
*        In ACD_MEASURE, you specify the ads115 pins being used for voltage measurement
*              Fill always 4 values. Still, if you do not need all 4 measurements, use the value ADS_OFF for the unrequired measurement(s)
*              Note : the more measurements you ask for, the more time it require to get each of them because ads can only perfom one at a time
*              Select 4 values between A0_TO_A1, A0_TO_A3, A1_TO_A3, A2_TO_A3, A0_TO_GND, A1_TO_GND, A2_TO_GND, A3_TO_GND, ADS_OFF
*        In ADC_FULL_SCALE_VOLT, you specify the ads1115 gain parameter for each of the 4 measurements.
*              Fill always 4 values even if you do not need all 4 measurements
*              This allows to amplify a low voltage applied on input pins before it is converted by the ADC. So accuracy of conversion is optimum. 
*              Select between MV6144 MV4096 MV2048 MV1024 MV512 MV256 where the digits give the max mvolt being applied on the pin (e.g. for A0_TO_GND) or between 2 pins (e.g. for A0_TO_A1)
*        In ADS_SCALE, you specify a scaling factor to be applied on each measurement in order to get a value that conforms the expected measurement.
*              Fill always 4 values even if you do not need all 4 measurements
*              When scaling parameter = 1, oXs returns a value = 1 when the voltage applied on ads1115 pin(s) is the max voltage set by ADC_FULL_SCALE_VOLT. 
*              So, if ADC_FULL_SCALE_VOLT is set on MV512, when input voltage will be 512mv (or more)), oXs will return 1 if ADS_SCALE = 1.
*              If you do not have a voltage divider on ads1115, you probably expect that oXs returns 512; then set ADS_SCALE to 512.
*              If you have a voltage divider, the voltage that you want to measure is greater than the voltage applied on ads1115 pin and you have to increase ADS_SCALE.
*              e.g. if your voltage divider divides your voltage by 10, you must set ADS_SCALE to 5120 (= 512 * 10)
*              Note: ADS_SCALE can have decimals (e.g. 100.5) . It can be positive or negative; It can't be 0
*        In ADS_OFFSET, you specify an offset to be applied. 
*              Fill always 4 values even if you do not need all 4 measurements
*              When no offset has to be applied, set the value on 0
*              The specified offset is added to the value calculated after scaling.  
*              each value must be an integer (positive or negative); it can be 0
*        In ADS_RATE, you specify the number of milli sec that ads1115 takes to convert a voltage.      
*              Fill always 4 values even if you do not need all 4 measurements
*              Using a high value reduces power consumption still, it reduces the number of measurements that can be performed/transmitted per second    
*              Select values between MS137, MS69, MS35, MS18, MS9, MS5, MS3 , MS2;  the digits correspond to the number of milli sec (e.g. MS18 means 18 msec)
*              Note : oXs will wait at least the specified delay but it can be that the delay is higher due to other tasks to be performed by oXs
*        In ADS_AVERAGING_ON, you specify the number of voltages to be collected in order to calculate an average. 
*              Fill always 4 values even if you do not need all 4 measurements. If you do not want averaging, set the value on 1
*              It must must be an integer, positive and different from 0.
*              Note : Increasing the value is a way to reduce noise. Still it increase the delay between 2 transmissions 
*        In ADS_CURRENT_BASED_ON, you specify which voltage measurement (if any) is used for current (and current consumption)
*              Uncomment this line only when a conversion is requested 
*              Keep this line as comment if there is no current sensor or if the wurrent sensor is connected to an Arduino pin as explained in section 6.4
*              Fill only one value; select a value between ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
*              Note : when oXs calculate a curent based on a voltage, it used also 2 parameters from section 6.4 : MVOLT_AT_ZERO_AMP and MVOLT_PER_AMP
*        In ADS_AIRSPEED_BASED_ON, you specify which voltage measurement (if any) is used for airspeed
*              Uncomment this line only when an analog airspeed sensor is connected to ads1115 
*              Keep this line as comment if there is no airspeed sensor connected to ads1115
*              Fill only one value; select a value between ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
*              Note : A typical airspeed sensor is the MPXV7002DP which is easaly available on ebay or aliexpress.com.
*                     When this sensor is used, you have to configure ads1115 in following way:
*                     - ask only for one ads1115 measurement and it must be a differential voltage : so use this setup : ADS_MEASURE A0_to_A1, ADS_OFF, ADS_OFF, ADS_OFF
*                     - connect one resistor of about 10kohm between ads1115 Vcc and ads1115 A1 and another one of the same value between ads1115 Ground and ads1115 A1; so A1 pin get Vcc/2 volt
*                     - set the ads1115 gain in order to get 2048 mvolt at full scale:  so use : ADC_FULL_SCALE_VOLT MV2048,MV2048,MV2048,MV2048  
*                     - set the ads rate in order to make ADC conversion as fast as possible; so use : ADS_RATE MS2, MS2, MS2, MS2
*                     - the other parameters are not critical (not used for airspeed)
************************************************************************************************************************
#define ADS_MEASURE A1_TO_GND ,   ADS_OFF , ADS_OFF , ADS_OFF // uncomment when ADS1115 is used; select 4 values between A0_TO_A1, A0_TO_A3, A1_TO_A3, A2_TO_A3, A0_TO_GND, A1_TO_GND, A2_TO_GND, A3_TO_GND, ADS_OFF
#define ADS_FULL_SCALE_VOLT  MV4096, MV4096, MV4096, MV4096 //  select between MV6144 MV4096 MV2048 MV1024 MV512 MV256
#define ADS_OFFSET 0, 0 , 0 , 0 // must be an integer (positive or negative)
#define ADS_SCALE 2, 10, 1, 1 // can be a float
#define ADS_RATE  MS137 , MS5, MS3 , MS2 // select between MS137, MS69, MS35, MS18, MS9, MS5, MS3 , MS2
#define ADS_AVERAGING_ON 1 , 10, 50, 50 // number of values used for averaging (must be between 1 and 254) 
#define ADS_CURRENT_BASED_ON ADS_VOLT_1  // uncomment if current, and comsumption have to be calculated based on one of ADS voltage measurement; select then the voltage to be used between ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
#define ADS_AIRSPEED_BASED_ON ADS_VOLT1  // uncomment if airspeed (and dte) have to be calculated based on one of ADS voltage measurement ; select then the voltage to be used between ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4

**** 7 - RPM (rotations per minute) sensor settings (optional) ***************************************************************************
*      It is possible to measure RPM using a sensor connected to pin ICP (=PB0, = pin 8) of OXS.
*      This sensor must provide a level change (0 - Vcc) on this pin each time a blade passes in front of it.
*      The number of blades is an important parameter to set up but this is done on Tx side.
*      It is also possible to build a small PCB board that will provide pulsed when connected on a brushless wire.
*      To activate this function, put YES instead of NO in line #define CALCULATE_RPM  .
*      Note: The digital pin 8 (PB0/ICP) is the only one to be used to measure RPM.
*            The value calculated by oXs is in Hertz (and not in roration per minute)
************************************************************************************************************************
#define CALCULATE_RPM     NO 


**** 8 - Persistent memory settings ************************************************************************************
*      Optional Feature.
*      If persistent memory is activated, current consumption and fuel consumption (+ flow parameters) will be stored in EEProm every 30 seconds.
*      This value will be restored every power-up.
*      So, you will get ongoing consumption even if the you turn off the model between flights.
*      If you want to save those data, you have to say YES in the line "#define SAVE_TO_EEPROM"
*      
*      It is also possible to reset:  
*       - the current and fuel consumption to zero pressing on a push button connected to OXS.
*         Therefore a DIGITAL Arduino pin has to be connected to a push button, the other pin of the push button being connected to Gnd (ground).
*         In order to use this feature, you must specify the DIGITAL Arduino pin being used.
*               Default: 10 ; other digital pin can be used; Do not use a pin that is already used for another purpose.
*       - the fuel consumption to zero from TX using a chanel on TX connected to Aduino using PPM feature (see section 3 PPM and 11 flow sensor )         
************************************************************************************************************************
#define SAVE_TO_EEPROM    NO
#define PIN_PUSHBUTTON    10   

**** 9 - GPS (optionnal)  *********************************************************************************************
* It is possible to connect a GPS module to Arduino. In this case, oXs will transmit to the Tx some data generated by the GPS module.
* Note: data are sent to the receiver only when the GPS has a fix. 
*     So, it can take several minutes before oXs start sending GPS data.
*     If GPS lost his fix, oXs will stop sending GPS data until GPS got a new fix.
* When Frsky (SPORT or HUB) receiver is used, oXs will always send longitude, latitude, altitude, ground speed and course. 
*   This does not require any additional lines in the section "Data to transmit" (see 9 here above) 
* When Multiplex receiver is used, the user has well to specify under the line "#define SETUP_MULTIPLEX_DATA_TO_SEND" which data are sent and the line number where the data have to appear on the display.
* Hardware points of attention.
*   Supported GPS modules: oXs supports modules based on UBLOX GPS (easily available on ebay or aliexpress) like Neo6M, Neo7M and Neo8M. 
*   Most modules have a build in voltage regulator in order to drop the voltage down to 3.3 volt which is about the max allowed by the GPS
*   They have 4 pins available that have to be connected to Arduino
*     - GPS gound is connected to Arduino ground
*     - GPS Vcc is normally connected to Arduino Raw pin (which is normally connected to Vcc from the receiver). 
*          Still take care that the voltage regulator on the GPS module is, most of the time, foreseen for a maximum voltage of 6 Volt while arduino Raw pin accept more. 
*          So, if you Raw pin get more that 6 volt, it is safe to add another voltage regulator to drop the GPS Vcc down.  
*    - GPS Tx pin is connected to Arduino Rx pin      
*    - GPS Rx pin is connected to a resistor (e.g. 10k) and the other pin of the resistor is connected to Arduino pin 6 (digital pin 6). 
*          This resistor is added (at least for an Arduino 5 volt) in order to protect the GPS pin. 
*          This is requested because Arduino will generate a signal with a high level equal to arduino Vcc (so normally 5 volt) while the GPS module should normally not accept more than 3.3 Volt.
*          To be safier, you could even add a second resistor between GPS Rx pin and Ground (value= 22k) but, in my case, it worked without this second resistor. 
*          Note: it would be possible to use another pin than Arduino pin 6 but then it requires to change some parameters in file oXs_gps.cpp (see "Setup the GPS sensor").
*   !! IMPORTANT NOTE : 
*      Arduino Rx pin is used for 2 purposed:  getting the data from the GPS and uploading the program into the Arduino (normally only to be done once) using a USB to serial adapter.
*              You must avoid having both GPS and USB connected at the same time because while uploading a program in Arduino, there will be conflicts between the signals sent by the 2 devices and programming will fail.
*              There is another risk: if your USB to serial adapter generates 5 volts signal, it could damage the GPS module. 
*              So, when you connect the usb to serial adapter to the Arduino, you should disconnect at least the GPS TX pin from the Arduino Rx pin.  
*              Personnaly I use a connector between Arduino and GPS module and so I can disconnect totally the GPS module.
* Software points of attention
*    UBLOX GPS module are normally delivered with a default configuration (generating automatically e.g some NMEA messages at 9600 bauds at a refresh rate of 1 hz).
*    oXs assumes that, at start up, GPS is working at 9600 bauds. oXs sent then some commands in order to
*       - disable all NMEA messages
*       - activates some UBX messages
*       - increase frequency of calculation (to 5 Hz instead of 1hz)
*       - set up the baud rate to 38400 instead of 9600.
*    Those parameters are not saved in the GPS (because some GPS modules does not allow it). So, oXs will send those commands at each power on.   
*    If you oXs does not send GPS data, please check that your GPS module has still the default configuration (most important is that it is configured to receive UBX command messages at 9600 bauds). 
*       An easy way to check the GPS configuration is to connect the GPS module to a 3.3 volt FTDI ( or USB to serial adapter) and to use a free software named "u-center". 
*       This software is available on the official web site of UBLOX. More info is easily available on the web. 
*  OXs allows to modify some parameters in the config.h file:
*     -   #define A_GPS_IS_CONNECTED      NO       : Replace NO by YES if a GPS is connected and has to transmit his data
*     -   #define GPS_SPEED_IN_KMH   : uncomment this line if GPS speed has to be sent in km/h instead of knot/h 
*     -   #define GPS_SPEED_3D       : uncomment this line if GPS speed has to be the 3d speed instead of the 2d speed (note: 3d is probably less accurate - to test)
*     
************************************************************************************************************************ 

******  10 - IMU based on mpu6050 (accelerometer/gyro sensor) (optionnal) ********************************************************
// ***** 10.1 - IMU 6050 *****
*  It is possible to connect an IMU sensor (=accelerometer/gyro) to arduino; this is optionnal.
*  It allows :
*      - to reduce the reaction time of the vario by about 0.5 sec (note: a baro sensor has to be connected too because oXs merges the data from both sensors)
*      - to transmit data about accelerations and/or orientation (pitch/roll); in this case it is important that oXs device is mounted in a fix position and is aligned with the plane axis. 
*  Only one IMU sensor is supported : the mpu6050. 
*  This sensor is easily available on ebay, ... under different modules. The best module to use is probably the GY-86 because it has also a voltage regulator (3.3volt), I2C level converters, and a baro sensor (MS5611)     
*  5 pins from the mpu6050 have to be connected to Arduino:
*       - MP6050 ground  <--->  Arduino ground
*       - MP6050 Vcc     <--->  Arduino Vcc
*       - MP6050 SDA     <--->  Arduino SDA = Arduino A4
*       - MP6050 SCL     <--->  Arduino SCL = Arduino A5
*       - MP6050 INT     <--->  Arduino INT0 = Arduino 2 OR Arduino INT1 = Arduino 3(do not use the same pin for another purpose like PPM!)
* In order to activate the IMU, set YES in the line #define A_MPU6050_IS_CONNECTED       
* When IMU is activated, this version of oXs calculates a vertical speed in a different way merging the altitude from baro sensor with vertical acceleration from IMU (in Earth reference).
* This other type of vertical speed can be send as Vspeed assigning the value BARO_AND_IMU in #define VSPEED_SOURCE (see section 4.2)
* It is also possible to assign it in  "VARIO_PRIMARY" or "VARIO_SECONDARY"  and so to switch between 2 vario sources from the Tx (using a ppm channel) 
* In order to get best results from IMU, it is required to calibrate the accelerometer offsets. To do so, please :
*    - upload a version of oXs firmware whith the line #define DISPLAY_ACC_OFFSET uncommented
*    - let oXs runs while connected to the PC (via USB serial interface = FTDI)
*    - open Arduino IDE terminal (press CTRL + SHIFT + M simultaniously)
*    - take care to set the baud rate to 115200 (or 38400 if GPS is activated too)
*    - after startup, terminal should, every 2 or 3 sec, display Acc followed by 3 numbers being respectively AccX, AccY and AccZ. Please note that those numbers change when mpu6050 moves.
*    - ensure that the mpu6050 (GY86) is perfectly horizontal and does not move (e.g. put on a table) 
*    - notice the 2 first numbers ( = AccX and AccY ) ; Don't take care of the 3rd number because when the sensor is in this position, it will reflect the gravity and will be around 16384. 
*    - rotate mpu6050 in order to get X or Y axis perfectly vertical and do not move. Now, the 3rd number would become much lower (because it does not measure gravity anymore)
*    - notice the 3rd number ( = Accz )
*    - update oXs_config.h file filling the 3 numbers in lines #define ACC_OFFSET_X , #define ACC_OFFSET_Y and #define ACC_OFFSET_Z
*    - set line #define DISPLAY_ACC_OFFSET as comment (adding "//" in front)
*    - upload again oXs firmware in arduino
************************************************************************************************************************ 
#define A_MPU6050_IS_CONNECTED      NO     : Replace NO by YES if a IMU6050 is connected and has to transmit his data
#define PIN_INT_6050 3    // Interrupt from 6050 has to be connected to Arduino pin 2 or pin 3 (do not use here the same pin as PPM) 
#define DISPLAY_ACC_OFFSET
#define ACC_OFFSET_X 0 // fill here the value reported when DISPLAY_ACC_OFFSET is activated (eg. -160)
#define ACC_OFFSET_Y 0 // fill here the value reported when DISPLAY_ACC_OFFSET is activated (eg. -150)
#define ACC_OFFSET_Z 0 // fill here the value reported when DISPLAY_ACC_OFFSET is activated (eg. -1100)

// ***** 10.2 - HMC5883 *****
*  If you use a module like GY-86 AND if IMU6050 is connected to arduino (see 10.1), oXs can get access to a magetometer HMC5883 in order to get a Yaw measurement.
*  This does not require additional wiring. 
* In order to activate the HMC5883, set YES in the line CALCULATE_YAW_WITH_HMC5883       
* Take care that to get reliable values from the magnetometer, it is mandatory to calibrate it.
* This is a non trivial process.
* I recommend to folow the process described in this link: http://www.physi.cz/hmc5883l-magnetometer-calibration.html
* It means that you have to:
* 1) collect sample data from oXs: 
*    This require that you: 
*      - uncomment the line #define GENERATE_MAG_CALIBRATION_DATA
*      - upload the program in the Arduino
*      - open the Arduino PC terminal
*      - in the PC terminal, you will get a few lines of general data followed by a list with 3 values per line (those are the raw X, Y and Z magnetometer readings) 
*      - rotate the sensor slowly on 360° along ALL axis in order to collect more than 1000 lines of data (this may take several minutes)
*  2)copy and paste the lines from PC terminal to a TXT file (using a text editor like notepad). Remove the first lines of general data (before the list)
*     and save this file as TXT file
*  3)download the windows program form the link here above (see "Download Executable" )
*      run this program, set norm to 1.0, click "open" and select the TXT file with your sample data and then click on "Calibrate"
*  4) note the 3 "combined bias" values and copy them in XMAG_OFFSET, YMAG_OFFSET and ZMAG_OFFSET (in oXs_config_advanced.h)
*  5) note the 9 "combined scale factors..." values and open now a link to a web site that will let you find the inverse of the matrix of the 9 values : 
*      https://www.wolframalpha.com/input/?i=%7B%7B591.0437,-13.1628,-15.0294%7D,%7B-13.1628,596.1147,30.5314%7D,%7B-15.0294,30.5314,552.0759%7D%7D%5E-1
*      - fill the 9 values in the first input field (respect the special format with {{ , , } , { , , } , { , , }} and click on the button on the right of the input line.
*  6) you will get a "result" matrix. Note the 9 values from result and copy them in XXMAG_CORRECTION , XYMAG_CORRECTION , XZMAG_CORRECTION ... ZZMAG_CORRECTION  (in oXs_config_advanced.h)
*  7) set the line #define GENERATE_MAG_CALIBRATION_DATA as comment and upload the program again in Arduino
************************************************************************************************************************ 
#define CALCULATE_YAW_WITH_HMC5883   NO    // select between YES , NO ; YES requires that A_MPU6050_IS_CONNECTED is YES here above

//#define GENERATE_MAG_CALIBRATION_DATA   // uncomment this line when HMC5883 calibration has to be performed. Set back as comment once calibration parameters have been introduced 

#define    XMAG_OFFSET 2     // must be an integer
#define    YMAG_OFFSET -1     // must be an integer
#define    ZMAG_OFFSET 139     // must be an integer
#define    XXMAG_CORRECTION  0.122082   // can have decimals
#define    XYMAG_CORRECTION  -0.00204026
#define    XZMAG_CORRECTION  0.00377534 
#define    YXMAG_CORRECTION  -0.00204026
#define    YYMAG_CORRECTION  0.130413
#define    YZMAG_CORRECTION  -0.00491189
#define    ZXMAG_CORRECTION  0.00377534
#define    ZYMAG_CORRECTION  -0.00491189
#define    ZZMAG_CORRECTION  0.138038

****** 11 - Flow sensor ******************
* If you use a fuel engine, you can connect a liquid flow meter to oXs
* This sensor generates a pulse each time some milli liters are going trough the sensor 
* oXs can count the number of pulses and calculates 3 data : the current consumed milli liter / min and, taking care of tank capacity, the remaining fuel in ml and in %.
* It is recommended to use following type of flow meter because it is probably more accurate than other when flow is low
* http://www.conrad.be/ce/nl/product/155374/BIO-TECH-eK-FCH-M-Doorstroomsensor-1-stuks-Voedingsspanning-bereik-5-24-VDC-Meetbereik-08-0015-lmin-l-x;jsessionid=EED7B26A7F28BA3F20F0060807E20FD1.ASTPCEN22?ref=searchDetail
* It is foreseen to measure from 0.015 up to 0.8 ml/min.
* The output signal of the flow meter sensor has to be connected to arduino pin 9 (and the other 2 pins to 5 volt VCC and to Ground).
* There are other cheap flow sensors on ebay or aliexpress but I expect that there are not accurate enough when flow is low.
* 
* To activate such a flow sensor, you have to: 
*   - assign YES to the define A_FLOW_SENSOR_IS_CONNECTED (in oXs_config_basic.h file)
*   - specify in PULSES_PER_ML the number of pulses generated by the sensor when 1 milli liter of liquid flows trough it
*   - specify in TANK_CAPACITY the maximum capacity in milli liter
*   - specify 8 values used to calibrate your sensor in INIT_FLOW_PARAM
*         Those parameters are used in order to take care that the number of pulses generated by the sensor when 1 milli liter of liquid flows trough it varies with the flow it self.
*         For 4 flow values (the first 4 parameters) , oXs allows you to define a correction (in %) in the last 4 parameters.  
*         The process to calibrate the sensor should be as follow : 
*              Set last 4 parameters to 0 (so 0% correction)
*              Run you engine at 4 different speeds (from very low, to some medium and finally to high speed) for a few minutes. 
*              For each run, 
*                - note the remaining fuel (in ml) reported by oXs at start (e.g. 1200) and at end (e.g. 1090)of the run 
*                - measure the real remaining fuel (in ml) in the tank at start (e.g. 1500) and at end (e.g. 1380)of the run 
*                - note the enlapsed time (in min) between start and end of the run (e.g. 2 min).  
*                - compare the consumed ml (difference between remaining fuel at start and at end) reported by oXs and in the reality (e.g. reported = 1200 - 1090 = 110ml; real = 1500 - 1380= 120ml) 
*                - calculate the correction factor to apply (e.g. (120 - 110) / 110 = 9%) ; note correction could be negative)
*                - calculate the flow where this correction applies (= consumed ml reported by oXs / enlapsed time = 110 ml / 2 min = 55 ml/min) 
*              Fill the first 4 parameters with the calculated flows (e.g. 55) and the last 4 parameters with the correction percentage (e.g. 9). 
*              Take care that the first 4 parameters have to be in ascending order (so from low speed to high speed).
*         Note: when oXs calculates the consumption, it will apply linear interpolation for the related range of values.
*
*   If you move an oXs device from one plane to another, you will probably have to modify the values set in TANK_CAPACITY and/or INIT_FLOW_PARAM.  
*   This requires to upload a new firmware in your oXs except if you are using JETI protocol or SPORT protocol with openTx 2.2.x (or above). 
*   For JETI protocol, you can just enter the JETIBOX dialog box, press DOWN key to go to the item to modify and press "<" or ">" to increase/decrease the value. 
*       Do not forget to activate the SAVE_TO_EEPROM option in section 8 in order to save the parameters and reuse them at next power on. 
*   For SPORT protocol, you must run some LUA scripts. In this package (in the folder "lua scripts"), you can find 3 lua scripts to be installed in the folder SCRIPTS/FUNCTIONS of the SD card:
*        - one (rstful.lua) to reset the consumed fuel to 0 (to be used when tank is filled) 
*        - one (tank.lua) to define the max capaicity of the tank (prior to run the script, the tank capacity has to be entered in GVAR9 for fase 9
*                The value in GVAR9 is the number of 50 ml in the thank. It means e.g. that for a thank of 1000 ml, GVAR9 must be set to 20 (=1000 / 50 )
*        - one (calful.lua) to define the 8 (4 X 2) calibration parameters (pior to run the script, the parameters have to be entered in GVAR9 (for fase 0 to 8)
*            The first 4 values are the flow in ml/min (min = 30, max  = 800) ; the values must be in ascending order
*            The last 4 values are the correction in percent to apply (min = -100, max = 100) for each flow 
*   NOte: the parameters saved in the eeprom or loaded by a LUA script take over the parameters defined in the config.
*
* Please note that the fuel consumption can be saved every 30 sec in a non volatile memory.
* To activate this option, you have to say YES in the line #define SAVE_TO_EEPROM from section 8
* If this option is activated, when oXs start at power on, it start counting from the last remaining fuel when power goes off. 
* Otherwise, oXs reset the fuel consumption and assumes a 100% tank.
* 
* A reset of the fuel consumption can be requested from the TX. This is really needed when SAVE_TO_EEPROM is activated
* This can be done in several ways depending also on the protocol being used:
* For all protocols, it can be requested using a PPM signal
*    This requires to activate the oXs PPM option (see section 3) and to connect a Rx channel to oXs.
*    Reset will occur when absolute value of PPM signal exceed the value specified in FLOW_SENSOR_RESET_AT_PPM (section 11)
* Furthermore:
*  -for JETI protocol it can be requested from the JETIBOX pressing simultanously the "<" and the ">" keys when the JETIBOX dislay the remaining fuel in percentage 
*  - for the FRSKY SPORT protocol, since openTX version 2.2.x it is possible to activate a LUA function script that will send a reset command to oXs
*  
* For JETI protocols, oXs transmits automatically the current flow, the remaining fuel in ml and in % 
* For other protocols, you have to ask oXs to transmit the data; so you have : 
*   - to uncomment the line #define FILL_TEST_1_2_3_WITH_FLOW_SENSOR_CONSUMPTION  (in section 2.5 )
*   - to specify in which telemetry fields, TEST_1 (current flow in ml/min), TEST_2 (remaining fuel in ml) and TEST_3 (remaining fuel in %) are sent (see section 2.1/2.4)
*       Please note that if you transmit TEST_1... TEST_3 in AccX...AccZ in FRSKY protocol, the values are divided by 100 by openTx. You can get the original values back if you set up a scale = 255 on Tx side in the telemetry fields
**************************************************************************************************************************************
#define A_FLOW_SENSOR_IS_CONNECTED      NO                    // select between YES , NO
#define PULSES_PER_ML                    10.0                 // number of pulses per milli liter (depends on sensor); can have decimals
#define TANK_CAPACITY                    1000                 // tank capacity in ml
#define INIT_FLOW_PARAM  30 , 100 , 500 , 700 , 20 , 10, -5, 15   // define at 4 levels of flow (in mliter/min) (e.g. 30, 100, 500, 700) 4 correction parameters (in %; e.g. 20, 10, -5, 15); flow levels have to be sorted from low to high
#define FLOW_SENSOR_RESET_AT_PPM         95                   // when absolute value of ppm is greater than this, flow counter is reset.

****** 20 - Sequencer (ON/OFF) for several digital outputs **************************************************************************************
* oXs allows you to control (HIGH/LOW) up to 6 digitals Arduino outputs in different sequences.
* Each sequence is composed of one or several steps; each step defines for how long (= a duration) which outputs are HIGH and which outputs are LOW.
* oXs determines normally the sequence to be played based on the signal received on a PPM channel (see section 3 in order to set up a PPM signal). 
* Still there are 2 exceptions:
*    At power on or when no PPM channel is configured/received, oXs will generate as default the sequence defined in the line #define SEQUENCE_m100 (see below)
*    When a low voltage alarm is configured (see below) and if the voltage becomes low, oXs will generate the sequence defined in the line #define SEQUENCE_LOW (see below) as long as the voltage remains low   
* When a sequence has been played, oXs can or repeat it or just wait for a new sequence. The difference is made in the set up of the sequence.
* Each time a new (= different) valid PPM signal is received, oXs start immediately the corresponding sequence (even if the current sequence is not completely played)
* - In order to use the sequencer functionality, you first have to define which Arduino digital pin have to be controlled by the sequencer.
*     The arduino pins that can be controlled are the pin 13, 12, 11, 10 , 9 and 8.
*     This set up is achived by a line like : #define SEQUENCE_OUTPUTS 0b100100
*     Each bit (1 or 0 after the "b") represent an output; the least significant bit correspond to pin 8, the bit on the left pin 9 etc... up to pin 13  
*     Put 1 when the pin has to be controlled by the sequencer, 0 otherwise; In this example, it means that only pins 13 and 10 would be controlled.
*     Note: if the line #define SEQUENCE_OUTPUTS xxxx is omitted or put as comment, then the sequencer is not active at all.
*           Take care not to use the same pin for the sequencer and for another oXs funtionallity (e.g. as Tx pin, for push button, for PPM, for RPM, ...)
*           If a pin is set up with 0 (= not controlled by oXs), it will never be forced to HIGH or LOW by the sequencer even if a 1 or 0 is set up in a sequence.
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
*               Value = 0 has a special meaning. When oXs reachs a duration = 0, it applies the corresponding combination of outputs and keeps it for an unlimitted time.
*               This allows to force the outputs to stay with a specific combination after having played the sequence.
*               If duration = 0 is used, it should be in the last step of the sequence (because oXs will never apply the following steps).
*               If duration is set to 0 in the first step, oXs will apply directly the specific combination of outputs and keep it. 
*               If duration = 0 is not used in a sequence, oXs will automatically repeat the whole sequence after reaching the last step.
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
*      Note: when a sequence name is not defined, oXs handles it like it would be defined with 0 , 0b000000 (so no repeat, all outputs LOW)
* - Finally you can (but it is not mandatory) set up the condition(s) for a low voltage detection. When a voltage becomes too low, oXs starts automatically SEQUENCE_LOW (and discard PPM channel)
*     A low voltage condition can be set up based on 1 or 2 voltage(s):
*         - the voltage on the Arduino pin defined by the 6th parameter PIN_VOLTAGE; this set up is achived by a line like : #define SEQUENCE_MIN_VOLT_6 6000 where 6000 is the "low" voltage in mVolt.
*           Note: if you use this option, do not forget assign a pin number to the 6th parameter in #define PIN_VOLTAGE and to fill (if requested) the 6th parameter of other voltage parameters.        
*                 The pin defined in the 6th parameter of PIN_VOLTAGE can be the same as another parameter in PIN_VOLTAGE; this can be useful if you want to set up low voltage parameters too.
*         - the lowest lipo cell voltage; this set up is achived by a line like : #define SEQUENCE_MIN_CELL 3000 where 3000 is the "low" voltage in mVolt.
*           Note: if you use this option, do not forget to define the other voltage parameters PIN_VOLTAGE , etc ... and NUMBEROFCELLS        
*     Note:  when no one low voltage parameter is defined, oXs will not automatically start SEQUENCE_LOW.
*            when both voltage parameters are defined, oXs will automatically start SEQUENCE_LOW as soon as one of the 2 voltages becomes low.
*            If you want that oXs notifies a low voltage detection do not forget to also define SEQUENCE_LOW (otherwise, oXs will just set all output pins to LOW)
*            If you have telemetry, you can also make a set up on Tx side in order to detect a low voltage and then send a specific value on the ppm channel.
*               In this case you do not have to define the set up in oXs and the same device can be used on several model.
************************************************************************************************************************
 
**** xx - Reserved for developer. **************************************************************************************
* DEBUG must be activated here when you want to debug one or several functions in some other files.
* Enabling DEBUG will allow to use Arduino Serial Monitor at 115200 baud (or 38600 when GPS is activated) to see init data and some live sensor values
* You can then select the parts that you want to debug by uncommenting the specifics DEBUG parameters you want in each file
* Note: OXS allows to transmit 3 fields named TEST1, TEST2, TEST3. You can fill those fields with whatever you want where you want if you want to transmit additional data to the Tx.
* Just fill in test1.value (or 2, 3) with an int32_t and test1.available (or 2, 3) with true and add those OXS measurements in the data to be sent section. 
************************************************************************************************************************
//#define DEBUG