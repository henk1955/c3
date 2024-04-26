// Files support

#ifndef __FILE_H__

#define __FILE_H__

// Change these for the app.
#include "c3.h"
#define ISTK_SZ 10

// These are expected to be defined by the calling app.
// Additionally, isPC or _LITTLEFS_ control which version is built.
// If neither, stubs are built.
extern void strCpy(char *d, const char *s);
extern int  strLen(const char *d);

#ifndef CELL_T
    #define CELL_T long
#endif

#ifndef BTW
    #define BTW(n,lo,hi) ((lo<=n) && (n<=hi))
#endif

extern CELL_T input_fp;

extern void   fileInit();
extern CELL_T fOpen(const char *nm, const char *md);
extern void   fClose(CELL_T fp);
extern int    fRead(uint8_t *addr, int sz, int num, CELL_T fp);
extern int    fWrite(uint8_t *addr, int sz, int num, CELL_T fp);
extern void   setBlockFN(const char *fn);
extern int    readBlock(int num, char *blk, int sz);
extern int    writeBlock(int num, char *blk, int sz);
extern void   blockLoad(int num);
extern int    fReadLine(CELL_T fh, char *buf, int sz);
extern int    fAvailable( CELL_T fh);
extern char   fReadC( CELL_T fp);


extern void   ipush(CELL_T fh);
extern CELL_T ipop();

#endif // __FILE_H__