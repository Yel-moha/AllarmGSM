/*
 * menu.ino - Gestione menu configurazione (LCD 20x4, da 0 a 11)
 */

void LCDConfig() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CONFIGURAZIONE");

  for (int i = 0; i < 3; i++) {
    int index = MenuPosition + i;
    if (index <= MenuMaxPos) {
      lcd.setCursor(0, i + 1);
      lcd.print(i == 0 ? ">" : " ");
      String label = MenuLabel[index];
      lcd.print(label.substring(0, min(19, label.length())));
    }
  }
}

void CheckKeypadMenu() {
  char key = getKey();
  if (key != NO_KEY) {
    Buzzer();
    Serial.print("KEY PRESSED: ");
    Serial.println(key);

    if (key == '*') {
      MenuPosition++;
      if (MenuPosition > MenuMaxPos) MenuPosition = 0;
      LCDConfig();
    }
    else if (key == '#') {
      ConfigOptions();
      LCDConfig();
    }
  }
}

void ConfigOptions() {
  lcd.setCursor(0, 3);
  lcd.print(">");

  switch (MenuPosition) {
    case 0: // ATTIVA ALLARME
      ACTIVE = true;
      AlarmStatus = "ATTIVO";
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print("ALLARME GSM");
      lcd.setCursor(0, 2);
      lcd.print("ATTIVAZIONE IN 60s");
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
        Buzzer(); delay(800);
        Buzzer(); delay(800);
        Buzzer(); delay(800);
      }

      analogWrite(LUMINOSITY, 0);
      Serial.println("ALARM IS ON");
      ALARMONSMS();
      DISABLEGSM();
      lcd.clear();
      break;

    case 1: // MODIFICA SERIALE
      SERIALE = "L" + GetConfig("Modifica Seriale", SERIALE.substring(1), 6);
      SMSTEXT = "ALLARME RILEVATO DEVICE " + SERIALE;
      SMSTEXTON = "ATTIVATO ALLARME DEVICE " + SERIALE;
      SMSTEXTOFF = "DISATTIVATO ALLARME DEVICE " + SERIALE;
      WriteDataToEEProm();
      break;

    case 2: // MODIFICA PIN
      PASSWORD = GetConfig("Modifica PIN", PASSWORD, 4);
      WriteDataToEEProm();
      break;

    case 3: // MODIFICA TELEFONO
      PHONENUMBER = GetConfig("Modifica Telefono", PHONENUMBER, 12);
      WriteDataToEEProm();
      break;

    case 4:
      GetRFCode(); break;
    case 5:
      DelRFCode(); break;
    case 6:
      SendRFCode(); break;
    case 7:
      TestIO(); break;

    case 11: // ESCI
      ALARM = false;
      AlarmStatus = "DISATTIVO";
      STATUS = "NORMAL";
      break;
  }

  MenuPosition = 0;
  Buzzer();
}

String GetConfig(String title, String value, int len) {
  String ret = "";
  lcd.clear();

  if (title.length() > 20) title = title.substring(0, 20);
  lcd.setCursor(0, 0);
  lcd.print(title);

  lcd.setCursor(0, 1);
  lcd.print("Attuale: ");
  lcd.print(value.substring(0, min(11, value.length())));

  lcd.setCursor(0, 2);
  lcd.print("Nuovo: ");
  lcd.blink();

  for (int i = 0; i < len; i++) {
    lcd.setCursor(7 + i, 2);
    char key = getKey();
    while (key == NO_KEY) {
      key = getKey();
    }

    Buzzer();
    if (key == '*') {
      lcd.print(" ");
    } else if (key == '#') {
      break;
    } else {
      ret += key;
      lcd.print(key);
    }
  }

  lcd.noBlink();
  return ret;
}
