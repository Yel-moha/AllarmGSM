void PowerOn(int PowerKey) {
  uint8_t answer = 0;
  Serial.println("INITIALIZE SIM7600");
  answer = sendATcommand("AT", "OK", 1000);
  if (answer == 0) {
    Serial.print("SIM7600 SWITCH ON");
    pinMode(PowerKey, OUTPUT);
    digitalWrite(PowerKey, HIGH);
    delay(500);
    digitalWrite(PowerKey, LOW);
    int MaxRetry = 60;
    while (answer == 0) {
      answer = sendATcommand("AT", "OK", 1000);
      Serial.print(".");
      delay(1000);
      MaxRetry = MaxRetry - 1;
      if (MaxRetry < 1) { break; }
    }
    Serial.print("\n");
  } else {
    Serial.println("SIM7600 IS ON");
  }
  delay(1000);
  Serial.print("SIM7600 CHECKING FOR SIGNAL");
  int MaxRetry = 60;
  while ((sendATcommand("AT+CREG?", "+CREG: 0,1", 500) || sendATcommand("AT+CREG?", "+CREG: 0,5", 500)) == 0) {
    Serial.print(".");
    delay(500);
    MaxRetry = MaxRetry - 1;
    if (MaxRetry < 1) { break; }
  }
  Serial.print("\n");
  if (MaxRetry > 1) {
    Serial.println("SIM7600 IS READY WITH SIGNAL");
  } else {
    Serial.println("SIM7600 ERROR NO SIGNAL");
  }
}

bool PhoneCall(const char* PhoneNumber) {
  char aux_str[60];
  Serial.println("SIM7600 TRY CALL PHONE");
  sprintf(aux_str, "ATD%s;", PhoneNumber);
  sendATcommand(aux_str, "OK", 5000);
  delay(10000);                // RING FOR 10 SECONDS
  Serial1.println("AT+CHUP");  // disconnects the existing call
  Serial.println("SIM7600 CALL DISCONNECTED");
  return true;
}

bool SendingShortMessage(const char* PhoneNumber, const char* Message) {
  uint8_t answer = 0;
  char aux_string[60];
  Serial.println("SIM7600 SET SMS MODE TO TEXT");
  sendATcommand("AT+CMGF=1", "OK", 1000);  // sets the SMS mode to text
  Serial.println("SIM7600 TRY SEND SMS");
  sprintf(aux_string, "AT+CMGS=\"%s\"", PhoneNumber);
  answer = sendATcommand(aux_string, ">", 3000);  // send the SMS number
  if (answer == 1) {
    Serial1.println(Message);
    Serial1.write(0x1A);
    answer = sendATcommand("", "OK", 20000);
    if (answer == 1) {
      Serial.println("SIM7600 SMS SENT SUCCESFULLY");
      return true;
    } else {
      Serial.println("SIM7600 ERROR SEND SMS - NO ANSWER");
      return false;
    }
  } else {
    Serial.println("SIM7600 ERROR SEND SMS");
    return false;
  }
}

uint8_t sendATcommand(const char* ATcommand, const char* expected_answer, unsigned int timeout) {
  uint8_t x = 0, answer = 0;
  char response[100];
  unsigned long previous;
  memset(response, '\0', 100);  // Initialize the string
  delay(100);
  while (Serial1.available() > 0) Serial1.read();  // Clean the input buffer
  Serial1.println(ATcommand);                      // Send the AT command
  x = 0;
  previous = millis();
  do {                               // this loop waits for the answer
    if (Serial1.available() != 0) {  // if there are data in the UART input buffer, reads it and checks for the asnwer
      response[x] = Serial1.read();
      x++;
      if (strstr(response, expected_answer) != NULL) {  // check if the desired answer is in the response of the module
        answer = 1;
      }
    }
  } while ((answer == 0) && ((millis() - previous) < timeout));  // Waits for the asnwer with time out
  return answer;
}