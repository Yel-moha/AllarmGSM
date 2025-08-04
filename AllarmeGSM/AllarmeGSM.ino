#include <LiquidCrystal_I2C.h>  // LIQUIDCRYSTAL I2C
#include <RCSwitch.h>           // RCSWITCH
#include <EEPROM.h>
#include <SPI.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);  // set the LCD address to 0x27 for a 20 chars and 4 line display
RCSwitch rfreceiver = RCSwitch();
RCSwitch rftrasmitter = RCSwitch();

char NO_KEY = 0;
const int numRows = 4;        // numero di righe
const int numCols = 3;        // numero di colonne
const int debounceTime = 20;  // millisecondi necessari allo switch per diventare stabile
const char keymap[numRows][numCols] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};
const int rowPins[numRows] = { 24, 26, 28, 30 };  // Pin di arduino connessi ai pin 24, 26, 28, 30 delle righe del keypad
const int colPins[numCols] = { 25, 27, 29 };      // Pin di arduino connessi ai pin 25, 27, 29 delle colonne del keypad

String serverName = "https://service.rcproject.it/allarmegsm/data.php";
String ServerHost = "service.rcproject.it";
String ServerPath = "/allarmegsm/data.php";
String apiKeyValue = "tPmAT5Ab3j7F9";

int POWERKEY = 4;
int FLIGHTMODE = 3;
int GSMRELAY = 11;
int RFDATA = 2;
int EXTALARM = 13;
int EXTRELAY = 12;
int BUZZERPIN = 5;
int LUMINOSITY = 6;
int RFTRASMISSION = 14;
int RFINTERRUPT = 0;  // Interrupt 0 Arduino Mega = PIN 2
bool BuzzerActive = false;

int RFSensors = 0;
int RFValues[20] = { 999 };

int VERSION = 119;
struct EEPromCRC {
  int CRC;
};
struct EEPromData {
  char SERIALN[7];
  char PIN[4];
  char PHONE[12];
};
struct EEPromSensors {
  int NUMBER;
  int SENSOR[20];
};
int EEP_SERIAL = 64;
int EEP_PIN = 100;
int EEP_PHONE = 128;
int EEP_NUMSENS = 200;
int EEP_STASENS = 250;
int EEP_INCSENS = 50;

String MenuLabel[] = { "Attiva Allarme", "Modifica Seriale", "Modifica PIN", "Modifica Telefono", "Associa Sensori", "Elimina Sensori", "Trasmetti Codice", "Test I/O", "Esci", "-", "-" };
int MenuPosition = 0;
int MenuMaxPos = 8;
String SERIALE = "L000001";
String PASSWORD = "1234";
bool ACTIVE = false;
String PHONENUMBER = "3450726127";
String SMSTEXT = "ALLARME RILEVATO DEVICE L000001";
String SMSTEXTON = "ATTIVATO ALLARME DEVICE L000001";
String SMSTEXTOFF = "DISATTIVATO ALLARME DEVICE L000001";
String Input_Keys = "";
String STATUS = "NORMAL";
String OLDSTATUS = "NORMAL";
int ConfigPos = 1;
bool ALARM = false;
String AlarmStatus = "DISATTIVO";
String AlarmActive = "NORMALE";
bool SMSSENT = false;
uint8_t buf[11];
uint8_t buflen = sizeof(buf);
String Text = "";
String Phone = "";

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  pinMode(EXTALARM, INPUT_PULLUP);  //pinMode(EXTALARM, INPUT_PULLUP); 
  pinMode(EXTRELAY, OUTPUT);
  pinMode(BUZZERPIN, OUTPUT);
  pinMode(POWERKEY, OUTPUT);
  pinMode(FLIGHTMODE, OUTPUT);
  pinMode(LUMINOSITY, OUTPUT);
  pinMode(GSMRELAY, OUTPUT);
  digitalWrite(EXTRELAY, HIGH);
  digitalWrite(BUZZERPIN, LOW);
  digitalWrite(POWERKEY, LOW);
  digitalWrite(FLIGHTMODE, LOW);
  digitalWrite(GSMRELAY, HIGH);
  analogWrite(LUMINOSITY, 100);
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
  Serial.print("Serial Number: ");
  Serial.println(SERIALE);
  SMSTEXT = "ALLARME RILEVATO DEVICE " + SERIALE;
  lcd.init();
  lcd.backlight();
  lcd.clear();
  LCDNormal();
  rfreceiver.enableReceive(RFINTERRUPT);
  rftrasmitter.enableTransmit(RFTRASMISSION);
  initKey();
  Buzzer();
  Serial.println("INIT COMPLETED");
  analogWrite(LUMINOSITY, 0);
}

void loop() {
  if (STATUS != OLDSTATUS) {
    Serial.println("STATUS: " + STATUS);
    OLDSTATUS = STATUS;
    lcd.clear();
  }
  if (STATUS == "NORMAL") {
    analogWrite(LUMINOSITY, 0);
    LCDNormal();
    CheckKeypadPassword();
  }
  if (STATUS == "CONFIG") {
    analogWrite(LUMINOSITY, 100);
    LCDConfig();
    CheckKeypadMenu();
  }
  if (ACTIVE == true) {
    // GESTIONE ALLARME
   /* if (digitalRead(EXTALARM) == LOW) {
      Serial.print("ALARM FROM EXTERNAL PIN");
      ALARM = true;
    } */
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
    // ATTIVAZIONE/SPEGNIMENTO ALLARME
    if (ALARM == true) {
      AlarmON();
    }
  } else {
    EnableAlarmRF();
  }
}

void (*RESET)(void) = 0;
