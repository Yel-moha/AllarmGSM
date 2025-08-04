/*
 * Gestione EEPROM - Memoria persistente
 */

// Scrive dati configurazione in EEPROM
void WriteDataToEEProm() {
  EEPROM_WriteString(EEP_SERIAL, SERIALE);
  EEPROM_WriteString(EEP_PIN, PASSWORD);
  EEPROM_WriteString(EEP_PHONE, PHONENUMBER);
  EEPROM_WriteString(EEP_PHONE2, PHONENUMBER2);
  EEPROM_WriteString(EEP_PHONE3, PHONENUMBER3);
}

// Legge dati configurazione da EEPROM
void ReadDataToEEProm() {
  SERIALE = EEPROM_ReadString(EEP_SERIAL);
  PASSWORD = EEPROM_ReadString(EEP_PIN);
  PHONENUMBER = EEPROM_ReadString(EEP_PHONE);
  PHONENUMBER2 = EEPROM_ReadString(EEP_PHONE2);
  PHONENUMBER3 = EEPROM_ReadString(EEP_PHONE3);
}

// Scrive sensori in EEPROM
void WriteSensorsToEEProm() {
  EEPROM_WriteString(EEP_NUMSENS, String(RFSensors));
  for (int i = 0; i < RFSensors; i++) {
    EEPROM_WriteString((EEP_STASENS + (i * EEP_INCSENS)), String(RFValues[i]));
  }
}

// Legge sensori da EEPROM
void ReadSensorsToEEProm() {
  String temp;
  temp = EEPROM_ReadString(EEP_NUMSENS);
  RFSensors = temp.toInt();
  for (int i = 0; i < RFSensors; i++) {
    temp = EEPROM_ReadString((EEP_STASENS + (i * EEP_INCSENS)));
    RFValues[i] = temp.toInt();
  }
}

// Scrive array di byte in EEPROM
void EEPROM_WriteBytes(int startAddr, const byte* array, int numBytes) {
  for (int i = 0; i < numBytes; i++) {
    EEPROM.write(startAddr + i, array[i]);
  }
}

// Scrive stringa C in EEPROM
void EEPROM_WriteChar(int addr, const char* string) {
  int numBytes = strlen(string) + 1;
  EEPROM_WriteBytes(addr, (const byte*)string, numBytes);
}

// Legge stringa C da EEPROM
void EEPROM_ReadChar(int addr, char* buffer, int bufSize) {
  byte ch;
  int bytesRead = 0;
  
  ch = EEPROM.read(addr + bytesRead);
  buffer[bytesRead] = ch;
  bytesRead++;
  
  while ((ch != 0x00) && (bytesRead < bufSize)) {
    ch = EEPROM.read(addr + bytesRead);
    buffer[bytesRead] = ch;
    bytesRead++;
  }
  
  if ((ch != 0x00) && (bytesRead >= 1)) {
    buffer[bytesRead - 1] = 0;
  }
}

// Scrive stringa String in EEPROM
void EEPROM_WriteString(int addr, String string) {
  int numBytes = string.length() + 1;
  char buf[numBytes];
  char myStringChar[numBytes];
  string.toCharArray(myStringChar, numBytes);
  strcpy(buf, myStringChar);
  EEPROM_WriteChar(addr, buf);
}

// Legge stringa String da EEPROM
String EEPROM_ReadString(int addr) {
  char buf[64];
  EEPROM_ReadChar(addr, buf, 64);
  return String(buf);
}