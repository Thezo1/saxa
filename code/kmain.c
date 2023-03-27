#define FB_GREEN     2
#define FB_DARK_GREY 8

#define FB_COMMAND_PORT 0x3D4
#define FB_DATA_PORT 0x3D5

#define FB_HIGH_BYTE_COMMAND 14
#define FB_LOW_BYTE_COMMAND 15

/*
 * NOTE(Thezo): These are for the serial ports
 * but to be able to communicate, the speed and number of bits has to be set
 * the serial line command port sets this
 * it requires a divisor which is 16bits, but only 8bits can be stored at a time
 * so the SERIAL_LINE_ENABLE_DLAB 0x80 tells the command port to take the first number 
 * I give as the high bit then the number that follows as the low bit
 *
 * These contains the settings for one port
 */

#define SERIAL_COM1_BASE 0x3F8      // COM1 base port
#define SERIAL_DATA_PORT(base) (base)
#define SERIAL_FIFO_COMMAND_PORT(base) (base + 2)
#define SERIAL_LINE_COMMAND_PORT(base) (base + 3)
#define SERIAL_MODEM_COMMAND_PORT(base) (base + 4)
#define SERIAL_LINE_STATUS_PORT(base) (base + 5)

#define SERIAL_LINE_ENABLE_DLAB 0x80

static char *fb = (char *)0x000B8000;

void outb(unsigned short port, unsigned char data);

/*
 * Takes in the address of a port and returns a byte from
 */
unsigned char inb(unsigned short port);

/*
 * i = location of frame buffer
 * c = character to write to the frame buffer
 * fg = foreground colour
 * bg = background colour
 */
void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg)
{
    fb[i] = c;
    fb[i + 1] = ((fg & 0x0F) << 4) | ((bg & 0x0F) << 0);
}

/*
 *  This functions positions the cursor on the framebuffer
 *  takes in a 2 byte(unsigned short) integer that represents
 *  a index into the framebuffer array
 */

void fb_move_cursor(unsigned short pos)
{
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
    outb(FB_DATA_PORT, ((pos >> 8) & 0xFF));
    outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
    outb(FB_DATA_PORT, ((pos >> 0) & 0xFF));
}

/*
 * Driver that writes the contents of a buffer to the frame buffer
 */
void fb_write(char *buf, unsigned int length)
{
    for(unsigned int buffer_index = 0;
                     buffer_index < length;
                     ++buffer_index)
    {
        unsigned int to_write = buffer_index * 2;
        fb_write_cell(to_write, buf[buffer_index], FB_DARK_GREY, 0);
        fb_move_cursor((unsigned short)(to_write/2));
    }
}

// @params com: The serial port to configure

/*
 *  This sets the speed of the data being sent, the default speed of a serial port
 *  is 115200 bits/s. this can be changed by passing a divisor, reducing the speed to
 *  (115200/divisor) bits/s
 */
void serial_configure_baud_rate(unsigned short com, unsigned short divisor)
{
    outb(SERIAL_LINE_COMMAND_PORT(com), SERIAL_LINE_ENABLE_DLAB);
    outb(SERIAL_DATA_PORT(com), ((divisor >> 8) & 0x00FF));
    outb(SERIAL_DATA_PORT(com), ((divisor >> 0) & 0x00FF));
}

/*
 * This configures the line of the given port
 * 
 */
void serial_configure_line(unsigned short com)
{
    /* Bit:     | 7 | 6 | 5 4 3 | 2 | 1 0 |
     * Content: | d | b | prty  | s | dl  |
     * Value:   | 0 | 0 | 0 0 0 | 0 | 1 1 | = 0x03
     */    

    outb(SERIAL_LINE_COMMAND_PORT(com), 0x03);
}

void serial_fifo_configure(unsigned short com)
{
    outb(SERIAL_FIFO_COMMAND_PORT(com), 0xC7);
}

void serial_modem_configure(unsigned short com)
{
    outb(SERIAL_MODEM_COMMAND_PORT(com), 0x03);
}

int serial_transmit_fifo_is_empty(unsigned short com)
{
    return inb(SERIAL_LINE_STATUS_PORT(com)) & 0x20;
}

void serial_write(unsigned short com, char *buf, unsigned int length)
{
    if(serial_transmit_fifo_is_empty(SERIAL_COM1_BASE))
    {
        for(unsigned int buffer_index = 0;
                buffer_index < length;
                ++buffer_index)
        {
            outb(com, buf[buffer_index]);
        }
    }
}

void serial_configure(unsigned short com, unsigned short divisor)
{
    serial_configure_baud_rate(com, divisor);
    serial_configure_line(com);
    serial_fifo_configure(com);
    serial_modem_configure(com);
}

int main()
{
    char fb_buf[] = "Something";
    char serial_buf[] = "Someone";
    fb_write(fb_buf, (sizeof(fb_buf)/sizeof(fb_buf)[0]));
    serial_configure(SERIAL_COM1_BASE, 1);
    serial_write(SERIAL_COM1_BASE, serial_buf, (sizeof(serial_buf)/sizeof(serial_buf)[0]) - 1);
}
