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
#include <stdlib.h>

// ============================ //
//        DEFINITIONS           //
// ============================ //
#ifndef bool 
#define bool unsigned char
#define true 1
#define false 0
#endif
#define ARRAY2WORD(array) (unsigned char) ((array[0] << 0) | (array[1] << 1) | (array[2] << 2) | (array[3] << 3) | (array[4] << 4) | (array[5] << 5) | (array[6] << 6) | (array[7] << 7))
#define BITOF(var, bit) ((var) & (1 << (bit)))

#define GAME_WIDTH 4
#define GAME_HEIGHT 8

typedef struct GridPosition {
    unsigned char x;
    unsigned char y;
} GridPosition;

typedef enum GameObjects {
    L_OBJECT,
    SQUARE_OBJECT,
    POINT_OBJECT,
} GameObjects;

typedef enum PIECE {
    L =         2,
    SQUARE =    1,
    POINT =     0,
} PIECE;

typedef struct {
    unsigned char x;
    unsigned char y;
    unsigned char shape[2][2];
    unsigned char rotation;
    PIECE type;
} GamePiece;

GameObjects gameObjects;
int movementKeysCount = 4;

typedef enum ControlKeys {
    SUBMIT_KEY,
    ROTATE_KEY,
} ControlKeys;

// ============================ //
//          GLOBALS             //
// ============================ //
unsigned char game_grid[8][4];
GamePiece current_piece;
unsigned char type = 2;

bool debounce_prev_submit = false;
bool submit_flag = false;
bool debounce_prev_rotate = false;
bool blink_switch;
int down_counter = 0;

ControlKeys controlKeys;

// ============================ //
//          FUNCTIONS           //
// ============================ //
void timer_init()
{
    T0CONbits.T08BIT = 0;       // 16-bit mode
    T0CONbits.T0CS = 0;         // Internal instruction cycle clock
    T0CONbits.PSA = 0;          // Prescaler is assigned
    T0CONbits.T0PS = 0b100;     // Prescaler 1:64

    TMR0H = (26474 >> 8);       // Set high byte of timer start
    TMR0L = (26474 & 0xFF);     // Set low byte of timer start

    INTCONbits.TMR0IE = 1;      // Enable Timer0 interrupt
    INTCONbits.GIE = 1;         // Enable global interrupt
    INTCONbits.PEIE = 1;        // Enable peripheral interrupt

    //T0CONbits.TMR0ON = 1;  // Start Timer0
}

void input_init()
{
    TRISA = 0b00001111;
    TRISB = 0b01100000;
    TRISG = 0b00011101;

    INTCONbits.INT0IE = 1;
    INTCON3bits.INT1IE = 1;

    // Initialize PORTC-F
    TRISC = 0b00000000;
    TRISD = 0b00000000;
    TRISE = 0b00000000;
    TRISF = 0b00000000;
    TRISH = 0b00000000;
    TRISJ = 0b00000000;
    
    // Clear all LEDs
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    PORTE = 0;
    PORTF = 0;
    PORTH = 0;
    PORTJ = 0;
    PORTG = 0;

    // Display zeros
    PORTH = 0b00111111;
    PORTJ = 0b00111111;
}

void grid_init()
{
    for (int i = 0; i < GAME_HEIGHT; i++) {
        for (int j = 0; j < GAME_WIDTH; j++) {
            game_grid[i][j] = 0;
        }
    }
}

void init()
{
    blink_switch = false;
    timer_init();
    input_init();
    grid_init();
}

void display()
{
    // Display game region on LEDs
    for (int i = 0; i < GAME_HEIGHT; i++) {
        PORTC = game_grid[i][0];
        PORTD = game_grid[i][1];
        PORTE = game_grid[i][2];
        PORTF = game_grid[i][3];
    }
}

void draw_piece(GamePiece piece)
{
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            game_grid[piece.y + j][piece.x + i] = current_piece.shape[i][j];
        }
    }
    
    unsigned char port_c = 0;
    unsigned char port_d = 0;
    unsigned char port_e = 0;
    unsigned char port_f = 0;
    
    for (int i = 0; i < 8; i++) {
        port_c |= game_grid[i][0] << i;
        port_d |= game_grid[i][1] << i;
        port_e |= game_grid[i][2] << i;
        port_f |= game_grid[i][3] << i;
    }
    
    PORTC = port_c;
    PORTD = port_d;
    PORTE = port_e;
    PORTF = port_f;
}

void spawn() 
{
    current_piece.x = 1;
    current_piece.y = 0;
    current_piece.rotation = 1;
    current_piece.type = type;
    
    type = (type + 1) % 3;
    
    switch (current_piece.type) {
        case 0:
            current_piece.shape[0][0] = 1;
            current_piece.shape[0][1] = 0;
            current_piece.shape[1][0] = 0;
            current_piece.shape[1][1] = 0;
            break;
        case 1:
            current_piece.shape[0][0] = 1;
            current_piece.shape[0][1] = 1;
            current_piece.shape[1][0] = 1;
            current_piece.shape[1][1] = 1;
            break;
        case 2:
            current_piece.shape[0][0] = current_piece.rotation % 4 == 0 ? 0 : 1;
            current_piece.shape[0][1] = current_piece.rotation % 4 == 1 ? 0 : 1;
            current_piece.shape[1][0] = current_piece.rotation % 4 == 2 ? 0 : 1;
            current_piece.shape[1][1] = current_piece.rotation % 4 == 3 ? 0 : 1;
            break;
    }
}

void spawn_pos(unsigned char x, unsigned char y) 
{
    current_piece.x = x;
    current_piece.y = y;
    
    switch (current_piece.type) {
        case 0:
            current_piece.shape[0][0] = 1;
            current_piece.shape[0][1] = 0;
            current_piece.shape[1][0] = 0;
            current_piece.shape[1][1] = 0;
            break;
        case 1:
            current_piece.shape[0][0] = 1;
            current_piece.shape[0][1] = 1;
            current_piece.shape[1][0] = 1;
            current_piece.shape[1][1] = 1;
            break;
        case 2:
            current_piece.shape[0][0] = current_piece.rotation % 4 == 0 ? 0 : 1;
            current_piece.shape[0][1] = current_piece.rotation % 4 == 1 ? 0 : 1;
            current_piece.shape[1][0] = current_piece.rotation % 4 == 2 ? 0 : 1;
            current_piece.shape[1][1] = current_piece.rotation % 4 == 3 ? 0 : 1;
            break;
    }
}

void update_game()
{   
    for (int i = 0; i < 2; i++) {
        game_grid[current_piece.y][current_piece.x + i] = 0;
    }
    
    for (int i = 0; i < 2; i++) {
        game_grid[current_piece.y + i][current_piece.x] = 0;
    }
    
    current_piece.y++;
    
    spawn_pos(current_piece.x, current_piece.y);
}

void move_left()
{    
    for (int i = 0; i < 2; i++) {
        game_grid[current_piece.y + i][current_piece.x] = 0;
    }
    
    for (int i = 0; i < 2; i++) {
        game_grid[current_piece.y][current_piece.x + i] = 0;
    }
    
    switch (current_piece.type)
    {
        case 0:
            if (current_piece.x >= 1) { current_piece.x--; }
            break;
        case 2:
            if (current_piece.x >= 1) { current_piece.x--; }
            break;
        case 1:
            if (current_piece.x >= 1) { current_piece.x--; }
            break;
    }
    
    spawn_pos(current_piece.x, current_piece.y);
}

void move_right()
{    
    for (int i = 0; i < 2; i++) {
        game_grid[current_piece.y + i][current_piece.x] = 0;
    }
    
    for (int i = 0; i < 2; i++) {
        game_grid[current_piece.y][current_piece.x + i] = 0;
    }
    
    switch (current_piece.type)
    {
        case 0:
            if (current_piece.x <= 2) { current_piece.x++; }
            break;
        case 2:
            if (current_piece.x <= 1) { current_piece.x++; }
            break;
        case 1:
            if (current_piece.x <= 1) { current_piece.x++; }
            break;
    }
    
    spawn_pos(current_piece.x, current_piece.y);
}

void move_up()
{
    for (int i = 0; i < 2; i++) {
        game_grid[current_piece.y][current_piece.x + i] = 0;
    }
    
    for (int i = 0; i < 2; i++) {
        game_grid[current_piece.y + i][current_piece.x] = 0;
    }
    
    current_piece.y = current_piece.y - 1;
    
    spawn_pos(current_piece.x, current_piece.y);
}

void move_down()
{
    for (int i = 0; i < 2; i++) {
        game_grid[current_piece.y][current_piece.x + i] = 0;
    }
    
    for (int i = 0; i < 2; i++) {
        game_grid[current_piece.y + i][current_piece.x] = 0;
    }
    
    current_piece.y = current_piece.y + 1;
    
    spawn_pos(current_piece.x, current_piece.y);
}

void rotate()
{
    if (current_piece.type != 2)
    {
        return;
    }
    
    current_piece.rotation++;
    
    if (current_piece.rotation >= 4)
    {
        current_piece.rotation = 1;
    }
    
    spawn_pos(current_piece.x, current_piece.y);
}

void poll_portG()
{
    if (PORTGbits.RG0)
    {
        move_right();
        while (PORTGbits.RG0);
    }
    if (PORTGbits.RG2)
    {
        move_up();
        while (PORTGbits.RG2);
    }
    if (PORTGbits.RG3)
    {
        move_down();
        while (PORTGbits.RG3);
    }
    if (PORTGbits.RG4)
    {
        move_left();
        while (PORTGbits.RG4);
    }
}

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
        //blink();
    }

    if (INTCONbits.RBIF){
        INTCONbits.RBIF = 0;

        if(PORTBbits.RB5){
            rotate();
        }
        debounce_prev_rotate = PORTBbits.RB5;
        if(PORTBbits.RB6){
            update_game();
        }
    }
}

// ============================ //
//            MAIN              //
// ============================ //
void main()
{
    init();
    T0CONbits.TMR0ON = 1;
    
    spawn();
    while(1)
    {
        poll_portG();
        draw_piece(current_piece);
    }
}