
#include <avr/pgmspace.h>

#define DATA_1 (PORTC |=  0X01)    // DEFINE a value for DATA as 1  // for UNO
#define DATA_0 (PORTC &=  0XFE)    // DEFINE a value for DATA 0   // for UNO
#define STRIP_PINOUT (DDRC=0x3F)  // DEFINE PORTC as OUTPUT // for UNO (change pins 0-5; leave PORTC 6 & 7 alone)
#define NOP "nop\n\t"

// Apply this to turn all lights off.
unsigned long off[][10]={{0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,0x000000},};

void setup() {              
  Serial.begin(115200);  // Note the baud rate!
  STRIP_PINOUT;	// sets the output pin to control the LED Strip
  reset_strip(); // resets each of the LED nodes to OFF	
  send_1M_pattern(off, 1, 5); // Start by turning all LEDs off.
}


void loop()
{
  int incomingByte = 0;
  
  // If we have serial data...
  if (Serial.available() > 0) {
    // Check if the byte is 255, if it is, that reprsents the start of a 40 byte sequence. 
    incomingByte = Serial.read();
    if (incomingByte != 255) {
      return;
    }
    // Wait until all 40 new bytes are available.
    while (Serial.available() < 40 ) {
      continue;
    }

    // Loop over the received bytes in 4-byte chunks.
    // Convert the 4 bytes into a long and save it in the payload.
    unsigned long payload[1][10];
    for (int i = 0; i < 10; i++) {
      unsigned long value;
      unsigned long byte1, byte2, byte3, byte4;
      byte1 = long(Serial.read());
      byte2 = long(Serial.read());
      byte3 = long(Serial.read());
      byte4 = long(Serial.read());
      value = (byte1 << 24) + (byte2 << 16) + (byte3 << 8) + byte4;
      payload[0][i] = value;
    }

    // Write the 10 long payload to the strip.
    send_1M_pattern(payload, 1, 10);
  }
}

/*******************************************************************************
 * Function Name:	send_1M_pattern
 * Description:		Transmit pattern to whole 1 meter strip              
 * Input	: 		Pointer to RAM pattern; pattern length; frame rate               
 * Output:		Sends out a serial pulse train using the send_strip function
 * Return:		None
 *******************************************************************************/
void send_1M_pattern(unsigned long data[][10], int pattern_no, int frame_rate)
{
  int i=0;
  int j=0;
  uint32_t temp_data;

  // data[][10] is the name of the ARRAY with the RGB LED pattern values
  // Each pattern sends out [x] packets - one for each NODE (RGB LED cluster) in the strip
  // pattern_no is the [y] dimension of the array - the number of ROWS in each pattern array series
  // frame_rate is the DELAY before transmitting the next ROW in the pattern array
  for (i=0;i<pattern_no;i++)
  {
    noInterrupts(); // Turn OFF Interupts for more precise Pulse Timing using MCU processing cycles
    for (j=0;j<10;j++)
    {
      temp_data=data[i][j];
      send_strip(temp_data);
    }
    interrupts(); // Turn ON Interrupts after data is sent

    delay(frame_rate); // Delay between each pulse train - sets the duration of each FRAME, before the next series is sent;
    /* CONTROLS THE VISUAL SPEED OF THE DISPLAY CHANGES */

  }

}


/*******************************************************************************
 * Function Name:	send_strip
 * Description:		Creates and Transmits a serial train of 24 pulses for the LED strip              
 * Input:		24-bit data sets intensity of each color, which will persist until
 *			the next cycle makes a change                
 * Output:		Sends a train of 24 pulses (bits) representing values of 0 or 1 to the output pin ()
 *                       TIMING the OUTPUT pulses using the duration of the MCU NOP @ 16MHz
 * Return:		Nothing returned from function
 *******************************************************************************/
void send_strip(uint32_t data)
{
  int i;
  unsigned long j=0x800000;

  int d;
  for (i=0;i<24;i++)
  {
    if (data & j)
    {
      DATA_1;
      // TM1803 BIT 1 - Set OUTPUT PIN HIGH & HOLD FOR 1.55us or ~ 26 MCU CYCLES = 1.625us;
      // The comparison (d<NN) uses ONE cycle, so the total delay is 2 X d X NOP (62.5 Nanoseconds);
      for (d=1;d<13;d++)
      {
        __asm__(NOP);
      }
      DATA_0; // Set OUTPUT PIN LOW ;
    }
    else
    {
      DATA_1; 
      // TM1803 BIT 0 - Set OUTPUT PIN HIGH & HOLD FOR 0.78us or ~ 12 MCU CYCLES = .750us;
      // The comparison (d<NN) uses ONE cycle, so the total delay is 2 X d X NOP (62.5 Nanoseconds);
      for (d=1;d<5;d++)
      {
        __asm__(NOP);
      }
      DATA_0; // Set OUTPUT PIN LOW ;
      /*----------------------------*/
      __asm__(NOP);
      __asm__(NOP);
      __asm__(NOP);    
      /*----------------------------*/
    }
    j>>=1;
  }

} 
// END of send_strip();

/*******************************************************************************
 * Function Name:	reset_strip
 * Description:		Send a 0 pulse to reset all colors on all NODES of the strip               
 * Input:		None            
 * Output: 		Sends out a looong duration 0 on the OUTPUT PIN to RESET the TM1803 chips
 * Return:		None
 *******************************************************************************/
void reset_strip()
{
  DATA_0;
  delayMicroseconds(20); // TM1803 specifies >= 24uS; // Arduino delay is actually slightly longer than assembler execution
}



