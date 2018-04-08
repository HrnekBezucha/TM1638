// TM1638 board with 8 buttons and 8 digit 7 segment display.

/*  Strobe pin - chip select
 *      input data when strobe is low, when high, clock is ignored.
 *      Used for board initialization for each command-data byte pair.
 *   
 *  Clock - reads data on rising edge and outputs on falling edge.
 *  
 *  DIO - Data I/O
 *      input data on raising edge, output on falling edge of CLK.
 * 
 *  Possible commands:
 *      Activate/Deactivate & init display
 *      Write byte at specific address
 *      Write byte starting from address (allows more bytes
 *          of data to be input without taking STB HIGH)
 *      Read buttons
 *
 *  Commands:
 *      10001bbb - Activate display with bbb brightness
 *              maximum brightness is 10001111
 *              minimum brightness is 10001000
 *      01000100 address value - Write a data at specific loc
 *              address 1100aaaa
 *              value   0100vvvv
 *      01000000 address value value - write data from location onwards
 *              address 1100aaaa
 *              value   0100vvvv
 *                  send more data bytes to light more segments
 *                  jumps automatically to next digit
 *                  when end reached, starts from beginning
 *      01000010 - Read buttons
 *              data pin must be set to INPUT
 *
 *
 *  Display addresses:
 *      0000    digit 1 (from left)
 *      0010    digit 2
 *      0100    digit 3
 *      0110    digit 4
 *      1000    digit 5
 *      1010    digit 6
 *      1100    digit 7
 *      1110    digit 8
 *      
 *      0001    led 1
 *      0011    led 2
 *      0101    led 3
 *      0111    led 4
 *      1001    led 5
 *      1011    led 6
 *      1101    led 7
 *      1111    led 8
 * 
 *  Segments:
 *      0b00000000
 *        .gfedcba
 *        
 *         ---a---
 *        |       |
 *        f       b
 *        |       |
 *         ---g---
 *        |       |
 *        e       c
 *        |       |
 *         ---d---  .
 * 
 */

const uint8_t stb   = 2;           //  strobe pin 2 - out only
const uint8_t clk   = 3;           //  clock pin 3 - out only
const uint8_t dio   = 4;           //  data pin 4 - in out

const uint8_t schar = 0b01000000;  //  definition of commands

const uint8_t blk   = 0b00000000;  //  blank character

const uint8_t chA   = 0b01110111;  //  definition of characters
const uint8_t chB   = 0b01111100;
const uint8_t chC   = 0b00111001;
const uint8_t chD   = 0b01011110;
const uint8_t chE   = 0b01111001;
const uint8_t chF   = 0b01110001;
const uint8_t chG   = 0b00111101;
const uint8_t chH   = 0b01110110;
const uint8_t chL   = 0b00111000;
const uint8_t chO   = 0b00111111;

const uint8_t nm0   = 0b00111111;  // definition of numbers
const uint8_t nm1   = 0b00000110;
const uint8_t nm2   = 0b01011011;
const uint8_t nm3   = 0b01001111;
const uint8_t nm4   = 0b01100110;
const uint8_t nm5   = 0b01101101;
const uint8_t nm6   = 0b01111101;
const uint8_t nm7   = 0b00000111;
const uint8_t nm8   = 0b01111111;
const uint8_t nm9   = 0b01101111;

const uint8_t dig1  = 0b11000000;  // definition of display digits
const uint8_t dig2  = 0b11000010;  // leftmost first
const uint8_t dig3  = 0b11000100;
const uint8_t dig4  = 0b11000110;
const uint8_t dig5  = 0b11001000;
const uint8_t dig6  = 0b11001010;
const uint8_t dig7  = 0b11001100;
const uint8_t dig8  = 0b11001110;




void sendCommand(uint8_t value) {
  digitalWrite(stb, LOW);
  shiftOut(dio, clk, LSBFIRST, value);
  digitalWrite(stb, HIGH);
}

void sendDigit(uint8_t address, uint8_t value) {
  digitalWrite(stb, LOW);
  shiftOut(dio, clk, LSBFIRST, schar);
  digitalWrite(stb, HIGH);
  digitalWrite(stb, LOW);
  shiftOut(dio, clk, LSBFIRST, address);
  shiftOut(dio, clk, LSBFIRST, value);
  digitalWrite(stb, HIGH);
}

uint8_t readButtons(void){
  uint8_t buttons = 0;
  digitalWrite(stb, LOW);
  shiftOut(dio, clk, LSBFIRST, 0b01000010);  // reading buttons command
  pinMode(dio, INPUT);  // setting data pin as input
  for(uint8_t i=0; i<4; i++){
    uint8_t v = shiftIn(dio, clk, LSBFIRST) << i;
    buttons |= v;
  }
  pinMode(dio, OUTPUT);  // setting data pin back to output
  digitalWrite(stb, HIGH);
  return buttons;
}

void setLed(uint8_t value, uint8_t possition){
  pinMode(dio, OUTPUT);  // make sure data pin is output
  sendCommand(0b01000100);  // write to single location
  digitalWrite(stb, LOW);
  shiftOut(dio, clk, LSBFIRST, 0b11000001 + (possition << 1));
  shiftOut(dio, clk, LSBFIRST, value);
  digitalWrite(stb, HIGH);
}

void reset(){  // clearing the registers
  sendCommand(0b01000000); // set auto-increment mode
  digitalWrite(stb, LOW);
  shiftOut(dio, clk, LSBFIRST, 0b11000000);    // start at position 0
  for(uint8_t i=0; i < 16; i++){
    shiftOut(dio, clk, LSBFIRST, 0b0000);      // send 0
    delay(10);
    }
  digitalWrite(stb, HIGH);
}

void setup() {
  pinMode(stb, OUTPUT);
  pinMode(clk, OUTPUT);
  pinMode(dio, OUTPUT);

  sendCommand(0b10001001);  //brightness
  reset();



}

void loop() {

  uint8_t buttons = readButtons();

  for(uint8_t possition = 0; possition < 8; possition++){
    uint8_t mask = 0b00000001 << possition;
    setLed(button & mask ? 1 : 0, possition);
    }

}

