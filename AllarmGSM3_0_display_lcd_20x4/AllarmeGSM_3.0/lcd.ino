/*
 * Gestione display LCD 20x4 I2C
 */

// Mostra schermata normale
void LCDNormal() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ALLARME GSM");
  
  lcd.setCursor(0, 1);
  lcd.print("Stato: " + AlarmStatus);
  
  lcd.setCursor(0, 2);
  lcd.print("Seriale: " + SERIALE.substring(0, 10)); // Mostra solo primi 10 caratteri
  
  lcd.setCursor(0, 3);
  lcd.print("Dig.PIN #=OK *=ESC");
}

// Pulisce parte di una riga
void clearPartOfLine(int col, int row, int length) {
  lcd.setCursor(col, row);
  for(int i=0; i<length; i++) {
    lcd.print(" ");
  }
}
/*
// Mostra menu configurazione (prima parte)
void LCDConfig() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CONFIGURAZIONE");
  
  // Mostra fino a 3 voci alla volta
  for (int i = 0; i < 3; i++) {
    int pos = MenuPosition/10 + i;
    if (pos <= 4) { // Solo prime 5 voci
      lcd.setCursor(0, i+1);
      if (i == 0) lcd.print(">"); // Freccia selezione
      lcd.print(MenuLabel[pos*10].substring(0, 19)); // Taglia se troppo lunga
    }
  }
}

// Mostra menu configurazione (seconda parte)
void LCDConfig_1() {
  if(DispChange == false) {
    lcd.clear();
    DispChange = true;
  }
  
  lcd.setCursor(0, 0);
  lcd.print("CONFIGURAZIONE");
  
  // Mostra fino a 3 voci alla volta (dal 5° in poi)
  for (int i = 0; i < 3; i++) {
    int pos = MenuPosition/10 + i - 4;
    if (pos >= 0 && pos <= 6) { // Dal 5° all'11° elemento
      lcd.setCursor(0, i+1);
      if (i == 0) lcd.print(">"); // Freccia selezione
      lcd.print(MenuLabel[(pos+4)*10].substring(0, 19)); // Taglia se troppo lunga
    }
  }
}
*/