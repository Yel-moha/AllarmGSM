/*
 * gsm.ino - Gestione modulo GSM con output su LCD
 */
 #include <Arduino.h>
#include <SoftwareSerial.h>

// Variabili per gestione SMS
String smsBuffer = "";
bool smsReceiving = false;
unsigned long smsReceiveStart = 0;

// Variabili per gestione chiamate
bool isRinging = false;
unsigned long lastRingTime = 0;
String lastCaller = "";


// Buffer per le righe del LCD
String lcdBuffer[4] = {"", "", "", ""};

// Funzione per aggiungere testo al buffer LCD
void addToLcdBuffer(String message) {
  // Sposta le righe esistenti verso l'alto
  for(int i = 0; i < 3; i++) {
    lcdBuffer[i] = lcdBuffer[i+1];
  }
  
  // Aggiungi la nuova riga (massimo 20 caratteri)
  lcdBuffer[3] = message.substring(0, 20);
  
  // Aggiorna il display
  updateLcdDisplay();
}

// Aggiorna il display con il buffer corrente
void updateLcdDisplay() {
  lcd.clear();
  for(int i = 0; i < 4; i++) {
    lcd.setCursor(0, i);
    lcd.print(lcdBuffer[i]);
  }
}

// Versione modificata di Serial.print() che scrive anche su LCD
void serialLcdPrint(String message) {
  Serial.print(message);
  
  // Gestione a capo e testo lungo
  int start = 0;
  while(start < message.length()) {
    int end = start + 20;
    if(end > message.length()) end = message.length();
    
    String line = message.substring(start, end);
    addToLcdBuffer(line);
    
    start = end;
    if(start < message.length() && message.charAt(start) == ' ') {
      start++; // Evita spazi all'inizio riga
    }
    
    delay(50); // Piccola pausa per leggibilità
  }
}

// Versione modificata di Serial.println()
void serialLcdPrintln(String message) {
  serialLcdPrint(message + "\n");
}

// Accensione modulo GSM
void PowerOn(int PowerKey) {
  uint8_t answer = 0;
  
  serialLcdPrintln("Avvio GSM...");
  
  // Configurazioni iniziali
  serialLcdPrint("Inizializzazione...");
  Serial1.println("AT+CSCS=\"IRA\"");
  delay(200);
  Serial1.println("AT+CPMS=\"SM\",\"SM\",\"SM\"");
  delay(200);
  Serial1.println("AT+CLIP=1");
  delay(200);
  Serial1.println("AT+CMGF=1");
  delay(200);
  Serial1.println("AT+CSCS=\"GSM\"");
  delay(200);
  Serial1.println("AT+CMGD=1,4");
  delay(1000);

  // Verifica modulo
  answer = sendATcommand("AT", "OK", 1000);
  
  if(answer == 0) {
    serialLcdPrintln("Modulo non risponde");
    serialLcdPrintln("Tentativo riavvio...");
    digitalWrite(PowerKey, HIGH);
    delay(500);
    digitalWrite(PowerKey, LOW);

    int MaxRetry = 10;
    while(answer == 0 && MaxRetry > 0) {
      answer = sendATcommand("AT", "OK", 1000);
      serialLcdPrint(".");
      
      lcd.setCursor(0, 1);
      lcd.print("Tentativo " + String(11 - MaxRetry));
      
      delay(1000);
      MaxRetry--;
    }
  }
  else {
    serialLcdPrintln("Modulo GSM pronto");
  }

  if(answer == 0) {
    serialLcdPrintln("Errore modulo GSM!");
    serialLcdPrintln("Controlla collegamenti");
    delay(3000);
    return;
  }

  // Attesa segnale
  serialLcdPrint("Ricerca segnale...");
  int MaxRetry = 20;
  
  while((sendATcommand("AT+CREG?", "+CREG: 0,1", 500) || 
        sendATcommand("AT+CREG?", "+CREG: 0,5", 500)) == 0 && MaxRetry > 0) {
    serialLcdPrint(".");
    delay(500);
    MaxRetry--;
  }
  
  if(MaxRetry > 0) {
    serialLcdPrintln("Segnale trovato");
    CheckSignalQuality();
    CheckOperator();
  }
  else {
    serialLcdPrintln("Nessun segnale!");
    serialLcdPrintln("Controlla SIM");
    delay(3000);
  }

  // Configurazioni finali
  Serial1.println("AT+CMGF=1");
  delay(500);
  Serial1.println("AT+CNMI=2,1,0,0,0");
  delay(500);
}

// Effettua chiamata
bool PhoneCall(const char* PhoneNumber) {
  char aux_str[30];
  serialLcdPrintln("Chiamata a: " + String(PhoneNumber));
  
  sprintf(aux_str, "ATD%s;", PhoneNumber);
  sendATcommand(aux_str, "OK", 5000);
  
  delay(10000);
  Serial1.println("AT+CHUP");
  serialLcdPrintln("Chiamata terminata");
  return true;
}

// Invia SMS
bool SendingShortMessage(const char* PhoneNumber, const char* Message) {
  uint8_t answer = 0;
  char aux_string[30];
  
  serialLcdPrintln("Invio SMS a: " + String(PhoneNumber));
  
  answer = sendATcommand("AT+CMGF=1", "OK", 1000);
  if(answer == 0) {
    serialLcdPrintln("Errore modalita testo");
    return false;
  }
  
  sprintf(aux_string, "AT+CMGS=\"%s\"", PhoneNumber);
  answer = sendATcommand(aux_string, ">", 3000);
  
  if(answer == 1) {
    Serial1.println(Message);
    Serial1.write(0x1A);
    answer = sendATcommand("", "OK", 20000);
    
    if(answer == 1) {
      serialLcdPrintln("SMS inviato!");
      return true;
    }
    else {
      serialLcdPrintln("Errore invio SMS");
      return false;
    }
  }
  else {
    serialLcdPrintln("Errore preparazione SMS");
    return false;
  }
}

// Invia comando AT
uint8_t sendATcommand(const char* ATcommand, const char* expected_answer, unsigned int timeout) {
  uint8_t x = 0, answer = 0;
  char response[100];
  unsigned long previous;
  
  memset(response, '\0', 100);
  delay(100);
  
  while(Serial1.available() > 0) Serial1.read();
  
  Serial1.println(ATcommand);
  x = 0;
  previous = millis();
  
  do {
    if(Serial1.available() != 0) {
      response[x] = Serial1.read();
      x++;
      
      if(strstr(response, expected_answer) != NULL) {
        answer = 1;
      }
    }
  } while((answer == 0) && ((millis() - previous) < timeout));
  
  return answer;
}

// Controlla qualità segnale
void CheckSignalQuality() {
  Serial1.println("AT+CSQ");
  delay(500);
  
  String response = "";
  while(Serial1.available()) {
    char c = Serial1.read();
    response += c;
  }
  
  int index = response.indexOf("+CSQ: ");
  if(index != -1) {
    int commaIndex = response.indexOf(",", index);
    String csqValue = response.substring(index + 6, commaIndex);
    int csq = csqValue.toInt();
    
    String signalMsg = "Segnale: " + String(csq) + "/31";
    serialLcdPrintln(signalMsg);
  }
}

// Controlla operatore
void CheckOperator() {
  Serial1.println("AT+COPS?");
  delay(500);
  
  String response = "";
  while(Serial1.available()) {
    char c = Serial1.read();
    response += c;
  }
  
  int quote1 = response.indexOf("\"");
  int quote2 = response.indexOf("\"", quote1 + 1);
  String operatorName = "Sconosciuto";
  
  if(quote1 != -1 && quote2 != -1) {
    operatorName = response.substring(quote1 + 1, quote2);
  }
  
  serialLcdPrintln("Operatore: " + operatorName);
}

// Test modulo GSM
void GSM_Test() {
  serialLcdPrintln("--- TEST GSM ---");
  
  uint8_t answer = sendATcommand("AT", "OK", 1000);
  if(answer == 0) {
    serialLcdPrintln("Modulo non risponde");
    return;
  }
  
  serialLcdPrintln("Modulo risponde OK");
  CheckSignalQuality();
  CheckOperator();
  
  Serial1.println("AT+CMGF=1");
  delay(300);
  Serial1.println("AT+CSCS=\"GSM\"");
  delay(300);
  
  serialLcdPrintln("Test completato");
}

// Leggi SMS
void ReadSMS() {
  Serial1.println("AT+CMGL=\"REC UNREAD\"");
  unsigned long startTime = millis();
  String response = "";
  
  while(millis() - startTime < 2000) {
    while(Serial1.available()) {
      char c = Serial1.read();
      response += c;
    }
  }
  
  if(response.indexOf("+CMGL:") != -1) {
    serialLcdPrintln("Nuovo SMS ricevuto:");
    ParseSMSBuffer(response);
  }
}

// Analizza SMS ricevuti
void ParseSMSBuffer(String smsText) {
  if(smsText.indexOf("+CMGL:") == -1) return;
  
  int indexCMGL = smsText.indexOf("+CMGL:");
  while(indexCMGL != -1) {
    int indexStart = smsText.indexOf(" ", indexCMGL) + 1;
    int indexEnd = smsText.indexOf(",", indexStart);
    String smsIndexStr = smsText.substring(indexStart, indexEnd);
    smsIndexStr.trim();
    int smsIndex = smsIndexStr.toInt();
    
    int smsStart = smsText.indexOf("\n", indexCMGL) + 1;
    int smsEnd = smsText.indexOf("\n", smsStart);
    if(smsEnd == -1) break;
    
    String smsContent = smsText.substring(smsStart, smsEnd);
    ProcessSMS(smsContent, "+CMGL:");
    
    String deleteCommand = "AT+CMGD=" + String(smsIndex);
    Serial1.println(deleteCommand);
    delay(300);
    
    indexCMGL = smsText.indexOf("+CMGL:", smsEnd);
  }
}

// Processa SMS
void ProcessSMS(String smsText, String prefix) {
  tone(BUZZERPIN, 1000, 200);
  
  lcd.clear();
  serialLcdPrintln("SMS Ricevuto!");
  
  // Estrai numero mittente
  int startIdx = smsText.indexOf(prefix + " \"") + prefix.length() + 2;
  int endIdx = smsText.indexOf("\"", startIdx);
  
  if(startIdx >= 0 && endIdx >= 0 && endIdx > startIdx) {
    String sender = smsText.substring(startIdx, endIdx);
    serialLcdPrintln("Da: " + sender);
  }

  // Estrai testo SMS
  int smsStart = smsText.indexOf("\n", smsText.indexOf(prefix)) + 1;
  String smsContent = "";
  
  if(smsStart > 0) {
    smsContent = smsText.substring(smsStart);
    smsContent.trim();
  }
  
  // Mostra testo SMS
  serialLcdPrintln("Contenuto:");
  serialLcdPrintln(smsContent.substring(0, 20));
  if(smsContent.length() > 20) {
    serialLcdPrintln(smsContent.substring(20, 40));
  }

  // Comandi da SMS
  if(smsContent.indexOf("ATTIVA") != -1) {
    ACTIVE = true;
    AlarmStatus = "ATTIVO";
    SMSSENT = false;
    serialLcdPrintln("Allarme attivato!");
    delay(2000);
  }
  
  if(smsContent.indexOf("DISATTIVA") != -1) {
    AlarmOFF();
    serialLcdPrintln("Allarme disattivato!");
    delay(2000);
  }
  
  if(smsContent.indexOf("STATO") != -1) {
    String sirenaStatus = (digitalRead(EXTRELAY) == LOW) ? "ACCESA" : "SPENTA";
    String smsResponse = "STATO ALLARME:\n";
    smsResponse += (ACTIVE ? "ATTIVO" : "DISATTIVO");
    smsResponse += "\nSeriale: " + SERIALE;
    smsResponse += "\nSirena: " + sirenaStatus;
    
    SendingShortMessage(PHONENUMBER.c_str(), smsResponse.c_str());
    serialLcdPrintln("Stato inviato!");
    delay(2000);
  }
}

// Controlla chiamate in ingresso
void CheckIncomingCall() {
  static String callBuffer = "";
  
  while(Serial1.available()) {
    char c = Serial1.read();
    callBuffer += c;
    
    if(c == '\n') {
      if(callBuffer.indexOf("RING") != -1) {
        isRinging = true;
        lastRingTime = millis();
        serialLcdPrintln("Chiamata in arrivo!");
      }
      
      int clipIndex = callBuffer.indexOf("+CLIP:");
      if(clipIndex != -1) {
        int start = callBuffer.indexOf("\"", clipIndex) + 1;
        int end = callBuffer.indexOf("\"", start);
        
        if(start > 0 && end > start) {
          lastCaller = callBuffer.substring(start, end);
          isRinging = true;
          lastRingTime = millis();
          serialLcdPrintln("Chiamata da:");
          serialLcdPrintln(lastCaller);
        }
      }
      
      callBuffer = "";
    }
  }
  
  if(isRinging && millis() - lastRingTime > 4000) {
    isRinging = false;
    lastCaller = "";
    lcd.clear();
  }
}