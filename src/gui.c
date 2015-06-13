
#include "gui.h"
#include "gui_phon.h"


//private variables and functions declaration
//static文件作用域变量和函数，用于控制访问权限

static muint Gui_fgcolor;	//32位无符号前景色
static muint Gui_bgcolor;	//32位无符号背景色
static FILE* hzLib;			//汉字库文件句柄
static Rectangle zeroRect;

static void			Gui_printDotMat(const char* matrix, mint width, mint height, mint x, mint y);	//输出witdh*height点阵
static const char*	Gui_getHanziDotMat(const char* letter);	//获取汉字点阵
static void			Gui_printline(char* word, mint* x, mint* y);
void Gui_initialize(void)
{
	mint driver = 0;
	mint mode = VESA_1024x768x24bit;
	initgraph(&driver, &mode, "");
}
void Gui_close(void)
{
	closegraph();
}
void Gui_printTextXY(char* text, mint x, mint y, mint maxlength)
{//在xy坐标处输出一行文字，超出maxlength之后的内容抛掉
	Rectangle border;
	//构建输出范围框
	border.m_left = x;
	border.m_top = y;
	border.m_right = x + maxlength;
	border.m_bottom = y + 2 * GUI_BORDEROFFSET;
	Gui_printText(text, border);	//转给框输出函数
}
void Rectangle_eraseContent(Rectangle border)
{ //擦除整个输出框图形
	setfillstyle(SOLID_FILL, Gui_bgcolor);
	bar(border.m_left + 3, border.m_top + 3, border.m_right - 3, border.m_bottom - 3);
}
void Gui_printWord(char* word, mint x, mint y)
{ //用前景色输出一段字符串
	setcolor(Gui_fgcolor);
	outtextxy(x, y, word);
}
void Gui_printWordBg(char* word, mint x, mint y)
{ //用背景色输出一段字符串
	int oldColor = Gui_fgcolor;
	Gui_fgcolor = Gui_bgcolor;
	Gui_printWord(word, x, y);
	Gui_fgcolor = oldColor;
	setcolor(Gui_fgcolor);
}
static void Gui_printline(char* word, mint* x, mint* y)
{ //危险函数，禁止外部调用，此函数不检查输出边界，过长的字符串可能导致内存溢出
	static bool isPhonetic = false;
	while (*word)
	{ //输出每个单词，后跟空格
		mint byteOut = 0;
		if (iswchar(*word))
		{ //输出汉字
			Gui_printHanzi(word, *x, *y);
			byteOut = 2;
		}
		else
		{ //输出其他字符
			if (*word == '[')
			{ //进入音标模式
				isPhonetic = true;
			}
			if (*word == ']')
			{ //退出音标模式
				isPhonetic = false;
			}
			if (isPhonetic && *word >= '/' && *word <= '9')
			{ //输出音标
				Gui_printPhoneticSymbol(*word - '/', *x, *y);
			}
			else
			{ //输出其他字符
				Gui_printChar(*word, *x, *y);
			}
			byteOut = 1;
		}
		*x += byteOut * GUI_CHARWIDTH; //直接移动坐标，不检查边界
		word += byteOut; //移动指针
	}
}
void Gui_printText(char* letter, Rectangle border)
{ //在border边界框内输出，多余的抛掉
	Vector lines = Str_split(letter, '\n'); //将文本按换行split
	size_t i;
	mint x = border.m_left + GUI_BORDEROFFSET;
	mint y = border.m_top + GUI_BORDEROFFSET;
	for (i = 0; i < Vector_getLength(lines); i++)
	{ //对于每行
		Vector words = Str_split(Vector_getPointer(lines, i), ' '); //再按空格拆分
		size_t j;
		for (j = 0; j < Vector_getLength(words); j++)
		{ //对于每个单词
			char* word = Vector_getPointer(words, j);
			if (!Gui_checkSpace(word, x, y, border))
			{ //检查剩下的空间是否够，不够就换行
				BREAK_IF(!Gui_nextLine(&x, &y, border, 1)); //要是换行后空间也不够就停止输出（情况少见）
			}
			Gui_printline(word, &x, &y); //已经保证word不会超出输出边界，调用printline函数
			Gui_movePos(&x, &y, border, 1);
		}
		Str_freeFragments(words); //释放存储split字符串的数据结构
		BREAK_IF(!Gui_nextLine(&x, &y, border, 1)); //输出每一行之后要换行，如果没空间就停止输出
	}
	Str_freeFragments(lines); //释放存储split字符串的数据结构
}
bool Gui_checkSpace(const char* word, mint x, mint y, Rectangle border)
{ //检查本行是否有足够的空间输出字符
	if (x + ((mint)strlen(word) + 1) * GUI_CHARWIDTH + GUI_BORDEROFFSET > border.m_right)
	{
		return false;
	}
	return true;
}
bool Gui_movePos(mint* x, mint* y, Rectangle border, mint byteCount)
{ //移动光标，byteCount可正可负，最左回到本行首，向右移动可能换行，如果有位置输出，则返回true
	*x += byteCount * GUI_CHARWIDTH;

	if (*x > border.m_right - GUI_BORDEROFFSET)
	{ //超出右边界
		*x = border.m_left + GUI_BORDEROFFSET;
		*y += GUI_LINEHEIGHT;
	}
	else
	{ //不超出边界或者超出左边界
		*x = MAX(border.m_left + GUI_BORDEROFFSET, *x); //有则改之无则加勉
	}
	if (*y + GUI_CHARHEIGHT > border.m_bottom - GUI_BORDEROFFSET)
	{ //超出下边界
		return false;
	}
	return true;
}
void Gui_printPhoneticSymbol(mint index, mint x, mint y)
{ //呼叫打印点阵函数
	Gui_printDotMat(Gui_phonetic[index], GUI_CHARWIDTH, GUI_CHARHEIGHT, x, y);
}
void Gui_printChar(char letter, mint x, mint y)
{ //呼叫系统自带函数
	char str[2];
	str[0] = letter;
	str[1] = '\0';
	setcolor(Gui_fgcolor);
	outtextxy(x, y, str);
}
void Gui_printHanzi(const char* letter, mint x, mint y)
{ //呼叫打印点阵函数
	const char* buf = Gui_getHanziDotMat(letter);
	Gui_printDotMat(buf, GUI_HANZIWIDTH, GUI_HANZIHEIGHT, x, y);
	//Gui_printChar('_', x, y);
	//Gui_printChar('_', x + 8, y);
}
void Gui_printDotMat(const char* matrix, mint width, mint height, mint x, mint y)
{ //输入为点阵，宽度（8的倍数），高度，坐标
	mint i, j, k;
	mint dup = width / GUI_CHARWIDTH;	//字符宽度，1或2，算法上支持更宽的字符
	for (j = 0; j < height * dup; j += dup)
	{ //纵坐标
		for (k = 0; k < dup; k++)
		{ //宽字符的每一边
			for (i = 0; i < GUI_CHARWIDTH; i++)
			{ //每一边的每8bit
				(matrix[j + k] >> (GUI_CHARWIDTH - 1 - i)) & 0x01
					? putpixel(x + k * GUI_CHARWIDTH + i, y + j / dup, Gui_fgcolor)
					: putpixel(x + k * GUI_CHARWIDTH + i, y + j / dup, Gui_bgcolor);
			}
		}
	}
}

bool Gui_nextLine(mint* x, mint* y, Rectangle border, mint lineCount)
{ //换行并检查是否还有空间输出
	*x = border.m_left + GUI_BORDEROFFSET;
	*y += GUI_LINEHEIGHT * lineCount;
	if (*y + GUI_LINEHEIGHT > border.m_bottom - GUI_BORDEROFFSET)
	{
		//*y -= GUI_LINEHEIGHT * lineCount;
		return false;
	}
	return true;
}

void Gui_eraseCharRect(mint x, mint y)
{ //涂掉一个字符，仅限字母
	setfillstyle(SOLID_FILL, Gui_bgcolor);
	bar(x, y, x + GUI_CHARWIDTH, y + GUI_CHARHEIGHT);
}
void Gui_eraseCharNRect(mint x, mint y, size_t count)
{
	setfillstyle(SOLID_FILL, Gui_bgcolor);
	bar(x, y, x + GUI_CHARWIDTH * count, y + GUI_CHARHEIGHT);
}

void Rectangle_print(Rectangle rect, bool isFill)
{ //画框，isFill设定是否填充显示区域
	setcolor(Gui_fgcolor);
	if (isFill)
	{
		setfillstyle(SOLID_FILL, Gui_bgcolor);
		bar(rect.m_left, rect.m_top, rect.m_right, rect.m_bottom);
	}
	rectangle(rect.m_left, rect.m_top, rect.m_right, rect.m_bottom);
	rectangle(rect.m_left + 2, rect.m_top + 2, rect.m_right - 2, rect.m_bottom - 2);
}
void Rectangle_highlight(Rectangle rect)
{
	static Rectangle lastOne;
	if (!bineq(lastOne, zeroRect) && !bineq(lastOne, rect))
	{
		Rectangle_unhighlight(lastOne);
		setcolor(GUI_GREEN);
		rectangle(rect.m_left, rect.m_top, rect.m_right, rect.m_bottom);
		rectangle(rect.m_left + 2, rect.m_top + 2, rect.m_right - 2, rect.m_bottom - 2);
		setcolor(GUI_GRAY);
		lastOne = rect;
	}
}
void Rectangle_unhighlight(Rectangle rect)
{
	Rectangle_print(rect, false);
}
void Rectangle_erase(Rectangle rect)
{ //擦掉框
	setfillstyle(SOLID_FILL, Gui_bgcolor);
	bar(rect.m_left, rect.m_top, rect.m_right, rect.m_bottom);
}
mint Gui_setbgColor(mint color)
{ //设置背景色，返回原来的背景色
	mint oldcolor = Gui_bgcolor;
	Gui_bgcolor = color;
	return oldcolor;
}
mint Gui_setfgColor(mint color)
{ //设置前景色，返回原来的前景色
	mint oldcolor = Gui_fgcolor;
	Gui_fgcolor = color;
	return oldcolor;
}
const char* Gui_getHanziDotMat(const char* letter)
{ //获取汉字点阵
	static char hzDotMat[32];
	long offset = ((letter[0] - 0xA1) * 94 + (letter[1] - 0xA1)) * 0x20;
	fseek(hzLib, offset, SEEK_SET);
	fread(hzDotMat, sizeof(hzDotMat), 1, hzLib);
	return hzDotMat;
}
int Gui_openhzLib(void)
{ //打开汉字库
	hzLib = fopen("HZK16", "rb");
	return hzLib != NULL;
}
int Gui_closehzLib(void)
{ //关闭汉字库
	return fclose(hzLib);
}
void Gui_getStartPos(Rectangle border, mint* x, mint* y)
{ //获取边界框的起始输出位置
	*x = border.m_left + GUI_BORDEROFFSET;
	*y = border.m_top + GUI_BORDEROFFSET;
}
