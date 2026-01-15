# GSM Alarm (Arduino Mega)

Home alarm system based on Arduino Mega 2560 with a 20x4 I2C LCD, a 4x3 keypad, 433 MHz RF sensors, and a GSM module (SIM7600) to send SMS notifications. Configuration (device serial, PIN, phone number) is stored in EEPROM. The LCD menu lets you arm/disarm, manage RF sensors, and run tests.

![Finished Project](Componenti_Allarme/Immagini_Componenti/Contenitore_Esterno/Progetto_Finito_Frontale.jpeg)

## Key Features
- 20x4 I2C LCD (address 0x27) for status and menu.
- 4x3 keypad for PIN entry, navigation, and configuration.
- 433 MHz RF sensors: pair, transmit code, and delete.
- Configuration stored in EEPROM with version check.
- SMS sending via SIM7600 using AT commands on `Serial1`.
- Arm/disarm via PIN or RF code.
- External siren through relay; built-in buzzer.
- Backlight brightness via PWM.

## Required Hardware
- Arduino Mega 2560 (FQBN: arduino:avr:mega).

  ![Arduino Mega 2560](Componenti_Allarme/Immagini_Componenti/Arduino_Mega.jpg)

- SIM7600 GSM module connected to `Serial1` (TX1/RX1) and `POWERKEY` pin.

  ![SIM7600 Module](Componenti_Allarme/Immagini_Componenti/Modulo%20Sim%207600E%20Fronte.png)

- 20x4 I2C LCD (PCF8574) at address 0x27.

  ![LCD Display I2C](Componenti_Allarme/Immagini_Componenti/Display%20LCD%20I2c_%20Fronte.jpg)

- RF 433 MHz receiver + transmitter compatible with `RCSwitch`.

  ![RF 433 MHz](Componenti_Allarme/Immagini_Componenti/RF%20433MHZ.jpg) ![RF Antenna](Componenti_Allarme/Immagini_Componenti/Antenna_RF433MHZ.jpg)

- 4x3 matrix keypad.

  ![4x3 Keypad](Componenti_Allarme/Immagini_Componenti/Tastiera.jpg)

- Piezo buzzer 5V.

  ![Buzzer](Componenti_Allarme/Immagini_Componenti/Buzzer_5V.png)

- Relay module for external siren (4 or 2 channels).

  ![4-Channel Relay](Componenti_Allarme/Immagini_Componenti/Modulo_Relé_4_canali.jpg)

- RF 433 MHz sensors (PIR, door open, vibration, remote).

  ![RF Sensors](Componenti_Allarme/Immagini_Componenti/Pir_433MHZ.jpg) ![Door Sensor](Componenti_Allarme/Immagini_Componenti/Sensore_Apertura_433MHZ.jpg)

- Mini breadboard and Dupont wires.

## Pinout (main)
- `POWERKEY = 4`
- `FLIGHTMODE = 3`
- `GSMRELAY = 11` (power control for GSM module)
- `RFDATA = 2` and `RFINTERRUPT = 0` (Interrupt 0 on Mega = PIN 2)
- `EXTALARM = 13` (external alarm input, optional)
- `EXTRELAY = 12` (siren output)
- `BUZZERPIN = 5`
- `LUMINOSITY = 6` (PWM backlight)
- `RFTRASMISSION = 14` (RF transmitter)
- Keypad rows: `24, 26, 28, 30`; columns: `25, 27, 29`
- LCD I2C: address 0x27 on I2C bus (Mega: 20/SDA, 21/SCL)

## Operation & States
- Main states: `NORMAL` (shows status, allows PIN entry) and `CONFIG` (configuration menu).
- Arming the alarm:
  - Via RF: receiving the device code (derived from serial, e.g., `L000001` → `000001`).
  - Via menu: "Attiva Allarme" with a countdown (about 60s in RF flow and ~30s in some routines).
- Disarming: enter PIN and confirm with `#`.
- Notifications: when the alarm triggers, an SMS is sent to the configured number; optional call logic exists (commented).

## LCD Menu
Labels defined in [AllarmeGSM.ino](AllarmeGSM.ino):
- Attiva Allarme (Arm Alarm)
- Modifica Seriale (Change Serial)
- Modifica PIN (Change PIN)
- Modifica Telefono (Change Phone)
- Associa Sensori (Pair Sensors)
- Elimina Sensori (Delete Sensors)
- Trasmetti Codice (Transmit Code)
- Test I/O
- Esci (Exit)

Navigation: `*` scroll, `#` confirm. Operational functions are implemented in [menu.ino](menu.ino) and [config.ino](config.ino).

### LCD Menu Screenshots (examples)
![Menu 1](Componenti_Allarme/Immagini_Componenti/Display_Options/Menu1.png)
![Menu 2](Componenti_Allarme/Immagini_Componenti/Display_Options/Menu2.png)
![Menu 3](Componenti_Allarme/Immagini_Componenti/Display_Options/Menu3.png)
![Menu 4](Componenti_Allarme/Immagini_Componenti/Display_Options/Menu4.png)
![Menu 5](Componenti_Allarme/Immagini_Componenti/Display_Options/Menu5.png)

## Assembly and Enclosure
The project includes a custom external enclosure with holes for cable routing and mounting points for components.

### Enclosure Views
| Front | Interior | Side |
|-------|----------|------|
| ![Front](Componenti_Allarme/Immagini_Componenti/Contenitore_Esterno/Frontale_Fissaggio.jpeg) | ![Interior](Componenti_Allarme/Immagini_Componenti/Contenitore_Esterno/Contenitore_Interno.jpg) | ![Side](Componenti_Allarme/Immagini_Componenti/Contenitore_Esterno/Laterale_Contenitore.jpeg) |

### Component Base
The circuit board base has been customized to house the Arduino Mega, RF modules, GSM module, and relay.

![Component Base Front](Componenti_Allarme/Immagini_Componenti/Base_Componenti_Fronte.png)
![Base with Components](Componenti_Allarme/Immagini_Componenti/Base_con_componenti.png)

## Files & Modules

The project is organized into multiple `.ino` files, each responsible for a specific functionality:

- **[AllarmeGSM.ino](AllarmeGSM.ino)**: setup/loop, global variables, state management, LCD/RF/keypad/EEPROM initialization. Main project file.
- **[gsm.ino](gsm.ino)**: SIM7600 control (`PowerOn()`, `sendATcommand()`, `SendingShortMessage()`), module power control via POWERKEY pin.
- **[eeprom.ino](eeprom.ino)**: read/write serial, PIN, phone, RF sensors in EEPROM; version check `VERSION = 119`.
- **[keyboard.ino](keyboard.ino)**: 4x3 keypad scanning, PIN handling, and state transitions (NORMAL ↔ CONFIG).
- **[lcd.ino](lcd.ino)**: 20x4 LCD display management with `LCDNormal()` (status) and `LCDConfig()` (menu) screens.
- **[menu.ino](menu.ino)**: configuration menu and actions (`ConfigOptions()`, `CheckKeypadMenu()`, `GetConfig()`).
- **[config.ino](config.ino)**: RF utilities and testing (`TestIO()`, `SendRFCode()`, `GetRFCode()`, `DelRFCode()`).
- **[utility.ino](utility.ino)**: buzzer, alarm logic (`AlarmON()`, `AlarmOFF()`), SMS for arm/disarm, RF enabling.
- **[sketch.json](sketch.json)**: Arduino Mega 2560 board configuration.
- **[launch.json](launch.json)**: VS Code placeholder.

## Libraries
- `LiquidCrystal_I2C`
- `RCSwitch`
- `EEPROM`
- `SPI`

Install via Arduino IDE Library Manager. The GSM module uses AT commands over `Serial1`.

## Initial Configuration
- Defaults: `SERIALE = L000001`, `PIN = 1234`, `PHONENUMBER = 3450726127`.
- Change via menu: "Modifica Seriale", "Modifica PIN", "Modifica Telefono".
- RF sensors: use "Associa Sensori" to register and "Elimina Sensori" to clear. Up to 20 sensors (`RFValues[20]`).

## Build & Upload
### Arduino IDE
- Open the project folder and select the board "Arduino Mega or Mega 2560".
- Install required libraries (LiquidCrystal_I2C, RCSwitch, EEPROM, SPI).
- Compile and upload the sketch.

### Arduino CLI (example)
Make sure `arduino-cli` is installed.

```bash
arduino-cli core update-index
arduino-cli core install arduino:avr
arduino-cli compile --fqbn arduino:avr:mega .
arduino-cli upload -p /dev/ttyACM0 --fqbn arduino:avr:mega .
```
Replace the serial port (`/dev/ttyACM0`) with your Mega's port.

## GSM (SIM7600) Notes
- Connect to Mega via `Serial1` (TX1/RX1) for AT commands.
- Power controlled by `GSMRELAY` and `POWERKEY` pin.
- Signal wait and SMS routines included (see [gsm.ino](gsm.ino)).

## Troubleshooting
- No GSM signal: check antenna, power, coverage; review `PowerOn()` flow.
- Blank LCD: check I2C address (0x27) and SDA/SCL wiring.
- RF sensors not detected: verify RF module power and correct pairing.
- Siren inactive: check `EXTRELAY` and relay wiring.

## Roadmap / Ideas
- HTTP/IoT integration (there are variables `serverName/ServerHost/ServerPath` not yet used in the main flow).
- Advanced state and event logging.

---
This README was generated from source analysis. I can add wiring diagrams or LCD screenshots if helpful.