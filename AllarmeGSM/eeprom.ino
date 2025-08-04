void WriteDataToEEProm() {
  EEPROM_WriteString(EEP_SERIAL, SERIALE);
  EEPROM_WriteString(EEP_PIN, PASSWORD);
  EEPROM_WriteString(EEP_PHONE, PHONENUMBER);
}

void ReadDataToEEProm() {
  SERIALE = EEPROM_ReadString(EEP_SERIAL);
  PASSWORD = EEPROM_ReadString(EEP_PIN);
  PHONENUMBER = EEPROM_ReadString(EEP_PHONE);
}

void WriteSensorsToEEProm() {
  EEPROM_WriteString(EEP_NUMSENS, String(RFSensors));
  for (int i = 0; i < RFSensors; i++) {
    EEPROM_WriteString((EEP_STASENS + (i * EEP_INCSENS)), String(RFValues[i]));
  }
}

void ReadSensorsToEEProm() {
  String temp;
  temp = EEPROM_ReadString(EEP_NUMSENS);
  RFSensors = temp.toInt();
  for (int i = 0; i < RFSensors; i++) {
    temp = EEPROM_ReadString((EEP_STASENS + (i * EEP_INCSENS)));
    RFValues[i] = temp.toInt();
  }
}

void EEPROM_WriteBytes(int startAddr, const byte* array, int numBytes) {
  for (int i = 0; i < numBytes; i++) {
    EEPROM.write(startAddr + i, array[i]);
  }
}

void EEPROM_WriteChar(int addr, const char* string) {
  int numBytes;
  numBytes = strlen(string) + 1;
  EEPROM_WriteBytes(addr, (const byte*)string, numBytes);
}

void EEPROM_ReadChar(int addr, char* buffer, int bufSize) {
  byte ch;
  int bytesRead;
  bytesRead = 0;
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

void EEPROM_WriteString(int addr, String string) {
  int numBytes;
  numBytes = string.length() + 1;
  char buf[numBytes];
  char myStringChar[numBytes];
  string.toCharArray(myStringChar, numBytes);
  strcpy(buf, myStringChar);
  EEPROM_WriteChar(addr, buf);
}

String EEPROM_ReadString(int addr) {
  char buf[64];
  EEPROM_ReadChar(addr, buf, 64);
  String string = String(buf);
  return string;
}