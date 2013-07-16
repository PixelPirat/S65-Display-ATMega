/* --------------------------------------------------
      Zuordnung der Displayanschluesse an den 
      Controller
   -------------------------------------------------- */

#define LCD_PORT    PORTB
#define LCD_DIR     DDRB

#define CS_DIR      DDRD
#define CS_PORT     PORTD

#define LCD_RESET   PB0
#define LCD_CS      PD7
#define LCD_RS      PB2                   // SPI: /SS
#define LCD_DATA    PB3                   // SPI: Mosi
#define LCD_CLK     PB5                   // SPI: Clock

/* --------------------------------------------------
      Anschluss fuer PWM gesteuerte Step-Up Wandlung
      zur Erzeugung der LED Spannung fuer Hinter-
      grundbeleuchtung 
   -------------------------------------------------- */

#define pwmdir      DDRB                  // PWM-Port : Erzeugung der Spannung für LED 
                                          // Hintergrundbeleuchtung mittels Step-Up Wandlung
#define pwmport     PORTB
#define pwmout      PB1                   // PB1 / OC1A => Steueranschluss fuer Step-Up Wandlung
#define horizontal  1                     // Querausgabe
#define vertikal    0                     // Ausgabe hochkant

/* --------------------------------------------------
      Prototypen
   -------------------------------------------------- */

static int lcd_fileout(char ch, FILE *stream);
static FILE lcdout = FDEV_SETUP_STREAM(lcd_fileout,NULL,_FDEV_SETUP_WRITE);	// einen Stream zuordnen

void spi_init(void);
void port_init(void);
void spi_out(char data);
void lcd_cmd(char data);
void lcd_data(char data);
void wrtab(PGM_P data, char len);
void pwm_init();
void setpwm(uint16_t tim1, uint16_t tim2);
void lcd_init(void);
void lcd_shutdown(void);
void clrscr(void);
void setoutmode(uint8_t mode);
void putpixel(uint8_t x, uint8_t y, uint8_t f);
void gotoxy(unsigned char x, unsigned char y);
void lcd_putchar(unsigned char ch);
void line(int x0, int y0, int x1, int y1, uint8_t color);
void rectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color);
void ellipse(int xm, int ym, int a, int b, uint8_t color );
void circle(int x, int y, int r, uint8_t color );

void graphchar(uint8_t x, uint8_t y, uint8_t ch);
void outtextxy(uint8_t x, uint8_t y, const uint8_t *msg);
void showbwimage(char ox, char oy, const unsigned char* const image,uint8_t resX,uint8_t resY);

/* --------------------------------------------------
      Initialisierungscodes /  Kommandos fuer 
      das Display
   -------------------------------------------------- */

const char init1[] PROGMEM =
{ 0xFD, 0xFD, 0xFD, 0xFD,
  0xEF, 0x00, 0xEE, 0x04, 0x1B, 0x04, 0xFE, 0xFE, 0xFE, 0xFE, 0xEF, 0x90, 0x4A, 0x04,
  0x7F, 0x3F, 0xEE, 0x04, 0x43, 0x06  };

const char init2[] PROGMEM = 
{ 0xEF, 0x90,  0x09, 0x83,  0x08, 0x00,  0x0B, 0xAF,  0x0A, 0x00,  0x05, 0x00,  0x06, 0x00,
  0x07, 0x00,  0xEF, 0x00,  0xEE, 0x0C,  0xEF, 0x90,  0x00, 0x80,  0xEF, 0xB0,  0x49, 0x02,
  0xEF, 0x00,  0x7F, 0x01,  0xE1, 0x81,  0xE2, 0x02,  0xE2, 0x76,  0xE1, 0x83 };

const char init3[] PROGMEM = 
{ 0x80, 0x01 };


const char shutdown1[] PROGMEM = 
{ 0xEF, 0x00,  0x1B, 0x04,  0xFE, 0xFE,  0xFE, 0xFE,  0x7E, 0x04,  0xE3, 0x04,  0xE4, 0x04,
  0xE2, 0x01,  0x80, 0x00,  0xE0, 0x01,  0x7F, 0x01 };

const char shutdown2[] PROGMEM =
{ 0xE0, 0x00,  0x7E, 0x01 };

const char shutdown3[] PROGMEM = 
{ 0x01, 0x01 };


// Farbpallette, kompatibel zu den 16 CGA/EGA Farben

const char coltab[] =
  { 0x00,0x02,0x10,0x12,0x80,0xa2,0x90,0x6f,0x25,0x03,0x1c,0x1f,0xe0,0xe3,0xfc,0xff };
  
  
/* --------------------------------------------------
      Ascii-Zeichensatz 8x8 Pixel
   -------------------------------------------------- */
  
#define fontsizex   8
#define fontsizey   8

static const unsigned font[][8] PROGMEM={
  {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},  // 0x20     Leerzeichen
  {0x30,0x78,0x78,0x30,0x30,0x00,0x30,0x00},  // 0x21     !
  {0x6C,0x6C,0x6C,0x00,0x00,0x00,0x00,0x00},  // 0x22     "
  {0x6C,0x6C,0xFE,0x6C,0xFE,0x6C,0x6C,0x00},  // 0x23     #
  {0x30,0x7C,0xC0,0x78,0x0C,0xF8,0x30,0x00},  // 0x24     $
  {0x00,0xC6,0xCC,0x18,0x30,0x66,0xC6,0x00},  // 0x25     %
  {0x38,0x6C,0x38,0x76,0xDC,0xCC,0x76,0x00},  // 0x26     &
  {0x60,0x60,0xC0,0x00,0x00,0x00,0x00,0x00},  // 0x27     '
  {0x18,0x30,0x60,0x60,0x60,0x30,0x18,0x00},  // 0x28     (
  {0x60,0x30,0x18,0x18,0x18,0x30,0x60,0x00},  // 0x29     )
  {0x00,0x66,0x3C,0xFF,0x3C,0x66,0x00,0x00},  // 0x2A     *
  {0x00,0x30,0x30,0xFC,0x30,0x30,0x00,0x00},  // 0x2B     +
  {0x00,0x00,0x00,0x00,0x00,0x70,0x30,0x60},  // 0x2C     ,
  {0x00,0x00,0x00,0xFC,0x00,0x00,0x00,0x00},  // 0x2D     -
  {0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00},  // 0x2E     .
  {0x06,0x0C,0x18,0x30,0x60,0xC0,0x80,0x00},  // 0x2F     /
  {0x78,0xCC,0xDC,0xFC,0xEC,0xCC,0x78,0x00},  // 0x30     0
  {0x30,0xF0,0x30,0x30,0x30,0x30,0xFC,0x00},  // 0x31     1
  {0x78,0xCC,0x0C,0x38,0x60,0xCC,0xFC,0x00},  // 0x32     2
  {0x78,0xCC,0x0C,0x38,0x0C,0xCC,0x78,0x00},  // 0x33     3
  {0x1C,0x3C,0x6C,0xCC,0xFE,0x0C,0x0C,0x00},  // 0x34     4
  {0xFC,0xC0,0xF8,0x0C,0x0C,0xCC,0x78,0x00},  // 0x35     5
  {0x38,0x60,0xC0,0xF8,0xCC,0xCC,0x78,0x00},  // 0x36     6
  {0xFC,0xCC,0x0C,0x18,0x30,0x60,0x60,0x00},  // 0x37     7
  {0x78,0xCC,0xCC,0x78,0xCC,0xCC,0x78,0x00},  // 0x38     8
  {0x78,0xCC,0xCC,0x7C,0x0C,0x18,0x70,0x00},  // 0x39     9
  {0x00,0x00,0x30,0x30,0x00,0x30,0x30,0x00},  // 0x3A     :
  {0x00,0x00,0x30,0x30,0x00,0x70,0x30,0x60},  // 0x3B     ;
  {0x18,0x30,0x60,0xC0,0x60,0x30,0x18,0x00},  // 0x3C     <
  {0x00,0x00,0xFC,0x00,0xFC,0x00,0x00,0x00},  // 0x3D     =
  {0x60,0x30,0x18,0x0C,0x18,0x30,0x60,0x00},  // 0x3E     >
  {0x78,0xCC,0x0C,0x18,0x30,0x00,0x30,0x00},  // 0x3F     ?
  {0x7C,0xC6,0xDE,0xDE,0xDE,0xC0,0x78,0x00},  // 0x40     @
  {0x30,0x78,0xCC,0xCC,0xFC,0xCC,0xCC,0x00},  // 0x41     A
  {0xFC,0x66,0x66,0x7C,0x66,0x66,0xFC,0x00},  // 0x42     B
  {0x3C,0x66,0xC0,0xC0,0xC0,0x66,0x3C,0x00},  // 0x43     C
  {0xFC,0x6C,0x66,0x66,0x66,0x6C,0xFC,0x00},  // 0x44     D
  {0xFE,0x62,0x68,0x78,0x68,0x62,0xFE,0x00},  // 0x45     E
  {0xFE,0x62,0x68,0x78,0x68,0x60,0xF0,0x00},  // 0x46     F
  {0x3C,0x66,0xC0,0xC0,0xCE,0x66,0x3E,0x00},  // 0x47     G
  {0xCC,0xCC,0xCC,0xFC,0xCC,0xCC,0xCC,0x00},  // 0x48     H
  {0x78,0x30,0x30,0x30,0x30,0x30,0x78,0x00},  // 0x49     I
  {0x1E,0x0C,0x0C,0x0C,0xCC,0xCC,0x78,0x00},  // 0x4A     J
  {0xE6,0x66,0x6C,0x78,0x6C,0x66,0xE6,0x00},  // 0x4B     K
  {0xF0,0x60,0x60,0x60,0x62,0x66,0xFE,0x00},  // 0x4C     L
  {0xC6,0xEE,0xFE,0xD6,0xC6,0xC6,0xC6,0x00},  // 0x4D     M
  {0xC6,0xE6,0xF6,0xDE,0xCE,0xC6,0xC6,0x00},  // 0x4E     N
  {0x38,0x6C,0xC6,0xC6,0xC6,0x6C,0x38,0x00},  // 0x4F     O
  {0xFC,0x66,0x66,0x7C,0x60,0x60,0xF0,0x00},  // 0x50     P
  {0x78,0xCC,0xCC,0xCC,0xDC,0x78,0x1C,0x00},  // 0x51     Q
  {0xFC,0x66,0x66,0x7C,0x78,0x6C,0xE6,0x00},  // 0x52     R
  {0x78,0xCC,0xE0,0x38,0x1C,0xCC,0x78,0x00},  // 0x53     S
  {0xFC,0xB4,0x30,0x30,0x30,0x30,0x78,0x00},  // 0x54     T
  {0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xFC,0x00},  // 0x55     U
  {0xCC,0xCC,0xCC,0xCC,0xCC,0x78,0x30,0x00},  // 0x56     V
  {0xC6,0xC6,0xC6,0xD6,0xFE,0xEE,0xC6,0x00},  // 0x57     W
  {0xC6,0xC6,0x6C,0x38,0x6C,0xC6,0xC6,0x00},  // 0x58     X
  {0xCC,0xCC,0xCC,0x78,0x30,0x30,0x78,0x00},  // 0x59     Y
  {0xFE,0xCC,0x98,0x30,0x62,0xC6,0xFE,0x00},  // 0x5A     Z
  {0x78,0x60,0x60,0x60,0x60,0x60,0x78,0x00},  // 0x5B     [
  {0xC0,0x60,0x30,0x18,0x0C,0x06,0x02,0x00},  // 0x5C     Backslash
  {0x78,0x18,0x18,0x18,0x18,0x18,0x78,0x00},  // 0x5D     ]
  {0x10,0x38,0x6C,0xC6,0x00,0x00,0x00,0x00},  // 0x5E     ^
  {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF},  // 0x5F     _
  {0x30,0x30,0x18,0x00,0x00,0x00,0x00,0x00},  // 0x60     `
  {0x00,0x00,0x78,0x0C,0x7C,0xCC,0x76,0x00},  // 0x61     a
  {0xE0,0x60,0x7C,0x66,0x66,0x66,0xFC,0x00},  // 0x62     b
  {0x00,0x00,0x78,0xCC,0xC0,0xCC,0x78,0x00},  // 0x63     c
  {0x1C,0x0C,0x0C,0x7C,0xCC,0xCC,0x76,0x00},  // 0x64     d
  {0x00,0x00,0x78,0xCC,0xFC,0xC0,0x78,0x00},  // 0x65     e
  {0x38,0x6C,0x60,0xF0,0x60,0x60,0xF0,0x00},  // 0x66     f
  {0x00,0x00,0x76,0xCC,0xCC,0x7C,0x0C,0xF8},  // 0x67     g
  {0xE0,0x60,0x6C,0x76,0x66,0x66,0xE6,0x00},  // 0x68     h
  {0x30,0x00,0x70,0x30,0x30,0x30,0x78,0x00},  // 0x69     i
  {0x18,0x00,0x78,0x18,0x18,0x18,0xD8,0x70},  // 0x6A     j
  {0xE0,0x60,0x66,0x6C,0x78,0x6C,0xE6,0x00},  // 0x6B     k
  {0x70,0x30,0x30,0x30,0x30,0x30,0x78,0x00},  // 0x6C     l
  {0x00,0x00,0xEC,0xFE,0xD6,0xC6,0xC6,0x00},  // 0x6D     m
  {0x00,0x00,0xF8,0xCC,0xCC,0xCC,0xCC,0x00},  // 0x6E     n
  {0x00,0x00,0x78,0xCC,0xCC,0xCC,0x78,0x00},  // 0x6F     o
  {0x00,0x00,0xDC,0x66,0x66,0x7C,0x60,0xF0},  // 0x70     p
  {0x00,0x00,0x76,0xCC,0xCC,0x7C,0x0C,0x1E},  // 0x71     q
  {0x00,0x00,0xD8,0x6C,0x6C,0x60,0xF0,0x00},  // 0x72     r
  {0x00,0x00,0x7C,0xC0,0x78,0x0C,0xF8,0x00},  // 0x73     s
  {0x10,0x30,0x7C,0x30,0x30,0x34,0x18,0x00},  // 0x74     t
  {0x00,0x00,0xCC,0xCC,0xCC,0xCC,0x76,0x00},  // 0x75     u
  {0x00,0x00,0xCC,0xCC,0xCC,0x78,0x30,0x00},  // 0x76     v
  {0x00,0x00,0xC6,0xC6,0xD6,0xFE,0x6C,0x00},  // 0x77     w
  {0x00,0x00,0xC6,0x6C,0x38,0x6C,0xC6,0x00},  // 0x78     x
  {0x00,0x00,0xCC,0xCC,0xCC,0x7C,0x0C,0xF8},  // 0x79     y
  {0x00,0x00,0xFC,0x98,0x30,0x64,0xFC,0x00},  // 0x7A     z
  {0x1C,0x30,0x30,0xE0,0x30,0x30,0x1C,0x00},  // 0x7B     {
  {0x18,0x18,0x18,0x00,0x18,0x18,0x18,0x00},  // 0x7C
  {0xE0,0x30,0x30,0x1C,0x30,0x30,0xE0,0x00},  // 0x7D     }
  {0x76,0xDC,0x00,0x00,0x00,0x00,0x00,0x00},  // 0x7E
  {0x10,0x38,0x6C,0xC6,0xC6,0xC6,0xFE,0x00},  // 0x7F
  {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},  // 0x80     ausgefuelltes Quadrat
  {0x70,0xd8,0xd8,0x70,0x00,0x00,0x00}        // 0x81     Grad Celcius Zeichen
};

uint8_t aktxp;                                // Beinhaltet die aktuelle Position des Textcursors in X-Achse
uint8_t aktyp;                                // dto. fuer die Y-Achse
uint8_t textcolor;                            // Beinhaltet die Farbwahl fuer die Vordergrundfarbe
uint8_t bkcolor;                              // dto. fuer die Hintergrundfarbe
uint8_t outmode;
uint8_t textsize = 0;                         // Skalierung der Ausgabeschriftgroesse

/* --------------------------------------------------
         Farben:
         Im 8 Bit Farbenmodus ist die Farbe wie folgt
         definiert:

                 |   rot      |   gruen    |  blau  |
         Bits:   | D7  D6  D5 | D4  D3  D2 | D1  D0 |
   -------------------------------------------------- */

#define black            0x00
#define blue             0x02
#define green            0x10
#define cyan             0x12
#define red              0x80
#define magenta          0xa2
#define brown            0x90
#define lightgrey        0x6f
#define grey             0x25
#define lightblue        0x03
#define lightgreen       0x1c
#define lightcyan        0x1f
#define lightred         0xe0
#define lightmagenta     0xe3
#define yellow           0xfc
#define white            0xff



/* --------------------------------------------------
        spi_init
        Initialisierung der SPI Funktionen zur
        Kommunikation mit dem Display
   -------------------------------------------------- */

void spi_init(void)
{
     // MOSI, SCK und /SS sind Ausgaenge
     DDRB |= (1 << LCD_DATA) | (1 << LCD_RS)  | (1 << LCD_CLK);

     //  SPI erlauben  |    Master
     SPCR = (1 << SPE) | (1 << MSTR);

     // Taktrate F_CPU/2
     SPSR = (1 << SPI2X);
}

/* --------------------------------------------------
        port_init
        initialisiert die Portanschluesse des 
        Displays am Controller als Ausgaenge und
        erzeugt einen Reset-Impuls am Display
   -------------------------------------------------- */

void port_init(void)
{
    LCD_PORT &= ~(1 << LCD_RESET);
    LCD_DIR  |=  (1 << LCD_RESET);

    CS_PORT  |=  (1 << LCD_CS);
    CS_DIR   |=  (1 << LCD_CS);

    LCD_PORT |=  (1 << LCD_RS);
    LCD_DIR  |=  (1 << LCD_RS);
    _delay_ms(50);

    LCD_PORT |= (1 << LCD_RESET);

    _delay_ms(50);
}


/* --------------------------------------------------
        spi_out
        sendet ein Byte ueber das SPI an das 
        Display
        
        Parameter:  data => zu sendendes Byte
   -------------------------------------------------- */

void spi_out(char data)
{
    // Daten in das SPI Register schreiben
    SPDR = data;
    // warten bis die Daten gesendet worden sind
    while( (SPSR & 0x80) != 0x80 );
}

/* --------------------------------------------------
        lcd_cmd
        sendet ein Kommando an das Display
        
        Parameter: data => zu sendendes Kommandobyte
   -------------------------------------------------- */

void lcd_cmd(char data)
{
    // Kommunikation zum Display aktivieren
    CS_PORT &= ~(1 << LCD_CS);

    // RS = 1 ==> an das Display geschickter Wert
    //            wird vom Display als Kommando
    //            interpretiert
    LCD_PORT |=  (1 << LCD_RS);

    spi_out(data);

    // Displaykommunikation deaktivieren
    CS_PORT |= (1 << LCD_CS);
}

/* --------------------------------------------------
        lcd_data
        sendet ein Kommando an das Display
        
        Parameter: data => zu sendendes Datum
   -------------------------------------------------- */

void lcd_data(char data)
{
    // Kommunikation zum Display aktivieren
    CS_PORT &= ~(1 << LCD_CS);

    // RS = 0 ==> an das Display geschickter Wert
    //            wird vom Display als Datum
    //            interpretiert
    LCD_PORT &= ~(1 << LCD_RS);

    spi_out(data);

    CS_PORT |= (1 << LCD_CS);
}

/* --------------------------------------------------
        wrtab
        sendet Steuerkommandos die in einem Array
        abgelegt sind ueber SPI an das Display
        
        
        Parameter:
            data => Zeiger auf im Programmspeicher
                    abgelegtes Array
            len  => Anzahl zu sendender Werte
   -------------------------------------------------- */

void wrtab(PGM_P data, char len)
{
    char c;
    while(len--)
    {
        c = pgm_read_byte(data++);
        lcd_cmd(c);
    }
}

/* --------------------------------------------------
        pwminit
        Initialisiert eine Pulsweitenmodulation
        PWM dient hier ueber variable Verhaeltnisse
        von Puls und Pause zur Erzeugung der LED-
        Spannung der Hintergrundbeleuchtung ueber 
        einen hieran angeschlossenen Step-Up Wandler
   -------------------------------------------------- */

void pwm_init()
{
    // OC1A .. PB1 als Ausgang
    pwmdir |= (1 << pwmout);

    // WGM11 WGM12, WGM13 = 1 Modus14 .. Prescaler durch CS10, CS11, CS12 gesetzt
    TCCR1A = (1 << COM1A1) | (1 << WGM11);
    TCCR1B = (1 << WGM12)  | (1 << WGM13) | (1 << CS11);
}

/* --------------------------------------------------
        setpwm
        definiert das Verhaeltnis von Gesamtzeitdauer
        zu Pulsdauer der PWM.
        
        SETPWM definiert somit die Helligkeit der
        Hintergrundbeleuchtung des Displays

        Parameter:
            tim1  => Gesamtzeitdauer
            tim2  => Pulsdauer
            
        Anmerkung: hier ist eine vorsichtige Be-
        nutzung angezeigt, da hier "heftige" Werte
        zu erhoehter Stromaufnahme fuehren.
        
        Eine gute Ausleuchtung findet mit der Ein-
        stellung (80,8) statt. Soll Strom gespart 
        werden und das Display dunkler geschaltet
        werden empfiehlt sich die Einstellung (80/4)
   -------------------------------------------------- */

void setpwm(uint16_t tim1, uint16_t tim2)
{
    ICR1H  = (tim1 >> 8);
    ICR1L  = (uint8_t) tim1 & 0x00ff;
    OCR1AH = (tim2 >> 8);
    OCR1AL = (uint8_t) tim2 & 0x00ff;
}


/* --------------------------------------------------
        lcd_init
        Initialisiert das Display und bereitet es
        zur Benutzung im 8 Farbenmodus vor.
        
        Hierbei werden die benoetigten Ports als
        Ausgabeports definiert und das SPI 
        initialisiert.
   -------------------------------------------------- */

void lcd_init(void)
{
    // SPI in Betrieb setzen
    spi_init();
    port_init();

    wrtab(init1, 24);
    _delay_ms(7);
    wrtab(init2, 40);
    _delay_ms(50);
    wrtab(init3, 2);

    // Display auswaehlen
    LCD_PORT &= ~(1 << LCD_CS);

    lcd_cmd(0xEF);
    lcd_cmd(0x90);
    lcd_cmd(0x00);
    lcd_cmd(0x00);

    /* --------------------------------------------------
        Rechteckgenerator fuer Step-Up Wandler
        Puls-Pauseverhaeltnis fuer Rechteckwandler
        festlegen (entspricht Hintergrundbeleuchtung)
    -------------------------------------------------- */
    pwm_init();
    setpwm(80,8);
                              
    _delay_ms(100);

    // 256 Farbenmodus setzen

    lcd_cmd(0xEF);
    lcd_cmd(0x90);
    lcd_cmd(0xE8);
    lcd_cmd(0x00);

    //outmode = horizontal;
    outmode = vertikal;
}

/* --------------------------------------------------
        lcd_shutdown
        faehrt das Display herunter
   -------------------------------------------------- */

void lcd_shutdown(void)
{
    wrtab(shutdown1, 22);
    _delay_us(10);
    wrtab(shutdown2, 4);
    _delay_us(10);
    wrtab(shutdown3, 2);
}

/* --------------------------------------------------
        clrscr
        loescht das Display mit der aktuell gesetzen
        Hintergrundfarbe
   -------------------------------------------------- */

void clrscr(void)
{
    uint16_t cnt;
    lcd_cmd(0xEF);
    lcd_cmd(0x90);
    lcd_cmd(0x05);
    lcd_cmd(0x00);
    lcd_cmd(0x06);
    lcd_cmd(0x00);
    lcd_cmd(0x07);
    lcd_cmd(0x00);
    for(cnt = 0; cnt < (132*176); cnt++)
    {
        lcd_data(bkcolor);
    }
}

/* --------------------------------------------------
        setoutmode
        estimmt, ob die Ausgabe auf dem Display
        horizontal oder vertikal ausgegeben werden 
        soll.
        Bei der Initialisierung des Displays ist
        vertikal eingestellt
        
        Parameter:
            mode = 0 => vertikale Ausgabe (hochkant)
            mode = 1 => horizontale Ausgabe (quer)
        Parameter:
             x ==> X-Koordinate
             y ==> Y-Koordinate
             f ==> Farbwert
   -------------------------------------------------- */

void setoutmode(uint8_t mode)
{
    outmode = mode;
}

/* --------------------------------------------------
        putpixel
        setzt einen Pixel auf das Display an der
        angegebenen Koordinate mit einer angegebenen
        Farbe. Der Outputmode bestimmt, ob das Pixel
        horizontal oder vertikal ausgegeben wird
        ( festzulegen mit <setoutputmode> ).
        Die Aufloesung des Displays betraegt 132x176
        Pixel
        
        Parameter:
             x => X-Koordinate des Pixels
             y => Y-Koordinate des Pixels
   -------------------------------------------------- */

void putpixel(uint8_t x, uint8_t y, uint8_t f)
{
     if (outmode == 0)
     {
       lcd_cmd(0xEF);
       lcd_cmd(0x90);
       lcd_cmd(0x07);
       lcd_cmd(y);
       lcd_cmd(0x06);
       lcd_cmd(x);
       lcd_data(f);
     }
     else
     {
       lcd_cmd(0xEF);
       lcd_cmd(0x90);
       lcd_cmd(0x07);
       lcd_cmd(175-x);
       lcd_cmd(0x06);
       lcd_cmd(y);
       lcd_data(f);
     }
}


/* --------------------------------------------------
        gotoxy
        Setzt den Textcursor an die angegebene
        Textkoordinate.
        
        Parameter:
            x = X-Koordinate (0..15)
            y = Y-Koordinate (0..21)
   -------------------------------------------------- */

void gotoxy(unsigned char x, unsigned char y)
{
  aktxp = x * (fontsizex + (textsize * fontsizex));
  aktyp = y * (fontsizey + (textsize * fontsizey));
}

/* --------------------------------------------------
        lcd_putchar
        setzt ein Ascii-Zeichen an der aktuellen
        Textcursorposition auf das Display.
        
        Die Variable <textsize> gibt an, ob die Ausgabe
        in einfacher oder doppelter Groesse erfolgt.
        
        Parameter:
            ch => anzuzeigendes Zeichen
   -------------------------------------------------- */

void lcd_putchar(unsigned char ch)
{
    uint8_t i,i2;
    uint8_t b;
    uint8_t oldx,oldy;

    if (ch == 13)                                          // Fuer <printf> "/r" Implementation
    {
      aktxp = 0;
      return;
    }
    if (ch == 10)                                          // fuer <printf> "/n" Implementation
    {
      aktyp = aktyp + fontsizey + (fontsizey * textsize);
      return;
    }

    oldx = aktxp;
    oldy = aktyp;
    for (i = 0; i < fontsizey; i++)
    {
      b=pgm_read_byte( &(font[(ch-32)][i]) );
      for (i2 = 0; i2 < fontsizex; i2++)
      {
        if (0x80 & b)
        {
          putpixel(oldx, oldy, textcolor);
          if (textsize == 1) // 2 klammern entfernt !!!
          {
            putpixel(oldx + 1, oldy    , textcolor);
            putpixel(oldx    , oldy + 1, textcolor);
            putpixel(oldx + 1, oldy + 1, textcolor);
          }
        }
        else
        { 
          putpixel(oldx, oldy, bkcolor);
          if ( textsize == 1 ) // 2 klammern entfernt !!!
          {
            putpixel(oldx + 1, oldy    , bkcolor);
            putpixel(oldx    , oldy + 1, bkcolor);
            putpixel(oldx + 1, oldy + 1, bkcolor);
          }
        }
        b = b << 1;
        oldx++;
        if (textsize == 1) { oldx++; } // 2 klammern entfernt !!!
      }
      oldy++;
      if (textsize == 1) { oldy++; } // 2 klammern entfernt !!!
      oldx = aktxp;
    }
    aktxp = aktxp + fontsizex + (fontsizex * textsize);
}


/* -------------------------------------------------------------
        LCD_FILEOUT

        Dateiausgabe. Ein Zeichen wird auf dem angegebenen File aus-
        gegeben!!!

         Wird die Standardausgabe hierhin umgeleitet sendet bspw.
         <printf> jedes Zeichen an das Display
   ------------------------------------------------------------- */

static int lcd_fileout(char ch, FILE *stream)
{
    lcd_putchar(ch);
    return 0;
}

/* -------------------------------------------------------------
        line

        Zeichnet eine Linie von den Koordinaten x0,y0 zu x1,y1
        mit der angegebenen Farbe

        Parameter:
           x0,y0 = Koordinate linke obere Ecke
           x1,y1 = Koordinate rechte untere Ecke
           color = Zeichenfarbe
         Linienalgorithmus nach Bresenham (www.wikipedia.org)
   ------------------------------------------------------------- */

void line(int x0, int y0, int x1, int y1, uint8_t color)
{
    // Linienalgorithmus nach Bresenham (www.wikipedia.org)

    int dx  =  abs(x1 - x0), sx = x0<x1 ? 1 : -1;
    int dy  = -abs(y1 - y0), sy = y0<y1 ? 1 : -1;
    int err = dx + dy, e2;                                     /* error value e_xy */

    for(;;)
    {
      putpixel(x0, y0, color);
      if (x0 == x1 && y0 == y1) break;
      e2 = 2*err;
      if (e2 > dy) { err += dy; x0 += sx; }                  /* e_xy+e_x > 0 */
      if (e2 < dx) { err += dx; y0 += sy; }                  /* e_xy+e_y < 0 */
    }
}

/* -------------------------------------------------------------
        rectangle

        Zeichnet ein Rechteck von den Koordinaten x0,y0 zu x1,y1
        mit der angegebenen Farbe

        Parameter:
           x0,y0 = Koordinate linke obere Ecke
           x1,y1 = Koordinate rechte untere Ecke
           color = Zeichenfarbe
   ------------------------------------------------------------- */

void rectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color)
{
    line(x1, y1, x2, y1, color);
    line(x2, y1, x2, y2, color);
    line(x1, y2, x2, y2, color);
    line(x1, y1, x1, y2, color);
}

/* -------------------------------------------------------------
        ellipse

        Zeichnet eine Ellipse mit Mittelpunt an der Koordinate 
        xm,ym mit den Hoehen- Breitenverhaeltnis a:b
        mit der angegebenen Farbe

        Parameter:
           xm,ym = Koordinate des Mittelpunktes der Ellipse
           a,b   = Hoehen- Breitenverhaeltnis
           color = Zeichenfarbe
   
        Ellipsenalgorithmus nach Bresenham (www.wikipedia.org)
   ------------------------------------------------------------- */

void ellipse(int xm, int ym, int a, int b, uint8_t color )
{
    // Algorithmus nach Bresenham (www.wikipedia.org)

    int dx = 0, dy = b;                       // im I. Quadranten von links oben nach rechts unten
    long a2 = a*a, b2 = b*b;
    long err = b2-(2*b-1)*a2, e2;             // Fehler im 1. Schritt */

    do
    {
      putpixel(xm + dx, ym + dy, color);            // I.   Quadrant
      putpixel(xm - dx, ym + dy, color);            // II.  Quadrant
      putpixel(xm - dx, ym - dy, color);            // III. Quadrant
      putpixel(xm + dx, ym - dy, color);            // IV.  Quadrant

      e2 = 2 * err;
      if (e2 <  (2*dx+1)*b2) { dx++; err += (2*dx+1)*b2; }
      if (e2 > -(2*dy-1)*a2) { dy--; err -= (2*dy-1)*a2; }
    } while (dy >= 0);

    while (dx++ < a)                        // fehlerhafter Abbruch bei flachen Ellipsen (b=1)
    {
      putpixel(xm + dx, ym, color);             // -> Spitze der Ellipse vollenden
      putpixel(xm - dx, ym, color);
    }
}

/* -------------------------------------------------------------
        circle

        Zeichnet einen Kreis mit Mittelpunt an der Koordinate xm,ym
        und dem Radius r mit der angegebenen Farbe

        Parameter:
           xm,ym = Koordinate des Mittelpunktes der Ellipse
           r     = Radius des Kreises
           color = Zeichenfarbe

   ------------------------------------------------------------- */

void circle(int x, int y, int r, uint8_t color )
{
    ellipse(x, y, r, r, color);
}

/* -------------------------------------------------------------
        graphchar

        gibt ein Zeichen an den Grafikkoordinaten x,y aus.
        Hierbei wird das Zeichen "auf das Display" gesetzt. Das
        bedeutet, dass der Hintergrund auf dem das Zeichen ge-
        setzt wird, NICHT geloescht wird.
        
        Die Variable <textsize> gibt an, ob die Ausgabe 
        in einfacher oder doppelter Groesse erfolgt.
        
        Parameter:
          x,y => Position (in Grafikpixeln) an der das Zeichen
                 ausgegeben wird
          ch  => auszugebendes Zeichen
   ------------------------------------------------------------- */

void graphchar(uint8_t x, uint8_t y, uint8_t ch)
{
  uint8_t i,i2;
  uint8_t b;
  uint8_t oldx,oldy;


  oldx = x;
  oldy = y;
  if ((textsize==1))
  {
    for (i=0; i<fontsizey; i++)
    {
      b=pgm_read_byte(&(font[(ch-32)][i]));
      for (i2= 0; i2<fontsizex;i2++)
      {
        if (0x80 & b)
        {
          putpixel(oldx,oldy,textcolor);
          putpixel(oldx+1,oldy,textcolor);
          putpixel(oldx,oldy+1,textcolor);
          putpixel(oldx+1,oldy+1,textcolor);
        }
        b= b<<1;
        oldx++;
        oldx++;
      }
      oldy++;
      oldy++;
      oldx=x;
    }
  }
  else
  {
    for (i=0; i<fontsizey; i++)
    {
      b=pgm_read_byte(&(font[(ch-32)][i]));
      for (i2= 0; i2<fontsizex;i2++)
      {
        if (0x80 & b)
        {
          putpixel(oldx,oldy,textcolor);
        }
        b= b<<1;
        oldx++;
      }
      oldy++;
      oldx=x;
    }
  }
}

/* -------------------------------------------------------------
        outtextxy

        gibt einen String an den Grafikkoordinaten x,y aus.
        Hierbei wird der String "auf das Display" gesetzt. Das
        bedeutet, dass der Hintergrund auf dem der String ge-
        setzt wird, NICHT geloescht wird.
        
        Die Variable <textsize> gibt an, ob die Ausgabe
        in einfacher oder doppelter Groesse erfolgt.
        
        Parameter:
          x,y  => Position (in Grafikpixeln) an der das Zeichen
                 ausgegeben wird
          *msg => Zeiger auf auszugebenden String
   ------------------------------------------------------------- */

void outtextxy(uint8_t x, uint8_t y, const uint8_t *msg)
{
  while (*msg)
  {
    graphchar(x, y, *msg++);
    x = x + fontsizex;
    if ((textsize == 1)) { x = x + fontsizex; }
  }
}

/* -------------------------------------------------------------
        showbwimage

        zeigt ein monochromes Bitmap auf dem Display an
        
        Parameter
           ox,oy => linke obere Koordinate des anzuzeigenden
                    Bitmaps
           image => Zeiger auf die Daten des Bitmaps im
                    Programmspeicher
           resx  => Anzahl X-Pixel des Bitmaps
           resy  => Anzahl Y-Pixel des Bitmaps
   ------------------------------------------------------------- */

void showbwimage(char ox, char oy, const unsigned char* const image,uint8_t resX,uint8_t resY)
{
  int x,y;
  char b,bp;
  for (y=0;y< resY;y++)
  {
    for (x= 0;x<resX;x++)
    {
      b= pgm_read_byte(&(image[y *resX +x]));
      for (bp=8;bp>0;bp--)
      {
        if (b & (1 << (bp-1)))
        {
            putpixel(ox + (x * 8) + 8 - bp, oy + y, textcolor);
        }
        else
        {
            putpixel(ox + (x * 8) + 8 - bp, oy + y, bkcolor);
        }
      }
    }
  }
}
