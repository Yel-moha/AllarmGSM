void ConfigOptions() {
  if (MenuPosition == 0) {  // ATTIVA ALLARME
    ACTIVE = true;
    AlarmStatus = "ATTIVO";
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("ALLARME  GSM");
    lcd.setCursor(0, 2);
    lcd.print("ATTIVAZIONE IN 60sec");
    analogWrite(LUMINOSITY, 100);
    ACTIVATEGSM();
    for (int i = 0; i < 16; i++) {
      lcd.setCursor(i, 3);
      lcd.print("*");
      Buzzer();
      delay(2800);
    }
    for (int i = 16; i < 20; i++) {
      lcd.setCursor(i, 3);
      lcd.print("*");
      Buzzer();
      delay(800);
      Buzzer();
      delay(800);
      Buzzer();
      delay(800);
    }
    analogWrite(LUMINOSITY, 0);
    Serial.println("ALARM IS ON");
    ALARMONSMS();
    DISABLEGSM();
    lcd.clear();
  }
  if (MenuPosition == 1) {  // MODIFICA SERIALE
    SERIALE = "L" + GetConfig("Modifica Seriale", SERIALE, 6, 1);
    SMSTEXT = "ALLARME RILEVATO DEVICE " + SERIALE;
    SMSTEXTON = "ATTIVATO ALLARME DEVICE " + SERIALE;
    SMSTEXTOFF = "DISATTIVATO ALLARME DEVICE " + SERIALE;
    WriteDataToEEProm();
  }
  if (MenuPosition == 2) {  // MODIFICA PIN
    PASSWORD = GetConfig("Modifica PIN", PASSWORD, 4, 0);
    WriteDataToEEProm();
  }
  if (MenuPosition == 3) {  // MODIFICA TELEFONO
    PHONENUMBER = GetConfig("Modifica Telefono", PHONENUMBER, 12, 0);
    WriteDataToEEProm();
  }
  if (MenuPosition == 4) {  // ASSOCIA SENSORI
    GetRFCode();
  }
  if (MenuPosition == 5) {  // ELIMINA SENSORI
    DelRFCode();
  }
  if (MenuPosition == 6) {  // INVIA CODICE
    SendRFCode();
  }
  if (MenuPosition == 7) {  // TEST I/O
    TestIO();
  }
  if (MenuPosition == MenuMaxPos) {
    ALARM = false;
    AlarmStatus = "DISATTIVO";
  }
  MenuPosition = 0;
  Buzzer();
  STATUS = "NORMAL";
}

void CheckKeypadMenu() {
  char key = getKey();
  if (key != NO_KEY) {
    Buzzer();
    Serial.print("KEY PRESSED: ");
    Serial.println(key);
    if (key == '*') {
      MenuPosition++;
      if (MenuPosition > MenuMaxPos) {
        MenuPosition = 0;
      }
    }
    if (key == '#') {
      ConfigOptions();
    }
  }
}

String GetConfig(String title, String value, int len, int stt) {
  String ret = "";
  lcd.clear();
  int ll = title.length();
  int spaces = int((20 - ll) / 2);
  for (int n = 0; n <= spaces; n++) {
    title = " " + title;
  }
  lcd.setCursor(0, 0);
  lcd.print(title);
  lcd.setCursor(0, 2);
  lcd.print("?: " + value);
  lcd.blink();
  for (int i = stt; i < (len + stt); i++) {
    lcd.setCursor((3 + i), 2);
    char key = getKey();
    while (key == NO_KEY) {
      key = getKey();
    }
    Buzzer();
    if (key == '*') {
      lcd.print(" ");
    } else if (key == '#') {
      i = len;
    } else {
      ret = ret + key;
      lcd.print(key);
    }
  }
  lcd.noBlink();
  return ret;
}