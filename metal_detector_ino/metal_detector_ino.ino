// Arduino based metal detector
// (C)Dzl july 2013
// http://dzlsevilgeniuslair.blogspot.dk/

// Connect search coil oscillator (20-200kHz) to pin 5
// Connect piezo between pin 13 and GND
// Connect NULL button between pin 12 anf GND

// REMEMBER TO PRESS NULL BUTTON AFTER POWER UP!!


#define SET(x,y) (x |=(1<<y))		        //-Bit set/clear macros
#define CLR(x,y) (x &= (~(1<<y)))       	// |
#define CHK(x,y) (x & (1<<y))           	// |
#define TOG(x,y) (x^=(1<<y))            	//-+
#define SERIAL_BAUDRATE  115200

unsigned long t0=0;         //-Last time
int t=0;                    //-time between ints
unsigned char tflag=0;      //-Measurement ready flag

float SENSITIVITY= 2000.0;  //-Guess what

//-Generate interrupt every 1000 oscillations of the search coil
SIGNAL(TIMER1_COMPA_vect)
{
  OCR1A+=1000;
  t=micros()-t0;
  t0+=t;
  tflag=1;
}

void setup()
{
  //Serial.begin(SERIAL_BAUDRATE);
  pinMode(13,OUTPUT);    //-piezo pin
  digitalWrite(12,HIGH); //-NULL SW. pull up
  //-Set up counter1 to count at pin 5
  TCCR1A=0;
  TCCR1B=0x07;
  SET(TIMSK1,OCF1A);
}
//-Float ABS
float absf(float f)
{
  if(f<0.0)
    return -f;
  else
    return f;
}

int   v0=0;  //-NULL value
float f=0;   //-Measurement value
unsigned int FTW=0;    //-Click generator rate
unsigned int PCW=0;    //-Click generator phase
unsigned long timer=0; //-Click timer
void loop()
{
  if(tflag)
  {
    if(digitalRead(12)==LOW)  //-Check NULL SW.
      v0=t;                   //-Sample new null value
    f=f*0.9+absf(t-v0)*0.1;   //-Running average over 10 samples
    tflag=0;                  //-Reset flag

    float clf=f*SENSITIVITY;  //-Convert measurement to click frequency
    if(clf>10000)            
      clf=10000;
    FTW=clf;
    //Serial.print("Frecuency: ");
    //Serial.println(f);
  }
  
  //-Click generator
  if(millis()>timer)
  {
    timer+=10;
    PCW+=FTW;
    if(PCW&0x8000)
    {
      digitalWrite(13,HIGH);
      PCW&=0x7fff; 
    }
    else
      digitalWrite(13,LOW);
  }
}

