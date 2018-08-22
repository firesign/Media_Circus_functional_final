/*
 Media Circus functional final
 by Michael LeBlanc, NSCAD University 2016
 http://generaleccentric.net
 with assistance from PaulS [http://arduino.cc/forum/index.php?action=profile;u=16084] 
 on the Arduino Forum
 
 Ethernet Shield code created 18 Dec 2009 by David A. Mellis
 
 HT1632 library by Gaurav Manek: 
 https://github.com/gauravmm/HT1632-for-Arduino/tree/master/Arduino/HT1632
 
 Notes on this version:  **********************************
 October 19 2016 -- 
 after a month-long downtime in Sept, unit repaired; main problem was the wrong font_5x4.h file.
 However, only the first two tweeets were displayed, which is a complication from Arduino 1.6.
 USE ARDUINO 1.0.5 ONLY !!!
 
 Also:
 - revised main server from IP address to ahclem.dyndns.info (so when IP address changes, it won't affect unit
 - revised time server to use time.nist.gov URL rather than IP address
 - commented out all serial monitor commands
 - **** NOTE **** the HT1632 library is updated from time to time, so before uploading code to the device,
   replace the HT1632 directory in Libraries with the unzipped directory called "HT1632 MASTER Media Circus.zip"
   which is located at the Arduino root level

***********************************************************

 TO UPDATE THIS UNIT:
 - disconnect ethernet shield from Mega board
 - use ONLY Arduino 1.0.5
 
 */

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <font_5x4.h>
#include <HT1632.h>
#include <MHV_io.h>



int wd, tempsecs; 
int i = 1;
char buffer[800];
int index = 0;
boolean fetched = false;
int DSTswitch = 22;         // Daylight Savings Time switch
boolean DSTState;
int timeZoneAdjust;
int resetSwitch = 24;
int ntpState = 0;           // the state of the ntp server query
long previousMillis = 0;
unsigned long currentMillis, epoch, daysecs;
unsigned long epochMillis;  // millis when epoch was determined

long previousMillisEpoch = 0;

byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// IPAddress server(24,222,117,110);    // my old ip address
// IPAddress server(76,11,25,18);       // another old ip address
// IPAddress holds the link to the file /cutup/cutup_1.php
// IPAddress server(24,138,58,129);            // current ip address
// unsigned int localPort = 8888;          // local port to listen for UDP packets

// Media Circus now uses:

char server[] = "ahclem.dyndns.info";     // The cutup program is on an RPi at this location


unsigned int localPort = 80;              // local port to listen for UDP packets

char timeServer[] = "time.nist.gov";      // time.nist.gov NTP server
const int NTP_PACKET_SIZE= 48;            // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE];      // buffer to hold incoming and outgoing packets 

// A UDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

EthernetClient client;

void setup() {

  /* reset hack*/
  digitalWrite(resetSwitch, 1);    // enable pullup
  pinMode(resetSwitch, OUTPUT);


  screen_clear();
  checkDipSwitches();     // check the DIP switches to identify unit's global location

  //pinMode(resetSafety, OUTPUT);
  //digitalWrite(resetSafety, 0);     //prevent unit from going into repeated resets

  pinMode(DSTswitch, INPUT);
  digitalWrite(DSTswitch, 1);   // enable pullup


  // start the serial library:
  //Serial.begin(57600);

  HT1632.begin(9, 8, 7, 6, 5, 4);

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    //Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    for(;;)
      ;
  }

  delay(1000);
  //Serial.println("connecting...");

  if (client.connect(server, 80)) {  // changed port from 8888 to 80 on Jan 8 2013
    //Serial.println("connected");
    // Make a HTTP request:
    client.println("GET /mediacircus/mc.py");
    client.println();
  } 
  else {
    //Serial.println("connection failed");
  }
  Udp.begin(localPort);
  previousMillis = millis();
}

void loop() {

  if ((client.available()) && (fetched == false)) {
    char c = client.read();
    if(index < 799)
    {
      buffer[index] = c;
      index++;
      buffer[index] = '\0';
    }
    //Serial.print(c);
  }

  // if the server's disconnected, stop the client:
  if ((!client.connected()) && (fetched == false)) {
    //Serial.println();
    //Serial.println("disconnecting.");
    client.stop();
    //Serial.print("Server response: ");
    //Serial.println(buffer);
    wd = HT1632.getTextWidth(buffer, FONT_5X4_WIDTH, FONT_5X4_HEIGHT);
    fetched = true;
  }
  if (fetched == true) {

    //Serial.println("Wait a sec... more stuff to do!");

    // Select board 1 as the target of subsequent drawing/rendering operations.
    HT1632.drawTarget(BUFFER_BOARD(1));
    HT1632.clear();
    HT1632.drawText(buffer, 4*OUT_SIZE - i, 0,
    FONT_5X4, FONT_5X4_WIDTH, FONT_5X4_HEIGHT, FONT_5X4_STEP_GLYPH);
    HT1632.render(); // Board 1's contents is updated.

    // Select board 2 as the target of subsequent drawing/rendering operations.
    HT1632.drawTarget(BUFFER_BOARD(2));
    HT1632.clear();
    HT1632.drawText(buffer, 3*OUT_SIZE - i, 0,
    FONT_5X4, FONT_5X4_WIDTH, FONT_5X4_HEIGHT, FONT_5X4_STEP_GLYPH);
    HT1632.render(); // Board 2's contents is updated.

    // Select board 3 as the target of subsequent drawing/rendering operations.
    HT1632.drawTarget(BUFFER_BOARD(3));
    HT1632.clear();
    HT1632.drawText(buffer, 2*OUT_SIZE - i, 0,
    FONT_5X4, FONT_5X4_WIDTH, FONT_5X4_HEIGHT, FONT_5X4_STEP_GLYPH);
    HT1632.render(); // Board 3's contents is updated.

    // Select board 4 as the target of subsequent drawing/rendering operations.
    HT1632.drawTarget(BUFFER_BOARD(4));
    HT1632.clear();
    HT1632.drawText(buffer, OUT_SIZE - i, 0,
    FONT_5X4, FONT_5X4_WIDTH, FONT_5X4_HEIGHT, FONT_5X4_STEP_GLYPH);
    HT1632.render(); // Board 4's contents is updated.

    i = (i+1)%(wd + OUT_SIZE * 2); // Make it repeating.
    delay(4); 

    // NTP server access section *************************************************
    switch (ntpState) {
    case 0:    // ask NTP server the time
      cursor_move(1,5);
      //Serial.println("Asking NTP server the time");
      sendNTPpacket(timeServer); // send an NTP packet to a time server
      ntpState = 1;
      break;

    case 1:    // waiting 2000 ms for NTP server to answer
      //Serial.println("Waiting for NTP server response");
      /*unsigned long*/      currentMillis = millis();
      if (currentMillis - previousMillis > 2000) {
        previousMillis = currentMillis;        
        ntpState = 2;
      }
      break;

    case 2:   // process NTP query
      //Serial.println("Processing NTP query");
      if ( Udp.parsePacket() ) {  
        // We've received a packet, read the data from it
        Udp.read(packetBuffer,NTP_PACKET_SIZE);  // read the packet into the buffer

        //the timestamp starts at byte 40 of the received packet and is four bytes,
        // or two words, long. First, esxtract the two words:

        unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
        unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);  
        // combine the four bytes (two words) into a long integer
        // this is NTP time (seconds since Jan 1 1900):
        unsigned long secsSince1900 = highWord << 16 | lowWord;  
        //Serial.print("Seconds since Jan 1 1900 = " );
        //Serial.println(secsSince1900);               

        // now convert NTP time into everyday time:
        //Serial.print("Unix time = ");
        // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
        const unsigned long seventyYears = 2208988800UL;     
        // subtract seventy years:
        epoch = secsSince1900 - seventyYears;

        if (digitalRead(DSTswitch) == true){
          //epoch = epoch - 10800; // adjust for Atlantic Daylight Savings Time
          epoch = epoch + timeZoneAdjust + 3600;
          DSTState = true;
        } 
        else {
          //epoch = epoch - 14400; // adjust for Atlantic Standard Time
          epoch = epoch + timeZoneAdjust;
          DSTState = false;
        }
        // print Unix time:
        //Serial.println(epoch);

        // print the hour, minute and second:
        //Serial.print("The local time is ");       // 
        //Serial.print((epoch  % 86400L) / 3600); // print the hour (86400 equals secs per day)
        //Serial.print(':');  
        if ( ((epoch % 3600) / 60) < 10 ) {
          // In the first 10 minutes of each hour, we'll want a leading '0'
          //Serial.print('0');
        }
        //Serial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
        //Serial.print(':'); 
        if ( (epoch % 60) < 10 ) {
          // In the first 10 seconds of each minute, we'll want a leading '0'
          //Serial.print('0');
        }
        //Serial.println(epoch %60); // print the second
        //Serial.print("Epoch = ");
        //Serial.println(epoch);
        epochMillis = millis();
        //delay(5000);
      }
      ntpState = 3;
      //Serial.println("ending now, nothing more to do");
      screen_clear();
      break;

    case 3:                        // to determine seconds passed since midnight
      daysecs = (epoch + ((millis() - epochMillis) / 1000)) % 86400;
      previousMillisEpoch = millis();
      if (daysecs == 10800){       // reset the unit at 0300 hrs daily 
        //tempsecs = daysecs % 90;
        //cursor_move(2,5);
        //Serial.print("tempsecs: ");
        //if (tempsecs < 10) {
        //Serial.print("0"); 
        //}
        //Serial.println(tempsecs);
        //if (tempsecs == 89) {       
        cursor_move(17,5);
        //Serial.println("Reset enabled");
        cursor_move(18,5);
        //Serial.println("RESET IN 2 SECONDS");
        delay(2000);
        digitalWrite(resetSwitch, 0);         // reset the unit
      }
      ntpState = 4;                // has one second passed?
      break;

    case 4:                        // only spend time here once a second
      long m = millis();
      if ((m - previousMillisEpoch) > 1000) {
        cursor_move(10,1) ;
        //Serial.print("Seconds passed since midnight: ");
        //Serial.println(daysecs);   // seconds passed since midnight
        if (DSTState == true) {
          cursor_move(12,4);
          //Serial.print("Daylight Savings Time: "); 
        } 
        else {
          cursor_move(12,9);
          //Serial.print("Standard Time: ");
        }
        if (timeZoneAdjust == -28800) {
          //Serial.println("Vancouver");
        } 
        else 
          if (timeZoneAdjust == -14400) {
          //Serial.println("Halifax");
        } 
        else 
          if (timeZoneAdjust == 0) {
          //Serial.println("London");
        } 
        else 
          if (timeZoneAdjust == 14400) {
          //Serial.println("Moscow");
        } 
        else {
          //Serial.println("Beijing");
        } 
        cursor_move(20,4);
        //Serial.print("Epoch time at reset: ");
        //Serial.print(epoch);
        ntpState = 3; 
      }
      break;
    }
  }
}

