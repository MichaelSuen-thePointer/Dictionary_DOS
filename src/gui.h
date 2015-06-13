#ifndef GUI_KEY_H
#define GUI_KEY_H
#include "basic.h"
#include "vector.h"
#include "graphics.h"
#include "strext.h"

#define GUI_WHITE 0x00FFFFFFL
#define GUI_BLACK 0x00000000L
#define GUI_BLUE  0x00FFFF00L
#define GUI_GRAY  0x00AAAAAAL
#define GUI_GREEN 0x0000FF00L
#define GUI_CHARHEIGHT 16
#define GUI_CHARWIDTH 8
#define GUI_HANZIHEIGHT 16
#define GUI_HANZIWIDTH 16
#define GUI_LINEHEIGHT 18
#define GUI_BORDEROFFSET 5

struct tag_rect {
	mint m_left, m_top, m_right, m_bottom;
};
typedef struct tag_rect Rectangle;

void	Gui_initialize			(void);
mint	Gui_setbgColor			(mint color);
mint	Gui_setfgColor			(mint color);
void	Gui_close				(void);
void	Gui_printText			(char* letter, Rectangle border);
void	Gui_printTextXY			(char* text, mint x, mint y, mint maxlength);
bool	Gui_movePos				(mint* x, mint* y, Rectangle border, mint byteCount);
bool	Gui_nextLine			(mint* x, mint* y, Rectangle border, mint lineCount);
mint	Gui_openhzLib			(void);
mint	Gui_closehzLib			(void);
void	Gui_eraseCharRect		(mint x, mint y);
void	Gui_eraseCharNRect		(mint x, mint y, size_t count);
void	Gui_getStartPos			(Rectangle border, mint* x, mint* y);
void	Gui_printPhoneticSymbol	(mint index, mint x, mint y);	//输出一个音标
void	Gui_printChar			(char letter, mint x, mint y);	//输出正常字符
void	Gui_printHanzi			(const char* letter, mint x, mint y);	//输出中文
bool	Gui_checkSpace			(const char* word, mint x, mint y, Rectangle border);
void	Gui_printWord			(char* word, mint x, mint y);
void	Gui_printWordBg			(char* word, mint x, mint y);

void	Rectangle_print			(Rectangle rect, bool isFill);
void	Rectangle_erase			(Rectangle rect);
void	Rectangle_eraseContent	(Rectangle border);
void	Rectangle_highlight		(Rectangle rect);
void	Rectangle_unhighlight	(Rectangle rect);
#endif
