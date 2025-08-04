/*
 * Gestione tastiera a matrice 4x3
 */

// Legge tasto premuto
char getKey() {
  char key = NO_KEY;
  
  // Scansiona tutte le colonne
  for (int column = 0; column < numCols; column++) {
    digitalWrite(colPins[column], LOW); // Attiva colonna corrente
    
    // Scansiona tutte le righe
    for (int row = 0; row < numRows; row++) {
      if (digitalRead(rowPins[row]) == LOW) { // Tasto premuto
        delay(debounceTime); // Anti-rimbalzo
        while (digitalRead(rowPins[row]) == LOW); // Attendi rilascio
        key = keymap[row][column]; // Ottieni tasto dalla mappa
      }
    }
    
    digitalWrite(colPins[column], HIGH); // Disattiva colonna
  }
  
  return key;
}

// Inizializza tastiera
void initKey() {
  // Configura righe come input con pull-up
  for (int row = 0; row < numRows; row++) {
    pinMode(rowPins[row], INPUT_PULLUP);
  }
  
  // Configura colonne come output
  for (int column = 0; column < numCols; column++) {
    pinMode(colPins[column], OUTPUT);
    digitalWrite(colPins[column], HIGH);
  }
}

// Controlla password da tastiera (modalità normale)
void CheckKeypadPassword() {
  char key = getKey();
  if (key != NO_KEY) {
    Buzzer();
    Serial.print("KEY PRESSED: ");
    Serial.println(key);
    
    // Mostra asterischi invece della password
    lcd.setCursor(8, 3);
    lcd.print("*");
    
    if (key == '*') {
      Input_Keys = ""; // Cancella input
    } 
    else if (key == '#') {
      if (Input_Keys == PASSWORD) {
        if (ACTIVE) {
          AlarmOFF();
          Serial.println("ALARM IS OFF (PASSWORD)");
          lcd.clear();
        } else {
          STATUS = "CONFIG";
          Serial.println("ENTER CONFIG");
          lcd.clear();
        }
        Input_Keys = "";
      } else {
        Input_Keys = ""; // Password errata, resetta
      }
    } 
    else {
      Input_Keys += key;
      if (Input_Keys.length() > 4) {
        Input_Keys = key; // Limita a 4 caratteri
      }
    }
  }
}

// Controlla password durante allarme
void CheckAlarmPassword() {
  char key = getKey();
  if (key != NO_KEY) {
    Buzzer();
    Serial.print("KEY PRESSED: ");
    Serial.println(key);
    
    lcd.setCursor(0, 3);
    lcd.print("PIN:");
    lcd.print(Input_Keys + key);
    
    if (key == '*') {
      Input_Keys = "";
    } 
    else if (key == '#') {
      if (Input_Keys == PASSWORD) {
        AlarmOFF();
        Serial.println("ALARM IS OFF (PASSWORD)");
        lcd.clear();
        Input_Keys = "";
      }
    } 
    else {
      Input_Keys += key;
      if (Input_Keys.length() > 4) {
        Input_Keys = key;
      }
    }
  }
}