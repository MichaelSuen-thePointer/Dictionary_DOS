#include "ctrl.h"

//private variables and functions declaration
//static文件作用域变量和函数，用于控制访问权限

static char inputbuffer[256];	//输入缓存
static size_t bufferlen;		//已输出长度

char* Ctrl_readline(Rectangle border)
{ //从键盘读入，回车结尾，border为输入框大小
	int key;
	int x, y;
	size_t caretPos = 0;
	bufferlen = 0;
	inputbuffer[0] = '\0';
	Gui_getStartPos(border, &x, &y);
	Rectangle_eraseContent(border);	//清除输入框
	Ctrl_showCaret(caretPos, 0, 1, border);

	while (bufferlen < 256)
	{ //循环，退出条件为buffer满或按回车
		if (bioskey(1))
		{
			key = bioskey(0);
			if (key == CTRL_ENTER)
			{ //回车，输入结束
				inputbuffer[bufferlen] = '\0';
				break;
			}
			else if (key == CTRL_BKSPACE)
			{ //退格，后面字符前移
				if (caretPos)
				{
					bufferlen--;
					Str_strmov(inputbuffer + caretPos, -1);
					caretPos--;
					Gui_eraseCharNRect(x + GUI_CHARWIDTH * caretPos, y, bufferlen - caretPos + 2);
					Gui_printWord(inputbuffer + caretPos, x + GUI_CHARWIDTH * caretPos, y);
				}
			}
			else if (key == CTRL_DEL)
			{ //DEL，差不多同上
				if (caretPos < bufferlen)
				{
					Str_strmov(inputbuffer + caretPos + 1, -1);
					Gui_eraseCharNRect(x + GUI_CHARWIDTH * caretPos, y, bufferlen - caretPos + 2);
					Gui_printWord(inputbuffer + caretPos, x + GUI_CHARWIDTH * caretPos, y);
					bufferlen--;
				}
			}
			else if (key == CTRL_LEFT)
			{ //光标左移
				if (caretPos > 0)
				{
					Ctrl_hideCaret(caretPos, 0, 1, border);
					caretPos--;
				}
			}
			else if (key == CTRL_RIGHT)
			{ //光标右移
				if (caretPos < bufferlen)
				{
					Ctrl_hideCaret(caretPos, 0, 1, border);
					caretPos++;
				}
			}
			else if (key == CTRL_ESC)
			{ //清空
				Gui_eraseCharNRect(x, y, bufferlen + 2);
				inputbuffer[0] = 0;
				bufferlen = 0;
				caretPos = 0;
			}
			else if (isprint(CTRL_ASCII(key)))
			{ //正常字符，读入
				Str_strmov(inputbuffer + caretPos, +1);
				inputbuffer[caretPos] = CTRL_ASCII(key);
				Gui_eraseCharNRect(x + GUI_CHARWIDTH * caretPos, y, bufferlen - caretPos + 2);
				Gui_printWord(inputbuffer + caretPos, x + GUI_CHARWIDTH * caretPos, y);
				bufferlen++;
				caretPos++;
			}
			else
			{ //非法字符跳过
				continue;
			}
			//刷新
			//Gui_eraseCharNRect(x, y, bufferlen + 2);
			//Gui_printWord(inputbuffer, x, y);
			Ctrl_showCaret(caretPos, 0, 1, border);
		}
	}
	Ctrl_hideCaret(caretPos, 0, 1, border);

	return inputbuffer;
}
void Ctrl_showCaret(mint col, mint ln, size_t length, Rectangle border)
{ //画caret
	static char caret[] = "______________________";
	mint x, y;
	Gui_getStartPos(border, &x, &y);
	Gui_nextLine(&x, &y, border, ln);
	Gui_movePos(&x, &y, border, col);
	Gui_printWord(caret + sizeof(caret) - length - 1, x, y + 1);
	Gui_printWord(caret + sizeof(caret) - length - 1, x, y + 2);
}
void Ctrl_hideCaret(mint col, mint ln, size_t length, Rectangle border)
{
	Gui_setfgColor(GUI_BLACK);
	Ctrl_showCaret(col, ln, length, border);
	Gui_setfgColor(GUI_GRAY);
}
void Ctrl_forceShowWordList(const Vector IDXlist, size_t index, Rectangle border)
{ //强制性输出一个搜索结果列表，此函数已废弃
	size_t elemCount = (border.m_bottom - border.m_top - 2 * GUI_BORDEROFFSET) / GUI_LINEHEIGHT;
	size_t start = index >= elemCount / 2 ? index - elemCount / 2 : 0;
	size_t end = MIN(start + elemCount, Vector_getLength(IDXlist));
	Rectangle_eraseContent(border);
	Ctrl_reprintWordList(IDXlist, start, end, border);
	Ctrl_showCaret(0, index - start, 20, border);
}
void Ctrl_showWordlist(const Vector IDXlist, size_t index, Rectangle border, bool forceShow)
{ //接受一个vector，元素为IDX指针，index为当前选中项
	//这个函数好丑，好丑，好丑，好丑
	//static变量记录上一次调用的信息
	static size_t lastindex;
	static size_t start, end;
	size_t elemCount = (border.m_bottom - border.m_top - 2 * GUI_BORDEROFFSET) / GUI_LINEHEIGHT;

	//Rectangle_eraseContent(border);
	if (forceShow)
	{ //强制显示
		start = index >= elemCount / 2 ? index - elemCount / 2 : 0;
		end = MIN(start + elemCount, Vector_getLength(IDXlist));
		start = end >= elemCount ? end - elemCount : 0;
		elemCount = (border.m_bottom - border.m_top - 2 * GUI_BORDEROFFSET) / GUI_LINEHEIGHT;
		//Rectangle_eraseContent(border);
		Ctrl_reprintWordList(IDXlist, start, end, border);
		Ctrl_showCaret(0, index - start, 20, border);
		lastindex = index;
		return;
	}
	//非强制显示
	//先擦掉之前的横线
	Ctrl_hideCaret(0, lastindex - start, 20, border);
	if (index != lastindex && (index < start || index >= end))
	{ //是同一个list，但是下标已经超出显示区域了
		size_t listLen = Vector_getLength(IDXlist);
		end += index - lastindex;
		end = end >= listLen ? listLen : end;
		start = end >= elemCount ? end - elemCount : 0;
		end = start + elemCount >= listLen ? listLen : start + elemCount;
		Ctrl_reprintWordList(IDXlist, start, end, border);
	}
	//Gui_setfgColor(GUI_GRAY);
	Ctrl_showCaret(0, index - start, 20, border);
	lastindex = index;
}
void Ctrl_reprintWordList(const Vector IDXlist, size_t start, size_t end, Rectangle border)
{ //重画列表，单提出来作为一个函数
	size_t i;
	mint x, y;
	Gui_getStartPos(border, &x, &y);
	for (i = start; i < end; i++)
	{
		Gui_eraseCharNRect(x, y, 20);
		Gui_printWord(((IDX*)Vector_getPointer(IDXlist, i))->m_entry, x, y);
		Gui_nextLine(&x, &y, border, 1);
	}
	Gui_nextLine(&x, &y, border, -1);
	while (Gui_nextLine(&x, &y, border, 1))
	{
		Gui_eraseCharNRect(x, y, 20);
	}
}
mint Ctrl_readKey(void)
{ //顾名思义
	while (bioskey(1))
	{
		bioskey(0);
	}
	return bioskey(0);
}
