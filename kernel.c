#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Check if the compiler thinks you are targeting
the wrong operating system*/
#if defined(__linux__)
#error "You are not using cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 
 32-bit ix86 targets*/
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiled"
#endif

/* Hardware text mode color constants */
typedef enum {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
} Vga_color;

static inline uint8_t vga_entry_color(Vga_color fg, Vga_color bg)
{
    return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color)
{
    return (uint16_t) uc | (uint16_t) color << 8;
}

size_t strlen(const char* str)
{
    size_t len = 0;
    while (str[len] != '\0')
        len++;
    return len;
}

// buffer has to have 11 chars of length to contain any number
void itoa(int n, char* buffer)
{
    // a special case
    if(n==0)
    {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }


    int digits = 0;
    for(int i = 10; i >= 0; i--)
    {
        if (n <= 0)
            break;

        buffer[i] = (n % 10) + 48;
        n /= 10;
        digits++;
    }

    for(int i = 0; i < digits; i++)
    {
        buffer[i] = buffer[i+(11-digits)];
    }

    buffer[10] = '\0';
}

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

void terminal_initialize(void)
{
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    terminal_buffer = (uint16_t*) 0xB8000; // Address of screen buffer in video memory
    
    for (size_t y = 0; y < VGA_HEIGHT; y++)
    {
        for(size_t x = 0; x < VGA_WIDTH; x++)
        {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
}

void terminal_setcolor(uint8_t color)
{
    terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, color);
}

void scroll_terminal_buffer()
{
    for(size_t y = 1; y < VGA_HEIGHT; y++)
    {
        for(size_t x = 0; x < VGA_WIDTH; x++)
        {
            terminal_buffer[(y-1)*VGA_WIDTH + x] = terminal_buffer[y*VGA_WIDTH + x];
        }
    }
    for(size_t x = 0; x < VGA_WIDTH; x++)
    {
        terminal_buffer[(VGA_HEIGHT-1)*VGA_WIDTH + x] = ' ';
    }
}

void newline()
{
    terminal_column = 0;
    if(++terminal_row == VGA_HEIGHT)
    {
        scroll_terminal_buffer(1);
        terminal_row--;
    }
}

void terminal_putchar(char c)
{
    switch(c){
        case '\n':{
            newline();
            return;
        }
    }
    
    terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
    if (++terminal_column == VGA_WIDTH)
    {
        newline();
    }
}

void terminal_write(const char* data, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        terminal_putchar(data[i]);
    }
}

void terminal_writestring(const char* data)
{
    terminal_write(data, strlen(data));
}

void kernel_main(void)
{
    /* Initialize terminal interface */
    terminal_initialize();

    for(int i = 0; i < 50; i++){
        char num[11];
        itoa(i, num);
        terminal_writestring(num);
        terminal_writestring("Witaj swiecie, tutaj kernel Filipa :)\n");
    }
}