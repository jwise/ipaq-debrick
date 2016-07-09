struct bus {
        void          (*init)(void);
        void          (*end)(void);
	unsigned long (*read_word)(unsigned long addr);
        void          (*write_word)(unsigned long addr, unsigned long data);
};

#define BUS_WRITE(b,a,d) (b->write_word(a,d))
#define BUS_READ(b,a)    (b->read_word(a))

/*
static struct device flash_strata = {
        .write_word = sflash_write_word,
        .erase_block = sflash_erase_block,
        .end = sflash_end
};
*/
