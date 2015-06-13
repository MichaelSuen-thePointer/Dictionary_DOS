
#include "gui.h"
#include "gui_phon.h"


//private variables and functions declaration
//static�ļ�����������ͺ��������ڿ��Ʒ���Ȩ��

static muint Gui_fgcolor;	//32λ�޷���ǰ��ɫ
static muint Gui_bgcolor;	//32λ�޷��ű���ɫ
static FILE* hzLib;			//���ֿ��ļ����
static Rectangle zeroRect;

static void			Gui_printDotMat(const char* matrix, mint width, mint height, mint x, mint y);	//���witdh*height����
static const char*	Gui_getHanziDotMat(const char* letter);	//��ȡ���ֵ���
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
{//��xy���괦���һ�����֣�����maxlength֮��������׵�
	Rectangle border;
	//���������Χ��
	border.m_left = x;
	border.m_top = y;
	border.m_right = x + maxlength;
	border.m_bottom = y + 2 * GUI_BORDEROFFSET;
	Gui_printText(text, border);	//ת�����������
}
void Rectangle_eraseContent(Rectangle border)
{ //�������������ͼ��
	setfillstyle(SOLID_FILL, Gui_bgcolor);
	bar(border.m_left + 3, border.m_top + 3, border.m_right - 3, border.m_bottom - 3);
}
void Gui_printWord(char* word, mint x, mint y)
{ //��ǰ��ɫ���һ���ַ���
	setcolor(Gui_fgcolor);
	outtextxy(x, y, word);
}
void Gui_printWordBg(char* word, mint x, mint y)
{ //�ñ���ɫ���һ���ַ���
	int oldColor = Gui_fgcolor;
	Gui_fgcolor = Gui_bgcolor;
	Gui_printWord(word, x, y);
	Gui_fgcolor = oldColor;
	setcolor(Gui_fgcolor);
}
static void Gui_printline(char* word, mint* x, mint* y)
{ //Σ�պ�������ֹ�ⲿ���ã��˺������������߽磬�������ַ������ܵ����ڴ����
	static bool isPhonetic = false;
	while (*word)
	{ //���ÿ�����ʣ�����ո�
		mint byteOut = 0;
		if (iswchar(*word))
		{ //�������
			Gui_printHanzi(word, *x, *y);
			byteOut = 2;
		}
		else
		{ //��������ַ�
			if (*word == '[')
			{ //��������ģʽ
				isPhonetic = true;
			}
			if (*word == ']')
			{ //�˳�����ģʽ
				isPhonetic = false;
			}
			if (isPhonetic && *word >= '/' && *word <= '9')
			{ //�������
				Gui_printPhoneticSymbol(*word - '/', *x, *y);
			}
			else
			{ //��������ַ�
				Gui_printChar(*word, *x, *y);
			}
			byteOut = 1;
		}
		*x += byteOut * GUI_CHARWIDTH; //ֱ���ƶ����꣬�����߽�
		word += byteOut; //�ƶ�ָ��
	}
}
void Gui_printText(char* letter, Rectangle border)
{ //��border�߽���������������׵�
	Vector lines = Str_split(letter, '\n'); //���ı�������split
	size_t i;
	mint x = border.m_left + GUI_BORDEROFFSET;
	mint y = border.m_top + GUI_BORDEROFFSET;
	for (i = 0; i < Vector_getLength(lines); i++)
	{ //����ÿ��
		Vector words = Str_split(Vector_getPointer(lines, i), ' '); //�ٰ��ո���
		size_t j;
		for (j = 0; j < Vector_getLength(words); j++)
		{ //����ÿ������
			char* word = Vector_getPointer(words, j);
			if (!Gui_checkSpace(word, x, y, border))
			{ //���ʣ�µĿռ��Ƿ񹻣������ͻ���
				BREAK_IF(!Gui_nextLine(&x, &y, border, 1)); //Ҫ�ǻ��к�ռ�Ҳ������ֹͣ���������ټ���
			}
			Gui_printline(word, &x, &y); //�Ѿ���֤word���ᳬ������߽磬����printline����
			Gui_movePos(&x, &y, border, 1);
		}
		Str_freeFragments(words); //�ͷŴ洢split�ַ��������ݽṹ
		BREAK_IF(!Gui_nextLine(&x, &y, border, 1)); //���ÿһ��֮��Ҫ���У����û�ռ��ֹͣ���
	}
	Str_freeFragments(lines); //�ͷŴ洢split�ַ��������ݽṹ
}
bool Gui_checkSpace(const char* word, mint x, mint y, Rectangle border)
{ //��鱾���Ƿ����㹻�Ŀռ�����ַ�
	if (x + ((mint)strlen(word) + 1) * GUI_CHARWIDTH + GUI_BORDEROFFSET > border.m_right)
	{
		return false;
	}
	return true;
}
bool Gui_movePos(mint* x, mint* y, Rectangle border, mint byteCount)
{ //�ƶ���꣬byteCount�����ɸ�������ص������ף������ƶ����ܻ��У������λ��������򷵻�true
	*x += byteCount * GUI_CHARWIDTH;

	if (*x > border.m_right - GUI_BORDEROFFSET)
	{ //�����ұ߽�
		*x = border.m_left + GUI_BORDEROFFSET;
		*y += GUI_LINEHEIGHT;
	}
	else
	{ //�������߽���߳�����߽�
		*x = MAX(border.m_left + GUI_BORDEROFFSET, *x); //�����֮�������
	}
	if (*y + GUI_CHARHEIGHT > border.m_bottom - GUI_BORDEROFFSET)
	{ //�����±߽�
		return false;
	}
	return true;
}
void Gui_printPhoneticSymbol(mint index, mint x, mint y)
{ //���д�ӡ������
	Gui_printDotMat(Gui_phonetic[index], GUI_CHARWIDTH, GUI_CHARHEIGHT, x, y);
}
void Gui_printChar(char letter, mint x, mint y)
{ //����ϵͳ�Դ�����
	char str[2];
	str[0] = letter;
	str[1] = '\0';
	setcolor(Gui_fgcolor);
	outtextxy(x, y, str);
}
void Gui_printHanzi(const char* letter, mint x, mint y)
{ //���д�ӡ������
	const char* buf = Gui_getHanziDotMat(letter);
	Gui_printDotMat(buf, GUI_HANZIWIDTH, GUI_HANZIHEIGHT, x, y);
	//Gui_printChar('_', x, y);
	//Gui_printChar('_', x + 8, y);
}
void Gui_printDotMat(const char* matrix, mint width, mint height, mint x, mint y)
{ //����Ϊ���󣬿�ȣ�8�ı��������߶ȣ�����
	mint i, j, k;
	mint dup = width / GUI_CHARWIDTH;	//�ַ���ȣ�1��2���㷨��֧�ָ�����ַ�
	for (j = 0; j < height * dup; j += dup)
	{ //������
		for (k = 0; k < dup; k++)
		{ //���ַ���ÿһ��
			for (i = 0; i < GUI_CHARWIDTH; i++)
			{ //ÿһ�ߵ�ÿ8bit
				(matrix[j + k] >> (GUI_CHARWIDTH - 1 - i)) & 0x01
					? putpixel(x + k * GUI_CHARWIDTH + i, y + j / dup, Gui_fgcolor)
					: putpixel(x + k * GUI_CHARWIDTH + i, y + j / dup, Gui_bgcolor);
			}
		}
	}
}

bool Gui_nextLine(mint* x, mint* y, Rectangle border, mint lineCount)
{ //���в�����Ƿ��пռ����
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
{ //Ϳ��һ���ַ���������ĸ
	setfillstyle(SOLID_FILL, Gui_bgcolor);
	bar(x, y, x + GUI_CHARWIDTH, y + GUI_CHARHEIGHT);
}
void Gui_eraseCharNRect(mint x, mint y, size_t count)
{
	setfillstyle(SOLID_FILL, Gui_bgcolor);
	bar(x, y, x + GUI_CHARWIDTH * count, y + GUI_CHARHEIGHT);
}

void Rectangle_print(Rectangle rect, bool isFill)
{ //����isFill�趨�Ƿ������ʾ����
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
{ //������
	setfillstyle(SOLID_FILL, Gui_bgcolor);
	bar(rect.m_left, rect.m_top, rect.m_right, rect.m_bottom);
}
mint Gui_setbgColor(mint color)
{ //���ñ���ɫ������ԭ���ı���ɫ
	mint oldcolor = Gui_bgcolor;
	Gui_bgcolor = color;
	return oldcolor;
}
mint Gui_setfgColor(mint color)
{ //����ǰ��ɫ������ԭ����ǰ��ɫ
	mint oldcolor = Gui_fgcolor;
	Gui_fgcolor = color;
	return oldcolor;
}
const char* Gui_getHanziDotMat(const char* letter)
{ //��ȡ���ֵ���
	static char hzDotMat[32];
	long offset = ((letter[0] - 0xA1) * 94 + (letter[1] - 0xA1)) * 0x20;
	fseek(hzLib, offset, SEEK_SET);
	fread(hzDotMat, sizeof(hzDotMat), 1, hzLib);
	return hzDotMat;
}
int Gui_openhzLib(void)
{ //�򿪺��ֿ�
	hzLib = fopen("HZK16", "rb");
	return hzLib != NULL;
}
int Gui_closehzLib(void)
{ //�رպ��ֿ�
	return fclose(hzLib);
}
void Gui_getStartPos(Rectangle border, mint* x, mint* y)
{ //��ȡ�߽�����ʼ���λ��
	*x = border.m_left + GUI_BORDEROFFSET;
	*y = border.m_top + GUI_BORDEROFFSET;
}
