
    void   fileInit() { input_fp = input_sp = 0; }
    CELL_T fOpen(const char *nm, const char *md) { return (CELL_T)fopen(nm, md); }
    void   fClose(CELL_T fp) { fclose((FILE*)fp); }
    
	int fRead(char *buf, int sz, int num, CELL_T fp) {
        return (int)fread(buf, sz, num, (FILE*)fp);
    }

    int fWrite(char *buf, int sz, int num, CELL_T fp) {
        return (int)fwrite(buf, sz, num, (FILE*)fp);
    }

    int writeBlock(int num, char *blk, int sz) {
        char fn[32];
        sprintf(fn, block_fn, num);
        num = 0;
        FILE *x = fopen(fn, "wb");
        if (x) {
            num = (int)fwrite(blk, 1, sz, x);
            fclose(x);
        }
        return num;
    }

    int readBlock(int num, char *blk, int sz) {
        char fn[32];
        sprintf(fn, block_fn, num);
        num = 0;
        FILE *x = fopen(fn, "rb");
        if (x) {
            num = (int)fread(blk, 1, sz, x);
            fclose(x);
        }
        return num;
    }

	int fReadLine(CELL_T fh, char *buf, int sz) {
		if (fgets(buf, sz, (FILE*)fh) == buf) { return strLen(buf); }
		return -1;
	}
