void Buzzer() {
  if (BuzzerActive == true) {
    digitalWrite(BUZZERPIN, HIGH);
    delay(100);
    digitalWrite(BUZZERPIN, LOW);
  } else {
    tone(BUZZERPIN, 1900, 100);
  }
}

void AlarmOFF() {
  ALARM = false;
  ACTIVE = false;
  SMSSENT = false;
  AlarmStatus = "DISATTIVO";
  AlarmActive = "NORMALE";
  STATUS = "NORMAL";
  digitalWrite(EXTRELAY, HIGH);
  ALARMOFFSMS();
}

bool CheckRFData(int value) {
  bool ret = false;
  for (int n = 0; n < RFSensors; n++) {
    if (RFValues[n] == value) {
      ret = true;
    }
  }
  return ret;
}

void CALLPHONE() {
  Serial.println("CALL TO: " + PHONENUMBER);
  int AL0 = PHONENUMBER.length() + 1;
  char* CA0 = PHONENUMBER.c_str();
  PhoneCall(CA0);
}

void SENDSMS() {
  Serial.print("SMS TO: " + PHONENUMBER);
  int AL1 = PHONENUMBER.length() + 1;
  char* CA1 = PHONENUMBER.c_str();
  int AL2 = SMSTEXT.length() + 1;
  char* CA2 = SMSTEXT.c_str();
  SendingShortMessage(CA1, CA2);
}

void AlarmON() {
  if (SMSSENT == false) {
    AlarmActive = "ALLARME";
    lcd.clear();
    LCDNormal();
    Serial.println("ATTIVAZIONE ALLARME IN 30sec");
    Buzzer();
    analogWrite(LUMINOSITY, 100);
    ACTIVATEGSM();
    for (int i = 0; i < 150; i++) { // questo for permette di verificare che non sia stata inserita la password per disattivare l'allarme.
      CheckAlarmPassword();
      //Buzzer();
      delay(200);
      if (ACTIVE == false) {
        break;
      }
    }
    analogWrite(LUMINOSITY, 0);
    if (ACTIVE == true) {
      Serial.println("ALLARME ATTIVO - SIRENA E SMS IN CORSO...");
      digitalWrite(EXTRELAY, LOW);
      LCDNormal();
      Buzzer();
      PowerOn(POWERKEY);
      SENDSMS();
      //CALLPHONE();
      SMSSENT = true;
      DISABLEGSM();
    }
    lcd.clear();
    LCDNormal();
  }
}

void ALARMOFFSMS() {
  ACTIVATEGSM();
  delay(30000); //aspetta 30 secondi per permettere al modulo GSM stabilire la rete Aggiunto da Youssef 
  Serial.print("SMS TO: " + PHONENUMBER);
  int AL1 = PHONENUMBER.length() + 1;
  char* CA1 = PHONENUMBER.c_str();
  int AL2 = SMSTEXTOFF.length() + 1;
  char* CA2 = SMSTEXTOFF.c_str();
  SendingShortMessage(CA1, CA2);
  delay(1000); // Aggiunto da youssef
  DISABLEGSM(); // Aggiunto da youssef
}


void ALARMONSMS() {
  Serial.print("SMS TO: " + PHONENUMBER);
  int AL1 = PHONENUMBER.length() + 1;
  char* CA1 = PHONENUMBER.c_str();
  int AL2 = SMSTEXTON.length() + 1;
  char* CA2 = SMSTEXTON.c_str();
  SendingShortMessage(CA1, CA2);
}

void ACTIVATEGSM() {
  Serial.println("ACTIVATE GSM MODULE");
  digitalWrite(GSMRELAY, LOW);
}

void DISABLEGSM() {
  Serial.println("DISABLE GSM MODULE");
  digitalWrite(GSMRELAY, HIGH);
}

void EnableAlarmRF() {
  if (rfreceiver.available()) {
    String codet = SERIALE.substring(1, 6);
    int code = codet.toInt();
    if (int(rfreceiver.getReceivedValue()) == code) { 
      Serial.println("ALARM ON FROM RF433");
      ACTIVE = true;
      AlarmStatus = "ATTIVO";
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print("ALLARME  GSM");
      lcd.setCursor(0, 2);
      lcd.print("ATTIVAZIONE IN 30sec");
      analogWrite(LUMINOSITY, 100);
      ACTIVATEGSM();
      for (int i = 0; i < 16; i++) { //Modificato da Youssef per farli diventare 60 secondi 
        lcd.setCursor(i, 3);
        lcd.print("*");
        Buzzer();
        delay(1400);
      }
      for (int i = 16; i < 20; i++) {
        lcd.setCursor(i, 3);
        lcd.print("*");
        Buzzer();
        delay(400);
        Buzzer();
        delay(400);
        Buzzer();
        delay(400);
      }
      analogWrite(LUMINOSITY, 0);
      Serial.println("ALARM IS ON");
      ALARMONSMS();
      DISABLEGSM();
      lcd.clear();
    }
    rfreceiver.resetAvailable();
  }
}