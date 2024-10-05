/*
This sketch was part of my beginners journey into CBUS. While it is not the best way of coding,
as I am not a qualified programmer, just code for fun.

More details can be found in the GitHub Wiki

https://github.com/johnmholmes/Arduino-CBUS--Introduction-Series/wiki

This allowed me to see that the two units would work and I was able to desplay this in JMRI.
*/

#include <mcp_can.h>
#include <SPI.h>

const int SPI_CS_PIN = 10;  // Chip Select pin for MCP2515
const int CAN_INT_PIN = 2;  // Interrupt pin for MCP2515

MCP_CAN CAN(SPI_CS_PIN);

void setup() {

  Serial.begin(115200);

  // Initialize MCP2515
  if (CAN.begin(CAN_125KBPS, 8000000) == CAN_OK) {  
    Serial.println("MCP2515 Initialized Successfully!");
    
  } else {
    // Handle initialization error
    while (1);
  }
}

void loop() {
  // Send first CAN message
  unsigned char data1[] = {0x99, 0x00, 0x00, 0x00, 0x02};
  sendCANMessage(0b01110000010, data1, sizeof(data1));

  // Wait for 10 seconds
  delay(10000);

  // Send second CAN message
  unsigned char data2[] = {0x98, 0x00, 0x00, 0x00, 0x02};
  sendCANMessage(0b01110000010, data2, sizeof(data2));

  // Wait for 30 seconds
  delay(30000);

  // Send third CAN message this has a different event number 
  unsigned char data3[] = {0x98, 0x00, 0x00, 0x00, 0x03};
  sendCANMessage(0b01110000010, data3, sizeof(data3));

  // Wait for 10 seconds before repeating the loop
  delay(10000);
}

void sendCANMessage(unsigned long canId, const unsigned char* data, byte len) {
  // Create a CAN message
  CAN.sendMsgBuf(canId, 0, len, data);
}

