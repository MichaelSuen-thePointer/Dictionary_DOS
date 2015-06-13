#include "strext.h"
void Str_freeFragments(Vector fragments)
{ //释放掉split的结果
	Vector_foreach(char*, str, fragments)
	{
		Atom_delete(str);
	}
	Vector_loopend;
	Vector_free(&fragments);
}

Vector Str_splitByWord(char* text)
{
	char* head = text;
	Vector fragrance = Vector_new(0);

	while (*head && *text)
	{
		if (Str_charClass(*text) != Str_charClass(*head++))
		{
			const char* str = Atom_new(text, head - text);
			Vector_push_back(fragrance, (void*)str);
			text = head + 1;
		}
	}
	return fragrance;
}

Vector Str_split(char* text, char delimiter)
{ //按delimiter拆分字符串
	char* head;
	Vector fragments = Vector_new(10);

	while ((head = strchr(text, delimiter)) != NULL)
	{
		Vector_push_back(fragments, (void*)Atom_new(text, head - text));
		text = head + 1;
	}
	Vector_push_back(fragments, (void*)Atom_string(text));

	return fragments;
}

mint Str_charClass(char ch)
{
	if (isspace(ch))
	{
		return SPACE;
	}
	if (isalpha(ch))
	{
		return ALPHA;
	}
	if (isdigit(ch))
	{
		return DIGIT;
	}
	if (iswchar(ch))
	{
		return HANZI;
	}
	return PUNCT;
}

void Str_strmov(char* str, mint offset)
{ //移动字符串
	int i, len = strlen(str);
	if (offset > 0)
	{
		for (i = len; i >= 0; i--)
		{
			str[i + offset] = str[i];
		}
	}
	else if (offset < 0)
	{
		for (i = 0; i <= len; i++)
		{
			str[i + offset] = str[i];
		}
	}
}
char* Str_toLowerCase(char* str)
{
	char* head = str;
	while (*str = tolower(*str))
	{
		str++;
	}
	return head;
}

char** Str_extractWords(char* text)
{
	Set words = Set_new(0, (int(*)(const void*, const void*))strcmp, NULL);
	char* head = text;

	char** strArray;
	while (*text && *head)
	{
		while (*text && iswchar(*text) || !isalpha(*text))
		{
			text++;
		}
		head = text;
		while (*head && iswchar(*head) || isalpha(*head))
		{
			head++;
		}
		if (head != text)
		{
			const char* str = Atom_new(Str_toLowerCase(text), head - text);
			Set_put(words, str);
		}
		text = head + 1;
	}
	strArray = (char**)Set_toArray(words, NULL);
	Set_free(&words);
	return strArray;
}

char* Str_copy(const char* text)
{
	return Str_copyN(text, strlen(text));
}

char* Str_copyN(const char* text, size_t count)
{
	char* str = malloc((count + 1) * sizeof(text[0]));
	memcpy(str, text, count);
	str[count] = '\0';
	return str;
}
