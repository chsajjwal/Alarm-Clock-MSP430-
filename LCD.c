
#include "LCD.h"

  UInt8 tsec0 = 0;
  UInt8 tsec1 = 0;
  UInt8 tsec2 = 0;
  UInt8 pos = 1;
  UInt8 sec1 =0;
  UInt8 sec2 =0;
  UInt8 min1 =0;
  UInt8 min2 =0;
int count=0;

Int8 sec12, sec22 , min12, min22;
int countsec22=0, countmin12=0;
 int csec22;
 int cmin12;
 int cmin22;
// LCD Segment Mapping
const UInt8 LCD_Char_Map[] =
{
  LCD_A+LCD_B+LCD_C+LCD_D+LCD_E+LCD_F,        // '0' or 'O'
  LCD_B+LCD_C,                                // '1' or 'I'
  LCD_A+LCD_B+LCD_D+LCD_E+LCD_G,              // '2' or 'Z'
  LCD_A+LCD_B+LCD_C+LCD_D+LCD_G,              // '3'
  LCD_B+LCD_C+LCD_F+LCD_G,                    // '4' or 'y'
  LCD_A+LCD_C+LCD_D+LCD_F+LCD_G,              // '5' or 'S'
  LCD_A+LCD_C+LCD_D+LCD_E+LCD_F+LCD_G,        // '6' or 'b'
  LCD_A+LCD_B+LCD_C,                          // '7'
  LCD_A+LCD_B+LCD_C+LCD_D+LCD_E+LCD_F+LCD_G,  // '8' or 'B'
  LCD_A+LCD_B+LCD_C+LCD_F+LCD_G,              // '9' or 'g'
  LCD_A+LCD_B+LCD_C+LCD_E+LCD_F+LCD_G,        // 'A'
  LCD_A+LCD_D+LCD_E+LCD_F,                    // 'C'
  LCD_B+LCD_C+LCD_D+LCD_E+LCD_G,              // 'd'
  LCD_A+LCD_D+LCD_E+LCD_F+LCD_G,              // 'E'
  LCD_A+LCD_E+LCD_F+LCD_G,                    // 'F'
  LCD_B+LCD_C+LCD_E+LCD_F+LCD_G,              // 'H'
  LCD_B+LCD_C+LCD_D+LCD_E,                    // 'J'
  LCD_D+LCD_E+LCD_F,                          // 'L'
  LCD_A+LCD_B+LCD_E+LCD_F+LCD_G,              // 'P'
  LCD_B+LCD_C+LCD_D+LCD_E+LCD_F               // 'U'
};

const UInt8 LCD_MAX_CHARS = (sizeof(LCD_Char_Map)/sizeof(UInt8));


//
// Initialize LCD_A
//
void initLCD_A(void)
{
  // Clear LCD memory
  clrLCD();

  // Configure COM0-COM3 and R03-R33 pins
  P5SEL |= (BIT4 | BIT3 | BIT2);
  P5DIR |= (BIT4 | BIT3 | BIT2);

  // Configure LCD_A
  LCDACTL = LCDFREQ_128 | LCDMX1 | LCDMX0 | LCDSON | LCDON;
  LCDAPCTL0 = LCDS4 | LCDS8 | LCDS12 | LCDS16 | LCDS20 | LCDS24;
  LCDAPCTL1 = 0;
  LCDAVCTL0 = LCDCPEN;
  LCDAVCTL1 = VLCD_2_60;
}


//
// Clear LCD
//
void clrLCD(void)
{
  int i;

  for(i = LCD_MEM_OFFSET; i < (LCD_MEM_OFFSET+LCD_MEM_LOC);  i++)
  {
    LCDMEM[i] = 0;
  }
}


//
// Display all segments on LCD
//
void dispAllLCDSegs(void)
{
  int i;

  for(i = LCD_MEM_OFFSET; i < (LCD_MEM_OFFSET+LCD_MEM_LOC);  i++)
  {
    LCDMEM[i] = 0xff;
  }
}


//
// Display character on LCD
//
void dispChar(UInt8 pos, UInt8 index)
{
  LCDMEM[pos + LCD_MEM_OFFSET] &= ~LCD_Char_Map[8];

  if( pos < LCD_NUM_DIGITS )
  {
    if( index < LCD_MAX_CHARS )
    {
      LCDMEM[pos + LCD_MEM_OFFSET] |= LCD_Char_Map[index];
    }
  }
}


#if LCD_TEST > 0

void testChar(void)
{

  dispChar(0, tsec0++);
  dispChar(1, tsec1);
  dispChar(2, tsec2);
  dispChar(3, sec1);
  dispChar(4, sec2);
  dispChar(5, min1);
  dispChar(6, min2);
  if(tsec0==10){
    tsec0=0;
    tsec1++;
  }
  if(tsec1==10){
    tsec1=0;
    tsec2++;
  }
  if(tsec2==10){
    tsec2=0;
    sec1++;
  }
  if(sec1==10){
    sec1=0;
    sec2++;
  }
  if(sec2==6){
    sec2=0;
    min1++;
  }
  if(min1==10){
    min1=0;
    min2++;
  }
}

void testSpecialChar(void)
{
static UInt16 mask = 20;
dispSpecialChar(mask);

}

void dispSpecialChar(UInt16 mask)
{
     UInt16 x;
     UInt8 index = 0;
        if( mask & LCD_8BC )
        {
              LCDMEM[10+LCD_MEM_OFFSET] |= BIT3;
        }
        else
        {
              LCDMEM[10+LCD_MEM_OFFSET] &= ~BIT3;
        }
        for(x = 1; x < LCD_8BC; x <<= 1)
        {
            if( x & mask )
            {
                LCDMEM[index+LCD_MEM_OFFSET] |= BIT7;
            }                 
              else
            {
                LCDMEM[index+LCD_MEM_OFFSET] &= ~(BIT7);
            }
        ++index;
        }
}

void resetTime(void){
  for(int i=0;i<=6;i++){
    dispChar(i, 0);
  } 
tsec0 = 0;
tsec1 = 0;
tsec2 = 0;
sec1 =0;
sec2 =0;
min1 =0;
min2 =0;
}


void decreaseTime(){
      testSpecialChar();
      
      sec12--;
      
      dispChar(0, 0);
      dispChar(1, 0);
      dispChar(2, 0);
      dispChar(3, sec12);
      dispChar(4, sec22);
      
     
      dispChar(5, min12);
      dispChar(6, min22);
     
      if(sec12==0 && sec22==0 && min12==0 && min22==0){
        TACCTL0&= ~CCIE;
        P3DIR |= BIT5;            // P3.5 output 
        P3SEL |= BIT5;            // P3.5 special function (TB0 output)  
        TB0CCTL4 = OUTMOD_4;      // TB0 output is in toggle mode 
        TB0CTL = TBSSEL_2 + MC_1; // SMCLK is clock source, Continuous mode
        TB0CCR0 = 637;
      }
      
      if(sec12==0){
        sec12=10;
        sec22--;
        countsec22++;
      }
      if(countsec22==(csec22+1) && cmin12==min12){
        sec22=5;
        min12--;
        countsec22=0;
        countmin12++;
      }
      else if(countsec22==6){
        sec22=5;
        min12--;
        countsec22=0;
        countmin12++;
      }
      /*if(sec22==5 && countmin12>=2 && min22!=0){
        min12=9;
        countmin12=0;
      }*/
      /*if(countmin12==2){
        min12=9;
        countmin12=0;
      }*/
      if(countmin12==cmin12+1 &&min22<=cmin22 && min22!=0){
        min12=9;
        min22--;
        countmin12=0;
      }
      /*if(min12==0 && min22!=0){
        min12=9;
        min22--;
      
      }
      else if(min12==0 && min22==0){
        min12=9;
        min22=0;
      }8?*/
      
}
void setTime(Int8 sec1, Int8 sec2 , Int8 min1, Int8 min2){
    sec12= sec1+1;
    sec22=sec2;
    min12=min1;
    min22=min2;
    csec22=sec22;
    cmin12=min12;
    cmin22=min22;
}


#endif // LCD_TEST
