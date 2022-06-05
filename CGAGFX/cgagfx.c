#include <dpmi.h>
#include <pc.h>
#include <stdio.h>
#include <conio.h>
#include <sys/nearptr.h>
#include <time.h>

#define CARD_CGA 0
#define CARD_EGA 1
#define CARD_VGA 2

#define COLOR_BLACK         0x00
#define COLOR_BLUE          0x01
#define COLOR_GREEN         0x02
#define COLOR_CYAN          0x03
#define COLOR_RED           0x04
#define COLOR_MAGENTA       0x05
#define COLOR_BROWN         0x06
#define COLOR_LIGHT_GRAY    0x07
#define COLOR_DARK_GRAY     0x08
#define COLOR_LIGHT_BLUE    0x09
#define COLOR_LIGHT_GREEN   0x0A
#define COLOR_LIGHT_CYAN    0x0B
#define COLOR_LIGHT_RED     0x0C
#define COLOR_LIGHT_MAGENTA 0x0D
#define COLOR_YELLOW        0x0E
#define COLOR_WHITE         0x0F

int g_vga_safe_mode;
int g_video_card;
int g_old_text_mode;

char *g_screen;

void init_ega_sequencer(void) {
    int i;
    char g_ega_sequencer_data[5] = {0x03, 0x01, 0x03, 0x00, 0x03};        
    for (i = 0; i < 5; i++ ) {
        outportb(0x03c4, i);
        outportb(0x03c5, g_ega_sequencer_data[i]);
    }
}

void init_ega_crtc(void) {
    int i;
    char g_ega_crtc_data[26] = {0x70, 0x4f, 0x5c, 0x2f,
                                0x5f, 0x07, 0x04, 0x11,
                                0x00, 0x01, 0x06, 0x07,
                                0x00, 0x00, 0x00, 0x00,
                                0xe1, 0x24, 0xc7, 0x28,
                                0x08, 0xe0, 0xf0, 0xa3,
                                0xff, 0x00};    
    for (i=0; i< 26; i++) {
        outportb(0x03d4, i);
        outportb(0x03d5, g_ega_crtc_data[i]);
    }
}

int detect_adapter() {
  __dpmi_regs regs;
  
  memset(&regs, 0, sizeof regs);
  regs.h.ah = 0x12;
  regs.h.bl = 0x10;
  __dpmi_int(0x10, &regs);
  if(regs.h.bl == 0x10)
    return CARD_CGA;

  memset(&regs, 0, sizeof regs);  
  regs.x.ax = 0x1a00;
  __dpmi_int(0x10, &regs);
  
  if(regs.h.al == 0x1a)
    return CARD_VGA;
  else 
    return CARD_EGA;
}

int set_cga_text_gfx_mode(void) {
    __dpmi_regs regs;
    char byte_data;

    // Get the current video mode
    regs.x.ax = 0x0f00;
    __dpmi_int(0x10, &regs);
    g_old_text_mode = regs.h.al;

    // If VGA safe mode is enabled, turn on 200 scanline mode
    if (g_video_card == CARD_VGA) {
        if (g_vga_safe_mode) {
            // Set the 200 scanline on VGA (apparently font size is broken)
            regs.x.ax = 0x1200;
            regs.h.bl = 0x30;
            __dpmi_int(0x10, &regs);
        }
    }

    // Set the CGA 80x25 mode
    regs.x.ax = 0x0002;
    __dpmi_int(0x10, &regs);

    // Turn off the video 
    outportb(0x03d8, 0x01);

    // Disable interrupts
    __dpmi_get_and_disable_virtual_interrupt_state();

    switch (g_video_card) {
        case CARD_CGA:
            // Program the CGA CRTC registers (4, 6, 7 and 9)
            outportb(0x03d4, 0x04);
            outportb(0x03d5, 0x7F);
            outportb(0x03d4, 0x06);
            outportb(0x03d5, 0x64);
            outportb(0x03d4, 0x07);
            outportb(0x03d5, 0x70);
            outportb(0x03d4, 0x09);
            outportb(0x03d5, 0x01);
            break;
        case CARD_EGA:
            if (!g_vga_safe_mode) {
                // Load the 8x8 font
                regs.x.ax = 0x1102;
                regs.h.bl = 0x00;
                __dpmi_int(0x10, &regs);
                regs.x.ax = 0x1003;
                regs.h.bl = 0x00;
                __dpmi_int(0x10, &regs);      
                outportb(0x03c2, 0x23);

                // Initialize the EGA sequencer and CRTC
                init_ega_sequencer();
                init_ega_crtc();
            }
            else {
                outportw(0x03d4, 0x0209);
                // Set background intensity bit
                regs.x.ax = 0x1003;
                regs.h.bl = 0x00;
                __dpmi_int(0x10, &regs);
                regs.x.ax = 0x1000;
                regs.x.bx = 0x1406;
                __dpmi_int(10, &regs);
            }
            break;
        case CARD_VGA:
            outportw(0x03d4, 0x0309);
            // Set the background intensity bit
            regs.x.ax = 0x1003;
            regs.h.bl = 0x00;
            __dpmi_int(0x10, &regs);
            break;
        default:
        break;
    }

    __dpmi_get_and_enable_virtual_interrupt_state();
    outportb(0x03d8, 0x09);

    // Perform some adapter-specific tweaks here

    // if vga/mcga 
    if (g_video_card == CARD_VGA) {
        regs.x.ax = 0x1003;
        regs.h.bl = 0x00;
        __dpmi_int(0x10, &regs);
    }

    // if PCJr
    // if (pcjr) {
    // byte_data = inportb(0x03da);
    // outportb(0x03da, 0x03);
    // outportb(0x03da, 0x00);
    //}

    return 0;
}

int set_previous_text_mode(void) {
    __dpmi_regs regs;

    // If in VGA safe mode, re-enable 400 scanline mode
    if(g_vga_safe_mode) {
        regs.x.ax = 0x1202;
        regs.h.bl = 0x30;
        __dpmi_int(0x10, &regs);
    }

    // Turn on the old text mode
    regs.h.al = g_old_text_mode;
    regs.h.ah = 0x00;
    __dpmi_int(0x10, &regs);

    return 0;
}

int plot(int x, int y, int color) {

    int offset = (y << 7 ) + (y << 5) + x;
    char attr;

    // if odd, change the background color
    // if even, change the foreground color
    // use ASCII code 221
    if (x % 2) {
        offset--;
        attr = (g_screen[offset+1] & 0x0F) | (color << 4);
    } 
    else {  
        attr = (g_screen[offset+1] & 0xF0) | color;
    }
    g_screen[offset+1] = attr;
    
}

int main(void) {

    int i,j ;
    __djgpp_nearptr_enable();    
    g_screen = (char *)(0xb8000 + __djgpp_conventional_base);

    srand(time(NULL));

    g_video_card = detect_adapter();
    set_cga_text_gfx_mode();

    for(i=0; i < 160; i++) {
        for(j=0; j < 100; j++) {
            g_screen[((j * 160) + i) * 2] = 221;
            g_screen[((j * 160) + i) * 2 + 1] = 0;            
        }
    }

    for(i=0; i < 160; i++) {
        for(j=0; j < 100; j++) {
            plot(i, j, 1);
        }
    }

    while(!kbhit()) {
    }
    set_previous_text_mode();

    __djgpp_nearptr_disable();    
    return 0;
}