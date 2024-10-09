/*
This sketch was provided by Phil Silver and will be tested after the 15 October 2024
*/
#include <SPI.h>    //required by the library
#include <MergCBUS.h>
#include <Message.h>
#include <EEPROM.h> //required by the library
#include <VarSpeedServo.h>

//Module definitions
#define NUM_SERVOS 1     //number of servos
#define VAR_PER_SERVO 3  //variables per servo - servo number if using an array, startangle, finish angle 

//CBUS definitions
#define GREEN_LED 4                  //merg green led port
#define YELLOW_LED 5                 //merg yellow led port
#define PUSH_BUTTON 6                //std merg push button
#define NODE_VARS 1                  //number of node variables.Servo speed
#define NODE_EVENTS 30               //max number of events
#define EVENTS_VARS VAR_PER_SERVO    //number of variables per event
#define DEVICE_NUMBERS NUM_SERVOS    //number of device numbers. each servo can be a device
#define SERVO_NUMBER_VAR 1
#define START_ANGLE_VAR 2            //var index for the start angle
#define END_ANGLE_VAR 3              //var index for the end angle


int servoNumber;
int speed;
int startAngle;
int finishAngle;


//create the merg object
MergCBUS cbus=MergCBUS(NODE_VARS,NODE_EVENTS,EVENTS_VARS,DEVICE_NUMBERS);


//servo controler
VarSpeedServo myservo;
byte servoPin = 9;


void setup(){


/********************************************************************************************/
//Configuration CBUS data for the node
/********************************************************************************************/

  cbus.getNodeId()->setNodeName("NANSERV",7);      	 	// node name shows in FCU when first detected set your own name for each module - max 7 characters
  cbus.getNodeId()->setModuleId(200);               	// module number - see cbusdefs
  cbus.getNodeId()->setManufacturerId(0xA5);        	// MERG code
  cbus.getNodeId()->setMinCodeVersion(0x62);      		// Version a
  cbus.getNodeId()->setMaxCodeVersion(1);           	// version 1
  cbus.getNodeId()->setProducerNode(true);          	// Produce Node - Set True if Producer Module
  cbus.getNodeId()->setConsumerNode(true);        		// Consumer Node = Set True if Consumer Module
  cbus.getNodeId()->setConsumeEvents(false);      	  // consume own event COE
  cbus.getNodeId()->setTransportType(1);            	// 1 CAN  2 ETHERNET 3 MiWi
  cbus.getNodeId()->setBetaRelease(1);             		// Beta Release set to zero for release version
  cbus.getNodeId()->setSuportBootLoading(false);      // Allways set false - no bootloading
  cbus.getNodeId()->setCpuManufacturer(2);            // 1 MICROCHIP  2  ATMEL  3 ARM


  cbus.setStdNN(999);                               // Node Number in SLIM Mode. The default is 0 for Consumers or 1 - 99 for Producers.
 

/********************************************************************************************/

/********************************************************************************************/
// Set ports and CAN Transport Layer
/********************************************************************************************/

  cbus.setLeds(GREEN_LED,YELLOW_LED);                 //set the led ports
  cbus.setPushButton(PUSH_BUTTON);                    //set the push button ports
  cbus.setUserHandlerFunction(&myUserFunc);           //function that implements the node logic when recieving events
  cbus.initCanBus(10,CAN_125KBPS,MCP_8MHz,10,200);    //initiate the transport layer. pin=10, rate=125Kbps,10 tries,200 millis between each try
  
  //Note the clock speed 8Mhz. If 16Mhz crystal fitted change above to MCP_16Mhz
/********************************************************************************************/
  
   myservo.attach(servoPin);  // attaches the servo on pin 9 to the servo object
  
}
  
/********************************************************************************************/
// Functions
/********************************************************************************************/

void moveServoOff(int startAngle, int speed){
         myservo.write(startAngle, speed);  
}


void moveServoOn( int finishAngle, int speed){
         myservo.write(finishAngle, speed);
}


//User defined CBUS function. Contains the module logic. Executed every time cbus.run() is called from loop
void myUserFunc(Message *msg,MergCBUS *mcbus){
   byte CBUSOpc = msg->getOpc();
   int speed=cbus.getNodeVar(1);

   if (mcbus->eventMatch()){  //The recived event has been taught this module
        //retrive the values entered when teaching the event to the modules these are stored on the module
        int ev1=mcbus->getEventVar(msg,1);  // select servo number if using an array 
        int ev2=mcbus->getEventVar(msg,2);  // start angle
        int ev3=mcbus->getEventVar(msg,3);  // finish angle

        
     if ((mcbus->isAccOn()==true)){  
          moveServoOn(ev3,speed);//Pass the ON Event to the event handler 
          } 

     if ((mcbus->isAccOff()==true)){  
          moveServoOff(ev2,speed);//Pass the OFF Event to the event handler 
          } 

            }
  }

void loop (){
  cbus.cbusRead();
  cbus.run();//do all logic
}




