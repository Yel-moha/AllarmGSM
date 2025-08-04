/*
 * utility.ino - Funzioni di utilità generale
 */

// Attiva/disattiva buzzer
void Buzzer() {
  if (BuzzerActive == true) {
    digitalWrite(BUZZERPIN, HIGH);
    delay(100);
    digitalWrite(BUZZERPIN, LOW);
  } else {
    tone(BUZZERPIN, 1900, 100);
  }
}

// Disattiva allarme
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

// Verifica codice RF
bool CheckRFData(int value) {
  bool ret = false;
  for (int n = 0; n < RFSensors; n++) {
    if (RFValues[n] == value) {
      ret = true;
    }
  }
  return ret;
}

// Effettua chiamata
void CALLPHONE() {
  Serial.println("CALL TO: " + PHONENUMBER);
  int AL0 = PHONENUMBER.length() + 1;
  char* CA0 = PHONENUMBER.c_str();
  PhoneCall(CA0);
}

// Invia SMS
void SENDSMS() {
  Serial.print("SMS TO: " + PHONENUMBER);
  int AL1 = PHONENUMBER.length() + 1;
  char* CA1 = PHONENUMBER.c_str();
  int AL2 = SMSTEXT.length() + 1;
  char* CA2 = SMSTEXT.c_str();
  SendingShortMessage(CA1, CA2);
}

// Attiva allarme
void AlarmON() {
  if (SMSSENT == false) {
    AlarmActive = "ALLARME";
    lcd.clear();
    LCDNormal();
    Serial.println("ATTIVAZIONE ALLARME IN 30sec");
    Buzzer();
    
    ACTIVATEGSM();
    for (int i = 0; i < 15; i++) { // Ridotto a 15 iterazioni per LCD
      CheckAlarmPassword();
      
      lcd.setCursor(i, 2);
      lcd.print("*");
      
      delay(2000); // Aumentato delay per visualizzazione
      if (ACTIVE == false) {
        break;
      }
    }
    
    if (ACTIVE == true) {
      ActivSound = true;
      Serial.println("ALLARME ATTIVO - SIRENA E SMS IN CORSO...");
      lcd.clear();
      LCDNormal();
      PowerOn(POWERKEY);
      SENDSMS();
      SMSSENT = true;
      DISABLEGSM();
      digitalWrite(EXTRELAY, LOW);
    }
    lcd.clear();
    LCDNormal();
  }
}

// Disattiva sirena
void SIRENAOFF() {
  digitalWrite(EXTRELAY, HIGH);
  ALARM = false;
  SMSSENT = false;
  AlarmStatus = "ATTIVO";
  AlarmActive = "NORMALE";
  CronoSoundBeg = 0;
  rfreceiver.resetAvailable();
}

// Invia SMS disattivazione
void ALARMOFFSMS() {
  ACTIVATEGSM();
  delay(30000); // Attesa connessione GSM
  Serial.print("SMS TO: " + PHONENUMBER);
  int AL1 = PHONENUMBER.length() + 1;
  char* CA1 = PHONENUMBER.c_str();
  int AL2 = SMSTEXTOFF.length() + 1;
  char* CA2 = SMSTEXTOFF.c_str();
  SendingShortMessage(CA1, CA2);
  delay(1000);
  DISABLEGSM();
}

// Invia SMS attivazione
void ALARMONSMS() {
  Serial.print("SMS TO: " + PHONENUMBER);
  int AL1 = PHONENUMBER.length() + 1;
  char* CA1 = PHONENUMBER.c_str();
  int AL2 = SMSTEXTON.length() + 1;
  char* CA2 = SMSTEXTON.c_str();
  SendingShortMessage(CA1, CA2);
}

// Attiva modulo GSM
void ACTIVATEGSM() {
  Serial.println("ACTIVATE GSM MODULE");
  digitalWrite(GSMRELAY, LOW);
}

// Disattiva modulo GSM
void DISABLEGSM() {
  Serial.println("DISABLE GSM MODULE");
  digitalWrite(GSMRELAY, HIGH);
}

// Abilita attivazione RF
void EnableAlarmRF() {
  if (rfreceiver.available()) {
    String codet = SERIALE.substring(1, 6);
    int code = codet.toInt();
    if (int(rfreceiver.getReceivedValue()) == code) { 
      Serial.println("ALARM ON FROM RF433");
      ACTIVE = true;
      AlarmStatus = "ATTIVO";
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("ALLARME GSM");
      lcd.setCursor(0, 1);
      lcd.print("ATTIVAZIONE IN 30s");
      
      ACTIVATEGSM();
      for (int i = 0; i < 15; i++) { // Ridotto a 15 iterazioni per LCD
        lcd.setCursor(i, 2);
        lcd.print("*");
        Buzzer();
        delay(2000); // Aumentato delay per visualizzazione
      }
      
      ALARMONSMS();
      DISABLEGSM();
      lcd.clear();
    }
    rfreceiver.resetAvailable();
  }
}