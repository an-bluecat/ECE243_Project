 
	/* This files provides address values that exist in the system */

#define BOARD                 "DE1-SoC"

/* Memory */
#define DDR_BASE              0x00000000
#define DDR_END               0x3FFFFFFF
#define A9_ONCHIP_BASE        0xFFFF0000
#define A9_ONCHIP_END         0xFFFFFFFF
#define SDRAM_BASE            0xC0000000
#define SDRAM_END             0xC3FFFFFF
#define FPGA_ONCHIP_BASE      0xC8000000
#define FPGA_ONCHIP_END       0xC803FFFF
#define FPGA_CHAR_BASE        0xC9000000
#define FPGA_CHAR_END         0xC9001FFF

/* Cyclone V FPGA devices */
#define LEDR_BASE             0xFF200000
#define HEX3_HEX0_BASE        0xFF200020
#define HEX5_HEX4_BASE        0xFF200030
#define SW_BASE               0xFF200040
#define KEY_BASE              0xFF200050
#define JP1_BASE              0xFF200060
#define JP2_BASE              0xFF200070
#define PS2_BASE              0xFF200100
#define PS2_DUAL_BASE         0xFF200108
#define JTAG_UART_BASE        0xFF201000
#define JTAG_UART_2_BASE      0xFF201008
#define IrDA_BASE             0xFF201020
#define TIMER_BASE            0xFF202000
#define AV_CONFIG_BASE        0xFF203000
#define PIXEL_BUF_CTRL_BASE   0xFF203020
#define CHAR_BUF_CTRL_BASE    0xFF203030
#define AUDIO_BASE            0xFF203040
#define VIDEO_IN_BASE         0xFF203060
#define ADC_BASE              0xFF204000

/* Cyclone V HPS devices */
#define HPS_GPIO1_BASE        0xFF709000
#define HPS_TIMER0_BASE       0xFFC08000
#define HPS_TIMER1_BASE       0xFFC09000
#define HPS_TIMER2_BASE       0xFFD00000
#define HPS_TIMER3_BASE       0xFFD01000
#define FPGA_BRIDGE           0xFFD0501C

/* ARM A9 MPCORE devices */
#define   PERIPH_BASE         0xFFFEC000    // base address of peripheral devices
#define   MPCORE_PRIV_TIMER   0xFFFEC600    // PERIPH_BASE + 0x0600

/* Interrupt controller (GIC) CPU interface(s) */
#define MPCORE_GIC_CPUIF      0xFFFEC100    // PERIPH_BASE + 0x100
#define ICCICR                0x00          // offset to CPU interface control reg
#define ICCPMR                0x04          // offset to interrupt priority mask reg
#define ICCIAR                0x0C          // offset to interrupt acknowledge reg
#define ICCEOIR               0x10          // offset to end of interrupt reg
/* Interrupt controller (GIC) distributor interface(s) */
#define MPCORE_GIC_DIST       0xFFFED000    // PERIPH_BASE + 0x1000
#define ICDDCR                0x00          // offset to distributor control reg
#define ICDISER               0x100         // offset to interrupt set-enable regs
#define ICDICER               0x180         // offset to interrupt clear-enable regs
#define ICDIPTR               0x800         // offset to interrupt processor targets regs
#define ICDICFR               0xC00         // offset to interrupt configuration regs

	
	
#include<stdio.h>

volatile int *LEDR_ptr = (int *) 0xFF200000;
volatile int *SW_ptr = (int *) 0xFF200040;
volatile int *KEY_EDGE_ptr = (int *) 0xFF20005C;
volatile char *character_buffer = (char *) 0xC9000000;// VGA character buffer
volatile int *pixel_ctrl_ptr = (int *) 0xFF203020; // pixel controller
volatile int pixel_buffer_start;


int main(void) {
	printf("here1");
	/* Declare volatile pointers to I/O registers (volatile means that IO load
	and store instructions will be used to access these pointer locations,
	instead of regular memory loads and stores) */
	volatile int * PS2_ptr = (int *)PS2_BASE;
	int PS2_data, RVALID;
	char byte1 = 0, byte2 = 0, byte3 = 0;
	// PS/2 mouse needs to be reset (must be already plugged in)
	*(PS2_ptr) = 0xFF; // reset
	while (1) {
		PS2_data = *(PS2_ptr); // read the Data register in the PS/2 port
		RVALID = PS2_data & 0x8000; // extract the RVALID field
		if (RVALID) {
			/* shift the next data byte into the display */
			byte1 = byte2;
			byte2 = byte3;
			byte3 = PS2_data & 0xFF;
			int test=ps2ToChar((int)byte3);

			if(test=='a'){
				*LEDR_ptr =1;
			}
			else if(test=='z'){
				*LEDR_ptr =3;
			}else{
				*LEDR_ptr =0;
			}
			HEX_PS2(byte1, byte2, byte3);
		}
	}
}

//Subroutine to show a string of HEX data on the HEX displays
void HEX_PS2(char b1, char b2, char b3) {
	volatile int * HEX3_HEX0_ptr = (int *)HEX3_HEX0_BASE;
	volatile int * HEX5_HEX4_ptr = (int *)HEX5_HEX4_BASE;
	unsigned char seven_seg_decode_table[] = {
	0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
	0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
	unsigned char hex_segs[] = {0, 0, 0, 0, 0, 0, 0, 0};
	unsigned int shift_buffer, nibble;
	unsigned char code;
	int i;
	shift_buffer = (b1 << 16) | (b2 << 8) | b3;
	for (i = 0; i < 6; ++i) {
	nibble = shift_buffer & 0x0000000F; // character is in rightmost nibble
	code = seven_seg_decode_table[nibble];
	hex_segs[i] = code;
	shift_buffer = shift_buffer >> 4;
}
/* drive the hex displays */
*(HEX3_HEX0_ptr) = *(int *)(hex_segs);
*(HEX5_HEX4_ptr) = *(int *)(hex_segs + 4);
}

//char a -> a is type int
int ps2ToChar(int input){
	if(input==0x1C){	
		return 'a';			 
	}
	if(input==0x32)	return 'b';
	if(input==0x21)	return 'c';
	if(input==0x23)	return 'd';
	if(input==0x24)	return 'e';
	if(input==0x2b)	return 'f';
	if(input==0x34)	return 'g';
	if(input==0x33)	return 'h';
	if(input==0x43)	return 'i';
	if(input==0x3b)	return 'j';
	if(input==0x32)	return 'k';
	if(input==0x4b)	return 'l';
	if(input==0x3a)	return 'm';
	if(input==0x31)	return 'n';
	if(input==0x44)	return 'o';
	if(input==0x4d)	return 'p';
	if(input==0x15)	return 'q';
	if(input==0x2d)	return 'r';
	if(input==0x1b)	return 's';
	if(input==0x2c)	return 't';
	if(input==0x3c)	return 'u';
	if(input==0x2a)	return 'v';
	if(input==0x1d)	return 'w';
	if(input==0x22)	return 'x';
	if(input==0x35)	return 'y';
	if(input==0x1a)	return 'z';
	//this is enter
	if(input==32)	return 1000;

	
	
	
	
	
	else{
		return '\0';
	}

}
/*
A	1C	F0,1C	R ALT	E0,11	E0,F0,11
B	32	F0,32	APPS	E0,2F	E0,F0,2F
C	21	F0,21	ENTER	5A	F0,5A
D	23	F0,23	ESC	76	F0,76
E	24	F0,24	F1	05	F0,05
F	2B	F0,2B	F2	06	F0,06
G	34	F0,34	F3	04	F0,04
H	33	F0,33	F4	0C	F0,0C
I	43	F0,43	F5	03	F0,03
J	3B	F0,3B	F6	0B	F0,0B
K	42	F0,42	F7	83	F0,83
L	4B	F0,4B	F8	0A	F0,0A
M	3A	F0,3A	F9	01	F0,01
N	31	F0,31	F10	09	F0,09
O	44	F0,44	F11	78	F0,78
P	4D	F0,4D	F12	07	F0,07
Q	15	F0,15	SCROLL	7E	F0,7E
R	2D	F0,2D	[	54	FO,54
S	1B	F0,1B	INSERT	E0,70	E0,F0,70
T	2C	F0,2C	HOME	E0,6C	E0,F0,6C
U	3C	F0,3C	PG UP	E0,7D	E0,F0,7D
V	2A	F0,2A	DELETE	E0,71	E0,F0,71
W	1D	F0,1D	END	E0,69	E0,F0,69
X	22	F0,22	PG DN	E0,7A	E0,F0,7A
Y	35	F0,35	UP	E0,75	E0,F0,75

*/








//from DE1-SOC manual
//Subroutine to send a string of text to the video monitor
void video_text(int x, int y, char * text_ptr) {
	int offset;
	volatile char * character_buffer =(char *)FPGA_CHAR_BASE; // video character buffer
	/* assume that the text string fits on one line */
	offset = (y << 7) + x;
	while (*(text_ptr)) {
		*(character_buffer + offset) =
		*(text_ptr); // write to the character buffer
		++text_ptr;
		++offset;
	}
}

//num=the number of characters you want to clear
void clear_video_text(int x, int y, int num) {
	int offset;
	volatile char * character_buffer =(char *)FPGA_CHAR_BASE; // video character buffer

	offset = (y << 7) + x;
	int i=0;
	while (i<num) {
		*(character_buffer + offset) =0; // write to the character buffer
		++i;
		++offset;
	}
	*(character_buffer + offset)=0;
}




void wait_for_sync(){
	volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
	int status;
	//write 1, cause swap
	*(pixel_ctrl_ptr)=1;
	
	status=*(pixel_ctrl_ptr+3);
	//poll, see if swap is done
	while((status & 0x1)==1){
		status=*(pixel_ctrl_ptr+3);
	}
	
}

void plot_pixel(int x, int y, short int line_color)
{
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

void clear_screen(){
	for(int i=0; i<320;i++){//x
		for(int j=0;j<240;j++){//y
			plot_pixel(i,j,getColor(0,0,0));
		}
	}
}

//r:0-31; g:0-63; b:0-31
int getColor(int r, int g, int b){
	return ((r<<11)+(g<<5)+b);
}
