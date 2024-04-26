#include <ESP8266WiFi.h>

enum {
  FOPEN=101, FCLOSE, FREAD, FWRITE, FLOAD, BLOAD,
  EDIT_BLK,

  OPEN_INPUT=110, OPEN_OUTPUT, OPEN_PULLUP,
  PIN_READ, PIN_READA, PIN_WRITE, PIN_WRITEA,

  X_PWD, X_CD, X_LS, X_MKDIR, X_RMDIR, X_DELETE, X_RENAME,

  X_WIFI_CONNECT, X_WIFI_STATUS, X_WIFI_IP
};

void LFF(char *fn);

void loadUserWords() {
    parseF("-ML- FOPEN   %d 3 -MLX- inline", FOPEN);
    parseF("-ML- FCLOSE  %d 3 -MLX- inline", FCLOSE);
    parseF("-ML- FREAD   %d 3 -MLX- inline", FREAD);
    parseF("-ML- FWRITE  %d 3 -MLX- inline", FWRITE);
    parseF("-ML- (LOAD)  %d 3 -MLX- inline", FLOAD);
    parseF("-ML- LOAD    %d 3 -MLX- inline", BLOAD);
    parseF("-ML- EDIT    %d 3 -MLX- inline", EDIT_BLK);

    parseF("-ML- PIN-INPUT   %d 3 -MLX- inline", OPEN_INPUT);
    parseF("-ML- PIN-OUTPUT  %d 3 -MLX- inline", OPEN_OUTPUT);
    parseF("-ML- PIN-PULLUP  %d 3 -MLX- inline", OPEN_PULLUP);
    parseF("-ML- DPIN@       %d 3 -MLX- inline", PIN_READ);
    parseF("-ML- APIN@       %d 3 -MLX- inline", PIN_READA);
    parseF("-ML- DPIN!       %d 3 -MLX- inline", PIN_WRITE);
    parseF("-ML- APIN!       %d 3 -MLX- inline", PIN_WRITEA);

    parseF("-ML- PWD     %d 3 -MLX- inline", X_PWD);
    parseF("-ML- CD      %d 3 -MLX- inline", X_CD);
    parseF("-ML- LS      %d 3 -MLX- inline", X_LS);
    parseF("-ML- MKDIR   %d 3 -MLX- inline", X_MKDIR);
    parseF("-ML- RMDIR   %d 3 -MLX- inline", X_RMDIR);
    parseF("-ML- DELETE  %d 3 -MLX- inline", X_DELETE);
    parseF("-ML- RENAME  %d 3 -MLX- inline", X_RENAME);

    parseF("-ML- WIFI-CONNECT %d 3 -MLX- inline", X_WIFI_CONNECT);
    parseF("-ML- WIFI-STATUS  %d 3 -MLX- inline", X_WIFI_STATUS);
    parseF("-ML- WIFI-IP      %d 3 -MLX- inline", X_WIFI_IP);

    parseF(": isPC 0 ;");
}
char *doUser(char *pc, int ir) {
  cell_t t, n;
  char fn[16];
  IPAddress addr;

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
    RCASE BLOAD:  sprintf(fn, "block-%03d.c3", (int)pop()); LFF(fn);    

    RCASE EDIT_BLK: t=pop(); editBlock(t); reboot();

    RCASE X_PWD:    handle_PWD();
    RCASE X_CD:     push( handle_CD( (char*) pop()));
    RCASE X_LS:     handle_LS( (char*) pop());
    RCASE X_MKDIR:  push( handle_MKDIR( (char*) pop()));
    RCASE X_RMDIR:  push( handle_RMDIR( (char*) pop()));
    RCASE X_DELETE: push( handle_DELETE( (char*) pop()));
    RCASE X_RENAME: push( handle_RENAME( (char*) pop()));

    RCASE X_WIFI_CONNECT:   n=pop(); WiFi.begin( (char*) pop(), (char*) n);
    RCASE X_WIFI_STATUS:    push( WiFi.status());
    RCASE X_WIFI_IP:        addr = WiFi.localIP(); push( addr[0]);push( addr[1]);push( addr[2]);push( addr[3]);

    return pc;
    
    default: return 0;
  }
}

  int tryOpen(const char *root, const char *loc, const char *fn) {
    char nm[64];
    strCpy(nm, root);
    strCat(nm, loc);
    strCat(nm, fn);
    char buf[ 256];
    // printf("try [%s]\n", nm);
    cell_t fp = fOpen(nm, "rb");
    if (! fp) return 0;
    while( (state != STOP_LOAD) && fAvailable( fp)){
       fReadLine( fp, buf, sizeof( buf));
//        printString( buf);
      ParseLine( buf);
    }
    fClose( fp);
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

