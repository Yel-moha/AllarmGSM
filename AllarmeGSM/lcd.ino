void LCDNormal() {
  lcd.setCursor(3, 0);
  lcd.print("ALLARME  GSM");
  lcd.setCursor(0, 1);
  lcd.print("Allarme: " + AlarmStatus);
  lcd.setCursor(0, 2);
  lcd.print("Stato: " + AlarmActive);
  lcd.setCursor(0, 3);
  lcd.print("Device: " + SERIALE);
}

void LCDConfig() {
  lcd.setCursor(2, 0);
  lcd.print("CONFIGURAZIONE");
  for (int i = MenuPosition; i < (MenuPosition + 3); i++) {
    lcd.setCursor(2, (1 + i - MenuPosition));
    int l = i;
    if (l <= MenuMaxPos) {
      lcd.print(MenuLabel[l]);
      int labellen = MenuLabel[l].length();
      for (int n = (2 + labellen); n <= 20; n++) {
        lcd.setCursor(n, (1 + i - MenuPosition));
        lcd.print(" ");
      }
    }
  }
  lcd.setCursor(0, 1);
  lcd.print("*");
}