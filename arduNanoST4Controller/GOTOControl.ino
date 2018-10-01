void GOTOMountControl()
{
    if (Serial.available() > 0) {
    //Received something
    String opcode = Serial.readStringUntil('#');
    boolean validOpcode=true;
    //Parse opcode
    if(opcode=="CONNECT"){
      isGOTOMovingAR = false;
      isGOTOMovingDEC = false;
    }
    else if (opcode=="DISCONNECT"){
      isGOTOMovingAR = false;
      isGOTOMovingDEC = false;
    }
    else if(opcode=="RA0"){
      isGOTOMovingAR = false;
    }
    else if(opcode=="RA+"){
      digitalWrite(dirPinAR, dirFwd);  
      isGOTOMovingAR = true;
    }
    else if(opcode=="RA-"){
      digitalWrite(dirPinAR, dirBck);  
      isGOTOMovingAR = true;
    }
    else if(opcode=="DEC0"){
      isGOTOMovingDEC = false;
    }
    else if(opcode=="DEC+"){
      digitalWrite(dirPinDEC, dirFwd);
      isGOTOMovingDEC = true;
    }
    else if(opcode=="DEC-"){
      digitalWrite(dirPinDEC, dirBck);
      isGOTOMovingDEC = true;
    }
    else{
      validOpcode=false;
    }
    if(validOpcode){
      //Acknowledge valid command
      Serial.println("OK#");
    }
  }
}
