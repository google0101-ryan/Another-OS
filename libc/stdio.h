#pragma once

#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <syscall.h>

int write(int fd, void* buf, size_t count)
{
	__syscall3(SYSCALL_WRITE, fd, (uint32_t)buf, count);
}

void putc(char c)
{
	write(1, &c, 1);
}

void puts(char* str)
{
	write(1, str, strlen(str));
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

void printf(const char* fmt, ...)
{
	va_list args;
	va_start(args,fmt);
	vprintf(fmt,args);
	va_end(args);
}