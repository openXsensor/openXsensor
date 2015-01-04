// ------------------------- HUB protocol data list array --------------------------

String hubDataList[][] = new String[][] {
  { "----------", "----------" },
  { "DEFAULTFIELD", "DEFAULT" },
  { "Vspd", "Vertical Speed" },            // FRSKY_USERDATA_VERT_SPEED
  { "Curr", "Current" },                   // FRSKY_USERDATA_CURRENT
  { "Vfas", "Vfas" },                      // FRSKY_USERDATA_VFAS_NEW
  { "T1", "Temperature 1" },               // FRSKY_USERDATA_TEMP1
  { "T2", "Temperature 2" },               // FRSKY_USERDATA_TEMP2
  { "Rpm", "RPM" },                        // FRSKY_USERDATA_RPM
  { "Fuel", "Fuel" },                      // FRSKY_USERDATA_FUEL
  { "AccX", "Acceleration X" },            // FRSKY_USERDATA_ACC_X
  { "AccY", "Acceleration Y" },            // FRSKY_USERDATA_ACC_Y
  { "AccZ", "Acceleration Z" }             // FRSKY_USERDATA_ACC_Z
} ;

/*
 FRSKY_USERDATA_GPS_ALT_B
 FRSKY_USERDATA_TEMP1
 FRSKY_USERDATA_RPM
 FRSKY_USERDATA_FUEL
 FRSKY_USERDATA_TEMP2
 FRSKY_USERDATA_CELL_VOLT
 FRSKY_USERDATA_GPS_ALT_A
 FRSKY_USERDATA_BARO_ALT_B
 FRSKY_USERDATA_GPS_SPEED_B
 FRSKY_USERDATA_GPS_LONG_B
 FRSKY_USERDATA_GPS_LAT_B
 FRSKY_USERDATA_GPS_CURSE_B
 FRSKY_USERDATA_GPS_DM
 FRSKY_USERDATA_GPS_YEAR
 FRSKY_USERDATA_GPS_HM
 FRSKY_USERDATA_GPS_SEC
 FRSKY_USERDATA_GPS_SPEED_A
 FRSKY_USERDATA_GPS_LONG_A
 FRSKY_USERDATA_GPS_LAT_A
 FRSKY_USERDATA_GPS_CURSE_A
 FRSKY_USERDATA_BARO_ALT_A
 FRSKY_USERDATA_GPS_LONG_EW
 FRSKY_USERDATA_GPS_LAT_EW
 FRSKY_USERDATA_ACC_X
 FRSKY_USERDATA_ACC_Y
 FRSKY_USERDATA_ACC_Z
 FRSKY_USERDATA_CURRENT
 FRSKY_USERDATA_VERT_SPEED
 FRSKY_USERDATA_ALT_MIN
 FRSKY_USERDATA_ALT_MAX
 FRSKY_USERDATA_RPM_MAX
 FRSKY_USERDATA_T1_MAX
 FRSKY_USERDATA_T2_MAX
 FRSKY_USERDATA_GPS_SPEED_MAX
 FRSKY_USERDATA_GPS_DIS_MAX
 FRSKY_USERDATA_VFAS_NEW
 FRSKY_USERDATA_VOLTAGE_B
 FRSKY_USERDATA_VOLTAGE_A
 FRSKY_USERDATA_GPS_DIST
 FRSKY_USERDATA_FUELPERCENT
 */

 // ----------------------- SMART PORT protocol data list array -----------------------------

String sPortDataList[][] = new String[][] {
  { "----------", "----------" },
  { "DEFAULTFIELD", "DEFAULT" },          // 1
  { "Alt", "Altitude" },                  // 2  ALT_FIRST_ID
  { "Vspd", "Vertical Speed" },           // 3  VARIO_FIRST_ID
  { "Curr", "Current" },                  // 4  CURR_FIRST_ID
  { "Vfas", "Vfas" },                     // 5  VFAS_FIRST_ID
  { "T1", "Temperature 1" },              // 6  T1_FIRST_ID
  { "T2", "Temperature 2" },              // 7  T2_FIRST_ID
  { "Rpm", "RPM" },                       // 8  RPM_FIRST_ID
  { "Fuel", "Fuel" },                     // 9  FUEL_FIRST_ID
  { "AccX", "Acceleration X" },           // 10 ACCX_FIRST_ID
  { "AccY", "Acceleration Y" },           // 11 ACCY_FIRST_ID
  { "AccZ", "Acceleration Z" },           // 12 ACCZ_FIRST_ID
  { "A3", "A3 (S.Port only)" },           // 13 A3_FIRST_ID
  { "A4", "A4 (S.Port only)" },           // 14 A4_FIRST_ID
  { "ASpd", "Air Speed (S.Port only)" }   // 15 AIR_SPEED_FIRST_ID
} ;

/*
//#define RSSI_ID            0xf101  // please do not use this code because it is already used by the receiver
//#define ADC1_ID            0xf102  // please do not use this code because it is already used by the receiver
#define ADC2_ID            0xf103
#define BATT_ID            0xf104
//#define SWR_ID             0xf105   // please do not use this code because it is already used by the receiver
#define T1_FIRST_ID        0x0400
#define T1_LAST_ID         0x040f
#define T2_FIRST_ID        0x0410
#define T2_LAST_ID         0x041f
#define RPM_FIRST_ID       0x0500
#define RPM_LAST_ID        0x050f
#define FUEL_FIRST_ID      0x0600
#define FUEL_LAST_ID       0x060f
#define ALT_FIRST_ID       0x0100
#define ALT_LAST_ID        0x010f
#define VARIO_FIRST_ID     0x0110
#define VARIO_LAST_ID      0x011f
#define ACCX_FIRST_ID      0x0700
#define ACCX_LAST_ID       0x070f
#define ACCY_FIRST_ID      0x0710
#define ACCY_LAST_ID       0x071f
#define ACCZ_FIRST_ID      0x0720
#define ACCZ_LAST_ID       0x072f
#define CURR_FIRST_ID      0x0200
#define CURR_LAST_ID       0x020f
#define VFAS_FIRST_ID      0x0210
#define VFAS_LAST_ID       0x021f
#define GPS_SPEED_FIRST_ID 0x0830
#define GPS_SPEED_LAST_ID  0x083f
#define CELLS_FIRST_ID     0x0300
#define CELLS_SECOND_ID    0x0301
#define CELLS_THIRD_ID     0x0302
#define CELLS_LAST_ID      0x030f
// End of list of all telemetry fields supported by SPORT  (defined by Frsky)
*/

