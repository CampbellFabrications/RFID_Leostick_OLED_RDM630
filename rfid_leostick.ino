#include "U8glib.h"


#include <ReceiveOnlySoftwareSerial.h>
/*  visit http://gammon.com.au/Arduino/ReceiveOnlySoftwareSerial.zip 
for the single pin SoftwareSerial interface library. Used only if building this on the LeoStick
Otherwise, use SoftwareSerial*/
int data1 = 0;
int ok = -1;

#define redPin 13
#define greenPin 9
#define soundPin 11
#define rxPin 10

//Comment this out to remove Debug Serial messages
#define DEBUG

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);
/* blue pin is receiving the reader data, useful for diagnostics */


ReceiveOnlySoftwareSerial RFID(rxPin); // RX only
int const readlength = 13;

int tag1[readlength] =   {0,0,0,0,0,0,0,0,0,0,0,0,0}; //2 tags are defined here
int tag2[readlength] =   {0,0,0,0,0,0,0,0,0,0,0,0,0}; 
int newtag[readlength] = {0,0,0,0,0,0,0,0,0,0,0,0,0}; // used for read comparisons
String input = "";
String input1 = "";
String input2 = "";
void setup()
{
  RFID.begin(9600);    // start serial to RFID reader
  Serial.begin(9600);  // start serial to PC 
  pinMode(soundPin, OUTPUT); // for status LEDs
  pinMode(redPin, OUTPUT);
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
  }
  u8g.setFont(u8g_font_unifont);
  
  u8g.firstPage();
  do {
    draw(15,27,39,51,"Scan a tag","","");
    }
  while(u8g.nextPage());
  delay(500);
}

boolean comparetag(int aa[readlength], int bb[readlength])
{
  boolean ff = false;
  int fg = 0;
  for (int cc = 0 ; cc < readlength ; cc++)
  {
    if (aa[cc] == bb[cc])
    {
      fg++;
    }
  }
  if (fg == readlength)
  {
    ff = true;
  }
  return ff;
}

void checkmytags() // compares each tag against the tag just read
{
  ok = 0; // this variable helps decision-making,
  // if it is 1 we have a match, zero is a read but no match,
  // -1 is no read attempt made
  if (comparetag(newtag, tag1) == true)
  {
    ok++;
  }
  if (comparetag(newtag, tag2) == true)
  {
    ok++;
  }
}

void readTags()
{
  ok = -1;

  if (RFID.available() > 0) 
  {
    // read tag numbers
    input = "";
    input1 = "";
    input2 = "";
    delay(100); // needed to allow time for the data to come in from the serial buffer.

    for (int z = 0 ; z < readlength ; z++) // read the rest of the tag
    {
      data1 = RFID.read();
      
      newtag[z] = data1;
    }
    RFID.flush(); // stops multiple reads

    // do the tags match up?
    checkmytags();
    for(int i = 0; i < readlength; i++) {
         input += newtag[i];
      }
      if(ok != -1) {
        input1 = input.substring(0,12); //set at 12 to allow for text wrap on an OLED screen
        input2 = input.substring(12);
        #ifdef DEBUG
        Serial.println("TAG ID: " + input); //copy this into tag1[] or tag2[]
        Serial.println("part1: " +input1);
        Serial.println("part2: " +input2);
        #endif
      }
     }
     

  // now do something based on tag type
  if (ok > 0) // if we had a match
  {
    
    #ifdef DEBUG
    Serial.println("Accepted");
    #endif
    //insert desired commands for successful authentication
    digitalWrite(greenPin,HIGH);
    delay(1000);
    digitalWrite(greenPin,LOW);
    ok = -1;
    clearOLED("Accepted");
  }
  else if (ok == 0) // if we didn't have a match
  {
    #ifdef DEBUG
    Serial.println("Rejected");
    #endif
    //insert desired commands for failed authentication
    digitalWrite(redPin,HIGH);
    delay(1000);
    digitalWrite(redPin, LOW);
    ok = -1;
    clearOLED("Rejected");
  }
  
}

void loop(void)
{
  readTags();
}

void clearOLED(String option)
{
  u8g.firstPage();
  do {
    draw(15,27,39,51,option,input1,input2);
    }
  while(u8g.nextPage());
  #ifdef DEBUG
  Serial.println("Cleared");
  #endif
  delay(500);
}

void draw(int line1, int line2, int line3, int line4, String string2, String string3, String string4) {
  // graphic commands to redraw the complete screen should be placed here  
  //clearOLED();
  u8g.setFont(u8g_font_unifont);
  u8g.drawStr(0,line1,"RFID Reader");
  u8g.drawStr(0,line2,string2.c_str());
  u8g.drawStr(0,line3,string3.c_str());
  u8g.drawStr(0,line4,string4.c_str());
}