#include "status.h"

static Vector recursionDeep;

void Deep_init(void)
{
	recursionDeep = Vector_new(10);
}
void Deep_push(const char* str)
{
	Vector_push_back(recursionDeep, Str_copy(str));
}
void Deep_pop(void)
{
	free(Vector_pop_back(recursionDeep));
}
void Deep_show(Rectangle border)
{
	size_t maxlen = (border.m_right - border.m_left) / GUI_CHARWIDTH;
	size_t len =0;
	size_t start = 0;
	size_t i;
	char* outstr;
	Vector_rforeach(const char*, str, recursionDeep)
	{
		len += strlen(str) + STATUS_DELIMLEN;
		if (len > maxlen)
		{
			start = _i + 1;
			break;
		}
	}
	Vector_loopend;
	outstr = calloc(maxlen, sizeof(outstr[0]));
	for (i = start; i < Vector_getLength(recursionDeep); i++)
	{
		strcat(outstr, Vector_getPointer(recursionDeep, i));
		strcat(outstr, STATUS_DELIMETER);
	}
	Rectangle_eraseContent(border);
	Gui_printText(outstr, border);
}
void Deep_uninit(void)
{
	Vector_foreach(char*, elem, recursionDeep)
	{
		free(elem);
	}
	Vector_loopend;
	Vector_free(&recursionDeep);
}
