void validateNumPins() {    

    String numPinsValidation[][] = new String[][] {                      // array { pin name, pin value, isActive }
      { "Serial output", "" + int(serialPin.getValue()), "2" }, 
      { "Reset button", "" + int(cp5.getGroup( "resetButtonPin" ).getValue()), "" + int( cp5.getController("resetButton").getValue() + 1 ) },
      { "PPM input", "" + int(ppmPin.getValue()), "" + int( cp5.getController("ppm").getValue() + cp5.getController("vario").getValue() ) }, 
      { "Climb output", "" + int(climbPin.getValue()), "" + int( cp5.getController("analogClimb").getValue() + cp5.getController("vario").getValue() ) },
      { "RPM input", "" + 8, "" + int( cp5.getController("rpm").getValue() + 1 ) }
    } ;     

  for ( int i = 0; i < numPinsValidation.length; i++ ) {
    for ( int j = i+1; j < numPinsValidation.length; j++ ) {      
      if ( int(numPinsValidation[i][1]) != -1 && int(numPinsValidation[j][1]) != -1 && int(numPinsValidation[i][2]) > 1 && int(numPinsValidation[j][2]) > 1 ) {
        if ( numPinsValidation[i][1].equals(numPinsValidation[j][1]) ) {
          //println("Attention !!  " + numPinsValidation[i][0] + " is using the same pin n°" + numPinsValidation[i][1] + " as " + numPinsValidation[j][0] + " !") ;
          numPinsValid = false ;
          messageList.append("- " + numPinsValidation[i][0] + " is using the same pin n°" + numPinsValidation[i][1] + " as " + numPinsValidation[j][0] + " !") ;          
        }
      }
      
    }
  }
/*
  if ( numPinsValid ) {
    println("No problem found with numeric pins ;)") ;    
    messageList.append("No problem found with numeric pins ;)");
  } 
*/  
}

void validateAnalogPins() {
  
  //boolean valid = true ;
  String voltActive[] = new String[7] ;
  int voltActiveCount = 0 ;
     
  for ( int i = 1 ; i <= voltNbr ; i++ ) {
    if ( int(cp5.getController("voltage").getValue()) == 1 && int(cp5.getController("volt" + i ).getValue()) == 1 ) {
      voltActive[i] = "1" ; 
      voltActiveCount ++ ;   
    } else {
      voltActive[i] = "0" ;     
    }  
  } 
  
  if ( int(cp5.getController("voltage").getValue()) == 1 && voltActiveCount == 0 ) {
    analogPinsValid = false ;
    messageList.append("- Voltage sensor is active but there is no voltage to measure !") ;    
  }
 
  String analogPinsValidation[][] = new String[][] {              // array { pin name, pin value, isActive }             
    { "Voltage n°1", "" + int(cp5.getGroup("ddlVolt1").getValue()), "" + voltActive[1] },
    { "Voltage n°2", "" + int(cp5.getGroup("ddlVolt2").getValue()), "" + voltActive[2] },
    { "Voltage n°3", "" + int(cp5.getGroup("ddlVolt3").getValue()), "" + voltActive[3] },
    { "Voltage n°4", "" + int(cp5.getGroup("ddlVolt4").getValue()), "" + voltActive[4] },
    { "Voltage n°5", "" + int(cp5.getGroup("ddlVolt5").getValue()), "" + voltActive[5] },
    { "Voltage n°6", "" + int(cp5.getGroup("ddlVolt6").getValue()), "" + voltActive[6] },
    { "Current Sensor", "" + int(cp5.getGroup("currentPin").getValue()), "" + int(cp5.getController("current").getValue()) }, 
    { "Temperature Sensor", "" + int(cp5.getGroup("tempPin").getValue()), "" + int(cp5.getController("temperature").getValue()) },
    { "Vario (A4-A5)", "4", "" + int(cp5.getController("vario").getValue()) },
    { "Vario (A4-A5)", "5", "" + int(cp5.getController("vario").getValue()) }
  } ;    

  for ( int i = 0; i < analogPinsValidation.length; i++ ) {
    if ( int(analogPinsValidation[i][1]) == -1 && int(analogPinsValidation[i][2]) == 1 ) {
      analogPinsValid = false ;
      messageList.append("- " + analogPinsValidation[i][0] + " has no pin assigned !") ;      
    }
    for ( int j = i+1; j < analogPinsValidation.length; j++ ) {
      
      if ( int(analogPinsValidation[i][1]) != -1 && int(analogPinsValidation[j][1]) != -1 && int(analogPinsValidation[i][2]) == 1 && int(analogPinsValidation[j][2]) == 1 ) {
        if ( analogPinsValidation[i][1].equals(analogPinsValidation[j][1]) ) {
          //println("Attention !!  " + analogPinsValidation[i][0] + " is using the same pin n°A" + analogPinsValidation[i][1] + " as " + analogPinsValidation[j][0] + " !") ;
          analogPinsValid = false ;
          messageList.append("- " + analogPinsValidation[i][0] + " is using the same pin n°A" + analogPinsValidation[i][1] + " as " + analogPinsValidation[j][0] + " !") ;          
        }
      }
      
    }
  }
/*  
  if ( analogPinsValid ) {
    println("No problem found with analog pins ;)") ;
    messageList.append("No problem found with analog pins ;)");  
  }
*/  
}

void validateCells() {
  
  int cellsNbr = 0 ;
  
 if ( cp5.getController("cells").getValue() == 1 ) {
   //println("Cells active") ;
   for ( int i = 1 ; i <= voltNbr ; i++ ) {
     if ( cp5.getController("volt" + i ).getValue() == 1 ) {
       cellsNbr ++ ;
     } else {       
       break ; 
     }    
   }
   if ( cp5.getGroup("ddlNbrCells").getValue() > cellsNbr ) {
     cellsValid = false ;
     messageList.append( "- You can't monitor more than " + cellsNbr + " cell(s)" ) ; 
   }
   //println(cellsNbr ) ;
 }
 
} 

void validateSentData() {
  
  int oxsMeasureCount = 0 ;
  
  String oxsMeasureValidationList[][] = new String[dataSentFieldNbr + 1][5] ;
  for ( int i = 1 ; i < oxsMeasureValidationList.length ; i++ ) { 
    oxsMeasureValidationList[i][0] = "" + sentDataList[ int( cp5.getGroup("sentDataField" + i ).getValue() ) ][0] ; // Data sent - ( OXS measurement )
    oxsMeasureValidationList[i][1] = "" + cp5.getGroup("sentDataField" + i ).getCaptionLabel().getText() ;          // OXS measurement - ( Data sent )
    oxsMeasureValidationList[i][2] = "" + cp5.getGroup("hubDataField" + i ).getCaptionLabel().getText() ;           // HUB data field
    oxsMeasureValidationList[i][3] = "" + cp5.getGroup("sPortDataField" + i ).getCaptionLabel().getText() ;         // SPORT data field 
    oxsMeasureValidationList[i][4] = "0" ;                                                                          // is measurement active
  }
  for ( int i = 1 ; i <= dataSentFieldNbr ; i++ ) {
    switch ( int( cp5.getGroup("sentDataField" + i ).getValue() ) ) {
      case 1 :                 // "ALTIMETER"
      case 2 :                 // "VERTICAL_SPEED"
      case 3 :                 // "SENSITIVITY"
      case 4 :                 // "ALT_OVER_10_SEC"
        if ( cp5.getController("vario").getValue() == 1 ) { oxsMeasureValidationList[i][4] = "1" ; }
        break ;
      case 5 :                 // "CURRENTMA"       
      case 6 :                 // "MILLIAH"
        if ( cp5.getController("current").getValue() == 1 ) { oxsMeasureValidationList[i][4] = "1" ; } 
        break ;
      case 7 :                 // "CELLS"
        if ( cp5.getController("voltage").getValue() == 1 && cp5.getController("cells").getValue() == 1 ) { oxsMeasureValidationList[i][4] = "1" ; }        
        break ;
      case 8 :                 // "VOLT1"  
        if ( cp5.getController("voltage").getValue() == 1 && cp5.getController("volt1").getValue() == 1 ) { oxsMeasureValidationList[i][4] = "1" ; }
        break ;
      case 9 :                 // "VOLT2"  
        if ( cp5.getController("voltage").getValue() == 1 && cp5.getController("volt2").getValue() == 1 ) { oxsMeasureValidationList[i][4] = "1" ; }
        break ;
      case 10 :                // "VOLT3"  
        if ( cp5.getController("voltage").getValue() == 1 && cp5.getController("volt3").getValue() == 1 ) { oxsMeasureValidationList[i][4] = "1" ; }
        break ;
      case 11 :                // "VOLT4"  
        if ( cp5.getController("voltage").getValue() == 1 && cp5.getController("volt4").getValue() == 1 ) { oxsMeasureValidationList[i][4] = "1" ; }
        break ;
      case 12 :                // "VOLT5"  
        if ( cp5.getController("voltage").getValue() == 1 && cp5.getController("volt5").getValue() == 1 ) { oxsMeasureValidationList[i][4] = "1" ; }
        break ;
      case 13 :                // "VOLT6"
        if ( cp5.getController("voltage").getValue() == 1 && cp5.getController("volt6").getValue() == 1 ) { oxsMeasureValidationList[i][4] = "1" ; }
        break ;
      case 14 :                // "RPM"
        if ( cp5.getController("rpm").getValue() == 1 ) { oxsMeasureValidationList[i][4] = "1" ; }
        break ;  
    }    
  }  
  
  int oxsMeasureCountHUB[][] = new int[sentDataList.length + 1][hubDataList.length] ;
  int oxsMeasureCountSPORT[][] = new int[sentDataList.length + 1][sPortDataList.length] ;
  
  for ( int i = 1 ; i <= dataSentFieldNbr ; i++ ) {
    if ( cp5.getGroup("sentDataField" + i).getValue() > 0 ) {   // if OXS measurement field is not empty
      oxsMeasureCount ++ ;
      if ( oxsMeasureValidationList[i][4].equals("1") ) {                // if OXS measurement is valid (sensor available)
        if ( cp5.getGroup("protocolChoice").getValue() == 1 ) {          // if HUB protocol
            if ( cp5.getGroup("hubDataField" + i).getValue() == 0 ) {    // if telemetry field is empty
              oxsMeasureValid = false ; 
              messageList.append( "- The " + oxsMeasureValidationList[i][1] + " measure is not sent !" ) ;
            } else if ( ( oxsMeasureValidationList[i][0].equals("ALTIMETER") || oxsMeasureValidationList[i][0].equals("CELLS")          // OXS measurement must be default
                          || oxsMeasureValidationList[i][0].equals("RPM") ) && !oxsMeasureValidationList[i][2].equals("DEFAULT") ) {
                oxsMeasureValid = false ; 
                messageList.append( "- " + oxsMeasureValidationList[i][1] + " must be set to DEFAULT !" ) ;
            } else if ( ( oxsMeasureValidationList[i][0].equals("SENSITIVITY") || oxsMeasureValidationList[i][0].equals("ALT_OVER_10_SEC")   // OXS measurement can't be default
                          || oxsMeasureValidationList[i][0].equals("VOLT1") || oxsMeasureValidationList[i][0].equals("VOLT2") 
                          || oxsMeasureValidationList[i][0].equals("VOLT3") || oxsMeasureValidationList[i][0].equals("VOLT4") 
                          || oxsMeasureValidationList[i][0].equals("VOLT5") || oxsMeasureValidationList[i][0].equals("VOLT6")
                          || oxsMeasureValidationList[i][0].equals("MILLIAH") ) && oxsMeasureValidationList[i][2].equals("DEFAULT") ) {
                oxsMeasureValid = false ; 
                messageList.append( "- " + oxsMeasureValidationList[i][1] + " can't be set to DEFAULT !" ) ;
            } 
            oxsMeasureCountHUB[int( cp5.getGroup("sentDataField" + i).getValue() )][int( cp5.getGroup("hubDataField" + i).getValue() )] ++ ;
            oxsMeasureCountHUB[sentDataList.length][int( cp5.getGroup("hubDataField" + i).getValue() )] ++ ; 
            
        } else if ( cp5.getGroup("protocolChoice").getValue() == 2 ) {          // if SMART PORT protocol
            if ( cp5.getGroup("sPortDataField" + i).getValue() == 0 ) {              
              oxsMeasureValid = false ; 
              messageList.append( "- The " + oxsMeasureValidationList[i][1] + " measure is not sent !" ) ;
            } else if ( ( oxsMeasureValidationList[i][0].equals("CELLS") || oxsMeasureValidationList[i][0].equals("RPM") )   // OXS measurement must be default
                          && !oxsMeasureValidationList[i][3].equals("DEFAULT") ) {                            
                oxsMeasureValid = false ; 
                messageList.append( "- " + oxsMeasureValidationList[i][1] + " must be set to DEFAULT !" ) ;                
            } else if ( ( oxsMeasureValidationList[i][0].equals("SENSITIVITY") || oxsMeasureValidationList[i][0].equals("ALT_OVER_10_SEC")   // OXS measurement can't be default
                          || oxsMeasureValidationList[i][0].equals("VOLT1") || oxsMeasureValidationList[i][0].equals("VOLT2") 
                          || oxsMeasureValidationList[i][0].equals("VOLT3") || oxsMeasureValidationList[i][0].equals("VOLT4") 
                          || oxsMeasureValidationList[i][0].equals("VOLT5") || oxsMeasureValidationList[i][0].equals("VOLT6")
                          || oxsMeasureValidationList[i][0].equals("MILLIAH") ) && oxsMeasureValidationList[i][3].equals("DEFAULT") ) {
                oxsMeasureValid = false ; 
                messageList.append( "- " + oxsMeasureValidationList[i][1] + " can't be set to DEFAULT !" ) ;
            }
            oxsMeasureCountSPORT[int( cp5.getGroup("sentDataField" + i).getValue() )][int( cp5.getGroup("sPortDataField" + i).getValue() )] ++ ;
            oxsMeasureCountSPORT[sentDataList.length][int( cp5.getGroup("sPortDataField" + i).getValue() )] ++ ;
        }
      } else if ( oxsMeasureValidationList[i][0].equals("CELLS") ) {
          oxsMeasureValid = false ; 
          messageList.append( "- " + oxsMeasureValidationList[i][1] + " is not activated in voltage tab. !" ) ;        
      } else {
          oxsMeasureValid = false ; 
          messageList.append( "- " + oxsMeasureValidationList[i][1] + " needs the sensor to be active !" ) ;        
      }    
    }
  }
  
  if ( cp5.getGroup("protocolChoice").getValue() == 1 ) {          //  HUB protocol --> Different duplicates tests
    for ( int k = 1 ; k <= sentDataList.length ; k++ ) {
/*
      if ( k == sentDataList.length ) {
        print( "TOTAL: " ) ;
      } else {  
        print( sentDataList[k][1] + ": " ) ;
      }
*/
      for ( int l = 1 ; l < hubDataList.length ; l++ ) {
         //print( oxsMeasureCountHUB[k][l] + " " ) ;
         if ( oxsMeasureCountHUB[k][l] > 1 && k < sentDataList.length ) {
           oxsMeasureValid = false ; 
           messageList.append( "- " + sentDataList[k][1] + " can't be sent " + oxsMeasureCountHUB[k][l] + "X to the same " +  hubDataList[l][1] + " field !" ) ;
         }       
         if ( k == sentDataList.length && l > 1 && oxsMeasureCountHUB[sentDataList.length][l] > 1 ) {
           oxsMeasureValid = false ; 
           messageList.append( "- Different measurements can't be sent to the same " +  hubDataList[l][1] + " field !" ) ;
         }           
      }
      //println("");    
    }
    if ( oxsMeasureCountHUB[2][1] == 1 && oxsMeasureCountHUB[sentDataList.length][2] >= 1 ) {  // Test VERTICAL_SPEED  default/VSpd
        oxsMeasureValid = false ; 
        messageList.append( "- Vertical Speed not available as it's already used by VERTICAL_SPEED !" ) ;
    }
    if ( oxsMeasureCountHUB[5][1] == 1 && oxsMeasureCountHUB[sentDataList.length][3] >= 1 ) {  // Test CURRENTMA  default/Curr
        oxsMeasureValid = false ; 
        messageList.append( "- Current not available as it's already used by CURRENTMA !" ) ;
    }
    if ( oxsMeasureCountHUB[14][1] == 1 && oxsMeasureCountHUB[sentDataList.length][7] >= 1 ) {  // Test RPM default/RPM
        oxsMeasureValid = false ; 
        messageList.append( "- RPM not available as it's already used by RPM !" ) ;
    }
 } else if ( cp5.getGroup("protocolChoice").getValue() == 2 ) {         //  SPORT protocol --> Different duplicates tests
   for ( int k = 1 ; k <= sentDataList.length ; k++ ) {
/*     
     if ( k == sentDataList.length ) {
       print( "TOTAL: " ) ;
     } else {  
       print( sentDataList[k][1] + ": " ) ;
     }
*/     
     for ( int l = 1 ; l < sPortDataList.length ; l++ ) {
         //print( oxsMeasureCountSPORT[k][l] + " " ) ;
         if ( oxsMeasureCountSPORT[k][l] > 1 && k < sentDataList.length ) {
           oxsMeasureValid = false ; 
           messageList.append( "- " + sentDataList[k][1] + " can't be sent " + oxsMeasureCountSPORT[k][l] + "X to the same " +  sPortDataList[l][1] + " field !" ) ;
         }       
         if ( k == sentDataList.length && l > 1 && oxsMeasureCountSPORT[sentDataList.length][l] > 1 ) {
           oxsMeasureValid = false ; 
           messageList.append( "- Different measurements can't be sent to the same " +  sPortDataList[l][1] + " field !" ) ;
         }           
      }
      //println("");    
    }
    if ( oxsMeasureCountSPORT[1][1] == 1 && oxsMeasureCountSPORT[sentDataList.length][2] >= 1 ) {  // Test ALTITUDE  default/Alt
        oxsMeasureValid = false ; 
        messageList.append( "- Altitude not available as it's already used by Altitude !" ) ;
    }
    if ( oxsMeasureCountSPORT[2][1] == 1 && oxsMeasureCountSPORT[sentDataList.length][3] >= 1 ) {  // Test VERTICAL_SPEED  default/VSpd
        oxsMeasureValid = false ; 
        messageList.append( "- VSpd not available as it's already used by VERTICAL_SPEED !" ) ;
    }
    if ( oxsMeasureCountSPORT[5][1] == 1 && oxsMeasureCountSPORT[sentDataList.length][4] >= 1 ) {  // Test CURRENTMA  default/Curr
        oxsMeasureValid = false ; 
        messageList.append( "- Curr not available as it's already used by CURRENTMA !" ) ;
    }
    if ( oxsMeasureCountSPORT[14][1] == 1 && oxsMeasureCountSPORT[sentDataList.length][8] >= 1 ) {  // Test RPM default/RPM
        oxsMeasureValid = false ; 
        messageList.append( "- RPM not available as it's already used by RPM !" ) ;
    }
   
 }  
  
  if ( oxsMeasureCount == 0 ) {  
    oxsMeasureValid = false ; 
    messageList.append( "- You don't have any OXS measurement set !" ) ;    
  }
  
}

void validateVersion () {  
  
  String version[] = loadStrings(oxsDirectory + "/version.oxs");
  
  if ( numPinsValid && analogPinsValid && cellsValid && oxsMeasureValid ) {
    if ( version == null ) {
      versionValid = 1 ;  
      
      messageList.append( "          -- The Configurator can't find OXS version number --" ) ;
      messageList.append( "          --      Configuration file may not be compatible...     --" ) ;
      
      //println("no version file");    
    } else if ( version[0].charAt(1) == oxsCversion.charAt(1) ) { 
      
      //println("OXS and the Configurator are compatible,");
      //println("OXS version = " + version[0] + " and OXSC version = " + oxsCversion);
      
    } else {
      versionValid = 0 ; 
      messageList.append( "-- The Configurator " + oxsCversion + " isn't compatible with OXS " + version[0] + " --" ) ;
      messageList.append( "" ) ;
      messageList.append( "  You may go to \" https://code.google.com/p/openxsensor/ \" " ) ;
      messageList.append( "  and download the latest version of both OXS and OXS" ) ;
      messageList.append( "  Configurator." ) ; 
      //println("OXS version " + version[0]);
    }
  }  
  
}
