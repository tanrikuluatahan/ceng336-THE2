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
#define GAME_WIDTH 4
#define GAME_HEIGHT 8


//  IMPLEMENET STATES 
// PORTA-0 MOVE THE PIECE RIGHT
// PORTA-1 MOVE THE PIECE UP
// PORTA-2 MOVE THE PIECE DOWN
// PORTA-3 MOVE THE PIECE LEFT
// POLL THE PORTA<0-3>


typedef struct GridPosition {
    unsigned char x;
    unsigned char y;
}GridPosition;

GridPosition gridPosition;
unsigned char gameGrid[4][8];

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

typedef enum L_Rotate{
    A,
    B,
    C,
    D,
} L_Rotate;

L_Rotate l_rotate = 0;

bool debounce_prev_submit = false;
bool submit_flag = false;
bool debounce_prev_rotate = false;
bool blink_switch;
int down_counter = 0;

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

void timer_init(){
    T0CONbits.T08BIT = 0; // 16-bit mode
    T0CONbits.T0CS = 0;  // Internal instruction cycle clock
    T0CONbits.PSA = 0;   // Prescaler is assigned
    T0CONbits.T0PS = 0b100; // Prescaler 1:64

    TMR0H = (26474 >> 8); // Set high byte of timer start
    TMR0L = (26474 & 0xFF); // Set low byte of timer start

    INTCONbits.TMR0IE = 1; // Enable Timer0 interrupt
    INTCONbits.GIE = 1;    // Enable global interrupt
    INTCONbits.PEIE = 1;   // Enable peripheral interrupt

    //T0CONbits.TMR0ON = 1;  // Start Timer0

}

void input_init(){
    TRISA = 0b00001111;
    TRISB = 0b01100000;

    INTCONTbits.INT0IE = 1;
    INTCON3bits.INT1IE = 1;

    // Initialize PORTC-F
    TRISC = 0b00000000;
    TRISD = 0b00000000;
    TRISE = 0b00000000;
    TRISF = 0b00000000;
    TRISH = 0b00000000;
    TRISJ = 0b00000000;
    
    // Clear all LEDs
    PORTC = 0;
    PORTD = 0;
    PORTE = 0;
    PORTF = 0;
    PORTH = 0;
    PORTJ = 0;

    // Display zeros
    PORTH = 0b00001111;
    PORTJ = 0b00111111;
}

void init(){
    blink_switch = false;
    timer_init();
    input_init();
}


// You can write globals definitions here...

// ============================ //
//          FUNCTIONS           //
// ============================ //


// GAME LOGIC

void move_left(GameObjects object){
    GridPosition alivePosition;
    // alivePosition.x = gridPosition.x - 1;

    // REFERANCE => UPPER LEFT CORNER OF THE OBJECTS
    // only point object has the area size of 1x1.
    // others should be considered as 2x2.
    
    if(gridPosition.x >=1){
        alivePosition.x = gridPosition.x - 1;
        gridPosition.x = alivePosition.x;
    }
}
void move_right(GameObjects object){
    // REFERANCE => UPPER LEFT CORNER OF THE OBJECTS
    // BEAWARE OF YOU NEED TO CHECK RIGHT BOUNDS;
    GridPosition alivePosition;
    
    switch (object)
    {
        case POINT_OBJECT:
            if(gridPosition.x <=2)
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

            // edge bound index: left=index<1> , right=index<2>
            //              for current position
            // after the shift, it would become:
            //                   left=index<2>, right=index<3>
        case SQUARE_OBJECT:
            // referance was the left tile, by adding one it 
            // become right tile
            // check the gridPosition.x(right tile) if it is in 
            // the edge bounds.
            if(gridPosition.x <=1){
                // if so, 
                alivePosition.x = gridPosition.x;
                gridPosition.x = alivePosition.x + 1;
            }
            break;
    }
}
void move_up(GameObjects object){
    GridPosition alivePosition;
    // alivePosition.y = gridPosition.y - 1;

    // REFERANCE => UPPER LEFT CORNER OF THE OBJECTS
    // only point object has the area size of 1x1.
    // others should be considered as 2x2.
    if(gridPosition.y >=1){
        alivePosition.y = gridPosition.y - 1;
        gridPosition.y = alivePosition.y;
    }
}
void move_down(GameObjects object){
    GridPosition alivePosition;

    switch (object)
    {
        case POINT_OBJECT:
            if(gridPosition.y<=6){
                alivePosition.y = gridPosition.y + 1;
                gridPosition.y = alivePosition.y;
            }
            break;
        case L_OBJECT:
                // our referance was the upper left, in order 
                // to check lower bounds we need to check the
                // low bounds. L_OBJECT and SQUARE_OBJECTS
                // should checked for the same boundaries.

                // edge bound index: lower=index<6> , upper=index<5>
                //              for current position
                // after the shift, it would become:
                //                   lower=index<7>, upper=index<6>
        case SQUARE_OBJECT:
            if(gridPosition.y<=5){
                alivePosition.y = gridPosition.y;
                gridPosition.y = alivePosition.y + 1;
            }
            break;
    }
}

bool submit(){
    submit_flag = false;
    switch (gameObjects)
    {
    case POINT_OBJECT:
        if(!gameGrid[gridPosition.x][gridPosition.y]){
            gameGrid[gridPosition.x][gridPosition.y] = 1;
            submit_flag = true;
        }
        break;
    case SQUARE_OBJECT:
        if(!gameGrid[gridPosition.x][gridPosition.y]&&
            !gameGrid[gridPosition.x][gridPosition.y+1]&&
            !gameGrid[gridPosition.x+1][gridPosition.y]&&
            !gameGrid[gridPosition.x+1][gridPosition.y+1]){
                gameGrid[gridPosition.x][gridPosition.y] = 1;
                gameGrid[gridPosition.x][gridPosition.y+1] = 1;
                gameGrid[gridPosition.x+1][gridPosition.y] = 1;
                gameGrid[gridPosition.x+1][gridPosition.y+1] = 1;
                submit_flag = true;
            }
        break;
    case L_OBJECT:
        // enum    **    **      *      *
        //         *      *     **      **
        //         (a)   (b)    (c)    (d)
        switch(l_rotate){
            case A:
            {
                if(!gameGrid[gridPosition.x][gridPosition.y]&&
                    !gameGrid[gridPosition.x+1][gridPosition.y]&&
                    !gameGrid[gridPosition.x][gridPosition.y+1]){

                    gameGrid[gridPosition.x][gridPosition.y] = 1;
                    gameGrid[gridPosition.x+1][gridPosition.y] = 1;
                    gameGrid[gridPosition.x][gridPosition.y+1] = 1;
                    submit_flag = true;
                    }
            }
            case B:
            {
                if(!gameGrid[gridPosition.x][gridPosition.y]&&
                    !gameGrid[gridPosition.x+1][gridPosition.y]&&
                    !gameGrid[gridPosition.x+1][gridPosition.y+1]){

                    gameGrid[gridPosition.x][gridPosition.y] = 1;
                    gameGrid[gridPosition.x+1][gridPosition.y] = 1;
                    gameGrid[gridPosition.x+1][gridPosition.y+1] = 1;
                    submit_flag = true;
                    }
            }    
            case C:
            {
                if(!gameGrid[gridPosition.x+1][gridPosition.y]&&
                    !gameGrid[gridPosition.x][gridPosition.y+1]&&
                    !gameGrid[gridPosition.x+1][gridPosition.y+1]){

                    gameGrid[gridPosition.x+1][gridPosition.y] = 1;
                    gameGrid[gridPosition.x][gridPosition.y+1] = 1;
                    gameGrid[gridPosition.x+1][gridPosition.y+1] = 1;
                    submit_flag = true;
                    }
                
            }
            case D:
            {
                if(!gameGrid[gridPosition.x][gridPosition.y]&&
                    !gameGrid[gridPosition.x][gridPosition.y+1]&&
                    !gameGrid[gridPosition.x+1][gridPosition.y+1]){

                    gameGrid[gridPosition.x][gridPosition.y] = 1;
                    gameGrid[gridPosition.x][gridPosition.y+1] = 1;
                    gameGrid[gridPosition.x+1][gridPosition.y+1] = 1;
                    submit_flag = true;
                    }
            }
        }
        break;
    }
    return submit_flag;
}

void blink(){
    //8 blink = one object down cycle
    down_counter++;
    if(down_counter==8){
        move_down(gameObjects);
        down_counter = 0;
        
    }
    blink_switch = !blink_switch;
}

// You can write function definitions here...

// ============================ //
//   INTERRUPT SERVICE ROUTINE  //
// ============================ //
__interrupt(high_priority)
void HandleInterrupt()
{
    // ISR ...
    
    if (INTCONbits.TMR0IF){
        INTCONbits.TMR0IF = 0;

        // ...
        // ...
        // not sure about the timer conditions.

        TMR0H = (26474 >> 8); // Reload high byte
        TMR0L = (26474 & 0xFF); // Reload low byte
        blink();

    }



    // PORTB INTERRUPTS
    // RB5 for rotation
    // RB6 for submit
    if (INTCONbits.RBIF){
        INTCONbits.RBIF = 0;

        // sanity check for debouncing
        if(PORTBbits.RB5){
            if(!debounce_prev_rotate){
                l_rotate = (l_rotate+1) % 4;
            }
        }
        debounce_prev_rotate = PORTBbits.RB5;
        if(PORTBbits.RB6){
            if(!debounce_prev_submit){
                if(submit()){
                    gameObjects = (gameObjects + 1) % 3;
                    gridPosition.x = 0;
                    gridPosition.y = 0;
                    l_rotate = 0;
                }
            }
        }


        
    }
}

// ============================ //
//            MAIN              //
// ============================ //
void main()
{
    // Main ...
    init();
    T0CONbits.TMR0ON = 1;
    while(1){
        // game logic 
    }
}