#include "idx.h"

//private variables and functions declaration
//static文件作用域变量和函数，用于控制访问权限

static IDX* wordlist;	//字典表
static size_t listlen;	//字典长度
static FILE* dictLib;	//解释和例句的文件句柄
static char explanation[DICT_BUFFERLEN];	//保存解释的局部变量
static char example[DICT_BUFFERLEN];	//保存例句的局部变量
static Vector IDXlist;
//Dict_readdic()函数内qsort使用的比较函数，外部不可见
static mint Dict_idxConpare(const void* a, const void* b);

mint Dict_openLib(void)
{
	dictLib = fopen("dict.lib", "rb");
	return dictLib != NULL;
}
mint Dict_closeLib(void)
{
	return fclose(dictLib);
}
mint Dict_readdic(void)/*copyright bhh ^_^*/
{ //读字典
	FILE* fpdict;
	long len;
	size_t i;
	fpdict = fopen("dict.idx", "rb");
	if (fpdict == NULL)
	{
		return DICT_OPENFAILED;
	}
	fseek(fpdict, 0, SEEK_END);
	len = ftell(fpdict);
	fseek(fpdict, 0, SEEK_SET);
	listlen = len / sizeof(IDX);
	wordlist = calloc(listlen, sizeof(IDX));
	if (wordlist == NULL)
	{
		return DICT_ALLOCATIONFAILED;
	}
	fread(wordlist, sizeof(IDX), listlen, fpdict);
	fclose(fpdict);
	//qsort(wordlist, listlen, sizeof(wordlist[0]), Dict_idxConpare);
	//C99 自带快速排序, 按大小写排序，已停用
	IDXlist = Vector_new(listlen);
	for (i = 0; i < listlen; i++)
	{ //将元素指针保存在vector中
		Vector_push_back(IDXlist, wordlist + i);
		//debug输出
		//printf("%s %#X %hd %hd\n", wordlist[i].m_entry, wordlist[i].m_libOffset, wordlist[i].xlat_len, wordlist[i].exam_len);
	}
	return listlen;
}
mint Dict_idxConpare(const void* a, const void* b)
{//用于qsort的字典条目比较函数，用不上了
	return strncmp(((const IDX*)a)->m_entry, ((const IDX*)b)->m_entry, 20);
}
size_t Dict_getIndex(const char* word)
{//二分查找获取条目下标，查找失败则返回(size_t)-1（无符号类型的最大值）
	size_t left = 0, right = listlen, i;
	char* entryLowerClass;
	char* wordLowerClass;
	mint result;

	while (left < right)
	{	//还可以继续优化
		i = (left + right) / 2;

		//大小写不敏感
		entryLowerClass = Str_toLowerCase(Str_copyN(wordlist[i].m_entry, 20));
		wordLowerClass = Str_toLowerCase(Str_copy(word));
		result = strncmp(word, entryLowerClass, 20);
		free(entryLowerClass);
		free(wordLowerClass);
		if (result > 0)
		{
			left = i + 1;
		}
		else if (result < 0)
		{
			right = i;
		}
		else
		{
			return i;
		}
	}
	return -1;
}
char* Dict_getExplanation(void)
{ //调用之前必须调用Dict_getInfo()，将\r\n替换为\n
	Dict_fixnewline(explanation);
	return explanation;
}
char* Dict_getExample(void)
{ //调用之前必须调用Dict_getInfo()
	Dict_fixnewline(example);
	return example;
}
void Dict_getInfo(IDX* pidx)
{ //获取解释和例句，保存在私有变量内
	memset(explanation, 0, sizeof(explanation));
	memset(example, 0, sizeof(example));
	fseek(dictLib, pidx->m_libOffset, SEEK_SET);
	fread(explanation, MIN(pidx->xlat_len, sizeof(explanation) - 1), 1, dictLib);
	fread(example, MIN(pidx->exam_len, sizeof(example) - 1), 1, dictLib);
}
char* Dict_fixnewline(char* text)
{ //将所有\r\n替换为\n
	mint offset = 0;
	mint i;
	for (i = 0; text[i + offset]; i++)
	{
		if (text[i + offset] == '\r')
		{
			offset++;
		}
		text[i] = text[i + offset];
	}
	text[i] = text[i + offset];
	return text;
}
const Vector Dict_getWordList(void)
{ //返回私有变量的接口而已
	return IDXlist;
}
size_t Dict_getWordCount(void)
{ //返回私有变量的接口而已
	return listlen;
}
void Dict_freewordlist(void)
{ //释放掉wordlist，程序退出时使用
	free(wordlist);
	Vector_free(&IDXlist);
}
Vector Dict_search(FSM fsm)
{ //接受一个有限状态机，匹配全字典，将匹配项做成vector返回
	Vector result = Vector_new(0);
	size_t i;
	for (i = 0; i < listlen; i++)
	{
		int state = 1;
		char* curr = wordlist[i].m_entry;
		do
		{
			state = fsm[state][tolower(*curr++)];
		} while (state != 0 && *curr);
		if (fsm[state][0] == -1 && *curr == '\0')
		{
			Vector_push_back(result, wordlist + i);
		}
	}
	return result;
}
IDX* Dict_getIDX(size_t index)
{
	if (index != (size_t)-1 && index < listlen)
	{
		return wordlist + index;
	}
	else
	{
		return NULL;
	}
}
