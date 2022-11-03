#include <arch/x86/drivers/vga.hpp>
#include <arch/x86/ports.h>

#include <string.h>
#include <stdarg.h>

VGA console;

void update_cursor(int x, int y)
{
	uint16_t pos = y * 80 + x;

	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

uint8_t VGA::vga_color(uint8_t fg, uint8_t bg)
{
	return fg | bg << 4;
}

uint16_t VGA::vga_entry(char c, uint8_t color)
{
	return (uint16_t)c | (uint16_t)color << 8;
}

void VGA::Init()
{
	fb = (uint16_t*)0xC00B8000;
	x = y = 0;
	cur_color = vga_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
	width = 80;
	height = 25;

	for (int cur_y = 0; cur_y < height; cur_y++)
	{
		for (int cur_x = 0; cur_x < width; cur_x++)
		{
			const int index = cur_y * width + cur_x;
			fb[index] = vga_entry(' ', cur_color);
		}
	}

	update_cursor(0, 0);
}

void VGA::_put_str(const char* c)
{
	while (*c != '\0')
		_put(*c++);
}

void VGA::_put(char c)
{
	if (c == '\n')
	{
		x = 0;
		y++;
	}
	else if (c == '\r')
	{
		x = 0;
	}
	else if (c == '\t')
	{
		while (x++ % 4);
	}
	else
	{
		const int index = y * width + x;
		fb[index] = vga_entry(c, cur_color);

		x++;
		if (x >= width)
		{
			x = 0;
			y++;
		}
		if (y >= height)
		{
			x = y = 0;
		}
	}

	update_cursor(x, y);
}

void swap(char* a, char* b)
{
	char tmp = *a;
	*a = *b;
	*b = tmp;
}

char* reverseStr(char* str, int length)
{
	int pos;
	for (pos = 0; pos < length / 2; pos++)
		swap(&str[pos], &str[length-pos-1]);
	return str;
}

char* intToString(char* str, int value, int base)
{
	static const char* chars = "0123456789ABCDEF";
	
	if (value == 0)
	{
		str[0] = '0';
		str[1] = '\0';
		return str;
	}

	if(value == 0 || base < 2 ||
			base > 16) {
		str[0] = '0';
		str[1] = '\0';
		return str;
	} else {
		int negate = 0, pos = 0;

		if(value < 0) {
			value = -value;
			negate = 1;
		}

		while(value != 0) {
			int curr = value % base;
			str[pos++] = chars[curr];
			value /= base;
		}

		if(negate)
			str[pos++] = '-';
		str[pos] = '\0';

		// Flip the number around
		return reverseStr(str, pos);
	}
}

char *uintToStr(char *str, unsigned int value, int base) {
	static const char *chars = "0123456789ABCDEF";

	if (value == 0)
	{
		str[0] = '0';
		str[1] = '\0';
		return str;
	}

	if(base < 2 || base > 16) {
		str[0] = '0';
		str[1] = '\0';
		return str;
	} else {
		int pos = 0;
		while(value != 0) {
			int curr = value % base;
			str[pos++] = chars[curr];
			value /= base;
		}

		str[pos] = '\0';

		// Flip the number around
		return reverseStr(str, pos);
	}
}

void vprintf(const char* fmt, va_list args)
{
	char ch;

	static char tmp[36];

	if (!fmt) return;
	while ((ch = *fmt++))
	{
		if (ch != '%')
			putc(ch);
		else
		{
			switch ((ch = *fmt++))
			{
			case 'c':
				putc((char)va_arg(args, int));
				break;
			case 's':
			{
				char* str = va_arg(args, char*);
				puts(str);
				break;
			}
			case '%':
				putc('%');
				break;
			case 'n':
				putc('\n');
				break;
			case 'd':
			{
				signed int value = va_arg(args, signed int);
				puts(intToString(tmp, value, 10));
				break;
			}
			case 'x':
			{
				uint32_t value = va_arg(args, uint32_t);
				puts(uintToStr(tmp, value, 16));
				break;
			}
			default:
				putc(ch);
				break;
			}
		}
	}
}

void VGA::_print(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
}

void VGA::set_cur_color(uint8_t fg, uint8_t bg)
{
	cur_color = vga_color(fg, bg);
}

int VGA::GetX()
{
	return x;
}

int VGA::GetY()
{
	return y;
}

void VGA::GoToXY(int nx, int ny)
{
	x = nx;
	y = ny;
}