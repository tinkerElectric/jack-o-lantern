/*
 * SPOOKY JACK O LANTERN
 * <https://www.facebook.com/tinkerelectric/>
 * 
 * John Missikos of Tinker Electric
 * Share yours online with the tag #TinkerElectric
 */

//External Libraries
#include <Adafruit_NeoPixel.h>  //Need to install this
#include <MD_YX5300.h>          //Need to install this
#include <SoftwareSerial.h>     //Included from Arduin

//Setup Pins
#define distanceSensorPin   A0  //Pin of the Distance Sensor
#define ledsPin             6   //Pin number your LEDs are connected to
#define NUMPIXELS           14  //Number of LEDs you have in your strip
// Connections for serial interface to the YX5300 module
const uint8_t ARDUINO_RX = 4;    // connect to TX of MP3 Player module
const uint8_t ARDUINO_TX = 5;    // connect to RX of MP3 Player module

//Setup of the MP3 Player
SoftwareSerial  MP3Stream(ARDUINO_RX, ARDUINO_TX);  // MP3 player serial stream for comms
const uint8_t PLAY_FOLDER = 1;   // tracks are all placed in this folder
MD_YX5300 mp3(MP3Stream);

//Setup of the LEDs
Adafruit_NeoPixel lights(NUMPIXELS, ledsPin, NEO_GRB + NEO_KHZ800);

//Configure what distances each taunt is triggered at, units are millimeters
//Adjust to suit your own
int Distance_Short = 350;
int Distance_Medium = 400;
int Distance_Far = 450;

//Initial variables for colours, leave these
int redColour = 0;
int greenColour = 0;
int blueColour = 0;

//State Machine, dont change this unless you understand
#define STATE_FAR 1
#define STATE_MEDIUM 2
#define STATE_SHORT 3
#define STATE_BOO 4
int state = 0;
int stateLast = state;

void setup () 
{
  Serial.begin (9600); //So we can view whats happening in serial monitor
  pinMode (distanceSensorPin, INPUT);

  lights.begin();
  lights.clear();

  //setup the sound system
  MP3Stream.begin(MD_YX5300::SERIAL_BPS);
  mp3.begin();
  mp3.setSynchronous(true);
  mp3.volume(20); //set volume %, can change this 0 to 100 %

}

//prototype
uint16_t getDistance ();

 
void loop () 
{

  uint16_t distance = getDistance();
  Serial.print ("Person Distance: ");
  Serial.print (distance);
  Serial.println (" mm");

  if(distance > Distance_Far){
    state = STATE_FAR;
    //Sensed FAR
    Serial.println ("LOITER MODE: Boil boil toil and trouble");
    redColour = 0;
    greenColour = 0;
    blueColour = 100;
    mp3.playSpecific(PLAY_FOLDER,1);    //play sound then delay
    delay(3000);

    
  }else if(distance > Distance_Medium){
    state = STATE_MEDIUM;
    //Sensed Medium
    Serial.println ("PROMPT MODE: Anyone there?");
    redColour = 50;
    greenColour = 50;
    blueColour = 0;
    if(state != stateLast){
      mp3.playSpecific(PLAY_FOLDER,2);
      delay(3000);
      mp3.playSpecific(PLAY_FOLDER,3);
    }

    Serial.println ("CLOSER MODE: Hello, come closer, I wont bite");

    
  }else if(distance > Distance_Short){
    state = STATE_SHORT;
    //Sensed Short
    Serial.println ("CLOSER MORE MODE: Come closer");
    redColour = 100;
    greenColour = 50;
    blueColour = 0;
    if(state != stateLast){
      mp3.playSpecific(PLAY_FOLDER,4);
    }

    
  }else{
    state = STATE_BOO;
    //Sensed IMMEDIATE
    state = 4;
    Serial.println ("SCARE MODE: BOOOO! HAHAHA");
    redColour = 255;
    greenColour = 0;
    blueColour = 0;
    if(state != stateLast){
      mp3.playSpecific(PLAY_FOLDER,5);
    }
  }

  //update the lights
  for(int i=0; i<NUMPIXELS; i++) {
    lights.setPixelColor(i, lights.Color(redColour, greenColour, blueColour));
    lights.show();   // Send the updated pixel colors to the hardware.
  }

  stateLast = state; //so we dont do this until changed
  Serial.println ();
  delay (3000);
}

//Distance sensor
uint16_t getDistance ()
{
  uint16_t value;
  value = analogRead (distanceSensorPin);
  delay(100);
  value = value + analogRead (distanceSensorPin);
  delay(200);
  value = value + analogRead (distanceSensorPin);
  value = value / 3; //average
  
  if (value < 10) value = 10;
  return ((67870.0 / (value - 3.0)) - 40.0);
}
