#include "stdio.h"
#include "math.h"
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
#define TIMER_2_BASE          0xFF202020
#define AV_CONFIG_BASE        0xFF203000
#define PIXEL_BUF_CTRL_BASE   0xFF203020
#define CHAR_BUF_CTRL_BASE    0xFF203030
#define AUDIO_BASE            0xFF203040
#define VIDEO_IN_BASE         0xFF203060
#define ADC_BASE              0xFF204000

/* Cyclone V HPS devices */
#define HPS_GPIO1_BASE        0xFF709000
#define I2C0_BASE             0xFFC04000
#define I2C1_BASE             0xFFC05000
#define I2C2_BASE             0xFFC06000
#define I2C3_BASE             0xFFC07000
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
/* globals */
#define BUF_SIZE 4000 // 4000-> 0.5s    80000 = about 10 seconds of buffer (@ 8K samples/sec)
#define BUF_THRESHOLD 96 // 75% of 128 word buffer
    
#define pi 3.14159265
    


int s1[4000] = {0};

void genSound(int freq) {
        float acc = 0;
        //get lower pitch
        float discrete = (freq*pi*2) / 4000;
        for (int i = 0 ; i < 400; i++){
            acc+= discrete;
            s1[i] = sin(acc) * 20000000000;
            //printf("%d\n", bob[(j-1)*1000 + i]);
        }
    
        //get higher pitch
        discrete = ((freq+200)*pi*2) / 4000;
        for (int i = 0 ; i < 400; i++){
            acc+= discrete;
            s1[i+400] = sin(acc) * 20000000000;
            //printf("%d\n", bob[(j-1)*1000 + i]);
        }
}

void playsound(){
    int buffer_index = 0;
    volatile int* red_LED_ptr = (int*)LEDR_BASE;
    volatile int* audio_ptr = (int*)AUDIO_BASE;
                    int fifospace =
                *(audio_ptr + 1); // read the audio port fifospace register
            if ((fifospace & 0x00FF0000) > BUF_THRESHOLD) // check WSRC
            {
                // output data until the buffer is empty or the audio-out FIFO
                // is full
                //float x = (900*3.14159*2) / 40000;
                //float acc = 0;
                
                while ((fifospace & 0x00FF0000) && (buffer_index < BUF_SIZE)) {
                    //acc+= x;
                    *(audio_ptr + 2) = s1[buffer_index];//(int)(sin(acc) * 10000000000);
                    *(audio_ptr + 3) = s1[buffer_index];//(int)(sin(acc) * 10000000000);
                    ++buffer_index;
                    if (buffer_index == BUF_SIZE) {
                        // done playback
                        //play = 0;
                        //*(red_LED_ptr) = 0x0; // turn off LEDR
                    }
                    fifospace = *(audio_ptr + 1); // read the audio port fifospace register
                }
            }
    
    
    
}

int main(void) {

    genSound(400);
    
    
    playsound();
    
    
    
    
    
}
