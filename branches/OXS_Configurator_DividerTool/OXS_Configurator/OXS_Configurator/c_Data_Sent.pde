// Data sent list array

String sentDataList[][] = new String[][] {
  { "----------", "----------" },
  { "ALTIMETER", "Altitude" },
  { "VERTICAL_SPEED", "Vertical Speed" },
  { "SENSITIVITY", "Sensitivity" },
  { "ALT_OVER_10_SEC", "Alt. over 10 seconds" },
  //{ "VARIOTEMP", "Vario Temperature" },
  { "CURRENTMA", "Current (mA)" },
  { "MILLIAH", "Consumption (mAh)" },
  { "CELLS", "Cells monitoring" },  
  { "VOLT1", "Volt 1" },
  { "VOLT2", "Volt 2" },
  { "VOLT3", "Volt 3" },
  { "VOLT4", "Volt 4" },
  { "VOLT5", "Volt 5" },
  { "VOLT6", "Volt 6" },
  { "RPM" , "RPM" }
  //{ "TEMP1", "Temperature 1" }
} ;

/*
  ALTIMETER = 1, // DEFAULTFIELD can be used in SPORT protocol (is then the same as ALT_FIRST_ID);  it MUST be used in Hub protocol because meters and centimeters are send in different fileds
  VERTICAL_SPEED, // DEFAULTFIELD can be used
  SENSITIVITY, // DEFAULTFIELD can NOT be used
  ALT_OVER_10_SEC, // DEFAULTFIELD can NOT be used ; this is the difference of altitude over the last 10 sec (kind of averaging vertical speed)
  // there is no telemetry field for this; it is possible to use e.g. T1 or T2; then you can use a custom function "play value" on Tx side
  VOLT1, // DEFAULTFIELD can NOT be used
  VOLT2, // DEFAULTFIELD can NOT be used
  VOLT3, // DEFAULTFIELD can NOT be used
  VOLT4, // DEFAULTFIELD can NOT be used
  VOLT5, // DEFAULTFIELD can NOT be used
  VOLT6, // DEFAULTFIELD can NOT be used
  CURRENTMA, // DEFAULTFIELD can be used
  MILLIAH, // if value must be sent as percentage, then uncomment the line "#define SEND_mAhPercentageAsFuel 4000" (see below)
  CELLS_1_2, // Only DEFAULTFIELD can be used
  CELLS_3_4, // Only DEFAULTFIELD can be used
  CELLS_5_6, // Only DEFAULTFIELD can be used
  RPM, // Only DEFAULTFIELD can be used
  VARIOTEMP, // DEFAULTFIELD can NOT be used   // Temperature from the MS5611 sensor
  TEMP1, // DEFAULTFIELD can be used
  TEMP2                     // Not usable yet  // DEFAULTFIELD can be used
*/
