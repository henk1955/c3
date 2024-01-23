// NOTE: this is a .h file because the PC version is straight C, while the Arduino version is CPP
// We need to suport both at the same time.

#ifndef __FILE_H__
#define __FILE_H__

#ifdef isPC

  void   fileInit() { }
  cell_t fOpen(cell_t nm, cell_t md) { return (cell_t)fopen((char*)nm, (char*)md); }
  void   fClose(cell_t fp) { fclose((FILE*)fp); }
  cell_t fRead(cell_t addr, cell_t sz, cell_t num, cell_t fp) {
      return (cell_t)fread((char*)addr, sz, num, (FILE*)fp);
  }
  cell_t fWrite(cell_t addr, cell_t sz, cell_t num, cell_t fp) {
      return (cell_t)fwrite((char*)addr, sz, num, (FILE*)fp);
  }

#elif defined (_LITTLEFS_)

  #include <LittleFS.h>

  LittleFS_Program myFS;
  void fileInit() {
      myFS.begin(1 * 1024 * 1024);
      printString("\r\nLittleFS: initialized");
      printStringF("\r\nBytes Used: %llu, Bytes Total:%llu", myFS.usedSize(), myFS.totalSize());
  }
  cell_t fOpen(cell_t nm, cell_t md) { return 0; }
  void   fClose(cell_t fp) { }
  cell_t fRead(cell_t addr, cell_t sz, cell_t num, cell_t fp) { return 0; }
  cell_t fWrite(cell_t addr, cell_t sz, cell_t num, cell_t fp) { return 0; }

#else

  static void nf() { printString("-noFile-"); }
  void   fileInit() { }
  cell_t fOpen(cell_t nm, cell_t md) { nf(); return 0; }
  void   fClose(cell_t fp) { nf(); }
  cell_t fRead(cell_t addr, cell_t sz, cell_t num, cell_t fp) { nf(); return 0; }
  cell_t fWrite(cell_t addr, cell_t sz, cell_t num, cell_t fp) { nf(); return 0; }

#endif

#endif // __FILE_H__