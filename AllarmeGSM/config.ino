void TestIO() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("      TEST I/O");
  lcd.setCursor(0, 2);
  lcd.print(" Premi * per Uscire");
  char key = getKey();
  while (key == NO_KEY) {
    key = getKey();
    Serial.print("EXTALARM STATUS:");
    Serial.println(digitalRead(EXTALARM));
    delay(1000);
    digitalWrite(EXTRELAY, HIGH);
    Serial.println("EXTRELAY: ON");
    delay(1000);
    digitalWrite(EXTRELAY, LOW);
    Serial.println("EXTRELAY: OFF");
    delay(1000);
    Serial.println("BUZZER: ON");
    if (BuzzerActive == true) {
      digitalWrite(BUZZERPIN, HIGH);
      delay(1000);
      digitalWrite(BUZZERPIN, LOW);
    } else {
      tone(BUZZERPIN, 1568, 1000);
    }
    Serial.println("BUZZER: OFF");
    delay(1000);
  }
  Buzzer();
}

void SendRFCode() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Invio Codice  RF  ");
  String codet = SERIALE.substring(1, 6);
  lcd.setCursor(0, 1);
  lcd.print("Codice: " + codet);
  lcd.setCursor(0, 2);
  lcd.print("Premi # per Salvare");
  lcd.setCursor(0, 3);
  lcd.print(" Premi * per Uscire");
  int code = codet.toInt();
  char key = getKey();
  while (key == NO_KEY) {
    key = getKey();
    rftrasmitter.send(code, 24);
    delay(500);
  }
  Buzzer();
}

void GetRFCode() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Associa  Sensori");
  lcd.setCursor(0, 1);
  lcd.print("Sensore ");
  lcd.print(String(RFSensors + 1));
  lcd.print(": ");
  lcd.setCursor(0, 2);
  lcd.print("Premi # per Salvare");
  lcd.setCursor(0, 3);
  lcd.print(" Premi * per Uscire");
  int vsens = 0;
  char key = getKey();
  while (key == NO_KEY) {
    if (rfreceiver.available()) {
      Serial.print("DATA FROM RF433 RAW: ");
      vsens = rfreceiver.getReceivedValue();
      String sens = String(vsens);
      Serial.println(sens);
      lcd.setCursor(12, 1);
      lcd.print(sens);
      Serial.println("DATA FROM RF433 : " + sens);
      rfreceiver.resetAvailable();
    }
    key = getKey();
  }
  if (key == '#') {
    RFValues[RFSensors] = vsens;
    RFSensors++;
    WriteSensorsToEEProm();
  }
  Buzzer();
}

void DelRFCode() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Elimina  Sensori");
  lcd.setCursor(0, 1);
  lcd.print("Premi # per Eliminare");
  lcd.setCursor(0, 2);
  lcd.print("   TUTTI i Sensori");
  lcd.setCursor(0, 3);
  lcd.print(" Premi * per Uscire");
  int vsens = 0;
  char key = getKey();
  while (key == NO_KEY) {
    key = getKey();
  }
  if (key == '#') {
    for (int i = 0; i < RFSensors; i++) {
      RFValues[i] = { 999 };
    }
    RFSensors = 0;
    WriteSensorsToEEProm();
  }
  Buzzer();
}
