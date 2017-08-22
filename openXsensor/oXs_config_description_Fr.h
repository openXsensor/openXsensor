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
comment

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
*   Ces différentes fonctions demandent des mixages dans l’émetteur et certaine configuration dans le fichier oXs-config.h (voir ci-dessous).
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
*     - décommenter la ligne #define PPM_VIA_SPORT
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
*     Note: Quand le vario est utilisé (#define VARIO décommenter), vous avez a spécifié (dans la section 9) comment la vitesse vertical est calculer.
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
*     Note: Dans cette version de oXs, si vous voulez envoyé les parameters calculé, vous devez definir d'envoyer les paramètres de la facon suivante:
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
* oXs peux envoyer la vitesse air en (1/10) de knot/h ou en (1/10) km/h. Pour OpenTx, vous dever utilisé l'option knot/h (selement certaine vielle version on besoin de l'option km/h).
* Pour activé l'option km/h, il faut décommenter cette ligne #define AIRSPEED_IN_KMH
* Sinon meter cette ligne en commentaire pour activer l'option knot/h.
*
* oXs peux aussi utiliser la mesure de vitesse air pour calculer le varimetre compenser appeller PRANDTL_DTE (=delta total energy).
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
*      - Reglé les paramètres dans oXs_config.h
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
*     La valeur de la PIN à remplir dans oXs_config.h est un nombre de 0 à 7 (0 signifie A0, 1 signifie A1, ... 7 signifie A7).
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