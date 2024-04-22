// ============================ //
// Do not edit this part!!!!    //
// ============================ //
// 0x300001 - CONFIG1H
#pragma config OSC = HSPLL      // Oscillator Selection bits (HS oscillator,
                                // PLL enabled (Clock Frequency = 4 x FOSC1))
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit
                                // (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit
                                // (Oscillator Switchover mode disabled)
// 0x300002 - CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bits (Brown-out
                                // Reset disabled in hardware and software)
// 0x300003 - CONFIG1H
#pragma config WDT = OFF        // Watchdog Timer Enable bit
                                // (WDT disabled (control is placed on the SWDTEN bit))
// 0x300004 - CONFIG3L
// 0x300005 - CONFIG3H
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator Enable bit
                                // (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled;
                                // RE3 input pin disabled)
// 0x300006 - CONFIG4L
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply
                                // ICSP disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit
                                // (Instruction set extension and Indexed
                                // Addressing mode disabled (Legacy mode))

#pragma config DEBUG = OFF      // Disable In-Circuit Debugger

#define KHZ 1000UL
#define MHZ (KHZ * KHZ)
#define _XTAL_FREQ (40UL * MHZ)

// ============================ //
//             End              //
// ============================ //

#include <xc.h>

// ============================ //
//        DEFINITIONS           //
// ============================ //
#ifndef bool 
#define bool unsigned char
#define TRUE 1
#define FALSE 0
#endif
#define ARRAY2WORD(array) (unsigned char) ((array[0] << 0) | (array[1] << 1) | (array[2] << 2) | (array[3] << 3) | (array[4] << 4) | (array[5] << 5) | (array[6] << 6) | (array[7] << 7))
#define BITOF(var, bit) ((var) & (1 << (bit)))


//  IMPLEMENET STATES 
// PORTA-0 MOVE THE PIECE RIGHT
// PORTA-1 MOVE THE PIECE UP
// PORTA-2 MOVE THE PIECE DOWN
// PORTA-3 MOVE THE PIECE LEFT
// POLL THE PORTA<0-3>

typedef enum LedOnOff{
    LED_ON,
    LED_OFF,
}LedOnOff;

typedef struct GridPosition {
    unsigned char x;
    unsigned char y;
}GridPosition;

GridPosition gridPosition;
LedOnOff gameGrid[4][8];

typedef enum GameObjects{
    L_OBJECT,
    SQUARE_OBJECT,
    POINT_OBJECT,
}GameObjects;

GameObjects gameObjects;
int movementKeysCount = 4;

typedef enum MovementKeys {
    RIGHT_ARROW_KEY,
    LEFT_ARROW_KEY,
    UP_ARROW_KEY,
    BOTTOM_ARROW_KEY,
}MovementKeys;

MovementKeys movementKeys;

typedef enum ControlKeys{
    SUBMIT_KEY,
    ROTATE_KEY,
}ControlKeys;

ControlKeys controlKeys;



// TETRIS BLOCKS WILL BE ROTATED BY THE PORTB-5
// TETRIS BLOCKS WILL BE SUBMITTED BY THE PORTB-6
// THOSE PORTB<5-6> WILL BE TRIGGERED BY INTERRUPTS

// TIMER0 INTERRUPT WILL BE USED TO BLINK THE "CURRENT"
// TETRIS PIECE
//
// ALSO THIS TETRIS PIECE SHOULD GO DOWN GRADUALLY BY
// THIS TIMER0 ALSO, THIS MOVEMENT SHOULD ALSO IN ADDITION
// TO THE USER INPUTS


// You can write struct definitions here...

// ============================ //
//          GLOBALS             //
// ============================ //

// INIT 
// -CLEAR THE PINS
// -INTERRUPTS AND I/O RELATED CHIP STATES
// SHOULD BE SET.
// -7-SEGMENT DISPLAY SHOULD SHOW ZEROES 
// -SYSTEM SHOULD WAIT ONE SECOND, ENABLE INTERRUPTS 
// AND START THE GAME LOOP

// You can write globals definitions here...

// ============================ //
//          FUNCTIONS           //
// ============================ //

void move_left(){
    GridPosition alivePosition;
    // alivePosition.x = gridPosition.x - 1;
    // alivePosition.y = gridPosition.y;
    // REFERANCE => UPPER LEFT CORNER OF THE OBJECTS
    // only point object has the area size of 1x1.
    // others should be considered as 2x2.
    alivePosition.x = gridPosition.x - 1;
    if(gridPosition.x >=1)
        gridPosition.x = alivePosition.x;
}
void move_right(GameObjects object){
    // REFERANCE => UPPER LEFT CORNER OF THE OBJECTS
    // BEAWARE OF YOU NEED TO CHECK RIGHT BOUNDS;
    GridPosition alivePosition;
    
    switch (object)
    {
        case POINT_OBJECT:
            if(gridPosition.x <=6)
            {
                alivePosition.x = gridPosition.x + 1;
                gridPosition.x = alivePosition.x;
            }
            break;
        case L_OBJECT:
            // our referance was the upper left, in order 
            // to check right bounds we need to check the
            // right bounds. L_OBJECT and SQUARE_OBJECTS
            // should checked for the same boundaries.
            
        case SQUARE_OBJECT:
            alivePosition.x = gridPosition.x + 1;
            alivePosition.x = gridPosition.x;
            if(alivePosition.x <=6){
                alivePosition.x = gridPosition.x + 2;
                gridPosition.x = alivePosition.x;
            }
            break;
    }
}


// You can write function definitions here...

// ============================ //
//   INTERRUPT SERVICE ROUTINE  //
// ============================ //
__interrupt(high_priority)
void HandleInterrupt()
{
    // ISR ...
}

// ============================ //
//            MAIN              //
// ============================ //
void main()
{
    // Main ...
}