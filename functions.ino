// send an NTP request to the time server at the given address 
unsigned long sendNTPpacket(char* address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE); 
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49; 
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:         
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer,NTP_PACKET_SIZE);
  Udp.endPacket(); 
}

int checkDipSwitches() {
	pinMode(11, INPUT);
	pinMode(12, INPUT);
	pinMode(13, INPUT);
	boolean dip1 = digitalRead(11);
	boolean dip2 = digitalRead(12);
	boolean dip3 = digitalRead(13);
	//cursor_move(4,6);
	if (dip1 == 0 && dip2 == 0 && dip3 == 0) {
	timeZoneAdjust = -28800;
	//Serial.print("Vancouver, Canada");
	} else if (dip1 == 1 && dip2 == 0 && dip3 == 0) {
	timeZoneAdjust = -14400;
	//Serial.print("Halifax, Canada");
	} else if (dip1 == 0 && dip2 == 1 && dip3 == 0) {
	timeZoneAdjust = 0;
	//Serial.print("London, UK");
	} else if (dip1 == 1 && dip2 == 1 && dip3 == 0) {
	timeZoneAdjust = 14400;
	//Serial.print("Moscow, Russia");
	} else if (dip1 == 0 && dip2 == 0 && dip3 == 1) {
	timeZoneAdjust = 28800;
	//Serial.print("Beijing, China");
	}	
	//delay(2000);
	return timeZoneAdjust;
}

/* Simple helper functions */
#define CHAR_ESC '\x1B'
void cursor_move( uint8_t row , uint8_t col )
{ // <ESC>Yrc
  Serial.write( CHAR_ESC ) ;
  Serial.write( 'Y' ) ;
  Serial.write( 32 + row ) ;
  Serial.write( 32 + col ) ;
}

void cursor_show( bool show )
{ // <ESC>e or <ESC>f
   Serial.write( CHAR_ESC ) ;
   Serial.write( show?'e':'f' ) ;
}

void screen_clear( void )
{ // <ESC>E
  Serial.write( CHAR_ESC ) ;
  Serial.write( 'E' );
}

