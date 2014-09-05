// Customize functions

void customizeToggleSensor(Controller tglS) {  
  tglS.setColorForeground(blueAct) ;
  tglS.setColorBackground(color(70)) ;  
  tglS.setColorLabel(#000000) ;  
  tglS.setSize(15, 15) ;  
  tglS.setTab("default") ;
  // reposition the Labels
  tglS.getCaptionLabel().align(ControlP5.LEFT_OUTSIDE, ControlP5.CENTER) ; 

  tglS.captionLabel().toUpperCase(false) ;
}

void customizeSlider(Controller sld) {
  sld.setColorForeground(blueAct) ;
  sld.setColorLabel(#000000) ;
  sld.setColorValue(#000000) ;
  sld.setSize(150, 15) ;  
  sld.setValue(50) ;
  sld.setTab("vario") ;
  // reposition the Labels for controller 'slider'
  sld.getCaptionLabel().align(ControlP5.LEFT_OUTSIDE, ControlP5.CENTER).setPaddingX(10) ;  
  sld.getValueLabel().align(ControlP5.RIGHT_OUTSIDE, ControlP5.CENTER).setPaddingX(10) ;

  sld.captionLabel().toUpperCase(false) ;
}

void customizeToggle(Controller tgl) {  
  tgl.setColorForeground(orangeAct) ;
  tgl.setColorBackground(color(70)) ;
  tgl.setColorActive(blueAct) ;
  tgl.setColorLabel(#000000) ;
  tgl.setSize(15, 15) ;

  // reposition the Labels
  tgl.getCaptionLabel().align(ControlP5.LEFT_OUTSIDE, ControlP5.CENTER).setPaddingX(10) ;

  tgl.captionLabel().toUpperCase(false) ;
}

void customizeRange(Controller rng) {
  rng.setColorForeground(blueAct) ;
  rng.setColorBackground(color(70)) ;
  rng.setColorActive(orangeAct) ;   
  //rng.setSize(200, 20) ;          
  rng.setColorCaptionLabel(0) ; 
  rng.setTab("vario") ;

  //rng.getCaptionLabel().align(ControlP5.LEFT_OUTSIDE, ControlP5.CENTER) ;
  rng.captionLabel().toUpperCase(false) ;
}

void customizeToggleVolt(Controller tglV) {  
  tglV.setColorForeground(orangeAct) ;
  tglV.setColorBackground(color(70)) ;
  tglV.setColorActive(blueAct) ;
  tglV.setColorLabel(#000000) ;
  tglV.setSize(15, 15) ;

  // reposition the Labels
  tglV.getCaptionLabel().align(ControlP5.CENTER, ControlP5.TOP_OUTSIDE).setPaddingX(10) ;

  tglV.captionLabel().toUpperCase(false) ;
}

void customizeDdlVpin(DropdownList ddlV) {  
  ddlV.setSize(30, 200) ;
  ddlV.setColorForeground(orangeAct) ;
  ddlV.setColorBackground(color(70)) ;
  ddlV.setColorActive(blueAct) ;
  ddlV.setItemHeight(20) ;
  ddlV.setBarHeight(20) ;
  ddlV.captionLabel().set(" ") ;
  ddlV.captionLabel().style().marginTop = 2 ;
  
  ddlV.addItem("--", -1) ;
  ddlV.addItems(analogPins) ;
  //ddlv.setValue(-1) ;
 
  ddlV.toUpperCase(false) ;  
}



