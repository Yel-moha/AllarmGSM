char getKey() {
  char key = NO_KEY;  // 0 indica nessun tasto premuto
  for (int column = 0; column < numCols; column++) {
    digitalWrite(colPins[column], LOW);      // Attiva la colonna corrente per la lettura.
    for (int row = 0; row < numRows; row++)  // Scorro tutte le righe cercano un tasto premuto
    {
      if (digitalRead(rowPins[row]) == LOW)  // E' premuto il tasto
      {
        delay(debounceTime);  // attendo che il segnale si stabilizzi
        while (digitalRead(rowPins[row]) == LOW)
          ;                         // attendo che il tasto sia rilasciato
        key = keymap[row][column];  // Memorizzo quale sia il tasto premuto
      }
    }
    digitalWrite(colPins[column], HIGH);  // Rendo inattiva la colonna.
  }
  return key;  // restituisce il tasto premuto 0 altrimenti
}

void initKey() {
  for (int row = 0; row < numRows; row++) {
    pinMode(rowPins[row], INPUT);      // Imposta i pin delle righe come input
    digitalWrite(rowPins[row], HIGH);  // Imposta le righe a HIGH (inattiva) - abilita le resistenze di pull-ups
  }
  for (int column = 0; column < numCols; column++) {
    pinMode(colPins[column], OUTPUT);     // Setta i pin delle colonne come input
    digitalWrite(colPins[column], HIGH);  // Imposta le colonne a HIGH (inattiva) - abilita le resistenze di pull-ups
  }
}

void CheckAlarmPassword() {
  char key = getKey();
  if (key != NO_KEY) {
    Buzzer();
    Serial.print("KEY PRESSED: ");
    Serial.println(key);
    if (key == '*') {
      Input_Keys = "";
    } else if (key == '#') {
      if (Input_Keys == String(PASSWORD)) {
        if (ACTIVE == true) {
          AlarmOFF();
          Serial.println("ALARM IS OFF (PASSWORD)");
          lcd.clear();
        }
        Input_Keys = "";
      }
    } else {
      Input_Keys += key;
      if (Input_Keys.length() > 4) {
        Input_Keys = key;
      }
    }
  }
}

void CheckKeypadPassword() {
  char key = getKey();
  if (key != NO_KEY) {
    Buzzer();
    Serial.print("KEY PRESSED: ");
    Serial.println(key);
    if (key == '*') {
      Input_Keys = "";
    } else if (key == '#') {
      if (Input_Keys == String(PASSWORD)) {
        if (ACTIVE == true) {
          AlarmOFF();
          Serial.println("ALARM IS OFF (PASSWORD)");
          lcd.clear();
        } else {
          ALARM = false;
          ACTIVE = false;
          SMSSENT = false;
          AlarmStatus = "DISATTIVO";
          STATUS = "CONFIG";
          Serial.println("ENTER CONFIG");
          lcd.clear();
        }
        Input_Keys = "";
      }
    } else {
      Input_Keys += key;
      if (Input_Keys.length() > 4) {
        Input_Keys = key;
      }
    }
  }
}