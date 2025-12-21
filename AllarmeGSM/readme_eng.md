# GSM Alarm (Arduino Mega)

Home alarm system based on Arduino Mega 2560 with a 20x4 I2C LCD, a 4x3 keypad, 433 MHz RF sensors, and a GSM module (SIM7600) to send SMS notifications. Configuration (device serial, PIN, phone number) is stored in EEPROM. The LCD menu lets you arm/disarm, manage RF sensors, and run tests.

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
- SIM7600 GSM module connected to `Serial1` (TX1/RX1) and `POWERKEY` pin.
- 20x4 I2C LCD (PCF8574) at address 0x27.
- RF 433 MHz receiver + transmitter compatible with `RCSwitch`.
- 4x3 matrix keypad.
- Piezo buzzer.
- Relay for external siren.

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

## Files & Modules
- [AllarmeGSM.ino](AllarmeGSM.ino): setup/loop, global variables, state, LCD/RF/keypad/EEPROM initialization.
- [gsm.ino](gsm.ino): SIM7600 control (`PowerOn()`, `sendATcommand()`, `SendingShortMessage()`), module power control.
- [eeprom.ino](eeprom.ino): read/write serial, PIN, phone, RF sensors in EEPROM; version check `VERSION = 119`.
- [keyboard.ino](keyboard.ino): keypad scanning, PIN handling, and state transitions.
- [lcd.ino](lcd.ino): screens `LCDNormal()` and `LCDConfig()`.
- [menu.ino](menu.ino): configuration menu and actions (`ConfigOptions()`, `CheckKeypadMenu()`, `GetConfig()`).
- [config.ino](config.ino): RF and test helpers (`TestIO()`, `SendRFCode()`, `GetRFCode()`, `DelRFCode()`).
- [utility.ino](utility.ino): buzzer, alarm logic (`AlarmON()`, `AlarmOFF()`), SMS for arm/disarm, RF enabling.
- [sketch.json](sketch.json): target board Mega 2560.
- [launch.json](launch.json): VS Code placeholder.

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