#include "ctrl.h"

//private variables and functions declaration
//static�ļ�����������ͺ��������ڿ��Ʒ���Ȩ��

static char inputbuffer[256];	//���뻺��
static size_t bufferlen;		//���������

char* Ctrl_readline(Rectangle border)
{ //�Ӽ��̶��룬�س���β��borderΪ������С
	int key;
	int x, y;
	size_t caretPos = 0;
	bufferlen = 0;
	inputbuffer[0] = '\0';
	Gui_getStartPos(border, &x, &y);
	Rectangle_eraseContent(border);	//��������
	Ctrl_showCaret(caretPos, 0, 1, border);

	while (bufferlen < 256)
	{ //ѭ�����˳�����Ϊbuffer���򰴻س�
		if (bioskey(1))
		{
			key = bioskey(0);
			if (key == CTRL_ENTER)
			{ //�س����������
				inputbuffer[bufferlen] = '\0';
				break;
			}
			else if (key == CTRL_BKSPACE)
			{ //�˸񣬺����ַ�ǰ��
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
			{ //DEL�����ͬ��
				if (caretPos < bufferlen)
				{
					Str_strmov(inputbuffer + caretPos + 1, -1);
					Gui_eraseCharNRect(x + GUI_CHARWIDTH * caretPos, y, bufferlen - caretPos + 2);
					Gui_printWord(inputbuffer + caretPos, x + GUI_CHARWIDTH * caretPos, y);
					bufferlen--;
				}
			}
			else if (key == CTRL_LEFT)
			{ //�������
				if (caretPos > 0)
				{
					Ctrl_hideCaret(caretPos, 0, 1, border);
					caretPos--;
				}
			}
			else if (key == CTRL_RIGHT)
			{ //�������
				if (caretPos < bufferlen)
				{
					Ctrl_hideCaret(caretPos, 0, 1, border);
					caretPos++;
				}
			}
			else if (key == CTRL_ESC)
			{ //���
				Gui_eraseCharNRect(x, y, bufferlen + 2);
				inputbuffer[0] = 0;
				bufferlen = 0;
				caretPos = 0;
			}
			else if (isprint(CTRL_ASCII(key)))
			{ //�����ַ�������
				Str_strmov(inputbuffer + caretPos, +1);
				inputbuffer[caretPos] = CTRL_ASCII(key);
				Gui_eraseCharNRect(x + GUI_CHARWIDTH * caretPos, y, bufferlen - caretPos + 2);
				Gui_printWord(inputbuffer + caretPos, x + GUI_CHARWIDTH * caretPos, y);
				bufferlen++;
				caretPos++;
			}
			else
			{ //�Ƿ��ַ�����
				continue;
			}
			//ˢ��
			//Gui_eraseCharNRect(x, y, bufferlen + 2);
			//Gui_printWord(inputbuffer, x, y);
			Ctrl_showCaret(caretPos, 0, 1, border);
		}
	}
	Ctrl_hideCaret(caretPos, 0, 1, border);

	return inputbuffer;
}
void Ctrl_showCaret(mint col, mint ln, size_t length, Rectangle border)
{ //��caret
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
{ //ǿ�������һ����������б��˺����ѷ���
	size_t elemCount = (border.m_bottom - border.m_top - 2 * GUI_BORDEROFFSET) / GUI_LINEHEIGHT;
	size_t start = index >= elemCount / 2 ? index - elemCount / 2 : 0;
	size_t end = MIN(start + elemCount, Vector_getLength(IDXlist));
	Rectangle_eraseContent(border);
	Ctrl_reprintWordList(IDXlist, start, end, border);
	Ctrl_showCaret(0, index - start, 20, border);
}
void Ctrl_showWordlist(const Vector IDXlist, size_t index, Rectangle border, bool forceShow)
{ //����һ��vector��Ԫ��ΪIDXָ�룬indexΪ��ǰѡ����
	//��������ó󣬺ó󣬺ó󣬺ó�
	//static������¼��һ�ε��õ���Ϣ
	static size_t lastindex;
	static size_t start, end;
	size_t elemCount = (border.m_bottom - border.m_top - 2 * GUI_BORDEROFFSET) / GUI_LINEHEIGHT;

	//Rectangle_eraseContent(border);
	if (forceShow)
	{ //ǿ����ʾ
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
	//��ǿ����ʾ
	//�Ȳ���֮ǰ�ĺ���
	Ctrl_hideCaret(0, lastindex - start, 20, border);
	if (index != lastindex && (index < start || index >= end))
	{ //��ͬһ��list�������±��Ѿ�������ʾ������
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
{ //�ػ��б����������Ϊһ������
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
{ //����˼��
	while (bioskey(1))
	{
		bioskey(0);
	}
	return bioskey(0);
}
