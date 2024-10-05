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

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(CAN_INT_PIN, INPUT);
  Serial.begin(115200);

  // Initialize MCP2515 with 8 MHz clock frequency
  if (CAN.begin(CAN_125KBPS, 8000000) == CAN_OK) {  
    // Attach interrupt for receiving messages
    attachInterrupt(digitalPinToInterrupt(CAN_INT_PIN), handleInterrupt, FALLING);
  } else {
    Serial.println("Error Initializing MCP2515...");
    while (1);
  }
}

void loop() {
  // Your main loop code here
}

void handleInterrupt() {
  // This function is called when a message is received
  unsigned long id;
  byte len;
  byte buf[8];

  // Read the CAN message (no parameters)
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

    // Original conditions
    if (len == 5 && buf[0] == 0x98 && buf[1] == 0x00 && buf[2] == 0x00 && buf[3] == 0x00 && buf[4] == 0x02) {
      digitalWrite(LED_PIN, HIGH);  // Turn on LED
      Serial.println("LED ON");
      
    } else if (len == 5 && buf[0] == 0x99 && buf[1] == 0x00 && buf[2] == 0x00 && buf[3] == 0x00 && buf[4] == 0x02) {
      digitalWrite(LED_PIN, LOW);   // Turn off LED
      Serial.println("LED OFF");
    }

  } else {
    Serial.println("Error reading CAN message");
  }
}
