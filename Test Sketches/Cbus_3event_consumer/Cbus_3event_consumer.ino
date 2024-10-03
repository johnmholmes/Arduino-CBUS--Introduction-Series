/*
This sketch was part of my beginners journey into CBUS. While it is not the best way of coding,
as I am not a qualified programmer, just code for fun.

More details can be found in the GitHub Wiki

https://github.com/johnmholmes/Arduino-CBUS--Introduction-Series/wiki

This allowed me to see that the two units would work and I was able to desplay this in JMRI.
*/

#include <mcp_can.h>
#include <SPI.h>

const int LED_PIN = 6;
const int SPI_CS_PIN = 10;  // Chip Select pin for MCP2515
const int CAN_INT_PIN = 2;  // Interrupt pin for MCP2515
MCP_CAN CAN(SPI_CS_PIN);

enum LedState {
  LED_OFF,
  LED_ON,
  LED_FLASH
};

LedState currentLedState = LED_OFF;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(CAN_INT_PIN, INPUT);
  Serial.begin(115200);

  // Initialize MCP2515
  if (CAN.begin(CAN_125KBPS, 8000000) == CAN_OK) { 
    
    // Attach interrupt for receiving messages
    attachInterrupt(digitalPinToInterrupt(CAN_INT_PIN), handleInterrupt, FALLING);
  } else {
    Serial.println("Error Initializing MCP2515...");
    while (1);
  }
  /*
    OCR1A = Desired interval×Clock Frequency / Prescaler - 1

    OCR1A = (0.5 x 16000000/ 1024) - 1 round up or down for an int vaalue

    OCR1A = 7812
 
  */
  // Set up a timer interrupt to handle LED_FLASH
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 7812; // Set the compare value for 1/2 second (assuming 16MHz clock)31250 for aprox 1 second
  TCCR1B |= (1 << WGM12); // Configure timer 1 for CTC mode
  TCCR1B |= (1 << CS12) | (1 << CS10); // Set prescaler to 1024 and start the timer
  TIMSK1 |= (1 << OCIE1A); // Enable timer compare interrupt
  interrupts();
}

void loop() {
  // Your main loop code here
  // You can add additional non-blocking code here if needed.
}

void handleInterrupt() {
  // This function is called when a message is received
  unsigned long id;
  byte len;
  byte buf[8];

  // Read the CAN message
  if (CAN.readMsgBuf(&len, buf) == CAN_OK) {  // Read message
    id = CAN.getCanId();  // Retrieve the message ID
    Serial.print("Received message. ID: 0x");
    Serial.print(id, HEX);
    Serial.print("  Data: ");
    for (int i = 0; i < len; i++) {
      Serial.print(buf[i], HEX);
      Serial.print(" ");
    }
    Serial.println();

    // Debug print statements
    Serial.print("len: ");
    Serial.println(len);

    for (int i = 0; i < len; i++) {
      Serial.print("buf[");
      Serial.print(i);
      Serial.print("]: ");
      Serial.println(buf[i], HEX);
    }

    // Check for LED state messages
    if (len == 5 && buf[0] == 0x98 && buf[1] == 0x00 && buf[2] == 0x00 && buf[3] == 0x00 && buf[4] == 0x03) {
      setLedState(LED_OFF);
    } else if (len == 5 && buf[0] == 0x99 && buf[1] == 0x00 && buf[2] == 0x00 && buf[3] == 0x00 && buf[4] == 0x02) {
      setLedState(LED_ON);
    } else if (len == 5 && buf[0] == 0x98 && buf[1] == 0x00 && buf[2] == 0x00 && buf[3] == 0x00 && buf[4] == 0x02) {
      setLedState(LED_FLASH);
    }
  } else {
    Serial.println("Error reading CAN message");
  }
}

void setLedState(LedState newState) {
  if (currentLedState != newState) {
    switch (newState) {
      case LED_OFF:
        digitalWrite(LED_PIN, LOW);
        Serial.println("LED OFF");
        break;
      case LED_ON:
        digitalWrite(LED_PIN, HIGH);
        Serial.println("LED ON");
        break;
      case LED_FLASH:
        // Do nothing here; the timer interrupt will handle flashing
        break;
    }
    currentLedState = newState;
  }
}

ISR(TIMER1_COMPA_vect) {
  if (currentLedState == LED_FLASH) {
    static int flashCount = 0;
    if (flashCount < 150) {
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
      Serial.println("flashing");
      flashCount++;
    } else {
      flashCount = 0;
      digitalWrite(LED_PIN, LOW); // Ensure the LED is turned off after flashing
      currentLedState = LED_OFF;
      Serial.println("LED FLASH");
    }
  }
}
