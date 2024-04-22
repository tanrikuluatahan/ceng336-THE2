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
#define GAME_WIDTH  4
#define GAME_HEIGHT 8

enum KEY {
    R   = 0,            // PORTA0 - poll
    U   = 1,            // PORTA1 - poll
    D   = 2,            // PORTA2 - poll
    L   = 3,            // PORTA3 - poll
    ROT = 5,            // PORTB5 - interrupt triggered
    SUB = 6,            // PORTB6 - interrupt triggered
};

typedef enum PIECE {
    DOT,                  // dot piece
    SQU,                  // square piece
    LPI                   // L piece
} PIECE;

typedef struct {
    unsigned char x;
    unsigned char y;
    unsigned char rotation;
    PIECE type;
} TetrisPiece;

// ============================ //
//          GLOBALS             //
// ============================ //
unsigned char game_grid[4][8];
volatile unsigned char timer_counter = 0;
TetrisPiece current_piece;

// ============================ //
//          FUNCTIONS           //
// ============================ //

void init(void)
{
    // Initialize PORTA and PORTB
    TRISA = 0b00001111;
    TRISB = 0b00100000;
    
    INTCONbits.INT0IE = 1; // PORT5 interrupt enable
    INTCON3bits.INT1IE = 1; // PORT6 interrupt enable
    
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
    PORTJ = 0b00011111;

    // Initialize TIMER0
    T0CON = 0b10000001;
    TMR0IE = 1;
    TMR0IF = 0;
}

void init_game(void)
{
    // Initialize game region
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 4; j++) {
            game_grid[i][j] = 0;
        }
    }
}

void spawn(void)
{
    current_piece.x = GAME_WIDTH / 2;
    current_piece.y = 0;
    current_piece.rotation = 0; // Initial rotation
    // Choose a random piece type (DOT, SQU, LPI)
    current_piece.type = rand() % 3;
}

void display(void)
{
    for (int i = 0; i < GAME_HEIGHT; i++) {
        PORTC = game_grid[i][0];
        PORTD = game_grid[i][1];
    }
}

void move_left(void)
{
    return;
}

void move_right(void)
{
    return;
}

void move_up(void)
{
    return;
}

void move_down(void)
{
    return;
}

void one_second_delay(void)
{
    while (timer_counter < 250)
    {
        unsigned char a = 0;
    }
    timer_counter = 0;
}

void poll_portA(void)
{
    //
}

void game(void)
{
    // point
    
}

void submit(void)
{
    unsigned int delay_counter = 0;
    while (delay_counter < 62)
    {
        while (!TMR0IF);
        TMR0IF = 0;
        delay_counter++;
    }
}

void rotate(void)
{
   // rotate
}

// ============================ //
//   INTERRUPT SERVICE ROUTINE  //
// ============================ //
__interrupt(high_priority)
void HandleInterrupt()
{
    if (TMR0IF)
    {
        timer_counter++;
        TMR0 = 100;
        TMR0IF = 0;
    }
    
    // B5
    if (INTCONbits.INT0IF && PORTBbits.RB5 == 0)
    {
        submit();
        INTCONbits.INT0IF = 0; // clear flag
    }
    
    // B6
    if (INTCON3bits.INT1IF && PORTBbits.RB6 == 0)
    {
        submit();
        INTCON3bits.INT1IF = 0; // clear flag
    }
}

// ============================ //
//            MAIN              //
// ============================ //
void main()
{
    // Initialize
    init();
    PORTA = 0b11111111;
    one_second_delay();
    PORTA = 0;
    
    // Enable interrupts
    GIE = 1;
    
    while (1)
    {
        // Poll PORTA inputs
        if (PORTAbits.RA0)
        {
            move_right();
        }
        if (PORTAbits.RA1)
        {
            move_up();
        }
        if (PORTAbits.RA2)
        {
            move_down();
        }
        if (PORTAbits.RA3)
        {
            move_left();
        }
    }
    
}