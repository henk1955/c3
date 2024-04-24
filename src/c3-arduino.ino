// Support for development boards

#include <Arduino.h>

#include "c3.h"
#include "file.h"
#include "fshell.h"

#define mySerial Serial

enum { FOPEN=101, FCLOSE, FREAD, FWRITE, FLOAD, BLOAD,
    OPEN_INPUT=110, OPEN_OUTPUT, OPEN_PULLUP,
    PIN_READ, PIN_READA, PIN_WRITE, PIN_WRITEA,
    EDIT_BLK,
    X_PWD, X_CD, X_LS, X_MKDIR, X_RMDIR, X_DELETE, X_RENAME
};

#ifdef mySerial
    void serialInit() { mySerial.begin( 19200);}
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


cell_t sysTime() { return micros(); }

// Cells are always 32-bit on dev boards (no 64-bit)
#define S1(x, y) (*(x)=((y)&0xFF))
#define G1(x, y) (*(x)<<y)
void Store(char *l, cell_t v) { S1(l,v); S1(l+1,v>>8); S1(l+2,v>>16); S1(l+3,v>>24); }
cell_t Fetch(const char *l) { return (*l)|G1(l+1,8)|G1(l+2,16)|G1(l+3,24); }

void loadUserWords() {
    parseF("-ML- PIN-INPUT   %d 3 -MLX- inline", OPEN_INPUT);
    parseF("-ML- PIN-OUTPUT  %d 3 -MLX- inline", OPEN_OUTPUT);
    parseF("-ML- PIN-PULLUP  %d 3 -MLX- inline", OPEN_PULLUP);
    parseF("-ML- DPIN@       %d 3 -MLX- inline", PIN_READ);
    parseF("-ML- APIN@       %d 3 -MLX- inline", PIN_READA);
    parseF("-ML- DPIN!       %d 3 -MLX- inline", PIN_WRITE);
    parseF("-ML- APIN!       %d 3 -MLX- inline", PIN_WRITEA);
    parseF("-ML- FOPEN       %d 3 -MLX- inline", FOPEN);
    parseF("-ML- FCLOSE      %d 3 -MLX- inline", FCLOSE);
    parseF("-ML- FREAD       %d 3 -MLX- inline", FREAD);
    parseF("-ML- FWRITE      %d 3 -MLX- inline", FWRITE);
    parseF("-ML- (LOAD)       %d 3 -MLX- inline", FLOAD);
    parseF("-ML- BLOAD       %d 3 -MLX- inline", BLOAD);
    parseF("-ML- EDIT        %d 3 -MLX- inline", EDIT_BLK);
    parseF("-ML- PWD         %d 3 -MLX- inline", X_PWD);
    parseF("-ML- CD          %d 3 -MLX- inline", X_CD);
    parseF("-ML- LS          %d 3 -MLX- inline", X_LS);
    parseF("-ML- MKDIR       %d 3 -MLX- inline", X_MKDIR);
    parseF("-ML- RMDIR       %d 3 -MLX- inline", X_RMDIR);
    parseF("-ML- DELETE       %d 3 -MLX- inline", X_DELETE);
    parseF("-ML- RENAME       %d 3 -MLX- inline", X_RENAME);
    parseF(": isPC 0 ;");
}

int tryOpen(const char *root, const char *loc, const char *fn) {
    char nm[64];
    strCpy(nm, root);
    strCat(nm, loc);
    strCat(nm, fn);
    // printf("try [%s]\n", nm);
    cell_t fp = fOpen(nm, "rb");
    if (!fp) { return 0; }
    if (input_fp) { ipush(input_fp); }
    input_fp = fp;
    return 1;
}


int lookForFile(const char *name) {
    if (tryOpen("", "", name)) { return 1; }
    // if (tryOpen("", "./", name)) { return 1; }
    // if (tryOpen(root, "/c3/", name)) { return 1; }
    // if (tryOpen(root, "/bin/c3/", name)) { return 1; }
    // if (tryOpen(root, "/bin/", name)) { return 1; }
    return 0;
}

void LFF(char *fn) { if (!lookForFile(fn)) { printStringF("-file[%s]?-", fn); } }


char *doUser(char *pc, int ir) {
  cell_t t, n;
  char fn[16];

  switch (ir) {
    case OPEN_INPUT:   t = pop(); pinMode(t, INPUT);
    RCASE OPEN_OUTPUT: t = pop(); pinMode(t, OUTPUT);
    RCASE OPEN_PULLUP: t = pop(); pinMode(t, INPUT_PULLUP);
    RCASE PIN_READ:    t = pop(); push(digitalRead(t));
    RCASE PIN_READA:   t = pop(); push(analogRead(t));
    RCASE PIN_WRITE:   t = pop(); n = pop(); digitalWrite(t,n);
    RCASE PIN_WRITEA:  t = pop(); n = pop(); analogWrite(t,n);

    RCASE FOPEN:  t=pop(); n=pop(); push(fOpen((char*)n, (char*)t));
    RCASE FCLOSE: t=pop(); fClose(t);
    RCASE FREAD:  t=pop(); n=pop(); push(fRead((uint8_t*)pop(), 1, (int)n, t));
    RCASE FWRITE: t=pop(); n=pop(); push(fWrite((uint8_t*)pop(), 1, (int)n, t));

    RCASE FLOAD:  LFF(cpop());
    RCASE BLOAD:  sprintf(fn, "block-%03d.c3", (int)pop()); lookForFile(fn);    

    // RCASE FLOAD:  n=pop(); t=fOpen((char*)n, "rt");
    //         if (t && input_fp) { ipush(input_fp); }
    //         if (t) { input_fp = t; *in = 0; in = (char*)0; }
    //         else { printStringF("-noFile[%s]-", (char*)n); }
    // RCASE BLOAD:  blockLoad((int)pop());
    RCASE EDIT_BLK: t=pop(); editBlock(t);

    RCASE X_PWD:  handle_PWD();
    RCASE X_CD:   push( handle_CD( (char*) pop()));
    RCASE X_LS:   handle_LS( (char*) pop());
    RCASE X_MKDIR: push( handle_MKDIR( (char*) pop()));
    RCASE X_RMDIR: push( handle_RMDIR( (char*) pop()));
    RCASE X_DELETE: push( handle_DELETE( (char*) pop()));
    RCASE X_RENAME: push( handle_RENAME( (char*) pop()));

    return pc; default: return 0;
  }
}

void setup() {
  serialInit();
  delay(5);
  fileInit();
  c3Init();
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
      printString("\r\n");
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
