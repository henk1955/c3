// Support for files, either standard or LittleFS

#include "file.h"
#include "c3.h"

#define FILE_WRITE_BEGIN "w"
#define FILE_READ "r"

static char block_fn[16];
CELL_T inputStk[ISTK_SZ+1], input_sp, input_fp;

void ipush(CELL_T x) { if (input_sp < ISTK_SZ) inputStk[++input_sp] = x; }
CELL_T ipop() { return (0 < input_sp) ? inputStk[input_sp--] : 0; }

void setBlockFN(const char *fn) {
	strCpy(block_fn, fn);
}

#ifdef isPC
	#include "file-pc.inc"
#elif defined (_LITTLEFS_)
	#include "file-littlefs.inc"
#else

  void   fileInit() { }
  static void nf() { printString("-noFile-"); }
  CELL_T fOpen(const char *nm, const char *md) { nf(); return 0; }
  void   fClose(CELL_T fp) { nf(); }
  int fRead(char *buf, int sz, int num, CELL_T fp) { nf(); return 0; }
  int fWrite(char *buf, int sz, int num, CELL_T fp) { nf(); return 0; }
  int fAvailable( CELL_T fh){{}

#endif
