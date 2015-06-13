#ifndef CTRL_H
#define CTRL_H

#include "gui.h"
#include "vector.h"
#include "graphics.h"
#include "idx.h"
#include "strext.h"

#define CTRL_ENTER		0x1C0D
#define CTRL_BKSPACE	0x0E08
#define CTRL_DEL		0x5300
#define CTRL_UP			0x4800
#define CTRL_DOWN		0x5000
#define CTRL_LEFT		0x4B00
#define CTRL_RIGHT		0x4D00
#define CTRL_ESC		0x011B
#define CTRL_PGUP		0x4900
#define CTRL_PGDOWN		0x5100
#define CTRL_LINECOUNT	10
#define CTRL_ASCII(keynum) (((keynum) & 0x00FF) - 0x0061 + 'a')

char*	Ctrl_readline			(Rectangle border);
int		Ctrl_readKey			(void);
void	Ctrl_showWordlist		(const Vector IDXlist, size_t index, Rectangle border, bool forceShow);
void	Ctrl_reprintWordList	(const Vector IDXlist, size_t start, size_t end, Rectangle border);
void	Ctrl_forceShowWordList	(const Vector IDXlist, size_t index, Rectangle border);
void	Ctrl_showCaret			(mint col, mint ln, size_t length, Rectangle border);
void	Ctrl_hideCaret			(mint col, mint ln, size_t length, Rectangle border);

#endif
