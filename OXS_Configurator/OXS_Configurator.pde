/****************************************************************************
**                                                                         **
**  OpenXsensor Configurator: GUI for generating oXs_config.h file         **
**  Copyright (C) 2015   David LABURTHE                                    **
**                                                                         **
**  This program is free software: you can redistribute it and/or modify   **
**  it under the terms of the GNU General Public License as published by   **
**  the Free Software Foundation, either version 3 of the License, or      **
**  (at your option) any later version.                                    **
**                                                                         **
**  This program is distributed in the hope that it will be useful,        **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of         **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          **
**  GNU General Public License for more details.                           **
**                                                                         **
**  You should have received a copy of the GNU General Public License      **
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.  **
**                                                                         **
*****************************************************************************
**                        Author: David LABURTHE                           **
**                      Contact: dlaburthe@free.fr                         **
**                           Date: 04.01.2015                              **
*****************************************************************************/

import controlP5.* ;

boolean tempActive = false ;           // Define temperature sensor availability

String oxsVersion = "v2.x" ;
String oxsCversion = "v2.1" ;

String day = (day() < 10) ? "0" + day() : "" + day() ;
String month = (month() < 10) ? "0" + month() : "" + month() ;

String oxsDirectory = "" ;
PrintWriter output ;
String outputConfigDir = "" ;

int blueAct = color(15, 165, 255) ;
int lightBlue = color(181, 207, 219) ;
int orangeAct = color(255, 128, 0) ;
int lightOrange = color(219, 191, 181) ;
int grayedColor = color(155) ;
int okColor = color(24, 208, 24) ;
int warnColor = color(255, 183, 0) ;
int errorColor = color(255, 35, 35) ;

PShape oxsI ;
PShape oxsL ;

PFont fontLabel ;     //= createFont("arial.ttf", 12, false) ;
PFont fontItalic ;    //= createFont("arial italic", 12, false) ;
PFont fontCells ;     //= createFont("arial bold", 12, false) ;
PFont font16 ;        //= createFont("arial", 16, false) ;
PFont font20 ;        //= createFont("arial", 20, false) ;

ControlP5 cp5 ;

ControlGroup messageBox ;

Tab general ;

Textfield oxsDir ;

DropdownList prot ;
Textlabel protocol ;

DropdownList serialPin ;
Textlabel serialPinL ;

Textlabel aVccL ;
Textlabel sensors ;

Textlabel sensitivity ;
DropdownList ppmPin ;
Textlabel ppmPinL ;
DropdownList climbPin ;

CheckBox checkbox ;

DropdownList ddlNbrCells ;

int voltNbr = 6 ;
int dataSentFieldNbr = 10 ;

String[] analogPins = new String[8] ;                                      // Analogic pins array
String[] sensorIDs = { "0x1B", "0xBA", "0x39", "0x16", "0x95", "0x34" } ;  // Sensor ID array

StringList messageList = new StringList() ;

boolean numPinsValid ;
boolean analogPinsValid ;
int vSpeedValid ;            // 0 -> not valid    1 -> warning   2 -> valid
boolean cellsValid ;
boolean sentDataValid ;
int versionValid ;           // 0 -> not valid    1 -> warning   2 -> valid

int allValid ;               // 0 -> not valid    1 -> warning   2 -> valid

int tabColor = 200 ;
int myColorBackground = 150 ;

int sensMin ;
int sensMax ;
int vSpeedMin ;
int vSpeedMax ;
int sensMinMax ;
int vSpeedMinMax ;
int ppmRngMinMax ;
int ppmRngMin ;
int ppmRngMax ;
int varioHysteresis ;
int ppmRngSensMinMax ;
int ppmSensMinMax ;
int ppmVspeedSwMin ;
int ppmVspeedSwMax ;
int outClimbRateMinMax ;
int aSpeedReset ;
int ppmRngCompMinMax ;
int ppmCompMinMax ;

int mBoxWidth = 400 ;
int mBoxHeight = 320 ;

// *************************************************************************
// ********************************  SETUP  ********************************
// *************************************************************************
void setup() {

  size(450, 460) ;
  noStroke() ;

  cp5 = new ControlP5(this) ;

  //Alt+mouseDragged to move controllers on the screen
  //Alt+Shift+h to show/hide controllers
  //Alt+Shift+s to save properties (what are properties? have a look at the properties examples)
  //Alt+Shift+l to load properties
  //cp5.enableShortcuts() ;

  oxsI = loadShape("OXSc_Icon.svg") ;
  oxsL = loadShape("OXSc_Logo.svg") ;

  PGraphics icon = createGraphics(64, 64, JAVA2D) ;
  icon.beginDraw() ;
  icon.shape(oxsI, 0, 0, 64, 64) ;
  icon.endDraw() ;
  frame.setIconImage(icon.image) ;

  fontLabel = createFont("arial.ttf", 12, false) ;
  fontItalic = createFont("ariali.ttf", 12, false) ;
  fontCells = createFont("arialbd.ttf", 12, false) ;
  font16 = createFont("arial.ttf", 16, false) ;
  font20 = createFont("arial.ttf", 20, false) ;

  for ( int i = 0; i < analogPins.length; i++ ) {
    analogPins[i] = ("A" + i ) ;
  }

  cp5.setControlFont(fontLabel, 12) ;

  // ------------------------ TABS definition ------------------------
  // By default all controllers are stored inside Tab 'default'
  cp5.window().setPositionOfTabs(0, 80) ;

  // About
  cp5.addButton("about")
     .setLabel("About")
     .setPosition(380, 14)
     .setSize(40, 15)
     .setColorLabel(#000000)
     .setColorBackground(color(150))
     .setColorForeground(blueAct)
     .setColorActive(orangeAct)
     .setTab("global")
     ;
  cp5.getController("about").captionLabel().toUpperCase(false) ;

  // ----------------------- Tab 0 : GENERAL SETTINGS ----------------------
  general = cp5.getTab("default")
               .setHeight(20)
               .setColorLabel(color(255))
               .setColorForeground(color(200))
               .setColorBackground(color(70))
               .setColorActive(orangeAct)
               .setLabel("GENERAL Settings")
               .setId(0)
               ;
  general.captionLabel().toUpperCase(false) ;

  // OXS directory
  oxsDir = cp5.addTextfield("oxsDirectory")
              .setLabel("OXS directory  ")
              .setPosition(95, 110)
              .setColorLabel(color(0))
              .setSize(310, 22)
              .setAutoClear(false)
              ;
  oxsDir.getCaptionLabel().align(ControlP5.LEFT_OUTSIDE, ControlP5.CENTER) ;
  oxsDir.captionLabel().toUpperCase(false) ;
  cp5.getTooltip().register("oxsDirectory", "Choose OXS source directory") ;

  cp5.addButton("oxsDirButton")
     .setColorForeground(blueAct)
     .setLabel(". . .")
     .setPosition(410, 111)
     .setSize(25, 20)
     ;
  cp5.getTooltip().register("oxsDirButton", "Choose OXS source directory") ;


  // Protocol choice
/*
  protocol = cp5.addTextlabel("protocol")
                .setText("FrSky Protocol                                   ")
                .setPosition(10, 141)
                .setColorValue(#000000)
                .setTab("default")
                ;
  //protocol.captionLabel().toUpperCase(false) ;
  cp5.getProperties().remove(cp5.getController("protocol")) ;
  cp5.getTooltip().register("protocol", "Choose protocol") ;
*/
  prot = cp5.addDropdownList("protocolChoice")
            .setPosition(100, 160)
            .setSize(105, 300)
            .setColorForeground(blueAct)
            .setBackgroundColor(color(190))
            .setItemHeight(20)
            .setBarHeight(20)
            .hide()
            ;
  prot.captionLabel().set("Choose") ;
  prot.captionLabel().style().marginTop = 2 ;
  prot.addItem("HUB - D rx", 1) ;
  prot.addItem("Smart Port - X rx", 2) ;
  prot.toUpperCase(false) ;
  cp5.getProperties().remove(cp5.getGroup("protocolChoice"), "ListBoxItems") ;


  // Serial PIN choice
  serialPinL = cp5.addTextlabel("serialPinlabel")
                  .setText("Serial output Pin number            ")
                  .setPosition(10, 144)
                  .setColorValue(#000000)
                  ;
  cp5.getProperties().remove(cp5.getController("serialPinlabel")) ;
  cp5.getTooltip().register("serialPinlabel", "Choose the serial output Pin number - Default: 4 -") ;

  serialPin = cp5.addDropdownList("serialPin")
                 .setPosition(160, 163)
                 .setSize(25, 300)
                 .setColorForeground(blueAct)
                 .setBackgroundColor(color(190))
                 .setItemHeight(20)
                 .setBarHeight(20)
                 ;
  serialPin.captionLabel().set(" ") ;
  serialPin.captionLabel().style().marginTop = 2 ;
  serialPin.addItem("2", 2) ;
  serialPin.addItem("4", 4) ;
  serialPin.setValue(4) ;
  cp5.getProperties().remove(cp5.getGroup("serialPin"), "ListBoxItems") ;

  // Sensor ID choice
  cp5.addTextlabel("sensorIDlabel")
     .setText("Sensor ID                    ")
     .setPosition(290, 144)
     .setColorValue(#000000)
     .setTab("default")
     ;
  cp5.getProperties().remove(cp5.getController("sensorIDlabel")) ;
  cp5.getTooltip().register("sensorIDlabel", "Choose S.Port sensor ID - Default: 0x1B -") ;

  cp5.addDropdownList("sensorID")
     .setPosition(355, 163)
     .setSize(50, 300)
     .setColorForeground(blueAct)
     .setBackgroundColor(color(190))
     .setItemHeight(20)
     .setBarHeight(20)
     ;
  cp5.getGroup("sensorID").captionLabel().align(ControlP5.LEFT, ControlP5.CENTER).setPaddingX(8) ;
  cp5.get(DropdownList.class, "sensorID").addItems(sensorIDs) ;
  cp5.get(DropdownList.class, "sensorID").setValue(0) ;
  cp5.get(DropdownList.class, "sensorID").toUpperCase(false) ;
  cp5.getProperties().remove(cp5.getGroup("sensorID"), "ListBoxItems") ;

  // Voltage reference  
  cp5.addTextlabel("voltRef")
     .setText("Voltage reference                                   ")
     .setPosition(10, 200)
     .setColorValue(#000000)
     .setTab("default")
     ;
  cp5.getProperties().remove(cp5.getController("voltRef")) ;
  cp5.getTooltip().register("voltRef", "Choose voltage reference") ;

  cp5.addDropdownList("voltRefChoice")
     .setPosition(160, 218)
     .setSize(95, 100)
     .setColorForeground(blueAct)
     .setBackgroundColor(color(190))
     .setItemHeight(20)
     .setBarHeight(20)
     ;
  cp5.getGroup("voltRefChoice").captionLabel().set("Choose") ;
  cp5.getGroup("voltRefChoice").captionLabel().style().marginTop = 2 ;
  cp5.get(DropdownList.class, "voltRefChoice").addItem("  Arduino VCC", 1) ;
  cp5.get(DropdownList.class, "voltRefChoice").addItem("Internal (1.1V)", 2) ;
  cp5.get(DropdownList.class, "voltRefChoice").setValue(1) ;
  cp5.get(DropdownList.class, "voltRefChoice").toUpperCase(false) ;
  cp5.getProperties().remove(cp5.getGroup("voltRefChoice"), "ListBoxItems") ;
  
  cp5.addNumberbox("arduinoVccNb")
     .setPosition(265, 197)
     .setSize(37, 20)
     .setRange(0, 9.99)
     .setMultiplier(0.01)                    // set the sensitifity of the numberbox
     .setDecimalPrecision(2)
     .setDirection(Controller.HORIZONTAL)    // change the control direction to left/right
     .setValue(5)
     .setLabel("volts")
     .setColorLabel(color(0))
     ;
  cp5.getController("arduinoVccNb").getCaptionLabel().align(ControlP5.RIGHT_OUTSIDE, ControlP5.CENTER).setPaddingX(5) ;
  cp5.getController("arduinoVccNb").captionLabel().toUpperCase(false) ;
  cp5.getTooltip().register("arduinoVccNb", "Arduino alimentation voltage") ;

  // Save to EEPROM
  cp5.addToggle("saveEprom")
     .setPosition(145, 258)
     .setLabel("Save data to EEPROM")
     ;
  customizeToggleSensor(cp5.getController("saveEprom")) ;
  cp5.getController("saveEprom").getCaptionLabel().align(ControlP5.LEFT_OUTSIDE, ControlP5.CENTER).setPaddingX(8) ;

  // Reset button pin
  cp5.addTextlabel("resetButtonPinLabel")
     .setText("Reset button Pin number           ")
     .setPosition(210, 257)
     .setColorValue(#000000)
     ;
  cp5.getProperties().remove(cp5.getController("resetButtonPinLabel")) ;
  cp5.getTooltip().register("resetButtonPinLabel", "- Default: 10 -") ;

  cp5.addDropdownList("resetButtonPin")
     .setPosition(355, 276)
     .setSize(30, 180)
     .setColorForeground(blueAct)
     .setBackgroundColor(color(190))
     .setItemHeight(20)
     .setBarHeight(20)
     ;
  cp5.get(DropdownList.class, "resetButtonPin").captionLabel().set(" ") ;
  cp5.get(DropdownList.class, "resetButtonPin").captionLabel().style().marginTop = 2 ;
  for ( int i = 2; i <= 12; i++ ) {
    cp5.get(DropdownList.class, "resetButtonPin").addItem("" + i, i) ;
  }
  cp5.getGroup("resetButtonPin").setValue(10) ;
  cp5.getProperties().remove(cp5.getGroup("resetButtonPin"), "ListBoxItems") ;

  // - Sensors  part -
  sensors = cp5.addTextlabel("sensors")
               .setText("SENSORS")
               .setPosition(191, 295)
               .setColorValue(#FFFFFF)
               ;
  cp5.getProperties().remove(cp5.getController("sensors")) ;
  cp5.getTooltip().register("sensors", "Sensors settings") ;

  // Toggle buttons
  cp5.addToggle("vario")
     .setPosition(145, 336)
     .setLabel("Vario 1        ")
     ;
  customizeToggleSensor(cp5.getController("vario")) ;
  
  cp5.addToggle("vario2")
     .setPosition(265, 336)
     .setLabel("Vario 2        ")
     ;
  customizeToggleSensor(cp5.getController("vario2")) ;
  
  cp5.addToggle("airSpeed")
     .setPosition(385, 336)
     .setLabel("Air Speed       ")
     ;
  customizeToggleSensor(cp5.getController("airSpeed")) ;

  cp5.addToggle("voltage")
     .setPosition(145, 361)
     .setLabel("Voltage / Other   ")
     ;
  customizeToggleSensor(cp5.getController("voltage")) ;

  cp5.addToggle("current")
     .setPosition(265, 361)
     .setLabel("Current        ")
     ;
  customizeToggleSensor(cp5.getController("current")) ;


  cp5.addToggle("temperature")
     .setPosition(85, 315)
     .setLabel("Temperature  ")
     ;
  customizeToggleSensor(cp5.getController("temperature")) ;
  if (!tempActive) {
    cp5.getController("temperature").hide() ;
  }

  cp5.addToggle("rpm")
     .setPosition(385, 361)
     .setLabel("RPM          ")
     ;
  customizeToggleSensor(cp5.getController("rpm")) ;

  // ---------------- PPM settings : Tabs Vario + Air Speed ----------------

  // RC Remote PPM pin and settings
  cp5.addToggle("ppm")
     .setLabel("PPM")
     .setPosition(53, 117)
     .hide()
     ;
  customizeToggle(cp5.getController("ppm")) ;

  ppmPinL = cp5.addTextlabel("ppmPinL")
               .setText("Pin       ")
               .setPosition(90, 117)
               .setColorValue(#000000)
               .hide()
               ;
  cp5.getProperties().remove(cp5.getController("ppmPinL")) ;
  cp5.getTooltip().register("ppmPinL", "- Default: 2 -");

  ppmPin = cp5.addDropdownList("ppmPin")
              .setPosition(118, 136)
              .setSize(30, 75)
              .setColorForeground(orangeAct)
              .setColorActive(blueAct)
              .setBackgroundColor(color(190))
              .setItemHeight(20)
              .setBarHeight(20)
              .hide()
              ;
  ppmPin.captionLabel().set(" ");
  ppmPin.captionLabel().style().marginTop = 2 ;
  ppmPin.addItem(" 2", 2);
  ppmPin.addItem(" 3", 3);
  ppmPin.setValue(2);
  ppmPin.toUpperCase(false) ;
  cp5.getProperties().remove(cp5.getGroup("ppmPin"), "ListBoxItems") ;

  // PPM range setting
  cp5.addTextlabel("ppmRngL")
     .setText("PPM range (µs)                                                                          ")
     .setPosition(175, 117)
     .setColorValue(#000000)
     .hide()
     ;
  cp5.getProperties().remove(cp5.getController("ppmRngL")) ;
  cp5.getTooltip().register("ppmRngL", "RC control range - Default: 988:2012 -");

  cp5.addNumberbox("ppmRngMin")
     .setPosition(306, 115)
     .setSize(40, 20)
     .setColorActive(blueAct)
     .setRange(888, 1088)
     .setMultiplier(0.5)                     // set the sensitifity of the numberbox
     .setDirection(Controller.HORIZONTAL)    // change the control direction to left/right
     .setValue(988)
     .setLabel("Min.")
     .setColorLabel(color(0))
     .hide()
     ;
  cp5.getController("ppmRngMin").getCaptionLabel().align(ControlP5.LEFT_OUTSIDE, ControlP5.CENTER).setPaddingX(5) ;
  cp5.getController("ppmRngMin").captionLabel().toUpperCase(false) ;

  cp5.addNumberbox("ppmRngMax")
     .setPosition(366, 115)
     .setSize(40, 20)
     .setColorActive(blueAct)
     .setRange(1912, 2112)
     .setMultiplier(0.5)                     // set the sensitifity of the numberbox
     .setDirection(Controller.HORIZONTAL)    // change the control direction to left/right
     .setValue(2012)
     .setLabel("Max.")
     .setColorLabel(color(0))
     .hide()
     ;
  cp5.getController("ppmRngMax").getCaptionLabel().align(ControlP5.RIGHT_OUTSIDE, ControlP5.CENTER).setPaddingX(5) ;
  cp5.getController("ppmRngMax").captionLabel().toUpperCase(false) ;

  // ---------------------------- Tab 1 : Vario settings ------------------------------
  cp5.getTab("vario")
     .activateEvent(true)
     .setHeight(20)
     .setColorForeground(color(200))
     .setColorBackground(color(70))
     .setColorActive(blueAct)
     .setLabel("Vario")
     .setId(1)
     .hide()
     ;
  cp5.getTab("vario").captionLabel().toUpperCase(false) ;

  // PPM sensitivity at PPM range
  cp5.addTextlabel("ppmRngSensL")
     .setText("PPM range for sensitivity   Min.")
     .setPosition(10, 153)
     .setColorValue(#000000)
     .setTab("vario")
     ;
  cp5.getProperties().remove(cp5.getController("ppmRngSensL")) ;

  cp5.addRange("ppmRngSensMinMax")
     .setPosition(186, 151)
     .setSize(220, 20)
     .setCaptionLabel("Max.")
     .setHandleSize(15)
     .setRange(0, 100)
     .setRangeValues(10, 40)
     ;
  customizeRange(cp5.getController("ppmRngSensMinMax")) ;
  cp5.getTooltip().register("ppmRngSensMinMax", "- Default: 10:40 -") ;

  // PPM sensitivity range
  cp5.addTextlabel("ppmSensRngL")
     .setText("PPM sensitivity                   Min.")
     .setPosition(10, 180)
     .setColorValue(#000000)
     .setTab("vario")
     ;
  cp5.getProperties().remove(cp5.getController("ppmSensRngL")) ;

  cp5.addRange("ppmSensMinMax")
     .setPosition(186, 178)
     .setSize(220, 20)
     .setCaptionLabel("Max.")
     .setHandleSize(15)
     .setRange(20, 150)
     .setRangeValues(20, 100.5)
     ;
  customizeRange(cp5.getController("ppmSensMinMax")) ;
  cp5.getTooltip().register("ppmSensMinMax", "RC control sensitivity range - Default: 20:100 -") ;

  // V.Speed type switching
  cp5.addTextlabel("vSpeedTypeSw")
     .setText("V.Speed type                                                                                                  ")
     .setPosition(10, 215)
     .setColorValue(#000000)
     .setTab("vario")
     ;
  cp5.getProperties().remove(cp5.getController("vSpeedTypeSw")) ;
  cp5.getTooltip().register("vSpeedTypeSw", "Choose the 2 different V.Speed you want to switch") ;

  cp5.addTextlabel("vStSwitching")
     .setText("switching")
     .setPosition(84, 215)
     .setColorValue(#000000)
     .setTab("vario")
     ;
  cp5.getProperties().remove(cp5.getController("vStSwitching")) ;

  cp5.addTextlabel("vStSw1L")
     .setText("1")
     .setPosition(171, 215)
     .setColorValue(#000000)
     .setTab("vario")
     ;
  cp5.getProperties().remove(cp5.getController("vStSw1L")) ;

  cp5.addDropdownList("vSpeed1")
     .setPosition(186, 234)
     .setSize(100, 75)
     .setColorForeground(orangeAct)
     .setColorActive(blueAct)
     .setBackgroundColor(color(190))
     .setItemHeight(20)
     .setBarHeight(20)
     .setTab("vario")
     ;
  cp5.getGroup("vSpeed1").captionLabel().style().marginTop = 2 ;
  cp5.get(DropdownList.class, "vSpeed1").addItem("       Vario 1", 0) ;
  cp5.get(DropdownList.class, "vSpeed1").addItem("       Vario 2", 1) ;
  cp5.get(DropdownList.class, "vSpeed1").addItem(" V1 + A.Speed", 2) ;
  cp5.get(DropdownList.class, "vSpeed1").setValue(0) ;
  cp5.get(DropdownList.class, "vSpeed1").toUpperCase(false) ;
  cp5.getProperties().remove(cp5.getGroup("vSpeed1"), "ListBoxItems") ;

  cp5.addTextlabel("vStSw2L")
     .setText("2")
     .setPosition(291, 215)
     .setColorValue(#000000)
     .setTab("vario")
     ;
  cp5.getProperties().remove(cp5.getController("vStSw2L")) ;

  cp5.addDropdownList("vSpeed2")
     .setPosition(306, 234)
     .setSize(100, 75)
     .setColorForeground(orangeAct)
     .setColorActive(blueAct)
     .setBackgroundColor(color(190))
     .setItemHeight(20)
     .setBarHeight(20)
     .setTab("vario")
     ;
  cp5.getGroup("vSpeed2").captionLabel().style().marginTop = 2 ;
  cp5.get(DropdownList.class, "vSpeed2").addItem("       Vario 1", 0) ;
  cp5.get(DropdownList.class, "vSpeed2").addItem("       Vario 2", 1) ;
  cp5.get(DropdownList.class, "vSpeed2").addItem(" V1 + A.Speed", 2) ;
  cp5.get(DropdownList.class, "vSpeed2").setValue(1) ;
  cp5.get(DropdownList.class, "vSpeed2").toUpperCase(false) ;
  cp5.getProperties().remove(cp5.getGroup("vSpeed2"), "ListBoxItems") ;

  // PPM Vertical speed switching values
  cp5.addTextlabel("ppmVspeedSw")
     .setText("PPM V.Speed switching (absolute values)                                                     ")
     .setPosition(10, 242)
     .setColorValue(#000000)
     .setTab("vario")
     ;
   cp5.getProperties().remove(cp5.getController("ppmVspeedSw")) ;
   cp5.getTooltip().register("ppmVspeedSw", "- Default: 10:90 -") ;

   cp5.addNumberbox("ppmVspeedSwMin")
      .setPosition(306, 240)
      .setSize(40, 20)
      .setColorActive(blueAct)
      .setRange(0, 100)
      .setMultiplier(0.5)                     // set the sensitifity of the numberbox
      .setDirection(Controller.HORIZONTAL)    // change the control direction to left/right
      .setValue(10)
      .setLabel("Min.")
      .setColorLabel(color(0))
      .setTab("vario")
      ;
  cp5.getController("ppmVspeedSwMin").getCaptionLabel().align(ControlP5.LEFT_OUTSIDE, ControlP5.CENTER).setPaddingX(5) ;
  cp5.getController("ppmVspeedSwMin").captionLabel().toUpperCase(false) ;

  cp5.addNumberbox("ppmVspeedSwMax")
     .setPosition(366, 240)
     .setSize(40, 20)
     .setColorActive(blueAct)
     .setRange(0, 100)
     .setMultiplier(0.5)                     // set the sensitifity of the numberbox
     .setDirection(Controller.HORIZONTAL)    // change the control direction to left/right
     .setValue(90)
     .setLabel("Max.")
     .setColorLabel(color(0))
     .setTab("vario")
     ;
  cp5.getController("ppmVspeedSwMax").getCaptionLabel().align(ControlP5.RIGHT_OUTSIDE, ControlP5.CENTER).setPaddingX(5) ;
  cp5.getController("ppmVspeedSwMax").captionLabel().toUpperCase(false) ;

  // Vario sensitivity range
  cp5.addTextlabel("sensitivityRange")
     .setText("Sensitivity                           Min.")
     .setPosition(10, 279)
     .setColorValue(#000000)
     .setTab("vario")
     ;
  cp5.getProperties().remove(cp5.getController("sensitivityRange")) ;
  cp5.getTooltip().register("sensitivityRange", "Sensitivity based on vertical speed - Default: 50:50 -") ;

  cp5.addRange("sensMinMax")
     .setPosition(186, 277)
     .setCaptionLabel("Max.")
     .setSize(220, 20)
     .setHandleSize(15)
     .setRange(20, 150)
     .setRangeValues(50.9, 50.9)
     ;
  customizeRange(cp5.getController("sensMinMax")) ;
  cp5.getTooltip().register("sensMinMax", "Sensitivity based on vertical speed - Default: 50:50 -") ;

  // Vario Vertical speed switching sensitivity range
  cp5.addTextlabel("vSpeedSensitivityRng")
     .setText("Sensitivity interpolated switching (cm/s)                                                           ")
     .setPosition(10, 306)
     .setColorValue(#000000)
     .setTab("vario")
     ;
   cp5.getProperties().remove(cp5.getController("vSpeedSensitivityRng")) ;
   cp5.getTooltip().register("vSpeedSensitivityRng", "Vertical speed threshold sensitivity - Default: 20:100 -") ;

   cp5.addNumberbox("vSpeedMin")
      .setPosition(306, 304)
      .setSize(40, 20)
      .setColorActive(blueAct)
      .setBroadcast(false)
      .setMultiplier(0.5)                     // set the sensitifity of the numberbox
      .setDirection(Controller.HORIZONTAL)    // change the control direction to left/right
      .setValue(20)
      .setLabel("Min.")
      .setColorLabel(color(0))
      .setTab("vario")
      ;
  cp5.getController("vSpeedMin").getCaptionLabel().align(ControlP5.LEFT_OUTSIDE, ControlP5.CENTER).setPaddingX(5) ;
  cp5.getController("vSpeedMin").captionLabel().toUpperCase(false) ;

  cp5.addNumberbox("vSpeedMax")
     .setPosition(366, 304)
     .setSize(40, 20)
     .setColorActive(blueAct)
     .setBroadcast(false)
     .setMultiplier(0.5)                     // set the sensitifity of the numberbox
     .setDirection(Controller.HORIZONTAL)    // change the control direction to left/right
     .setValue(100)
     .setLabel("Max.")
     .setColorLabel(color(0))
     .setTab("vario")
     ;
  cp5.getController("vSpeedMax").getCaptionLabel().align(ControlP5.RIGHT_OUTSIDE, ControlP5.CENTER).setPaddingX(5) ;
  cp5.getController("vSpeedMax").captionLabel().toUpperCase(false) ;

  cp5.getController("vSpeedMin").setBroadcast(true) ;
  cp5.getController("vSpeedMax").setBroadcast(true) ;

  // Vario hysteresis
  cp5.addSlider("varioHysteresis")
     .setPosition(186, 340)
     .setSize(220, 15)
     .setLabel("Hysteresis (cm/s)")
     .setColorForeground(blueAct)
     .setColorLabel(#000000)
     .setColorValue(#000000)
     .setRange(0, 100)
     .setValue(5)
     .setTab("vario")
     ;
  // reposition the Labels for controller 'slider'
  cp5.getController("varioHysteresis").getCaptionLabel().align(ControlP5.LEFT_OUTSIDE, ControlP5.CENTER).setPaddingX(78) ;
  cp5.getController("varioHysteresis").getValueLabel().align(ControlP5.RIGHT_OUTSIDE, ControlP5.CENTER).setPaddingX(10) ;
  cp5.getController("varioHysteresis").captionLabel().toUpperCase(false) ;
  cp5.getTooltip().register("varioHysteresis", "Minimum measurements difference - Default: 5 -") ;


  // Analog climb rate  pin and settings
  cp5.addToggle("analogClimb")
     .setLabel("Analog climb rate")
     .setPosition(117, 375)
     .setTab("vario")
     ;
  customizeToggle(cp5.getController("analogClimb")) ;

  cp5.addTextlabel("climbPinL")
     .setText("Pin            ")
     .setPosition(139, 374)
     .setColorValue(#000000)
     .setTab("vario")
     ;
  cp5.getProperties().remove(cp5.getController("climbPinL")) ;
  cp5.getTooltip().register("climbPinL", "- Default: 3 -") ;

  climbPin = cp5.addDropdownList("climbPin")
                .setPosition(165, 393)
                .setSize(30, 75)
                .setColorForeground(orangeAct)
                .setColorActive(blueAct)
                .setBackgroundColor(color(190))
                .setItemHeight(20)
                .setBarHeight(20)
                .setTab("vario")
                ;
  climbPin.captionLabel().style().marginTop = 2 ;
  climbPin.addItem(" 3", 3) ;
  climbPin.addItem("11", 11) ;
  climbPin.setValue(3) ;
  climbPin.toUpperCase(false) ;
  cp5.getProperties().remove(cp5.getGroup("climbPin"), "ListBoxItems") ;

  // Output climb rate range
  cp5.addTextlabel("outClimbRateRngL")
     .setText("Range (m/s)  Min.")
     .setPosition(201, 374)
     .setColorValue(#000000)
     .setTab("vario")
     ;
  cp5.getProperties().remove(cp5.getController("outClimbRateRngL")) ;

  cp5.addRange("outClimbRateMinMax")
     .setPosition(306, 372)
     .setSize(100, 20)
     .setCaptionLabel("Max.")
     .setHandleSize(15)
     .setRange(-20, 20)
     .setRangeValues(-3, 3.9)
     ;
  customizeRange(cp5.getController("outClimbRateMinMax")) ;
  cp5.getTooltip().register("outClimbRateMinMax", "Analog climb rate range - Default: -3:3 -") ;

  // ---------------------------- Tab 2 : Air Speed settings ------------------------------
  cp5.getTab("airSpeed")
     .activateEvent(true)
     .setHeight(20)
     .setColorForeground(color(200))
     .setColorBackground(color(70))
     .setColorActive(blueAct)
     .setLabel("Air Speed")
     .setId(2)
     .hide()
     ;
  cp5.getTab("airSpeed").captionLabel().toUpperCase(false) ;

  // PPM Air Speed reset
  cp5.addNumberbox("aSpeedReset")
     .setPosition(206, 154)
     .setSize(40, 20)
     .setColorActive(blueAct)
     .setRange(0, 100)
     .setMultiplier(0.5)                     // set the sensitifity of the numberbox
     .setDirection(Controller.HORIZONTAL)    // change the control direction to left/right
     .setValue(100)
     .setLabel("PPM Air Speed reset                       ")
     .setColorLabel(color(0))
     .setTab("airSpeed")
     ;
  cp5.getController("aSpeedReset").getCaptionLabel().align(ControlP5.LEFT_OUTSIDE, ControlP5.CENTER).setPaddingX(5) ;
  cp5.getController("aSpeedReset").captionLabel().toUpperCase(false) ;

  // PPM compensation at PPM range
  cp5.addTextlabel("ppmRngCompL")
     .setText("PPM range for compensation   Min.")
     .setPosition(10, 197)
     .setColorValue(#000000)
     .setTab("airSpeed")
     ;
  cp5.getProperties().remove(cp5.getController("ppmRngCompL")) ;

  cp5.addRange("ppmRngCompMinMax")
     .setPosition(206, 195)
     .setSize(200, 20)
     .setCaptionLabel("Max.")
     .setHandleSize(15)
     .setRange(0, 100)
     .setRangeValues(60, 90)
     ;
  customizeRange(cp5.getController("ppmRngCompMinMax")) ;
  cp5.getTooltip().register("ppmRngCompMinMax", "- Default: 10:40 -") ;
  cp5.getController("ppmRngCompMinMax").setTab("airSpeed") ;

  // PPM compensation
  cp5.addTextlabel("ppmCompL")
     .setText("PPM compensation                  Min.")
     .setPosition(10, 225)
     .setColorValue(#000000)
     .setTab("airSpeed")
     ;
  cp5.getProperties().remove(cp5.getController("ppmCompL")) ;

  cp5.addRange("ppmCompMinMax")
     .setPosition(206, 223)
     .setSize(200, 20)
     .setCaptionLabel("Max.")
     .setHandleSize(15)
     .setRange(0, 150)
     .setRangeValues(80.5, 120)
     ;
  customizeRange(cp5.getController("ppmCompMinMax")) ;
  cp5.getTooltip().register("ppmCompMinMax", "- Default: 10:40 -") ;
  cp5.getController("ppmCompMinMax").setTab("airSpeed") ;


  // ------------------------------ Tab 3 : Voltage settings ------------------------------
  cp5.getTab("voltage")
     .setHeight(20)
     .setColorForeground(color(200))
     .setColorBackground(color(70))
     .setColorActive(blueAct)
     .setLabel("Voltage...")
     .setId(3)
     .hide()
     ;
  cp5.getTab("voltage").captionLabel().toUpperCase(false) ;

  // Voltage 1-6 toggle
  cp5.addTextlabel("voltages")
     .setText("Voltage number")
     .setPosition(10, 145)
     .setColorValue(#000000)
     .setTab("voltage")
     ;
  cp5.getProperties().remove(cp5.getController("voltages")) ;

  for ( int i = 1; i <= voltNbr; i++ ) {
    cp5.addToggle("volt" + i)
       .setLabel("" + i)
       .setPosition(128 + 55 * (i-1), 147)
       .setTab("voltage")
       ;
    customizeToggleVolt(cp5.getController("volt" + i)) ;
  }

  // Voltage 1-6 pin
  cp5.addTextlabel("voltPin")
     .setText("Pin number")
     .setPosition(10, 173)
     .setColorValue(#000000)
     .setTab("voltage")
     ;
  cp5.getProperties().remove(cp5.getController("voltPin")) ;

  for ( int i = 1; i <= voltNbr; i++ ) {
    cp5.addDropdownList("ddlVolt" + i)
       .setPosition(113 + 55 * (i-1), 192)
       .setTab("voltage")
       ;
    customizeDdlVpin(cp5.get(DropdownList.class, "ddlVolt" + i)) ;
    cp5.get(DropdownList.class, "ddlVolt" + i).setValue(-1) ;
    cp5.getProperties().remove(cp5.getGroup("ddlVolt" + i), "ListBoxItems") ;
  }

  // Voltage 1-6 divider factor
  cp5.addTextlabel("voltDivider")
     .setText("Divider factor                                                                                           ")
     .setPosition(10, 204)
     .setColorValue(#000000)
     .setTab("voltage")
     ;
  cp5.getProperties().remove(cp5.getController("voltDivider")) ;
  cp5.getTooltip().register("voltDivider", "- Default: 1 -") ;

  for ( int i = 1; i <= voltNbr; i++ ) {
    cp5.addNumberbox("dividerVolt" +i)
       .setPosition(113 + 55 * (i-1), 202)
       .setSize(45, 20)
       .setColorActive(blueAct)
       .setDecimalPrecision(2)
       .setRange(0.01, 99.99)
       .setMultiplier(0.01)                    // set the sensitifity of the numberbox
       .setDirection(Controller.HORIZONTAL)    // change the control direction to left/right
       .setValue(1)
       .setCaptionLabel("")
       .setTab("voltage")
       ;
  }

  // Voltage 1-6 Offset
  cp5.addTextlabel("voltOffset")
     .setText("Offset (mV)                                                                                              ")
     .setPosition(10, 233)
     .setColorValue(#000000)
     .setTab("voltage")
     ;
  cp5.getProperties().remove(cp5.getController("voltOffset")) ;
  cp5.getTooltip().register("voltOffset", "- Default: 0 -") ;

  for ( int i = 1; i <= voltNbr; i++ ) {
    cp5.addNumberbox("offsetVolt" +i)
       .setPosition(113 + 55 * (i-1), 231)
       .setSize(45, 20)
       .setColorActive(blueAct)
       .setDecimalPrecision(0)
       .setRange(-5000, 5000)
       .setMultiplier(1)                       // set the sensitifity of the numberbox
       .setDirection(Controller.HORIZONTAL)    // change the control direction to left/right
       .setValue(0)
       .setCaptionLabel("")
       .setTab("voltage")
       ;
  }

  // Cells monitoring -> Number of Cells
  cp5.addToggle("cells")
     .setPosition(148, 296)
     .setLabel("Battery cells monitoring")
     .setColorForeground(orangeAct)
     .setColorBackground(color(70))
     .setColorActive(blueAct)
     .setColorLabel(#000000)
     .setSize(15, 15)
     .setTab("voltage")
     ;
  cp5.getController("cells").getCaptionLabel().align(ControlP5.LEFT_OUTSIDE, ControlP5.CENTER).setPaddingX(10) ;
  cp5.getController("cells").captionLabel().toUpperCase(false) ;

  cp5.addTextlabel("NbrCells")
     .setText("Number of Cells")
     .setPosition(190, 295)
     .setColorValue(#000000)
     .setTab("voltage")
     ;
  cp5.getProperties().remove(cp5.getController("NbrCells")) ;

  ddlNbrCells = cp5.addDropdownList("ddlNbrCells")
                   .setColorForeground(orangeAct)
                   .setColorBackground(color(70))
                   .setColorActive(blueAct)
                   .setPosition(288, 314)
                   .setSize(25, 140)
                   .setItemHeight(20)
                   .setBarHeight(20)
                   .setTab("voltage")
                   ;
  for ( int i = 1; i <= 6; i++ ) {
    ddlNbrCells.addItem("" + i, i) ;
  }
  ddlNbrCells.setValue(1) ;
  ddlNbrCells.captionLabel().style().marginTop = 2 ;
  ddlNbrCells.toUpperCase(false) ;
  cp5.getProperties().remove(cp5.getGroup("ddlNbrCells"), "ListBoxItems") ;


  // ------------------------------ Tab 4 : Current settings ------------------------------
  cp5.getTab("current")
     .setHeight(20)
     .setColorForeground(color(200))
     .setColorBackground(color(70))
     .setColorActive(blueAct)
     .setLabel("Current")
     .setId(4)
     .hide()
     ;
  cp5.getTab("current").captionLabel().toUpperCase(false) ;

  // Current pin
  cp5.addTextlabel("currentPinL")
     .setText("Current sensor Pin number")
     .setPosition(9, 127)
     .setColorValue(#000000)
     .setTab("current")
     ;
  cp5.getProperties().remove(cp5.getController("currentPinL")) ;

  cp5.addDropdownList("currentPin")
     .setColorForeground(orangeAct)
     .setColorBackground(color(70))
     .setColorActive(blueAct)
     .setPosition(165, 146)
     .setSize(45, 200)
     .setItemHeight(20)
     .setBarHeight(20)
     .setTab("current")
     ;
  cp5.get(DropdownList.class, "currentPin").addItem(" --", -1) ;
  cp5.get(DropdownList.class, "currentPin").addItems(analogPins) ;
  cp5.getGroup("currentPin").getCaptionLabel().setPaddingX(12) ;
  cp5.getGroup("currentPin").captionLabel().style().marginTop = 2 ;
  cp5.get(DropdownList.class, "currentPin").setValue(-1) ;
  cp5.get(DropdownList.class, "currentPin").toUpperCase(false) ;
  cp5.getProperties().remove(cp5.getGroup("currentPin"), "ListBoxItems") ;

  // Current sensor output sensitivity
  cp5.addNumberbox("currentOutSensNb")
     .setColorActive(blueAct)
     .setPosition(165, 160)
     .setSize(45, 20)
     .setRange(0, 999)
     .setMultiplier(1)                       // set the sensitifity of the numberbox
     .setDecimalPrecision(0)
     .setDirection(Controller.HORIZONTAL)    // change the control direction to left/right
     .setLabel("Output sensitivity (mV/A)    ")
     .setColorLabel(color(0))
     .setTab("current")
     ;
  cp5.getController("currentOutSensNb").getCaptionLabel().align(ControlP5.LEFT_OUTSIDE, ControlP5.CENTER).setPaddingX(7) ;
  cp5.getController("currentOutSensNb").captionLabel().toUpperCase(false) ;

  // Current sensor offset
  cp5.addNumberbox("currentOutOffsetNb")
     .setBroadcast(false)
     .setColorActive(blueAct)
     .setPosition(165, 195)
     .setSize(45, 20)
     .setRange(-5000, 5000)
     .setMultiplier(1)                       // set the sensitifity of the numberbox
     .setDecimalPrecision(0)
     .setDirection(Controller.HORIZONTAL)    // change the control direction to left/right
     .setLabel("Output offset                  mV")
     .setColorLabel(color(0))
     .setTab("current")
     ;
  cp5.getController("currentOutOffsetNb").getCaptionLabel().align(ControlP5.LEFT_OUTSIDE, ControlP5.CENTER).setPaddingX(5) ;
  cp5.getController("currentOutOffsetNb").captionLabel().toUpperCase(false) ;
  //cp5.getTooltip().register("currentOutOffsetNb", "...") ;

  cp5.addNumberbox("currentOutOffsetMA")
     .setBroadcast(false)
     .setColorActive(blueAct)
     .setPosition(220, 195)
     .setSize(50, 20)
     .setRange(-99999, 99999)
     .setMultiplier(1)                       // set the sensitifity of the numberbox
     .setDecimalPrecision(0)
     .setDirection(Controller.HORIZONTAL)    // change the control direction to left/right
     .setLabel("mA")
     .setColorLabel(color(0))
     .setTab("current")
     ;
  cp5.getController("currentOutOffsetMA").getCaptionLabel().align(ControlP5.RIGHT_OUTSIDE, ControlP5.CENTER).setPaddingX(5) ;
  cp5.getController("currentOutOffsetMA").captionLabel().toUpperCase(false) ;
  cp5.getProperties().remove(cp5.getController("currentOutOffsetMA")) ;
  //cp5.getTooltip().register("currentOutOffsetMA", "...") ;

  cp5.getController("currentOutOffsetNb").setBroadcast(true) ;
  cp5.getController("currentOutOffsetMA").setBroadcast(true) ;

  // Current sensor divider factor
  cp5.addNumberbox("currentDivNb")
     .setColorActive(blueAct)
     .setPosition(165, 230)
     .setSize(45, 20)
     .setRange(0.01, 99.99)
     .setMultiplier(0.01)                    // set the sensitifity of the numberbox
     .setDecimalPrecision(2)
     .setDirection(Controller.HORIZONTAL)    // change the control direction to left/right
     .setValue(1)
     .setLabel("Divider factor                      ")
     .setColorLabel(color(0))
     .setTab("current")
     ;
  cp5.getController("currentDivNb").getCaptionLabel().align(ControlP5.LEFT_OUTSIDE, ControlP5.CENTER).setPaddingX(9) ;
  cp5.getController("currentDivNb").captionLabel().toUpperCase(false) ;
  cp5.getTooltip().register("currentDivNb", "- Default: 1 -") ;

  /*
  // Current sensor Vcc
  cp5.addNumberbox("currentVccNb")
     .setColorActive(blueAct)
     .setPosition(330, 118)
     .setSize(30, 18)
     .setRange(0, 9.9)
     .setMultiplier(0.05)                    // set the sensitifity of the numberbox
     .setDecimalPrecision(1)
     .setDirection(Controller.HORIZONTAL)    // change the control direction to left/right
     //.setValue(5)
     .setLabel("Current sensor Vcc             volts")
     .setColorLabel(color(0))
     .setTab("current")
     ;
  cp5.getController("currentVccNb").getCaptionLabel().align(ControlP5.LEFT_OUTSIDE, ControlP5.CENTER).setPaddingX(-59) ;
  cp5.getController("currentVccNb").captionLabel().toUpperCase(false) ;
  cp5.getTooltip().register("currentVccNb", "Current sensor alimentation source value") ;
  */
  /*
  // Current sensor direction
  cp5.addTextlabel("currentDirL")
     .setText("Unidirectional")
     .setPosition(10, 157)
     .setColorValue(#000000)
     .setTab("current")
     ;
  cp5.getProperties().remove(cp5.getController("currentDirL")) ;

  cp5.addToggle("currentDir")
     .setColorForeground(orangeAct)
     .setColorBackground(color(70))
     .setColorActive(blueAct)
     .setColorLabel(#000000)
     .setPosition(98, 155)
     .setSize(50, 20)
     .setLabel("Bidirectional")
     .setMode(ControlP5.SWITCH)
     .setTab("current")
     ;
  cp5.getController("currentDir").getCaptionLabel().align(ControlP5.RIGHT_OUTSIDE, ControlP5.CENTER).setPaddingX(10) ;
  cp5.getController("currentDir").captionLabel().toUpperCase(false) ;
  */

  // ------------------------------ Tab 5 : Temperature settings ------------------------------
  cp5.getTab("temperature")
     .setHeight(20)
     .setColorForeground(color(200))
     .setColorBackground(color(70))
     .setColorActive(blueAct)
     .setLabel("Temperature")
     .setId(5)
     .hide()
     ;
  cp5.getTab("temperature").captionLabel().toUpperCase(false) ;

  // Use vario temp
  cp5.addToggle("vTemp")
     .setPosition(142, 120)
     .setLabel("Use vario temperature")
     .setColorForeground(orangeAct)
     .setColorBackground(color(70))
     .setColorActive(blueAct)
     .setColorLabel(#000000)
     .setSize(15, 15)
     .setTab("temperature")
     ;
  cp5.getController("vTemp").getCaptionLabel().align(ControlP5.LEFT_OUTSIDE, ControlP5.CENTER).setPaddingX(10) ;
  cp5.getController("vTemp").captionLabel().toUpperCase(false) ;

  // Temperature sensor pin
  cp5.addTextlabel("tempPinL")
     .setText("Temperature sensor pin number")
     .setPosition(10, 157)
     .setColorValue(#000000)
     .setTab("temperature")
     ;
  cp5.getProperties().remove(cp5.getController("tempPinL")) ;

  cp5.addDropdownList("tempPin")
     .setColorForeground(orangeAct)
     .setColorBackground(color(70))
     .setColorActive(blueAct)
     .setPosition(193, 176)
     .setSize(30, 200)
     .setItemHeight(20)
     .setBarHeight(20)
     .setTab("temperature")
     ;
  cp5.get(DropdownList.class, "tempPin").addItem("--", -1) ;
  cp5.get(DropdownList.class, "tempPin").addItems(analogPins) ;
  cp5.get(DropdownList.class, "tempPin").setValue(-1) ;
  cp5.getGroup("tempPin").captionLabel().style().marginTop = 2 ;
  cp5.get(DropdownList.class, "tempPin").toUpperCase(false) ;
  cp5.getProperties().remove(cp5.getGroup("tempPin"), "ListBoxItems") ;

  // Temperature sensor offset
  cp5.addTextfield("tempOffset")
     .setLabel("Temperature sensor offset")
     .setPosition(164, 192)
     .setColorLabel(color(0))
     .setColorValueLabel(color(255))
     .setColorActive(blueAct)
     .setSize(28, 20)
     .setAutoClear(false)
     .setText("0")
     .setTab("temperature")
     ;
  cp5.getController("tempOffset").getCaptionLabel().align(ControlP5.LEFT_OUTSIDE, ControlP5.CENTER).setPaddingX(10) ;
  cp5.getController("tempOffset").captionLabel().toUpperCase(false) ;


  // ------------------------------ Tab 6 : RPM settings ------------------------------   needed ?
  cp5.getTab("rpm")
     .setHeight(20)
     .setColorForeground(color(200))
     .setColorBackground(color(70))
     .setColorActive(blueAct)
     .setLabel("RPM")
     .setId(6)
     .hide()
     ;
  cp5.getTab("rpm").captionLabel().toUpperCase(false) ;

  // ------------------------------ Tab 7 : DATA to send ------------------------------
  cp5.getTab("data")
     .setHeight(20)
     .setColorForeground(color(200))
     .setColorBackground(color(40, 90, 40))
     .setColorActive(color(60, 190, 60))
     .setLabel("DATA sent")
     .setId(7)
     .hide()
     ;
  cp5.getTab("data").captionLabel().toUpperCase(false) ;

  // Text Labels
  cp5.addTextlabel("sentData")
     .setText("OXS measurement")
     .setPosition(22, 104)
     .setColorValue(#000000)
     .setTab("data")
     ;
  cp5.getProperties().remove(cp5.getController("sentData")) ;

  cp5.addTextlabel("DataFS")
     .setText("Telemetry data field")
     .setPosition(162, 104)
     .setColorValue(#000000)
     .setTab("data")
     ;
  cp5.getProperties().remove(cp5.getController("DataFS")) ;

  cp5.addTextlabel("multiplierL")
     .setText("Multiplier")
     .setPosition(291, 104)
     .setColorValue(#000000)
     .setTab("data")
     ;
  cp5.getProperties().remove(cp5.getController("multiplierL")) ;

  cp5.addTextlabel("dividerL")
     .setText("Divider")
     .setPosition(347, 104)
     .setColorValue(#000000)
     .setTab("data")
     ;
  cp5.getProperties().remove(cp5.getController("dividerL")) ;

  cp5.addTextlabel("offsetL")
     .setText("Offset")
     .setPosition(400, 104)
     .setColorValue(#000000)
     .setTab("data")
     ;
  cp5.getProperties().remove(cp5.getController("offsetL")) ;

  for ( int i = 1; i <= dataSentFieldNbr; i++ ) {
    // Transmitted DATA field
    cp5.addDropdownList("sentDataField" + i)
       .setColorForeground(orangeAct)
       .setColorBackground(color(70))
       .setColorActive(blueAct)
       .setPosition(10, 146 - 25 + i * 25)
       .setSize(135, 336 - 25 * i)
       .setItemHeight(20)
       .setBarHeight(20)
       .setTab("data")
       ;
    for (int j = 0; j < sentDataList.length; j++ ) {
      cp5.get(DropdownList.class, "sentDataField" + i).addItem("" + sentDataList[j][1], j) ;
    }
    cp5.get(DropdownList.class, "sentDataField" + i).setValue(0) ;
    cp5.getGroup("sentDataField" + i).captionLabel().style().marginTop = 2 ;
    cp5.get(DropdownList.class, "sentDataField" + i).toUpperCase(false) ;
    cp5.getProperties().remove(cp5.getGroup("sentDataField" + i), "ListBoxItems") ;


    // HUB DATA field
    cp5.addDropdownList("hubDataField" + i)
       .setColorForeground(orangeAct)
       .setColorBackground(color(70))
       .setColorActive(blueAct)
       .setPosition(150, 146 - 25 + i * 25)
       .setSize(135, 336 - 25 * i)
       .setItemHeight(20)
       .setBarHeight(20)
       .setTab("data")
       ;
    for (int j = 0; j < hubDataList.length; j++ ) {
      cp5.get(DropdownList.class, "hubDataField" + i).addItem("" + hubDataList[j][1], j) ;
    }
    cp5.getGroup("hubDataField" + i).captionLabel().style().marginTop = 2 ;
    cp5.get(DropdownList.class, "hubDataField" + i).toUpperCase(false) ;
    cp5.get(DropdownList.class, "hubDataField" + i).setValue(0) ;
    cp5.getGroup("hubDataField" + i).hide() ;
    cp5.getProperties().remove(cp5.getGroup("hubDataField" + i), "ListBoxItems") ;


    // SMART PORT DATA field
    cp5.addDropdownList("sPortDataField" + i)
       .setColorForeground(orangeAct)
       .setColorBackground(color(70))
       .setColorActive(blueAct)
       .setPosition(150, 146 - 25 + i * 25)
       .setSize(135, 336 - 25 * i)
       .setItemHeight(20)
       .setBarHeight(20)
       .setTab("data")
       ;
    for (int j = 0; j < sPortDataList.length; j++ ) {
      cp5.get(DropdownList.class, "sPortDataField" + i).addItem("" + sPortDataList[j][1], j) ;
    }
    cp5.getGroup("sPortDataField" + i).captionLabel().style().marginTop = 2 ;
    cp5.get(DropdownList.class, "sPortDataField" + i).toUpperCase(false) ;
    cp5.get(DropdownList.class, "sPortDataField" + i).setValue(0) ;
    //cp5.getGroup("sPortDataField" + i).hide() ;
    cp5.getProperties().remove(cp5.getGroup("sPortDataField" + i), "ListBoxItems") ;

    // Data sent multiplier
    cp5.addNumberbox("dataMultiplier" + i)
       .setColorActive(blueAct)
       .setPosition(300, 125 - 25 + i * 25)
       .setSize(40, 20)
       .setRange(-1000, 1000)
       .setMultiplier(0.5)                     // set the sensitifity of the numberbox
       .setDecimalPrecision(0)
       .setDirection(Controller.HORIZONTAL)    // change the control direction to left/right
       .setValue(1)
       .setLabel("")
       .setTab("data")
       ;
    cp5.getTooltip().register("dataMultiplier" + i, "- Default: 1 -") ;

    // Data sent divider
    cp5.addNumberbox("dataDivider" + i)
       .setColorActive(blueAct)
       .setPosition(350, 125 - 25 + i * 25)
       .setSize(40, 20)
       .setRange(1, 1000)
       .setMultiplier(0.5)                     // set the sensitifity of the numberbox
       .setDecimalPrecision(0)
       .setDirection(Controller.HORIZONTAL)    // change the control direction to left/right
       .setValue(1)
       .setLabel("")
       .setTab("data")
       ;
    cp5.getTooltip().register("dataDivider" + i, "- Default: 1 -") ;

    // Data sent offset
    cp5.addNumberbox("dataOffset" + i)
       .setColorActive(blueAct)
       .setPosition(400, 125 - 25 + i * 25)
       .setSize(40, 20)
       .setRange(-999, 999)
       .setMultiplier(0.5)                     // set the sensitifity of the numberbox
       .setDecimalPrecision(0)
       .setDirection(Controller.HORIZONTAL)    // change the control direction to left/right
       .setValue(0)
       .setLabel("")
       .setTab("data")
       ;
    cp5.getTooltip().register("dataOffset" + i, "- Default: 0 -") ;
  }

  prot.setValue(2);  // Set the protocol ddl value after telemetry fields creation

  // ------------------------------ File dialog ------------------------------

  // Load preset button
  cp5.addButton("loadButton")
     .setColorForeground(blueAct)
     .setLabel("Load Preset")
     .setPosition(20, 419)
     .setSize(100, 25)
     .setTab("global")
     ;
  cp5.getController("loadButton").captionLabel().setFont(font16) ;
  cp5.getController("loadButton").captionLabel().toUpperCase(false) ;
  cp5.getController("loadButton").getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER) ;

  // Save preset button
  cp5.addButton("saveButton")
     .setColorForeground(orangeAct)
     .setColorActive(blueAct)
     .setLabel("Save Preset")
     .setPosition(140, 419)
     .setSize(100, 25)
     .setTab("global")
     ;
  cp5.getController("saveButton").captionLabel().setFont(font16) ;
  cp5.getController("saveButton").captionLabel().toUpperCase(false) ;
  cp5.getController("saveButton").getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER) ;


  // Write button
  cp5.addButton("writeConfButton")
     .setColorForeground(orangeAct)
     .setColorActive(blueAct)
     .setLabel("Write Config")
     .setPosition(300, 416)
     .setSize(120, 30)
     .setTab("data")
     ;
  cp5.getController("writeConfButton").captionLabel().setFont(font20) ;
  cp5.getController("writeConfButton").captionLabel().toUpperCase(false) ;
  cp5.getController("writeConfButton").getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER) ;

  // --------------------------------------------------------------------------

  // dropdownlist overlap
//prot.bringToFront() ;
  cp5.getGroup("resetButtonPin").bringToFront() ;
  cp5.getGroup("voltRefChoice").bringToFront() ;
  cp5.getGroup("sensorID").bringToFront() ;
  serialPin.bringToFront() ;

  climbPin.bringToFront() ;
  cp5.getGroup("vSpeed1").bringToFront() ;
  cp5.getGroup("vSpeed2").bringToFront() ;

  ddlNbrCells.bringToFront() ;
  for ( int i = 1; i <= voltNbr; i++ ) {
    cp5.getGroup("ddlVolt" + i).bringToFront() ;
  }
  cp5.getGroup("currentPin").bringToFront() ;
  cp5.getGroup("tempPin").bringToFront() ;
  for ( int i = dataSentFieldNbr; i >= 1; i-- ) {
    cp5.getGroup("sentDataField" + i).bringToFront() ;
    //cp5.getGroup("hubDataField" + i).bringToFront() ;
    cp5.getGroup("sPortDataField" + i).bringToFront() ;
  }

  // Tooltips general settings
  cp5.getTooltip().setDelay(1000) ;
  cp5.getTooltip().getLabel().toUpperCase(false) ;

  createMessageBox() ;          //  Message box creation

// *************************************************************************
} // ***************************** END SETUP *******************************
// *************************************************************************

// *************************************************************************
// ********************************   DRAW   *******************************
// *************************************************************************
void draw() {
  background(myColorBackground) ;
  // Main screen background
  fill(tabColor) ;
  rect(0, 100, width, 300) ;
  fill(70) ;
  rect(0, 97, width, 3) ;

  // Compatibility subtitle
  fill(0) ;
  textFont(fontLabel) ;
  text("For OXS " + oxsVersion, 75, 65) ;

  // OXS Configurator version display
  textFont(fontItalic) ;
  text(oxsCversion, 377, 68) ;

  // Logo display
  shapeMode(CENTER) ;
  shape(oxsL, width/2, 38, 300, 300) ;

  // File dialog Zone
  fill(myColorBackground) ;
  rect(0, height, width, -60) ;
  fill(70) ;
  rect(0, height-60, width, 3) ;

  // Show preset buttons
  cp5.getController("loadButton").show() ;
  cp5.getController("saveButton").show() ;

  // ------------ Tabs specific display ------------

  int ctid = cp5.window(this).currentTab().id() ;

  switch( ctid ) {

  case 0 :                                        // TAB GENERAL Settings
    // separation lines
    stroke(70) ;
    noSmooth() ;
    line(10, 178, 440, 178) ;
    line(10, 236, 440, 236) ;
    noStroke() ;
    smooth() ;

    if ( cp5.get(DropdownList.class, "voltRefChoice").value() == 2 ) {
      cp5.getController("arduinoVccNb").lock() ;
      cp5.getController("arduinoVccNb").setColorBackground(grayedColor) ;
      cp5.getController("arduinoVccNb").setColorValueLabel(grayedColor) ;
      cp5.getController("arduinoVccNb").setColorLabel(grayedColor) ;
    } else {
      cp5.getController("arduinoVccNb").unlock() ;
      cp5.getController("arduinoVccNb").setColorBackground(color(70)) ;
      cp5.getController("arduinoVccNb").setColorValueLabel(color(255)) ;
      cp5.getController("arduinoVccNb").setColorLabel(color(0)) ;
    }

    if ( cp5.getController("saveEprom").value() == 0 ) {
      cp5.getController("resetButtonPinLabel").setColorValue(grayedColor) ;
      cp5.getGroup("resetButtonPin").hide() ;
      fill(grayedColor) ;
      rect(355, 255, 30, 20) ;
    } else {
      cp5.getController("resetButtonPinLabel").setColorValue(color(0)) ;
      cp5.getGroup("resetButtonPin").show() ;
      fill(lightOrange) ;               // rectangle border filled
      rect(10, 255, 152, 20) ;
      noFill() ;
    }
    stroke(70) ;                        // rectangle border
    noFill() ;
    rect(10, 255, 152, 20) ;
    noStroke() ;

    if ( !tempActive ) {
      cp5.getController("temperature").setColorLabel(grayedColor) ;
      cp5.getController("temperature").setColorBackground(grayedColor) ;
    }

    // SENSORS part
    fill(255, 128, 0) ;
    rect(width / 2 - 35, 295, 69, 18) ;
    rect(0, 310, width, 3) ;
    noFill() ;

    if ( cp5.getController("vario").value() == 1 ) {                 // Vario 2 enabled if vario 1
      cp5.getController("vario2").unlock() ;
      cp5.getController("vario2").setColorLabel(0) ;
      cp5.getController("vario2").setColorBackground(color(70)) ;
    } else {
      cp5.getController("vario2").lock() ;
      cp5.getController("vario2").setColorLabel(grayedColor) ;
      cp5.getController("vario2").setColorBackground(grayedColor) ;
    }

    // Sensors table
    if ( cp5.getController("vario").value() == 1 ) {
      fill(219, 191, 181) ;
      rect(45, 331, 120, 25) ;
      noFill() ;
    }
    if ( cp5.getController("vario2").value() == 1 ) {
      fill(219, 191, 181) ;
      rect(165, 331, 120, 25) ;
      noFill() ;
    }
    if ( cp5.getController("airSpeed").value() == 1 ) {
      fill(219, 191, 181) ;
      rect(285, 331, 120, 25) ;
      noFill() ;
    }
    if ( cp5.getController("voltage").value() == 1 ) {
      fill(219, 191, 181) ;
      rect(45, 356, 120, 25) ;
      noFill() ;
    }
    if ( cp5.getController("current").value() == 1 ) {
      fill(219, 191, 181) ;
      rect(165, 356, 120, 25) ;
      noFill() ;
    }
    if ( cp5.getController("rpm").value() == 1 ) {
      fill(219, 191, 181) ;
      rect(285, 356, 120, 25) ;
      noFill() ;
    }

    stroke(70) ;
    rect(45, 331, 120, 25) ;
    rect(165, 331, 120, 25) ;
    rect(285, 331, 120, 25) ;
    rect(45, 356, 120, 25) ;
    rect(165, 356, 120, 25) ;
    rect(285, 356, 120, 25) ;
    noStroke() ;

    break ;

  case 1 :                                                 // TAB Vario

    stroke(blueAct) ;     // blue border
    strokeWeight(3) ;
    noFill() ;
    rect(4, 106, 442, 162) ;
    line(4, 142, 446, 142) ;
    strokeWeight(1) ;
    noStroke() ;

    drawPPMzone() ;

    // separation lines
    stroke(70) ;
    line(10, 205, 440, 205) ;
    line(10, 331, 440, 331) ;
    line(10, 363, 440, 363) ;
    noStroke() ;

    if ( cp5.getController("ppm").value() == 0 ) {
      cp5.getController("ppmRngSensL").setColorValue(grayedColor) ;
      cp5.getController("ppmRngSensMinMax").lock() ;
      cp5.getController("ppmRngSensMinMax").setColorForeground(grayedColor) ;
      cp5.getController("ppmRngSensMinMax").setColorBackground(grayedColor) ;
      cp5.getController("ppmRngSensMinMax").setColorValueLabel(grayedColor) ;
      cp5.getController("ppmRngSensMinMax").setColorLabel(grayedColor) ;

      cp5.getController("ppmSensRngL").setColorValue(grayedColor) ;
      cp5.getController("ppmSensMinMax").lock() ;
      cp5.getController("ppmSensMinMax").setColorForeground(grayedColor) ;
      cp5.getController("ppmSensMinMax").setColorBackground(grayedColor) ;
      cp5.getController("ppmSensMinMax").setColorValueLabel(grayedColor) ;
      cp5.getController("ppmSensMinMax").setColorLabel(grayedColor) ;
    } else {
      cp5.getController("ppmRngSensL").setColorValue(color(0)) ;
      cp5.getController("ppmRngSensMinMax").unlock() ;
      cp5.getController("ppmRngSensMinMax").setColorForeground(blueAct) ;
      cp5.getController("ppmRngSensMinMax").setColorBackground(color(70)) ;
      cp5.getController("ppmRngSensMinMax").setColorValueLabel(color(255)) ;
      cp5.getController("ppmRngSensMinMax").setColorLabel(color(0)) ;

      cp5.getController("ppmSensRngL").setColorValue(color(0)) ;
      cp5.getController("ppmSensMinMax").unlock() ;
      cp5.getController("ppmSensMinMax").setColorForeground(blueAct) ;
      cp5.getController("ppmSensMinMax").setColorBackground(color(70)) ;
      cp5.getController("ppmSensMinMax").setColorValueLabel(color(255)) ;
      cp5.getController("ppmSensMinMax").setColorLabel(color(0)) ;
    }

    if ( cp5.getController("ppm").value() == 0 || ( cp5.getController("vario2").value() == 0 && cp5.getController("airSpeed").value() == 0 ) ) {
      cp5.getController("vStSw2L").setColorValue(grayedColor) ;
      cp5.getController("vStSwitching").setColorValue(grayedColor) ;
      cp5.getGroup("vSpeed2").hide() ;
      fill(grayedColor) ;
      rect(306, 213, 100, 20) ;

      cp5.getController("ppmVspeedSw").setColorValue(grayedColor) ;
      cp5.getController("ppmVspeedSwMin").lock() ;
      cp5.getController("ppmVspeedSwMin").setColorBackground(grayedColor) ;
      cp5.getController("ppmVspeedSwMin").setColorForeground(grayedColor) ;
      cp5.getController("ppmVspeedSwMin").setColorValueLabel(grayedColor) ;
      cp5.getController("ppmVspeedSwMin").setColorCaptionLabel(grayedColor) ;
      cp5.getController("ppmVspeedSwMax").lock() ;
      cp5.getController("ppmVspeedSwMax").setColorBackground(grayedColor) ;
      cp5.getController("ppmVspeedSwMax").setColorForeground(grayedColor) ;
      cp5.getController("ppmVspeedSwMax").setColorValueLabel(grayedColor) ;
      cp5.getController("ppmVspeedSwMax").setColorCaptionLabel(grayedColor) ;
    } else {
      cp5.getController("vStSw2L").setColorValue(color(0)) ;
      cp5.getController("vStSwitching").setColorValue(color(0)) ;
      cp5.getGroup("vSpeed2").show() ;

      cp5.getController("ppmVspeedSw").setColorValue(color(0)) ;
      cp5.getController("ppmVspeedSwMin").unlock() ;
      cp5.getController("ppmVspeedSwMin").setColorBackground(color(70)) ;
      cp5.getController("ppmVspeedSwMin").setColorValueLabel(color(255)) ;
      cp5.getController("ppmVspeedSwMin").setColorCaptionLabel(color(0)) ;
      cp5.getController("ppmVspeedSwMax").unlock() ;
      cp5.getController("ppmVspeedSwMax").setColorBackground(color(70)) ;
      cp5.getController("ppmVspeedSwMax").setColorValueLabel(color(255)) ;
      cp5.getController("ppmVspeedSwMax").setColorCaptionLabel(color(0)) ;
    }

    if ( cp5.getController("analogClimb").value() == 0 ) {                    // Analog climb rate
      cp5.getController("climbPinL").setColorValue(grayedColor) ;
      cp5.getGroup("climbPin").hide() ;
      fill(grayedColor) ;
      rect(165, 372, 30, 20) ;

      cp5.getController("outClimbRateRngL").setColorValue(grayedColor) ;
      cp5.getController("outClimbRateMinMax").lock() ;
      cp5.getController("outClimbRateMinMax").setColorForeground(grayedColor) ;
      cp5.getController("outClimbRateMinMax").setColorBackground(grayedColor) ;
      cp5.getController("outClimbRateMinMax").setColorValueLabel(grayedColor) ;
      cp5.getController("outClimbRateMinMax").setColorLabel(grayedColor) ;
    } else {
      fill(lightBlue) ;                                    // toggle border filled
      rect(10, 372, 124, 20) ;
      cp5.getController("climbPinL").setColorValue(color(0)) ;
      cp5.getGroup("climbPin").show() ;

      cp5.getController("outClimbRateRngL").setColorValue(color(0)) ;
      cp5.getController("outClimbRateMinMax").unlock() ;
      cp5.getController("outClimbRateMinMax").setColorForeground(blueAct) ;
      cp5.getController("outClimbRateMinMax").setColorBackground(color(70)) ;
      cp5.getController("outClimbRateMinMax").setColorValueLabel(color(255)) ;
      cp5.getController("outClimbRateMinMax").setColorLabel(color(0)) ;
    }

    stroke(70) ;                               // toggle border
    noFill() ;
    rect(10, 372, 124, 20) ;
    noStroke() ;

    if ( cp5.getGroup("climbPin").isOpen() ) {    // climb pin dropdown free
      cp5.getController("saveButton").hide() ;
    } else {
       cp5.getController("saveButton").show() ;
    }

    break ;

  case 2 :                                                            // TAB Air Speed sensor

    stroke(blueAct) ;     // blue border
    strokeWeight(3) ;
    noFill() ;
    rect(4, 106, 442, 148) ;
    line(4, 142, 446, 142) ;
    strokeWeight(1) ;
    noStroke() ;

    drawPPMzone() ;

    // separation lines
    stroke(70) ;
    line(10, 184, 440, 184) ;
    noStroke() ;

    if ( cp5.getController("ppm").value() == 0 ) {
      cp5.getController("aSpeedReset").lock() ;
      cp5.getController("aSpeedReset").setColorBackground(grayedColor) ;
      cp5.getController("aSpeedReset").setColorForeground(grayedColor) ;
      cp5.getController("aSpeedReset").setColorValueLabel(grayedColor) ;
      cp5.getController("aSpeedReset").setColorCaptionLabel(grayedColor) ;

      cp5.getController("ppmRngCompL").setColorValue(grayedColor) ;
      cp5.getController("ppmRngCompMinMax").lock() ;
      cp5.getController("ppmRngCompMinMax").setColorForeground(grayedColor) ;
      cp5.getController("ppmRngCompMinMax").setColorBackground(grayedColor) ;
      cp5.getController("ppmRngCompMinMax").setColorValueLabel(grayedColor) ;
      cp5.getController("ppmRngCompMinMax").setColorLabel(grayedColor) ;

      cp5.getController("ppmCompL").setColorValue(grayedColor) ;
      cp5.getController("ppmCompMinMax").lock() ;
      cp5.getController("ppmCompMinMax").setColorForeground(grayedColor) ;
      cp5.getController("ppmCompMinMax").setColorBackground(grayedColor) ;
      cp5.getController("ppmCompMinMax").setColorValueLabel(grayedColor) ;
      cp5.getController("ppmCompMinMax").setColorLabel(grayedColor) ;

    } else {
      cp5.getController("aSpeedReset").unlock() ;
      cp5.getController("aSpeedReset").setColorBackground(color(70)) ;
      cp5.getController("aSpeedReset").setColorValueLabel(color(255)) ;
      cp5.getController("aSpeedReset").setColorCaptionLabel(color(0)) ;

      cp5.getController("ppmRngCompL").setColorValue(color(0)) ;
      cp5.getController("ppmRngCompMinMax").unlock() ;
      cp5.getController("ppmRngCompMinMax").setColorForeground(blueAct) ;
      cp5.getController("ppmRngCompMinMax").setColorBackground(color(70)) ;
      cp5.getController("ppmRngCompMinMax").setColorValueLabel(color(255)) ;
      cp5.getController("ppmRngCompMinMax").setColorLabel(color(0)) ;

      cp5.getController("ppmCompL").setColorValue(color(0)) ;
      cp5.getController("ppmCompMinMax").unlock() ;
      cp5.getController("ppmCompMinMax").setColorForeground(blueAct) ;
      cp5.getController("ppmCompMinMax").setColorBackground(color(70)) ;
      cp5.getController("ppmCompMinMax").setColorValueLabel(color(255)) ;
      cp5.getController("ppmCompMinMax").setColorLabel(color(0)) ;
    }

    break ;

  case 3 :                                                 // TAB Voltage / Other

    // separation lines
    stroke(70) ;
    line(10, 272, 440, 272) ;
    noStroke() ;

    for ( int i = 1; i <= voltNbr; i++ ) {                   // Voltage tab grayed items
      if ( cp5.getController("volt" + i).value() == 0 ) {
        cp5.getGroup("ddlVolt" + i).hide() ;
        fill(grayedColor) ;
        rect(113 + 55 * (i-1), 171, 45, 20) ;
        cp5.getController("dividerVolt" + i).lock() ;
        cp5.getController("dividerVolt" + i).setColorBackground(color(165)) ;
        cp5.getController("dividerVolt" + i).setColorForeground(color(195)) ;
        cp5.getController("dividerVolt" + i).setColorValueLabel(color(165)) ;
        cp5.getController("offsetVolt" + i).lock() ;
        cp5.getController("offsetVolt" + i).setColorBackground(color(175)) ;
        cp5.getController("offsetVolt" + i).setColorForeground(color(195)) ;
        cp5.getController("offsetVolt" + i).setColorValueLabel(color(175)) ;
      } else {
        cp5.getGroup("ddlVolt" + i).show() ;
        cp5.getController("dividerVolt" + i).unlock() ;
        cp5.getController("dividerVolt" + i).setColorBackground(color(70)) ;
        cp5.getController("dividerVolt" + i).setColorValueLabel(color(255)) ;
        cp5.getController("offsetVolt" + i).unlock() ;
        cp5.getController("offsetVolt" + i).setColorBackground(color(70)) ;
        cp5.getController("offsetVolt" + i).setColorValueLabel(color(255)) ;
      }
    }

    if ( cp5.getController("volt1").value() == 0 ) {      // Battery cells monitoring grayed
      stroke(grayedColor) ;                    // toggle border gray
      noFill() ;
      rect(10, 293, 155, 20) ;
      noStroke() ;
      cp5.getController("cells").setBroadcast(false) ;    // deactivate continuous controller event
      cp5.getController("cells").setValue(0) ;
      cp5.getController("cells").lock() ;
      cp5.getController("cells").setColorBackground(grayedColor) ;
      cp5.getController("cells").setColorLabel(grayedColor) ;
    } else {
      stroke(70) ;                             // toggle border
      noFill() ;
      rect(10, 293, 155, 20) ;
      noStroke() ;
      cp5.getController("cells").setBroadcast(true) ;
      cp5.getController("cells").unlock() ;
      cp5.getController("cells").setColorBackground(color(70)) ;
      cp5.getController("cells").setColorLabel(color(0)) ;
    }

    if ( cp5.getController("cells").value() == 0 ) {                   // Cells number grayed
      cp5.getController("NbrCells").setColorValue(grayedColor) ;
      cp5.getGroup("ddlNbrCells").hide() ;
      fill(grayedColor) ;
      rect(288, 293, 25, 20) ;
    } else {
      stroke(70) ;                              // toggle border filled
      fill(lightBlue) ;
      rect(10, 293, 155, 20) ;
      noStroke() ;
      cp5.getController("NbrCells").setColorValue(color(0)) ;
      cp5.getGroup("ddlNbrCells").show() ;
    }

    // Voltage tab ->  Cells indicator
    if ( cp5.getController("cells").value() == 1 && ddlNbrCells.getValue() > 0 ) {
      int nCells = int(ddlNbrCells.getValue()) ;
      noSmooth() ;
      stroke(blueAct) ;
      strokeWeight(3) ;
      strokeCap(PROJECT) ;
      fill(blueAct) ;
      textFont(fontCells) ;
      text("BATTERY CELLS", 13, 122) ;
      line ( 115, 122, 115, 137 ) ;
      line ( 115, 122, 100 + 55 * nCells, 122 ) ;
      line ( 100 + 55 * nCells, 122, 100 + 55 * nCells, 137 ) ;
      strokeWeight(1) ;
      noStroke() ;
      smooth() ;
    }
    break ;

  case 4 :                                                            // TAB Current sensor
    /*
    if ( cp5.getController("currentDir").value() == 0 ) {             // Current grayed switch
      cp5.getController("currentDirL").setColorValue(grayedColor) ;
      cp5.getController("currentDir").setColorCaptionLabel(color(0)) ;
    } else {
      cp5.getController("currentDirL").setColorValue(color(0)) ;
      cp5.getController("currentDir").setColorCaptionLabel(grayedColor) ;
    }
    */
    break ;

  case 7 :                                                            // TAB DATA sent  dataSentDdlOpen
    fill(10) ;
    rect(298, 414, 124, 34) ;

    for ( int i = 1 ; i <= dataSentFieldNbr ; i++ ) {            // Load and Save preset buttons hide
      if ( cp5.getGroup("sentDataField" + i ).isOpen() || cp5.getGroup("sPortDataField" + i ).isOpen() ) {
        cp5.getController("loadButton").hide() ;
        cp5.getController("saveButton").hide() ;
        break ;
      } else {
         cp5.getController("loadButton").show() ;
         cp5.getController("saveButton").show() ;
      }
    }
/*
    // Grayed multiplier + divider + offset if Telemetry data field == DEFAULT
    for ( int i = 1 ; i <= dataSentFieldNbr ; i++ ) {
      if ( cp5.getGroup("protocolChoice").value() == 1 ) {
        if ( cp5.getGroup("hubDataField" + i).value() == 1 ) {
          cp5.getController("dataMultiplier" + i).lock() ;
          cp5.getController("dataMultiplier" + i).setColorBackground(grayedColor) ;
          cp5.getController("dataMultiplier" + i).setColorValueLabel(grayedColor) ;
          cp5.getController("dataDivider" + i).lock() ;
          cp5.getController("dataDivider" + i).setColorBackground(grayedColor) ;
          cp5.getController("dataDivider" + i).setColorValueLabel(grayedColor) ;
          cp5.getController("dataOffset" + i).lock() ;
          cp5.getController("dataOffset" + i).setColorBackground(grayedColor) ;
          cp5.getController("dataOffset" + i).setColorValueLabel(grayedColor) ;
        } else {
          cp5.getController("dataMultiplier" + i).unlock() ;
          cp5.getController("dataMultiplier" + i).setColorBackground(color(70)) ;
          cp5.getController("dataMultiplier" + i).setColorValueLabel(color(255)) ;
          cp5.getController("dataDivider" + i).unlock() ;
          cp5.getController("dataDivider" + i).setColorBackground(color(70)) ;
          cp5.getController("dataDivider" + i).setColorValueLabel(color(255)) ;
          cp5.getController("dataOffset" + i).unlock() ;
          cp5.getController("dataOffset" + i).setColorBackground(color(70)) ;
          cp5.getController("dataOffset" + i).setColorValueLabel(color(255)) ;
        }
      } else if ( cp5.getGroup("protocolChoice").value() == 2 ) {
        if ( cp5.getGroup("sPortDataField" + i).value() == 1 ) {
          cp5.getController("dataMultiplier" + i).lock() ;
          cp5.getController("dataMultiplier" + i).setColorBackground(grayedColor) ;
          cp5.getController("dataMultiplier" + i).setColorValueLabel(grayedColor) ;
          cp5.getController("dataDivider" + i).lock() ;
          cp5.getController("dataDivider" + i).setColorBackground(grayedColor) ;
          cp5.getController("dataDivider" + i).setColorValueLabel(grayedColor) ;
          cp5.getController("dataOffset" + i).lock() ;
          cp5.getController("dataOffset" + i).setColorBackground(grayedColor) ;
          cp5.getController("dataOffset" + i).setColorValueLabel(grayedColor) ;
        } else {
          cp5.getController("dataMultiplier" + i).unlock() ;
          cp5.getController("dataMultiplier" + i).setColorBackground(color(70)) ;
          cp5.getController("dataMultiplier" + i).setColorValueLabel(color(255)) ;
          cp5.getController("dataDivider" + i).unlock() ;
          cp5.getController("dataDivider" + i).setColorBackground(color(70)) ;
          cp5.getController("dataDivider" + i).setColorValueLabel(color(255)) ;
          cp5.getController("dataOffset" + i).unlock() ;
          cp5.getController("dataOffset" + i).setColorBackground(color(70)) ;
          cp5.getController("dataOffset" + i).setColorValueLabel(color(255)) ;
        }
      }
    }
*/
    break ;
  }

  // ---------------- End TAB specific display ---------------

  // Load and Save preset buttons deco
  if ( cp5.getController("loadButton").isVisible() ) {
    fill(blueAct) ;
    rect(19, 418, 102, 27) ;
  }

  if ( cp5.getController("saveButton").isVisible() ) {
    fill(orangeAct) ;
    rect(139, 418, 102, 27) ;
  }

  // ----------------- Texfield and Numberbox mouse-over -----------------

  if ( cp5.isMouseOver (oxsDir) ) {
    oxsDir.setColorForeground(blueAct) ;
  } else {
    oxsDir.setColorForeground(color(200)) ;
  }

  if ( cp5.isMouseOver ( cp5.getController("arduinoVccNb") ) ) {
    cp5.getController("arduinoVccNb").setColorForeground(blueAct) ;
  } else {
    cp5.getController("arduinoVccNb").setColorForeground(grayedColor) ;
  }

  if ( cp5.isMouseOver ( cp5.getController("ppmRngMin") ) ) {
    cp5.getController("ppmRngMin").setColorForeground(orangeAct) ;
  } else {
    cp5.getController("ppmRngMin").setColorForeground(grayedColor) ;
  }

  if ( cp5.isMouseOver ( cp5.getController("ppmRngMax") ) ) {
    cp5.getController("ppmRngMax").setColorForeground(orangeAct) ;
  } else {
    cp5.getController("ppmRngMax").setColorForeground(grayedColor) ;
  }

  if ( cp5.isMouseOver ( cp5.getController("vSpeedMin") ) ) {
    cp5.getController("vSpeedMin").setColorForeground(orangeAct) ;
  } else {
    cp5.getController("vSpeedMin").setColorForeground(grayedColor) ;
  }

  if ( cp5.isMouseOver ( cp5.getController("vSpeedMax") ) ) {
    cp5.getController("vSpeedMax").setColorForeground(orangeAct) ;
  } else {
    cp5.getController("vSpeedMax").setColorForeground(grayedColor) ;
  }

  if ( cp5.isMouseOver ( cp5.getController("ppmVspeedSwMin") ) ) {
    cp5.getController("ppmVspeedSwMin").setColorForeground(orangeAct) ;
  } else {
    cp5.getController("ppmVspeedSwMin").setColorForeground(grayedColor) ;
  }

  if ( cp5.isMouseOver ( cp5.getController("ppmVspeedSwMax") ) ) {
    cp5.getController("ppmVspeedSwMax").setColorForeground(orangeAct) ;
  } else {
    cp5.getController("ppmVspeedSwMax").setColorForeground(grayedColor) ;
  }

  if ( cp5.isMouseOver ( cp5.getController("aSpeedReset") ) ) {
    cp5.getController("aSpeedReset").setColorForeground(orangeAct) ;
  } else {
    cp5.getController("aSpeedReset").setColorForeground(grayedColor) ;
  }

  for ( int i = 1; i <= voltNbr; i++ ) {
    if ( cp5.isMouseOver ( cp5.getController( "dividerVolt" + i ) ) ) {
      cp5.getController( "dividerVolt" + i ).setColorForeground(orangeAct) ;
    } else {
      cp5.getController( "dividerVolt" + i ).setColorForeground(color(170)) ;
    }
  }

  for ( int i = 1; i <= voltNbr; i++ ) {
    if ( cp5.isMouseOver ( cp5.getController( "offsetVolt" + i ) ) ) {
      cp5.getController( "offsetVolt" + i ).setColorForeground(orangeAct) ;
    } else {
      cp5.getController( "offsetVolt" + i ).setColorForeground(color(170)) ;
    }
  }
/*
  if ( cp5.isMouseOver ( cp5.getController( "currentVccNb" ) ) ) {
    cp5.getController( "currentVccNb" ).setColorForeground(orangeAct) ;
  } else {
    cp5.getController( "currentVccNb" ).setColorForeground(color(170)) ;
  }
*/

  if ( cp5.isMouseOver ( cp5.getController( "currentOutSensNb" ) ) ) {
    cp5.getController( "currentOutSensNb" ).setColorForeground(orangeAct) ;
  } else {
    cp5.getController( "currentOutSensNb" ).setColorForeground(color(170)) ;
  }

  if ( cp5.isMouseOver ( cp5.getController( "currentOutOffsetNb" ) ) ) {
    cp5.getController( "currentOutOffsetNb" ).setColorForeground(orangeAct) ;
  } else {
    cp5.getController( "currentOutOffsetNb" ).setColorForeground(color(170)) ;
  }

  if ( cp5.isMouseOver ( cp5.getController( "currentOutOffsetMA" ) ) ) {
    cp5.getController( "currentOutOffsetMA" ).setColorForeground(orangeAct) ;
  } else {
    cp5.getController( "currentOutOffsetMA" ).setColorForeground(color(170)) ;
  }

  if ( cp5.isMouseOver ( cp5.getController( "currentDivNb" ) ) ) {
    cp5.getController( "currentDivNb" ).setColorForeground(orangeAct) ;
  } else {
    cp5.getController( "currentDivNb" ).setColorForeground(color(170)) ;
  }

  if ( cp5.isMouseOver ( cp5.getController( "tempOffset" ) ) ) {
    cp5.getController( "tempOffset" ).setColorForeground(orangeAct) ;
  } else {
    cp5.getController( "tempOffset" ).setColorForeground(color(200)) ;
  }

  for ( int i = 1; i <= dataSentFieldNbr; i++ ) {
    if ( cp5.isMouseOver ( cp5.getController( "dataMultiplier" + i ) ) ) {
      cp5.getController( "dataMultiplier" + i ).setColorForeground(orangeAct) ;
    } else {
      cp5.getController( "dataMultiplier" + i ).setColorForeground(grayedColor) ;
    }
  }

  for ( int i = 1; i <= dataSentFieldNbr; i++ ) {
    if ( cp5.isMouseOver ( cp5.getController( "dataDivider" + i ) ) ) {
      cp5.getController( "dataDivider" + i ).setColorForeground(orangeAct) ;
    } else {
      cp5.getController( "dataDivider" + i ).setColorForeground(grayedColor) ;
    }
  }

  for ( int i = 1; i <= dataSentFieldNbr; i++ ) {
    if ( cp5.isMouseOver ( cp5.getController( "dataOffset" + i ) ) ) {
      cp5.getController( "dataOffset" + i ).setColorForeground(orangeAct) ;
    } else {
      cp5.getController( "dataOffset" + i ).setColorForeground(grayedColor) ;
    }
  }

  // ----------------- Dropdownlist: mouse pressed elsewhere closes list -----------------

/*
  if ( !cp5.isMouseOver ( prot ) ) {
    if (mousePressed == true) {
      prot.close() ;
    }
  }
*/

  if ( !cp5.isMouseOver ( serialPin ) ) {
    if (mousePressed == true) {
      serialPin.close() ;
    }
  }

  if ( !cp5.isMouseOver ( cp5.getGroup( "sensorID" ) ) ) {
    if (mousePressed == true) {
      cp5.getGroup( "sensorID" ).close() ;
    }
  }

  if ( !cp5.isMouseOver ( cp5.getGroup( "voltRefChoice" ) ) ) {
    if (mousePressed == true) {
      cp5.getGroup( "voltRefChoice" ).close() ;
    }
  }

  if ( !cp5.isMouseOver ( cp5.getGroup( "resetButtonPin" ) ) ) {
    if (mousePressed == true) {
      cp5.getGroup( "resetButtonPin" ).close() ;
    }
  }


  if ( !cp5.isMouseOver ( ppmPin ) ) {
    if (mousePressed == true) {
      ppmPin.close() ;
    }
  }

  if ( !cp5.isMouseOver ( cp5.getGroup( "vSpeed1" ) ) ) {
    if (mousePressed == true) {
      cp5.getGroup( "vSpeed1" ).close() ;
    }
  }

  if ( !cp5.isMouseOver ( cp5.getGroup( "vSpeed2" ) ) ) {
    if (mousePressed == true) {
      cp5.getGroup( "vSpeed2" ).close() ;
    }
  }

  if ( !cp5.isMouseOver ( climbPin ) ) {
    if (mousePressed == true) {
      climbPin.close() ;
    }
  }

  for ( int i = 1; i <= voltNbr; i++ ) {
    if ( !cp5.isMouseOver ( cp5.getGroup( "ddlVolt" + i ) ) ) {
      if (mousePressed == true) {
        cp5.getGroup( "ddlVolt" + i ).close() ;
      }
    }
  }

  if ( !cp5.isMouseOver ( cp5.getGroup( "ddlNbrCells") ) ) {
    if (mousePressed == true) {
      cp5.getGroup( "ddlNbrCells" ).close() ;
    }
  }

  if ( !cp5.isMouseOver ( cp5.getGroup( "currentPin") ) ) {
    if (mousePressed == true) {
      cp5.getGroup( "currentPin" ).close() ;
    }
  }

  if ( !cp5.isMouseOver ( cp5.getGroup( "tempPin") ) ) {
    if (mousePressed == true) {
      cp5.getGroup( "tempPin" ).close() ;
    }
  }

  for ( int i = 1; i <= dataSentFieldNbr; i++ ) {
    if ( !cp5.isMouseOver ( cp5.getGroup( "sentDataField" + i ) ) ) {
      if (mousePressed == true) {
        cp5.getGroup( "sentDataField" + i ).close() ;
      }
    }
  }

/*
  for ( int i = 1; i <= dataSentFieldNbr; i++ ) {
    if ( !cp5.isMouseOver ( cp5.getGroup( "hubDataField" + i ) ) ) {
      if (mousePressed == true) {
        cp5.getGroup( "hubDataField" + i ).close() ;
      }
    }
  }
*/

  for ( int i = 1; i <= dataSentFieldNbr; i++ ) {
    if ( !cp5.isMouseOver ( cp5.getGroup( "sPortDataField" + i ) ) ) {
      if (mousePressed == true) {
        cp5.getGroup( "sPortDataField" + i ).close() ;
      }
    }
  }

  // ----------------- TAB DATA sent display -----------------

  if ( cp5.getController("vario").getValue() == 1 || cp5.getController("airSpeed").getValue() == 1 || cp5.getController("voltage").getValue() == 1 ||
    cp5.getController("current").getValue() == 1 || cp5.getController("temperature").getValue() == 1 ||
  cp5.getController("rpm").getValue() == 1 ) {
    cp5.getTab("data").show() ;
  } else {
    cp5.getTab("data").hide() ;
  }
// *************************************************************************
} // ***************************   end DRAW   ******************************
// *************************************************************************

void drawPPMzone() {
  // PPM zone
  fill(70) ;
  rect(width / 2 - 14, 102, 30, 13) ;

  stroke(blueAct) ;
  strokeWeight(3) ;
  noFill() ;
  //rect(4, 106, 442, 36) ;
  strokeWeight(1) ;
  noStroke() ;

  fill(blueAct) ;
  rect(width / 2 - 15, 101, 30, 13) ;
  fill(255) ;
  textFont(fontLabel) ;
  text("PPM", 212, 112) ;

  if ( cp5.getController("ppm").value() == 0 ) {         // RC remote
    cp5.getGroup("ppmPin").hide() ;
    fill(grayedColor) ;
    rect(118, 115, 30, 20) ;
    cp5.getController("ppmPinL").setColorValue(grayedColor) ;

    cp5.getController("ppmRngL").setColorValue(grayedColor) ;
    cp5.getController("ppmRngMin").lock() ;
    cp5.getController("ppmRngMin").setColorBackground(grayedColor) ;
    cp5.getController("ppmRngMin").setColorValueLabel(grayedColor) ;
    cp5.getController("ppmRngMin").setColorLabel(grayedColor) ;

    cp5.getController("ppmRngMax").lock() ;
    cp5.getController("ppmRngMax").setColorBackground(grayedColor) ;
    cp5.getController("ppmRngMax").setColorValueLabel(grayedColor) ;
    cp5.getController("ppmRngMax").setColorLabel(grayedColor) ;
  } else {
    fill(lightBlue) ;                                    // toggle border filled
    rect(12, 114, 58, 20) ;
    cp5.getGroup("ppmPin").show() ;
    cp5.getController("ppmPinL").setColorValue(color(0)) ;

    cp5.getController("ppmRngL").setColorValue(color(0)) ;
    cp5.getController("ppmRngMin").unlock() ;
    cp5.getController("ppmRngMin").setColorBackground(color(70)) ;
    cp5.getController("ppmRngMin").setColorValueLabel(color(255)) ;
    cp5.getController("ppmRngMin").setColorLabel(color(0)) ;

    cp5.getController("ppmRngMax").unlock() ;
    cp5.getController("ppmRngMax").setColorBackground(color(70)) ;
    cp5.getController("ppmRngMax").setColorValueLabel(color(255)) ;
    cp5.getController("ppmRngMax").setColorLabel(color(0)) ;
  }

  stroke(70) ;                               // toggle border
  noFill() ;
  rect(12, 114, 58, 20) ;
  noStroke() ;

}

void controlEvent(ControlEvent theEvent) {
  // DropdownList is of type ControlGroup.
  // A controlEvent will be triggered from inside the ControlGroup class.
  // therefore you need to check the originator of the Event with if (theEvent.isGroup()) to avoid an error message thrown by controlP5.

  if ( theEvent.isFrom(cp5.getTab("vario")) ) {           //  Tab vario : display PPM parameters
    cp5.getController("ppm").setTab("vario") ;
    cp5.getController("ppm").show() ;
    cp5.getController("ppmPinL").setTab("vario") ;
    cp5.getController("ppmPinL").show() ;
    cp5.getController("ppmRngL").setTab("vario") ;
    cp5.getController("ppmRngL").show() ;
    cp5.getController("ppmRngMin").setTab("vario") ;
    cp5.getController("ppmRngMin").show() ;
    cp5.getController("ppmRngMax").setTab("vario") ;
    cp5.getController("ppmRngMax").show() ;
    cp5.getGroup("ppmPin").setTab("vario") ;
  } else if ( theEvent.isFrom(cp5.getTab("airSpeed")) ) {  //  Tab Air Speed : display PPM parameters
    cp5.getController("ppm").setTab("airSpeed") ;
    cp5.getController("ppm").show() ;
    cp5.getController("ppmPinL").setTab("airSpeed") ;
    cp5.getController("ppmPinL").show() ;
    cp5.getController("ppmRngL").setTab("airSpeed") ;
    cp5.getController("ppmRngL").show() ;
    cp5.getController("ppmRngMin").setTab("airSpeed") ;
    cp5.getController("ppmRngMin").show() ;
    cp5.getController("ppmRngMax").setTab("airSpeed") ;
    cp5.getController("ppmRngMax").show() ;
    cp5.getGroup("ppmPin").setTab("airSpeed") ;
  }

  if ( theEvent.isFrom(cp5.getController("vSpeedMax")) || theEvent.isFrom(cp5.getController("vSpeedMin")) ) {   //  V speed sensitivity range interaction
    cp5.getController("vSpeedMax").setBroadcast(false) ;
    cp5.getController("vSpeedMin").setBroadcast(false) ;
    cp5.get(Numberbox.class, "vSpeedMax").setRange( cp5.getController("vSpeedMin").getValue(), 1000 ) ;
    cp5.get(Numberbox.class, "vSpeedMin").setRange( 0, cp5.getController("vSpeedMax").getValue() ) ;
    cp5.getController("vSpeedMin").setBroadcast(true) ;
    cp5.getController("vSpeedMax").setBroadcast(true) ;
  }

  //  Current sensor output offset interaction
  if ( theEvent.isFrom(cp5.getController("currentOutOffsetNb")) /*|| theEvent.isFrom(cp5.getController("currentOutSensNb"))*/ ) {
    cp5.getController("currentOutOffsetMA").setBroadcast(false) ;
    float currentOutSens = cp5.getController("currentOutSensNb").getValue() ;
    float currentOutOffsetMV = cp5.getController("currentOutOffsetNb").getValue() ;
    cp5.getController("currentOutOffsetMA").setValue( ( currentOutOffsetMV / currentOutSens ) * 1000.0 ) ;
    cp5.getController("currentOutOffsetMA").setBroadcast(true) ;
  }
  if ( theEvent.isFrom(cp5.getController("currentOutOffsetMA")) ) {
    cp5.getController("currentOutOffsetNb").setBroadcast(false) ;
    float currentOutSens = cp5.getController("currentOutSensNb").getValue() ;
    float currentOutOffsetMA = cp5.getController("currentOutOffsetMA").getValue() ;
    cp5.getController("currentOutOffsetNb").setValue( ( currentOutOffsetMA / 1000.0 ) * currentOutSens ) ;
    cp5.getController("currentOutOffsetNb").setBroadcast(true) ;
  }


  if ( theEvent.isFrom(cp5.getGroup("protocolChoice")) ) {           // Showing right Telemetry data list in fields
    switch( int(theEvent.getGroup().getValue()) ) {
      case 1 :
        for ( int i = 1; i <= dataSentFieldNbr; i++ ) {
          cp5.getGroup("hubDataField" + i).show() ;
          cp5.getGroup("sPortDataField" + i).hide() ;
        }
        break ;
      case 2 :
        for ( int i = 1; i <= dataSentFieldNbr; i++ ) {
          cp5.getGroup("sPortDataField" + i).show() ;
          cp5.getGroup("hubDataField" + i).hide() ;
        }
        break ;
    }
  }

  // Selecting DEFAULT automaticaly in Telemetry data fields
  for ( int i = 1; i <= dataSentFieldNbr; i++ ) {
    if ( theEvent.isFrom(cp5.getGroup("sentDataField" + i )) ) {
      switch( int(theEvent.getGroup().getValue()) ) {
        case 1 :     // "ALTIMETER"
        case 2 :     // "VERTICAL_SPEED"
        case 4 :     // "ALTIMETER_2"
        case 5 :     // "VERTICAL_SPEED_2"
        case 8 :     // "AIR_SPEED"
        case 12 :    // "CURRENTMA"
        case 14 :    // "CELLS"
        case 21 :    // "RPM"
          //cp5.getGroup("hubDataField" + i).setValue(1) ;
          cp5.getGroup("sPortDataField" + i).setValue(1) ;
          break ;
      }
    }
  }
/*
  if (theEvent.isGroup()) {
    // check if the Event was triggered from a ControlGroup
    println("event from group : "+theEvent.getGroup().getValue()+" from "+theEvent.getGroup()) ;
  } else if (theEvent.isController()) {
    println("event from controller : "+theEvent.getController().getValue()+" from "+theEvent.getController()) ;
  }

  if (theEvent.isTab()) {
    // check if the Event was triggered from a Tab
    println("event from tab : " + theEvent.getTab().getName() ) ;
  }
*/
}

public void mouseWheelMoved(java.awt.event.MouseWheelEvent e) {  // Mouse wheel support for scroll bars !!
  super.mouseWheelMoved(e) ;
  cp5.setMouseWheelRotation(e.getWheelRotation()) ;
}

public void oxsDirButton(int theValue) {
  //println("oxsDir button: "+theValue) ;
  selectFolder("Select OXS source folder:", "folderSelected") ;
}

void vario(boolean theFlag) {
  if (theFlag==true) {
    cp5.getTab("vario").show() ;
    
  } else {
    cp5.getTab("vario").hide() ;
    cp5.getController("vario2").setValue(0) ;
  }
  //println("a toggle event.") ;
}

void airSpeed(boolean theFlag) {
  if (theFlag==true) {
    cp5.getTab("airSpeed").show() ;
  } else {
    cp5.getTab("airSpeed").hide() ;
  }
}

void voltage(boolean theFlag) {
  if (theFlag==true) {
    cp5.getTab("voltage").show() ;
  } else {
    cp5.getTab("voltage").hide() ;
  }
  //println("a toggle event.") ;
}

void current(boolean theFlag) {
  if (theFlag==true) {
    cp5.getTab("current").show() ;
  } else {
    cp5.getTab("current").hide() ;
  }
  //println("a toggle event.") ;
}

void temperature(boolean theFlag) {
  if (theFlag==true) {
    cp5.getTab("temperature").show() ;
  } else {
    cp5.getTab("temperature").hide() ;
  }
  //println("a toggle event.") ;
}

/* // RPM TAB display
 void rpm(boolean theFlag) {
 if (theFlag==true) {
 cp5.getTab("rpm").show() ;
 } else {
 cp5.getTab("rpm").hide() ;
 }
 println("a toggle event.") ;
 }
 */

void about(boolean theFlag) {

  mbClose() ;

  messageList.clear() ;

  messageList.append( "                            OXS Configurator " + oxsCversion + " for OXS " + oxsVersion ) ;
  messageList.append( "                                                       ---" ) ;
  messageList.append( "                         -- OpenXsensor configuration file GUI --" ) ;
  messageList.append( "\n" ) ;
  messageList.append( "Contributors:" ) ;
  messageList.append( "" ) ;
  messageList.append( "- Rainer Schloßhan" ) ;
  messageList.append( "- Bertrand Songis" ) ;
  messageList.append( "- André Bernet" ) ;
  messageList.append( "- Michael Blandford" ) ;
  messageList.append( "- Michel Strens" ) ;
  messageList.append( "- David Laburthe" ) ;
  messageList.append( "" ) ;
  messageList.append( "" ) ;

  String[] messageListArray = messageList.array() ;

  String joinedMessageList = join(messageListArray, "\n") ;

  cp5.get(Textarea.class, "messageBoxLabel").setText(joinedMessageList) ;

  cp5.getController("buttonOK").setColorForeground(orangeAct) ;
  cp5.getController("buttonOK").setColorBackground(color(100)) ;
  cp5.getController("buttonOK").setColorActive(blueAct) ;
  messageBox.setBackgroundColor(blueAct) ;
  messageBox.show() ;
}

public void loadButton(int theValue) {                                     // Load preset button
  File presetDir = new File( sketchPath("Preset/...") ) ;
  selectInput("Select a preset file to load:", "presetLoad", presetDir) ;
}

public void saveButton(int theValue) {                                     // Save preset button
  mbClose() ;
  validationProcess("preset") ;
  if ( allValid == 2 ) {
    messageBox.hide() ;
  }
  if ( allValid != 0 ) {
    File presetDir = new File( sketchPath("Preset/type name") ) ;
    selectOutput("Type preset name to save:", "presetSave", presetDir) ;
  }
}

void presetLoad(File selection) {
  if (selection == null) {
    //println("Window was closed or the user hit cancel.") ;
  } else {
    //println("User selected " + selection.getAbsolutePath()) ;
    cp5.loadProperties(selection.getAbsolutePath()) ;

    // Hack to keep slider labels alignement
    cp5.getController("varioHysteresis").getCaptionLabel().align(ControlP5.LEFT_OUTSIDE, ControlP5.CENTER).setPaddingX(10) ;
    cp5.getController("varioHysteresis").getValueLabel().align(ControlP5.RIGHT_OUTSIDE, ControlP5.CENTER).setPaddingX(10) ;
  }
}

void presetSave(File selection) {
  if (selection == null) {
    //println("Window was closed or the user hit cancel.") ;
  } else {
    //println("User selected " + selection.getAbsolutePath()) ;
    cp5.saveProperties(selection.getAbsolutePath()) ;
  }
}

public void writeConfButton(int theValue) {
  mbOkCancel() ;
  validationProcess("Config") ;
  if ( allValid == 0) {
    mbClose() ;
  }
}

public void validationProcess(String theString) {

  // Config file writing destination
  oxsDirectory = trim( cp5.get(Textfield.class, "oxsDirectory").getText() ) ;
  if ( oxsDirectory.equals("") ) {
    outputConfigDir = sketchPath("oXs_config.h") ;
  } else {
    outputConfigDir = oxsDirectory + "/oXs_config.h" ;
  }

  messageList.clear() ;
  messageList.set(0, "") ;
  messageList.append("") ;

  numPinsValid = true ;
  analogPinsValid = true ;
  vSpeedValid = 2 ;           // 0 -> not valid    1 -> warning   2 -> valid
  cellsValid = true ;
  sentDataValid = true ;
  versionValid = 2 ;          // 0 -> not valid    1 -> warning   2 -> valid

  validateNumPins() ;
  validateAnalogPins() ;
  validateVspeed() ;
  validateCells() ;

  if ( theString.equals("Config") ) {
    validateSentData() ;
    if ( numPinsValid && analogPinsValid && vSpeedValid != 0 && cellsValid && sentDataValid ) validateVersion() ;
  }

  if ( !numPinsValid || !analogPinsValid || vSpeedValid == 0 || !cellsValid || !sentDataValid || versionValid == 0 ) {

    messageBox.setBackgroundColor(errorColor) ;

    messageList.set(0, "                                              --- ERROR ---") ;
    messageList.append("") ;
    messageList.append("                                             ----------------------") ;
    messageList.append("") ;
    if ( theString.equals("preset") ) {
      messageList.append("Preset file can't be saved !") ;
    } else {
      messageList.append("Config file can't be written !") ;
    }
    //cp5.get(Textarea.class, "messageBoxLabel").setColor(color(255,0,0)) ;
    allValid = 0 ;

  } else if ( vSpeedValid == 1 || versionValid == 1 ) {

    messageBox.setBackgroundColor(warnColor) ;

    messageList.set(0, "                                           ----  WARNING  ----") ;
    messageList.append("") ;
    messageList.append("                                             ----------------------") ;
    messageList.append("") ;
    if ( theString.equals("Config") ) {
      messageList.append("Configuration file will be written to:") ;
      messageList.append(outputConfigDir) ;
      messageList.append("") ;
      messageList.append("                       ! If the file already exists, it will be replaced !") ;
    }

    allValid = 1 ;

  } else {

    messageBox.setBackgroundColor(okColor) ;

    messageList.set(0, "                                         --- ALL IS GOOD ! ---") ;
    if ( theString.equals("preset") ) {
      messageList.append("Preset file can be saved !") ;
    }
    messageList.append("") ;
    messageList.append("                                             ----------------------") ;

    allValid = 2 ;
  }

  String[] messageListArray = messageList.array() ;

  String joinedMessageList = join(messageListArray, "\n") ;

  cp5.get(Textarea.class, "messageBoxLabel").setText(joinedMessageList) ;
  //println(messageList) ;

  //messageBox.setBackgroundColor(color(240)) ;
  cp5.getController("buttonOK").setColorForeground(color(blueAct)) ;
  cp5.getController("buttonOK").setColorActive(color(orangeAct)) ;
  messageBox.show() ;

}

void folderSelected(File selection) {
  if (selection == null) {
    //println("Window was closed or the user hit cancel.") ;
  } else {
    //println("User selected " + selection.getAbsolutePath()) ;
    cp5.get(Textfield.class, "oxsDirectory").setText(selection.getAbsolutePath()) ;
  }
}

// =========================================== Shortcuts ===========================================
/*
void keyPressed() {
  // default properties load/save key combinations are
  // alt+shift+l to load properties
  // alt+shift+s to save properties
  if (key=='s') {
    cp5.saveProperties(("settings.oxs")) ;
  } else if (key=='l') {
    cp5.loadProperties(("settings.oxs")) ;

    // Hack to keep slider labels alignement
    cp5.getController("varioHysteresis").getCaptionLabel().align(ControlP5.LEFT_OUTSIDE, ControlP5.CENTER).setPaddingX(10) ;
    cp5.getController("varioHysteresis").getValueLabel().align(ControlP5.RIGHT_OUTSIDE, ControlP5.CENTER).setPaddingX(10) ;

    cp5.getProperties().print() ;
  } else if ( key == 'c' ) {
    println( "mAmp / step " + mAmpStep() ) ;
    println( "Current offset " + offsetCurrent() ) ;
  } else if ( key == 'v' ) {

  }
}

// =================================================================================================
*/
float round(float number, float decimal) {      // Rounding function
  return (float)(round((number*pow(10, decimal))))/pow(10, decimal) ;
}

float mVoltStep(int NbrVolt) {    // Voltage measurements milliVolt per ADC step calculation

  float mVoltStep ;
  float voltageDiv = cp5.getController("dividerVolt" + NbrVolt ).getValue() ;
  float arduinoVcc = cp5.getController("arduinoVccNb").getValue() ;

  if ( cp5.get(DropdownList.class, "voltRefChoice").value() == 1 ) {
    mVoltStep = ( arduinoVcc * 1000.0 / 1024.0 ) * voltageDiv ;
  } else {
    mVoltStep = ( 1.1 * 1000.0 / 1024.0 ) * voltageDiv ;
  }
  return mVoltStep ;
}

float mAmpStep() {    // Current sensor milliAmp per ADC step calculation

  float mAmpStep ;
  float mAmpPmV ;
  float arduinoVcc = cp5.getController("arduinoVccNb").getValue() ;
  float currentDiv = cp5.getController("currentDivNb").getValue() ;
  float currentOutSens = cp5.getController("currentOutSensNb").getValue() ;

  mAmpPmV = ( currentOutSens == 0 ) ? 0 : 1000.0 / currentOutSens ;
  if ( cp5.get(DropdownList.class, "voltRefChoice").value() == 1 ) {
    mAmpStep = ( arduinoVcc * 1000.0 / 1024.0 ) * mAmpPmV * currentDiv ;
  } else {
    mAmpStep = ( 1.1 * 1000.0 / 1024.0 ) * mAmpPmV ;
  }
  return mAmpStep ;
}

int offsetCurrent() {    // Current sensor offset calculation in ADC step

  int offsetCurrent ;
  //float currentVcc = cp5.getController("currentVccNb").getValue() ;
  float currentOutOffset = cp5.getController("currentOutOffsetNb").getValue() ;
  float currentDiv = cp5.getController("currentDivNb").getValue() ;
  float arduinoVcc = cp5.getController("arduinoVccNb").getValue() ;

  //if ( cp5.getController( "currentDir" ).value() == 0 ) {
  //  offsetCurrent = int( ( currentVcc / 2.0 + currentOutOffset / 1000.0 ) / arduinoVcc  * 1024.0 * currentDiv ) ;
  //} else {
    offsetCurrent = int( currentOutOffset / 1000.0 / arduinoVcc  * 1024.0 * currentDiv ) ;
  //}

  return offsetCurrent ;
}
