
void createMessageBox() {

  // create a group to store the messageBox elements
  messageBox = cp5.addGroup("messageBox", width / 2 - mBoxWidth / 2, 76, mBoxWidth) ;
  messageBox.setBackgroundHeight(mBoxHeight) ;
  messageBox.setBackgroundColor(color(240)) ;
  messageBox.setTab("global") ;
  messageBox.hideBar() ;
  messageBox.hide() ;

  // add a Textaera to the messageBox.
  cp5.addTextarea("messageBoxLabel")
     .setPosition(5,5)
     .setSize(mBoxWidth - 10, mBoxHeight - 48)
     .setLineHeight(14)
     .setColor(color(255))
     .setColorActive(orangeAct)
     //.setBorderColor(color(0))
     .setColorBackground(color(120))
     .setColorForeground(blueAct)
     .setScrollBackground(color(80))
     //.setTab("global")
     ;
  cp5.get(Textarea.class, "messageBoxLabel").moveTo(messageBox) ;

  // OK button to the messageBox.
  controlP5.Button b1 = cp5.addButton("buttonOK", 0, width / 2 - 60, 218, 80, 30) ;
  b1.moveTo(messageBox) ;
  b1.setColorForeground(color(blueAct)) ;
  b1.setColorBackground(color(100)) ;
  b1.setColorActive(color(orangeAct)) ;
  b1.getCaptionLabel().setFont(font20) ;
  b1.captionLabel().toUpperCase(false) ;
  b1.getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER).setPaddingX(10) ;

  // Cancel button to the messageBox.
  cp5.addButton("buttonCancel", 0, mBoxWidth / 2 + 5, mBoxHeight - 37, 80, 30)
     .moveTo(messageBox)
     .setCaptionLabel("Cancel")
     .setColorForeground(blueAct)
     .setColorBackground(color(100))
     .setColorActive(orangeAct)
     .hide()
     ;
  cp5.getController("buttonCancel").getCaptionLabel().setFont(font20) ;
  cp5.getController("buttonCancel").captionLabel().toUpperCase(false) ;
  cp5.getController("buttonCancel").getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER).setPaddingX(10) ;

}

void mbOkCancel() {
  cp5.getController("buttonOK").setPosition(mBoxWidth / 2 - 80 - 5, mBoxHeight - 37) ;
  cp5.getController("buttonOK").setSize(80, 30) ;
  cp5.getController("buttonOK").setCaptionLabel("OK") ;

  cp5.getController("buttonCancel").show() ;
}

void mbClose() {
  cp5.getController("buttonOK").setPosition(mBoxWidth / 2 - 40 , mBoxHeight - 37) ;
  cp5.getController("buttonOK").setSize(80, 30) ;
  cp5.getController("buttonOK").setCaptionLabel("CLOSE") ;

  cp5.getController("buttonCancel").hide() ;
}

public void buttonOK(int theValue) {
  //println("OK button: "+theValue) ;
  if ( allValid != 0 ) {
    writeConf() ;
  }
  messageBox.hide() ;
}

public void buttonCancel(int theValue) {
  //println("Cancel button: "+theValue) ;
  messageBox.hide() ;
}

