#include "idx.h"

//private variables and functions declaration
//static�ļ�����������ͺ��������ڿ��Ʒ���Ȩ��

static IDX* wordlist;	//�ֵ��
static size_t listlen;	//�ֵ䳤��
static FILE* dictLib;	//���ͺ�������ļ����
static char explanation[DICT_BUFFERLEN];	//������͵ľֲ�����
static char example[DICT_BUFFERLEN];	//��������ľֲ�����
static Vector IDXlist;
//Dict_readdic()������qsortʹ�õıȽϺ������ⲿ���ɼ�
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
{ //���ֵ�
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
	//C99 �Դ���������, ����Сд������ͣ��
	IDXlist = Vector_new(listlen);
	for (i = 0; i < listlen; i++)
	{ //��Ԫ��ָ�뱣����vector��
		Vector_push_back(IDXlist, wordlist + i);
		//debug���
		//printf("%s %#X %hd %hd\n", wordlist[i].m_entry, wordlist[i].m_libOffset, wordlist[i].xlat_len, wordlist[i].exam_len);
	}
	return listlen;
}
mint Dict_idxConpare(const void* a, const void* b)
{//����qsort���ֵ���Ŀ�ȽϺ������ò�����
	return strncmp(((const IDX*)a)->m_entry, ((const IDX*)b)->m_entry, 20);
}
size_t Dict_getIndex(const char* word)
{//���ֲ��һ�ȡ��Ŀ�±꣬����ʧ���򷵻�(size_t)-1���޷������͵����ֵ��
	size_t left = 0, right = listlen, i;
	char* entryLowerClass;
	char* wordLowerClass;
	mint result;

	while (left < right)
	{	//�����Լ����Ż�
		i = (left + right) / 2;

		//��Сд������
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
{ //����֮ǰ�������Dict_getInfo()����\r\n�滻Ϊ\n
	Dict_fixnewline(explanation);
	return explanation;
}
char* Dict_getExample(void)
{ //����֮ǰ�������Dict_getInfo()
	Dict_fixnewline(example);
	return example;
}
void Dict_getInfo(IDX* pidx)
{ //��ȡ���ͺ����䣬������˽�б�����
	memset(explanation, 0, sizeof(explanation));
	memset(example, 0, sizeof(example));
	fseek(dictLib, pidx->m_libOffset, SEEK_SET);
	fread(explanation, MIN(pidx->xlat_len, sizeof(explanation) - 1), 1, dictLib);
	fread(example, MIN(pidx->exam_len, sizeof(example) - 1), 1, dictLib);
}
char* Dict_fixnewline(char* text)
{ //������\r\n�滻Ϊ\n
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
{ //����˽�б����Ľӿڶ���
	return IDXlist;
}
size_t Dict_getWordCount(void)
{ //����˽�б����Ľӿڶ���
	return listlen;
}
void Dict_freewordlist(void)
{ //�ͷŵ�wordlist�������˳�ʱʹ��
	free(wordlist);
	Vector_free(&IDXlist);
}
Vector Dict_search(FSM fsm)
{ //����һ������״̬����ƥ��ȫ�ֵ䣬��ƥ��������vector����
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
