struct flash_device {
	void (*start)(struct bus *bus);
	int (*unlock)(struct bus *bus);
	int (*write_word)(struct bus *bus, unsigned long addr, unsigned long data);
	int (*erase_block)(struct bus *bus, unsigned long block);
	unsigned long (*read_word)(struct bus *bus, unsigned long addr);
	void (*end)(struct bus *bus);
};

