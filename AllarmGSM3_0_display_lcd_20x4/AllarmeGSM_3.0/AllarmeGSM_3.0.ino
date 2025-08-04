/*
 * Allarme GSM con LCD 20x4 I2C
 * Versione: 3.0
 * Autore: [Il Tuo Nome]
 * Data: [Data]
 * 
 * Descrizione: Sistema di allarme GSM con ricevitore/trasmettitore RF433
 * e interfaccia su LCD 20x4 I2C
 */

// Includiamo le librerie necessarie
#include <LiquidCrystal_I2C.h>  // Per il display LCD I2C
#include <RCSwitch.h>           // Per la comunicazione RF433
#include <EEPROM.h>             // Per la memoria persistente
#include <SPI.h>                // Per comunicazione SPI (non usata direttamente)
#include <Wire.h>               // Per comunicazione I2C

// Inizializzazione oggetti
LiquidCrystal_I2C lcd(0x27, 20, 4);  // LCD I2C all'indirizzo 0x27, 20 colonne, 4 righe
RCSwitch rfreceiver = RCSwitch();     // Ricevitore RF433
RCSwitch rftrasmitter = RCSwitch();   // Trasmettitore RF433

// Definizione costanti per la tastiera
const char NO_KEY = 0;
const int numRows = 4;        // Numero righe tastiera
const int numCols = 3;        // Numero colonne tastiera
const int debounceTime = 20;  // Tempo di debounce in ms

// Mappatura tasti tastiera
const char keymap[numRows][numCols] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};

// Pin collegati alla tastiera
const int rowPins[numRows] = { 24, 26, 28, 30 };  // Pin righe
const int colPins[numCols] = { 25, 27, 29 };      // Pin colonne

// Definizione pin
const int POWERKEY = 4;        // Pin per accensione modulo GSM
const int FLIGHTMODE = 3;      // Pin modalità aereo GSM
const int GSMRELAY = 11;       // Pin relè GSM
const int RFDATA = 2;          // Pin dati RF433 (Interrupt 0 su Mega)
const int EXTALARM = 13;       // Pin allarme esterno
const int EXTRELAY = 12;       // Pin relè uscita
const int BUZZERPIN = 5;       // Pin buzzer
const int LUMINOSITY = 6;      // Pin luminosità (non usato con LCD)
const int RFTRASMISSION = 14;  // Pin trasmissione RF433
const int RFINTERRUPT = 0;     // Interrupt 0 Arduino Mega = PIN 2

// Variabili stato sistema
bool BuzzerActive = false;     // Stato buzzer attivo/passivo
bool DispChange = false;       // Flag cambio display
bool ActivSound = false;       // Flag sirena attiva
bool ACTIVE = false;           // Allarme attivo/disattivo
bool ALARM = false;            // Stato allarme
bool SMSSENT = false;          // Flag SMS inviato

// Variabili temporali
unsigned long CronoSoundBeg = 0;  // Inizio attivazione sirena
unsigned long CronoSoundTime = 0;  // Durata attivazione sirena
unsigned long lastSMSPoll = 0;     // Ultimo polling SMS

// Variabili RF
int RFSensors = 0;             // Numero sensori RF registrati
int RFValues[20] = { 999 };    // Codici sensori RF

// Versione firmware
const int VERSION = 119;       // Versione firmware per controllo EEPROM

// Strutture EEPROM
struct EEPromCRC {
  int CRC;
};

struct EEPromData {
  char SERIALN[7];  // Numero seriale (6 caratteri + terminatore)
  char PIN[4];      // PIN (4 caratteri)
  char PHONE[12];   // Numero telefono 1
  char PHONE2[12];  // Numero telefono 2
  char PHONE3[12];  // Numero telefono 3
};

struct EEPromSensors {
  int NUMBER;      // Numero sensori
  int SENSOR[20];  // Codici sensori
};

// Indirizzi EEPROM
const int EEP_SERIAL = 64;     // Indirizzo numero seriale
const int EEP_PIN = 100;       // Indirizzo PIN
const int EEP_PHONE = 128;     // Indirizzo telefono 1
const int EEP_PHONE2 = 152;    // Indirizzo telefono 2
const int EEP_PHONE3 = 176;    // Indirizzo telefono 3
const int EEP_NUMSENS = 200;   // Indirizzo numero sensori
const int EEP_STASENS = 250;   // Indirizzo inizio codici sensori
const int EEP_INCSENS = 50;    // Incremento tra codici sensori

// Testi menu
String MenuLabel[] = { 
  "Stato rete", "Attiva Allarme", "Modifica Seriale", "Modifica PIN", "Modifica Telefono", 
  "Associa Sensori", "Elimina Sensori", "Trasmetti Codice", "Test I/O",
  "Modifica Tel_2", "Modifica Tel_3", "Esci", "-", "-" 
};

// Variabili sistema
int MenuPosition = 0;          // Posizione corrente nel menu
const int MenuMaxPos = 12;    // Posizione massima menu
String SERIALE = "L000001";    // Numero seriale default
String PASSWORD = "1234";      // Password default
String PHONENUMBER = "+393450726127";  // Telefono 1 default
String PHONENUMBER2 = "+393450726127"; // Telefono 2 default
String PHONENUMBER3 = "+393450726127"; // Telefono 3 default
String SMSTEXT = "";           // Testo SMS allarme
String SMSTEXTON = "";         // Testo SMS attivazione
String SMSTEXTOFF = "";        // Testo SMS disattivazione
String Input_Keys = "";        // Tasti premuti
String STATUS = "NORMAL";      // Stato corrente
String OLDSTATUS = "NORMAL";   // Stato precedente
String AlarmStatus = "DISATTIVO";  // Stato allarme
String AlarmActive = "NORMALE";    // Attività allarme
int ConfigPos = 1;             // Posizione configurazione

// Prototipi delle funzioni
void ReadDataToEEProm();
void ReadSensorsToEEProm();
void WriteDataToEEProm();
void WriteSensorsToEEProm();
void initKey();
void Buzzer();
void LCDNormal();
void CheckKeypadPassword();
void LCDConfig();
//void LCDConfig_1();
void CheckKeypadMenu();
bool CheckRFData(int value);
void AlarmOFF();
void AlarmON();
void SIRENAOFF();

void setup() {
  // Inizializzazione seriale
  Serial.begin(115200);
  Serial1.begin(115200);  // Serial1 per SIM7600 (TX1/RX1 su Mega)
  
  // Inizializzazione LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Inizializzazione...");
  
  // Configurazione pin
  pinMode(EXTALARM, INPUT_PULLUP); 
  pinMode(EXTRELAY, OUTPUT);
  pinMode(BUZZERPIN, OUTPUT);
  pinMode(POWERKEY, OUTPUT);
  pinMode(FLIGHTMODE, OUTPUT);
  pinMode(LUMINOSITY, OUTPUT);
  pinMode(GSMRELAY, OUTPUT);
  
  // Stato iniziale pin
  digitalWrite(EXTRELAY, HIGH);
  digitalWrite(BUZZERPIN, LOW);
  digitalWrite(POWERKEY, LOW);
  digitalWrite(FLIGHTMODE, LOW);
  digitalWrite(GSMRELAY, HIGH);
  
  // Verifica/carica dati EEPROM
  EEPromCRC CHECK;
  EEPROM.get(0, CHECK);
  if (CHECK.CRC == VERSION) {
    ReadDataToEEProm();
    ReadSensorsToEEProm();
    Serial.println("EEPROM VERSION IS CORRECT");
  } else {
    EEPromCRC CHECK = { VERSION };
    EEPROM.put(0, CHECK);
    WriteDataToEEProm();
    WriteSensorsToEEProm();
    Serial.println("EEPROM VERSION INITIALIZZED");
  }
  
  // Inizializza testi SMS
  SMSTEXT = "ALLARME RILEVATO DEVICE " + SERIALE;
  SMSTEXTON = "ATTIVATO ALLARME DEVICE " + SERIALE;
  SMSTEXTOFF = "DISATTIVATO ALLARME DEVICE " + SERIALE;
  
  // Inizializzazione RF433
  rfreceiver.enableReceive(RFINTERRUPT);
  rftrasmitter.enableTransmit(RFTRASMISSION);
  
  // Inizializzazione tastiera
  initKey();
  
  // Segnale acustico iniziale
  Buzzer();
  
  // Mostra schermata iniziale
  LCDNormal();
  
  Serial.println("INIT COMPLETED");
  lastSMSPoll = millis();  // Inizializza timer polling SMS
}

void loop() {
  // Controlla chiamate in ingresso
  
  /*
  CheckIncomingCall();
  
  // Polling SMS ogni 10 secondi
  if (millis() - lastSMSPoll > 10000) {
    lastSMSPoll = millis();
    ReadSMS();
  }
  */

  // Gestione cambio stato
  if (STATUS != OLDSTATUS) {
    Serial.println("STATUS: " + STATUS);
    OLDSTATUS = STATUS;
    lcd.clear();
  }
  
  // Macchina a stati principale
  if (STATUS == "NORMAL") {
    LCDNormal();
    CheckKeypadPassword();
  } 
  else if (STATUS == "CONFIG") {
    analogWrite(LUMINOSITY, 100);
    LCDConfig();
    //if (MenuPosition <= 4) LCDConfig();
    //else LCDConfig_1();
    CheckKeypadMenu();
  }
  /*
  // Gestione allarme attivo
  if (ACTIVE == true) {
    // Controlla sensore esterno
    if (digitalRead(EXTALARM) == LOW) {
      Serial.print("ALARM FROM EXTERNAL PIN");
      ALARM = true;
    }
    */
    
    // Controlla ricevitore RF433
    if (rfreceiver.available()) {
      int getrf = int(rfreceiver.getReceivedValue());
      if (CheckRFData(getrf)) {
        Serial.println("ALARM FROM RF433 SENSOR");
        ALARM = true;
      } else {
        String codet = SERIALE.substring(1, 6);
        int code = codet.toInt();
        if (getrf == code) {
          AlarmOFF();
        } else {
          Serial.println("DATA FROM RF433 NOT REGISTERED: " + String(rfreceiver.getReceivedValue()));
        }
      }
      rfreceiver.resetAvailable();
    }
    
    // Gestione allarme attivato
    if (ALARM == true) {
      AlarmON();
      
      // Gestione timeout sirena (5 minuti)
      if(ActivSound == true) {
        Serial.println("SIRENA ATTIVA");
        CronoSoundBeg = millis();
        ActivSound = false;
      }
      
      CronoSoundTime = millis() - CronoSoundBeg;
      if(CronoSoundTime >= 300000) {
        Serial.println("SIRENA DISATTIVA TEMPO TRASCORSO: " + String(CronoSoundTime/60000) + " MINUTI");
        SIRENAOFF();
      }
    }
  else {
    EnableAlarmRF();
  }
}
void(*RESET)(void) = 0;