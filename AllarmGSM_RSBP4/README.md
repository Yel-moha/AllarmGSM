# SIM7600X Raspberry Pi Demo

Esempi C++ per utilizzare il modem 4G SIM7600X HAT su Raspberry Pi tramite UART GPIO e la libreria `arduPi` (seriale/GPIO) più il driver `bcm2835`.

## Requisiti
- Raspberry Pi con UART abilitata (console seriale disabilitata) e accesso GPIO.
- HAT SIM7600X alimentato e collegato (il pin `POWERKEY` è mappato su GPIO6).
- SIM con traffico dati e APN valido; eventuale copertura GPS se si usa il fix satellitare.
- Toolchain C++ (g++), libreria `bcm2835` installata sul sistema (`bcm2835/` contiene i sorgenti per compilarla).

## Installazione libreria bcm2835
Se non è già presente nel sistema:
1. `cd SIM7600X/bcm2835`
2. `./configure && make`
3. `sudo make install`

## Compilazione demo principale
1. `cd SIM7600X`
2. `make`
   - Genera l'eseguibile `sim7600` partendo da [SIM7600X/main.cpp](SIM7600X/main.cpp), [SIM7600X/arduPi.cpp](SIM7600X/arduPi.cpp) e [SIM7600X/sim7x00.cpp](SIM7600X/sim7x00.cpp).
3. `make clean` per ripulire gli oggetti.

## Esecuzione
- Eseguire come root (necessario per GPIO/UART) dal path `SIM7600X`:
  - `sudo ./sim7600`
- Il flusso di default fa:
  - accensione del modem via `POWERKEY` (GPIO6),
  - interrogazione registrazione di rete (`AT+CREG?`/`AT+CGREG?`),
  - apertura PDP con APN configurato,
  - connessione TCP al server configurato, invio di "HELLO" e chiusura socket/rete.
- Assicurarsi che `/dev/ttyS0` sia libero e accessibile; la porta è configurabile in [SIM7600X/arduPi.cpp](SIM7600X/arduPi.cpp).

## Parametri da personalizzare
Modificare [SIM7600X/main.cpp](SIM7600X/main.cpp) prima di compilare:
- `phone_number`: numero per chiamate/SMS.
- `ftp_user_name`, `ftp_user_password`, `ftp_server`, `download_file_name`, `upload_file_name`.
- `APN`, `ServerIP`, `Port` per dati.
- Abilitare/disabilitare funzioni scommentando le chiamate in `setup()` (chiamate vocali, SMS, FTP, GPS, ecc.).

## Altri esempi
Cartelle dedicate contengono versioni minimali dei comandi (ognuna con Makefile):
- [SIM7600X/AT](SIM7600X/AT), [SIM7600X/SMS](SIM7600X/SMS), [SIM7600X/PhoneCall](SIM7600X/PhoneCall), [SIM7600X/FTP](SIM7600X/FTP), [SIM7600X/GPS](SIM7600X/GPS), [SIM7600X/TCP](SIM7600X/TCP).
- Per compilarne uno: `cd SIM7600X/<cartella> && make` quindi eseguire l'output generato.

## Note utili
- Richiede permessi di gruppo `dialout` o esecuzione con `sudo` per accedere alla seriale.
- Lasciare il modulo con antenna collegata; alcune operazioni (GPS) necessitano cielo libero.
- Il codice stampa i log delle risposte AT su stdout; usare un terminale per monitorare.
