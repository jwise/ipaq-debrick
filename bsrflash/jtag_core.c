#include "libjtag.c"

void put_chain(unsigned char *chain, unsigned long pos, unsigned long value){
	int byte, bit;

	byte = pos/8;
	bit = pos%8;
	if(value)
		chain[byte] |= 1<<bit;
	else
		chain[byte] &= ~(1<<bit);
}

void zero_chain(unsigned char *chain, unsigned long len) {
	int i;
	for(i = 0 ; i < ((len-1)/8) + 1 ; i++)
		chain[i] = 0;
}

void write_chain(unsigned char *chain, unsigned long len){
	int i;
	capture_dr();
	clockin(0,0);           // enter shift state
	for(i = 0 ; i < len ; i++){
		clockin(i == len-1, chain[i/8] >> (i%8) & 1);
	}
	clockin(1,0);           // enter update DR state
        clockin(0,0);           // enter run-test/idle state
	set_instr(EXTEST);
}

unsigned long get_long_from_chain(unsigned char *chain, unsigned long pos) {
	unsigned long out = 0;
	int i;

	for(i = pos ; i < pos+32 ; i++)
		out |= (chain[i/8] >> (i%8) & 1) << ((i-pos));
	return out;
}

void read_chain(unsigned char *chain, unsigned long len){
        int i;
        capture_dr();
        for(i = 0 ; i < len ; i++) {
		clockin(i == len-1, 0);
		chain[i/8] &= ~(1 << (i%8));
		chain[i/8] |= clockout() << (i%8);
	}
       clockin(1,0);           // enter update DR state
       clockin(0,0);           // enter run-test/idle state
#if 0
	for(i = 0 ; i < ((len-1)/8) ; i++) {
		if(!(i & 3) && i)
			printf(" ");
		printf("%02x", chain[i]);
	}
	printf("\n");
#endif
}

void jtag_start(void) {
	openport();
	test_reset();
	clockin(0,0);
}
