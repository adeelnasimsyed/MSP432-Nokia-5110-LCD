
// Blue Nokia 5110
// ---------------
// Signal        (Nokia 5110) LaunchPad pin
// Reset         (RST, pin 1) connected to P9.3
// UCA3STE       (CE,  pin 2) connected to P9.4
// Data/Command  (DC,  pin 3) connected to P9.2
// UCA3SIMO      (Din, pin 4) connected to P9.7
// UCA3CLK       (Clk, pin 5) connected to P9.5
// 3.3V          (Vcc, pin 6) power
// back light    (BL,  pin 7) not connected, consists of 4 3.3 V white LEDs which draw ~80mA total
// Ground        (Gnd, pin 8) ground

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>



// Maximum dimensions of the LCD
#define MAX_X                   84
#define MAX_Y                   48

// Contrast value 0xB1 looks good on red SparkFun
// and 0xB8 looks good on blue Nokia 5110.
// Adjust this from 0xA0 (lighter) to 0xCF (darker) for your display.
#define CON_TRAST                0xAF

enum typeOfWrite{
  COMMAND,                              // the transmission is an LCD command
  DATA                                  // the transmission is data
};

// This table contains the hex values that represent pixels
// for a font that is 5 pixels wide and 8 pixels high
static const uint8_t ASCII[][5] = {
  {0x00, 0x00, 0x00, 0x00, 0x00} // 20
  ,{0x00, 0x00, 0x5f, 0x00, 0x00} // 21 !
  ,{0x00, 0x07, 0x00, 0x07, 0x00} // 22 "
  ,{0x14, 0x7f, 0x14, 0x7f, 0x14} // 23 #
  ,{0x24, 0x2a, 0x7f, 0x2a, 0x12} // 24 $
  ,{0x23, 0x13, 0x08, 0x64, 0x62} // 25 %
  ,{0x36, 0x49, 0x55, 0x22, 0x50} // 26 &
  ,{0x00, 0x05, 0x03, 0x00, 0x00} // 27 '
  ,{0x00, 0x1c, 0x22, 0x41, 0x00} // 28 (
  ,{0x00, 0x41, 0x22, 0x1c, 0x00} // 29 )
  ,{0x14, 0x08, 0x3e, 0x08, 0x14} // 2a *
  ,{0x08, 0x08, 0x3e, 0x08, 0x08} // 2b +
  ,{0x00, 0x50, 0x30, 0x00, 0x00} // 2c ,
  ,{0x08, 0x08, 0x08, 0x08, 0x08} // 2d -
  ,{0x00, 0x60, 0x60, 0x00, 0x00} // 2e .
  ,{0x20, 0x10, 0x08, 0x04, 0x02} // 2f /
  ,{0x3e, 0x51, 0x49, 0x45, 0x3e} // 30 0
  ,{0x00, 0x42, 0x7f, 0x40, 0x00} // 31 1
  ,{0x42, 0x61, 0x51, 0x49, 0x46} // 32 2
  ,{0x21, 0x41, 0x45, 0x4b, 0x31} // 33 3
  ,{0x18, 0x14, 0x12, 0x7f, 0x10} // 34 4
  ,{0x27, 0x45, 0x45, 0x45, 0x39} // 35 5
  ,{0x3c, 0x4a, 0x49, 0x49, 0x30} // 36 6
  ,{0x01, 0x71, 0x09, 0x05, 0x03} // 37 7
  ,{0x36, 0x49, 0x49, 0x49, 0x36} // 38 8
  ,{0x06, 0x49, 0x49, 0x29, 0x1e} // 39 9
  ,{0x00, 0x36, 0x36, 0x00, 0x00} // 3a :
  ,{0x00, 0x56, 0x36, 0x00, 0x00} // 3b ;
  ,{0x08, 0x14, 0x22, 0x41, 0x00} // 3c <
  ,{0x14, 0x14, 0x14, 0x14, 0x14} // 3d =
  ,{0x00, 0x41, 0x22, 0x14, 0x08} // 3e >
  ,{0x02, 0x01, 0x51, 0x09, 0x06} // 3f ?
  ,{0x32, 0x49, 0x79, 0x41, 0x3e} // 40 @
  ,{0x7e, 0x11, 0x11, 0x11, 0x7e} // 41 A
  ,{0x7f, 0x49, 0x49, 0x49, 0x36} // 42 B
  ,{0x3e, 0x41, 0x41, 0x41, 0x22} // 43 C
  ,{0x7f, 0x41, 0x41, 0x22, 0x1c} // 44 D
  ,{0x7f, 0x49, 0x49, 0x49, 0x41} // 45 E
  ,{0x7f, 0x09, 0x09, 0x09, 0x01} // 46 F
  ,{0x3e, 0x41, 0x49, 0x49, 0x7a} // 47 G
  ,{0x7f, 0x08, 0x08, 0x08, 0x7f} // 48 H
  ,{0x00, 0x41, 0x7f, 0x41, 0x00} // 49 I
  ,{0x20, 0x40, 0x41, 0x3f, 0x01} // 4a J
  ,{0x7f, 0x08, 0x14, 0x22, 0x41} // 4b K
  ,{0x7f, 0x40, 0x40, 0x40, 0x40} // 4c L
  ,{0x7f, 0x02, 0x0c, 0x02, 0x7f} // 4d M
  ,{0x7f, 0x04, 0x08, 0x10, 0x7f} // 4e N
  ,{0x3e, 0x41, 0x41, 0x41, 0x3e} // 4f O
  ,{0x7f, 0x09, 0x09, 0x09, 0x06} // 50 P
  ,{0x3e, 0x41, 0x51, 0x21, 0x5e} // 51 Q
  ,{0x7f, 0x09, 0x19, 0x29, 0x46} // 52 R
  ,{0x46, 0x49, 0x49, 0x49, 0x31} // 53 S
  ,{0x01, 0x01, 0x7f, 0x01, 0x01} // 54 T
  ,{0x3f, 0x40, 0x40, 0x40, 0x3f} // 55 U
  ,{0x1f, 0x20, 0x40, 0x20, 0x1f} // 56 V
  ,{0x3f, 0x40, 0x38, 0x40, 0x3f} // 57 W
  ,{0x63, 0x14, 0x08, 0x14, 0x63} // 58 X
  ,{0x07, 0x08, 0x70, 0x08, 0x07} // 59 Y
  ,{0x61, 0x51, 0x49, 0x45, 0x43} // 5a Z
  ,{0x00, 0x7f, 0x41, 0x41, 0x00} // 5b [
  ,{0x02, 0x04, 0x08, 0x10, 0x20} // 5c '\'
  ,{0x00, 0x41, 0x41, 0x7f, 0x00} // 5d ]
  ,{0x04, 0x02, 0x01, 0x02, 0x04} // 5e ^
  ,{0x40, 0x40, 0x40, 0x40, 0x40} // 5f _
  ,{0x00, 0x01, 0x02, 0x04, 0x00} // 60 `
  ,{0x20, 0x54, 0x54, 0x54, 0x78} // 61 a
  ,{0x7f, 0x48, 0x44, 0x44, 0x38} // 62 b
  ,{0x38, 0x44, 0x44, 0x44, 0x20} // 63 c
  ,{0x38, 0x44, 0x44, 0x48, 0x7f} // 64 d
  ,{0x38, 0x54, 0x54, 0x54, 0x18} // 65 e
  ,{0x08, 0x7e, 0x09, 0x01, 0x02} // 66 f
  ,{0x0c, 0x52, 0x52, 0x52, 0x3e} // 67 g
  ,{0x7f, 0x08, 0x04, 0x04, 0x78} // 68 h
  ,{0x00, 0x44, 0x7d, 0x40, 0x00} // 69 i
  ,{0x20, 0x40, 0x44, 0x3d, 0x00} // 6a j
  ,{0x7f, 0x10, 0x28, 0x44, 0x00} // 6b k
  ,{0x00, 0x41, 0x7f, 0x40, 0x00} // 6c l
  ,{0x7c, 0x04, 0x18, 0x04, 0x78} // 6d m
  ,{0x7c, 0x08, 0x04, 0x04, 0x78} // 6e n
  ,{0x38, 0x44, 0x44, 0x44, 0x38} // 6f o
  ,{0x7c, 0x14, 0x14, 0x14, 0x08} // 70 p
  ,{0x08, 0x14, 0x14, 0x18, 0x7c} // 71 q
  ,{0x7c, 0x08, 0x04, 0x04, 0x08} // 72 r
  ,{0x48, 0x54, 0x54, 0x54, 0x20} // 73 s
  ,{0x04, 0x3f, 0x44, 0x40, 0x20} // 74 t
  ,{0x3c, 0x40, 0x40, 0x20, 0x7c} // 75 u
  ,{0x1c, 0x20, 0x40, 0x20, 0x1c} // 76 v
  ,{0x3c, 0x40, 0x30, 0x40, 0x3c} // 77 w
  ,{0x44, 0x28, 0x10, 0x28, 0x44} // 78 x
  ,{0x0c, 0x50, 0x50, 0x50, 0x3c} // 79 y
  ,{0x44, 0x64, 0x54, 0x4c, 0x44} // 7a z
  ,{0x00, 0x08, 0x36, 0x41, 0x00} // 7b {
  ,{0x00, 0x00, 0x7f, 0x00, 0x00} // 7c |
  ,{0x00, 0x41, 0x36, 0x08, 0x00} // 7d }
  ,{0x10, 0x08, 0x08, 0x10, 0x08} // 7e ~
//  ,{0x78, 0x46, 0x41, 0x46, 0x78} // 7f DEL
  ,{0x1f, 0x24, 0x7c, 0x24, 0x1f} // 7f UT sign
};


void timer_for_delay(void){

    TIMER32_2 ->CONTROL |= TIMER32_CONTROL_SIZE | TIMER32_CONTROL_PRESCALE_0;
    TIMER32_2 ->CONTROL &= ~TIMER32_CONTROL_ONESHOT;
}

void wait_ms(uint32_t  time){

    TIMER32_2 ->LOAD = (time * 4800);
    TIMER32_2 ->CONTROL |= TIMER32_CONTROL_ENABLE;

    while((TIMER32_2 ->RIS & 1) == 0);
    TIMER32_2 ->INTCLR = 0;

}

void SPI_init() {

//    The recommended eUSCI initialization/reconfiguration process is:
//    1. Set UCSWRST.
//    2. Initialize all eUSCI registers with UCSWRST = 1 (including UCxCTL1).
//    3. Configure ports.
//    4. Clear UCSWRST through software.
//    5. Enable interrupts (optional) with UCRXIE or UCTXIE.

    // Hold the eUSCI module in reset mode
    EUSCI_A3 ->CTLW0 |= UCSWRST;

    // configure UCA3CTLW0 for:
    // bit15      UCCKPH = 1; data shifts in on first edge, out on following edge
    // bit14      UCCKPL = 0; clock is low when inactive
    // bit13      UCMSB = 1; MSB first
    // bit12      UC7BIT = 0; 8-bit data
    // bit11      UCMST = 1; master mode
    // bits10-9   UCMODEx = 2; UCSTE active low
    // bit8       UCSYNC = 1; synchronous mode
    // bits7-6    UCSSELx = 2; eUSCI clock SMCLK
    // bits5-2    reserved
    // bit1       UCSTEM = 1; UCSTE pin enables slave
    // bit0       UCSWRST = 1; reset enabled

    EUSCI_A3 ->CTLW0 = 0xAD83;

    // The SMCLK is set to 12 MHz, divide by 3 for 4 MHz SPI clock
    EUSCI_A3 ->BRW = 3;

    // Modulation is not used in SPI mode, so MCTLW is cleared
    EUSCI_A3 ->MCTLW = 0;

    //Enable Pins for SPI mode
    P9 ->SEL0 |= BIT4 | BIT5 | BIT7;
    P9 ->SEL1 &= ~BIT4 | BIT5 | BIT7;   // configure P9.7, P9.5, and P9.4 as primary module function
    P9 ->SEL0 &= ~BIT2 | BIT3;
    P9 ->SEL1 &= ~BIT2 | BIT3;          // configure P9.3 and P9.2 as GPIO (Reset and D/C pins)
    P9 ->DIR |= BIT2 | BIT3;            // make P9.3 and P9.2 out (Reset and D/C pins)

    // Enable eUSCI module
    EUSCI_A3 ->CTLW0 &= ~UCSWRST;

    // Disable interrupts
    EUSCI_A3 ->IE &= ~(UCTXIE | UCRXIE);

}
//Write data or command to LCD
void LCD_write(enum typeOfWrite type, uint8_t message){

    volatile uint8_t dummy;

    P9 ->OUT &= ~BIT4;

    while(!(EUSCI_A3->IFG & 0x02));      // wait until UCA3TXBUF empty

    if(type == COMMAND){
        P9->OUT &= ~BIT2;                //D/C is cleared
    }
    else{
        P9->OUT |= BIT2;                //D/C is set
    }
    //SPI_transmitData(EUSCI_A3_BASE, message);
    EUSCI_A3 ->TXBUF = message;          // message out
    dummy = EUSCI_A3 ->RXBUF;            // response meaningless here

    P9 ->OUT |= BIT4;
}

void LCD_init(void){

    P9->OUT &= ~BIT3;                   // reset the LCD to a known state, RESET low
    wait_ms(10);                        // delay minimum 100 ns (I set at 10ms)
    P9->OUT |= BIT3;                    // hold RESET high

    LCD_write(COMMAND, 0x21);           // chip active; horizontal addressing mode (V = 0); use extended instruction set (H = 1)
                                        // set LCD Vop (contrast), which may require some tweaking:
    LCD_write(COMMAND, CON_TRAST);       // try 0xB1 (for 3.3V red SparkFun), 0xB8 (for 3.3V blue SparkFun), 0xBF if your display is too dark, or 0x80 to 0xFF if experimenting
    LCD_write(COMMAND, 0x04);           // set temp coefficient
    LCD_write(COMMAND, 0x14);           // LCD bias mode 1:48: try 0x13 or 0x14

    LCD_write(COMMAND, 0x20);           // we must send 0x20 before modifying the display control mode
    LCD_write(COMMAND, 0x0C);           // set display control to normal mode: 0x0D for inverse


}

void LCD_output_char(char data){

    int i;

    LCD_write(DATA, 0x00);        // blank vertical line padding

    for(i=0; i<5; i++){

        LCD_write(DATA, ASCII[data - 0x20][i]);
    }
    LCD_write(DATA, 0x00);        // blank vertical line padding
}


// Move the cursor to the desired X- and Y-position.
void LCD_set_cursor(uint8_t newX, uint8_t newY){

    if((newX > 11) || (newY > 5)){        // bad input
        return;                           // do nothing
    }


    // multiply newX by 7 because each character is 7 columns wide
    LCD_write(COMMAND, 0x80 | (newX*7));     // setting bit 7 updates X-position
    LCD_write(COMMAND, (0x40 | newY));         // setting bit 6 updates Y-position
}

// Clear the LCD by writing zeros to the entire screen and
// reset the cursor to top left corner of screen.
void LCD_clear(void){
    int i;

    for(i=0; i<(MAX_X*MAX_Y/8); i++){

        LCD_write(DATA,0x00);
    }

    LCD_set_cursor(0, 0);
}

void LCD_draw_image(const uint8_t *ptr){

    int i;

    LCD_set_cursor(0, 0);
    for(i=0; i<(MAX_X*MAX_Y/8); i++){
    LCD_write(DATA,ptr[i]);
  }
}

void LCD_output_string(char *ptr){
    while(*ptr){
        LCD_output_char((unsigned char)*ptr);
        ptr = ptr + 1;
    }
}



int main(void)
{
    // Halting WDT
    WDT_A_holdTimer();

    //Setting CPU clock to high frequency crystal at 48MHz, setting sub-main clock to 12MHz
    CS_setExternalClockSourceFrequency(32000, 48000000);
    MAP_CS_initClockSignal(CS_MCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_1); //48MHz
    MAP_CS_initClockSignal(CS_SMCLK,CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_4); // 12MHz

    timer_for_delay();
    SPI_init();
    //wait_ms(1000);

    LCD_init();


    char num[10];
    sprintf(num, "D = %d", 1001);

    while(1){

        wait_ms(1000);
        LCD_clear();
        LCD_output_string(num);

    }
}

