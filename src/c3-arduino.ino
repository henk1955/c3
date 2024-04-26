// Support for development boards

#include <Arduino.h>

#include "c3.h"
#include "file.h"
#include "fshell.h"

#define mySerial Serial

#ifdef mySerial
    void serialInit() { mySerial.begin( 115200);}
    void printChar(char c) { mySerial.print(c); }
    void printString(const char *s) { mySerial.print(s); }
    int qKey() {
      delay(0); 
      if( input_fp){
        if( fAvailable( input_fp)) return fAvailable( input_fp);
        fClose( input_fp);
        input_fp = ipop();
        return qKey();
      }
      return  mySerial.available();
    }
    int key() { 
      char c;
        while (!qKey()) {}
        if( input_fp) c=fReadC( input_fp);
        else c=mySerial.read();
        return c;
    }
#else
    void serialInit() { }
    void printChar(char c) {}
    void printString(char *s) {}
    int qKey() { return 0; }
    int key() { return 0; }
#endif

void intercept(){
  yield();
}


cell_t sysTime() { return micros(); }
void reboot(){ ESP.reset();}

// Cells are always 32-bit on dev boards (no 64-bit)
#define S1(x, y) (*(x)=((y)&0xFF))
#define G1(x, y) (*(x)<<y)
void Store(char *l, cell_t v) { S1(l,v); S1(l+1,v>>8); S1(l+2,v>>16); S1(l+3,v>>24); }
cell_t Fetch(const char *l) { return (*l)|G1(l+1,8)|G1(l+2,16)|G1(l+3,24); }


#include "user-words.h"

void setup() {
  serialInit();
  delay(5);
  fileInit();
  c3Init();

  lookForFile( "block-001.c3");

  input_fp = 0;
  in = (char*)0;

  printString(" ok\r\n");
}

void idle() {
  // TODO
}



void loop() {
  if (qKey() == 0) { idle(); return; }
  int c = key();
  if (!in) {
      in = tib;
      fill(tib, 0, sizeof(tib));
  }

  if (c==9) { c = 32; }
  if (c==13 || c==10) {
      if( ! input_fp) printString("\r\n");
      *(in) = 0;
      ParseLine(tib);
      if( ! input_fp) printString(" ok\r\n");
      in = 0;
  } else if ((c==8) || (c==127)) {
      if ((--in) < tib) { in = tib; }
      else { if( ! input_fp){ PC(8); PC(32); PC(8);} }
  } else {
      if (BTW(c,32,126)) { *(in++) = c; if( ! input_fp) PC(c); }
  }
}
