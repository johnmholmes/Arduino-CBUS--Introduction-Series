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
  // Initialize MCP2515 with 8 MHz clock frequency
  if (CAN.begin(CAN_125KBPS, 8000000) == CAN_OK) {  // Replaced MCP_8MHZ with 8000000
    // No need to explicitly set the operating mode to normal
  } else {
    // Handle initialization error
    while (1);
  }
}

void loop() {
  // Send first CAN message
  unsigned char data1[] = {0x99, 0x00, 0x00, 0x00, 0x02};
  sendCANMessage(0b01110000010, data1, sizeof(data1));

  // Wait for 2 seconds
  delay(1000);

  // Send second CAN message
  unsigned char data2[] = {0x98, 0x00, 0x00, 0x00, 0x02};
  sendCANMessage(0b01110000010, data2, sizeof(data2));

  // Wait for 2seconds before repeating the loop
  delay(2000);
}

void sendCANMessage(unsigned long canId, const unsigned char* data, byte len) {
  // Create a CAN message
  CAN.sendMsgBuf(canId, 0, len, data);
}
