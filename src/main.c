#include "basic.h"
#include "idx.h"
#include "gui.h"
#include "ctrl.h"
#include "parser.h"
#include "regex.h"
#include "main.h"
#include "strext.h"
#include "except.h"
#include "status.h"
enum enum_flag {
	COMMAND,
	REGEX,
	RAWSTRING,
	WILDCARD,
	INPUT,
	SEARCH,
	EXPLANATION,
	EXAMPLE
};

static struct tag_status {
	bool exitFlag;
	mint matchPattern;
	Rectangle inputRect, explanRect, examplRect,
		searchRect, chooseRect, statusRect;
} status = {0, WILDCARD,
0, 0, 1023, 31,
176, 32, 847, 255,
176, 256, 847, 735,
0, 32, 175, 735,
848, 32, 1023, 735,
0, 736, 1023, 767
};

int main(void)
{
	char* input;
	//��ʼ��
	Main_initialize();
	Main_statusInitiailze();
	Main_confirmMessage("����ʹ�ã�������--help�鿴���� ");
	do
	{ //��ѭ��
		Rectangle_highlight(status.inputRect);
		Rectangle_eraseContent(status.examplRect);
		Rectangle_eraseContent(status.explanRect);
		input = Ctrl_readline(status.inputRect);
		Str_toLowerCase(input);
		switch (Main_kind(input))
		{ //��ȡ���������
		case COMMAND:
			//����
			Main_command(input);
			break;
		case REGEX:case WILDCARD:
			//ģ������
			Main_search(input);
			break;
		case RAWSTRING:
			//��ȷ����
			Main_rawstring(input);
			break;
		default:
			//�����ܵ��⣬���Ǹ���ת
			Main_confirmMessage("���򵽴���һ�������ܵ���ĵط������п�������Ϊ�ڲ����ж���ת���޸��ˣ��������Բ������ߵĴ� ");
			break;
		}
	} while (!status.exitFlag);
	Main_uninitialize();
	//�˳�
	return 0;
}

void Main_command(char* input)
{ //��������
	Vector argv = Str_split(input + MAIN_CMDOFFSET, ' '); //�������
	char* first = Vector_getPointer(argv, 0); //��ȡ��һ������
	if (streq(first, "exit"))
	{ //����˼��
		status.exitFlag = 1;
	}
	else if (streq(first, "matchpattern"))
	{ //�޸�ƥ��ģʽ
		if (Vector_getLength(argv) >= 2)
		{ //�޸�ģʽ
			char* second = Vector_getPointer(argv, 1);
			if (streq(second, "regex"))
			{
				status.matchPattern = REGEX;
			}
			else if (streq(second, "wildcard"))
			{
				status.matchPattern = WILDCARD;
			}
			else
			{
				Main_confirmMessage("��ֻ֧��regex��wildcard������ʽ���㻹����ô������ ");
			}
		}
		else
		{ //��ʾ��ǰģʽ
			if (status.matchPattern == REGEX)
			{
				Main_confirmMessage("match pattern: regex ");
			}
			else
			{
				Main_confirmMessage("match pattern: wildcard ");
			}
		}
	}
	else if (streq(first, "showall"))
	{ //��ʾ�����ֵ�
		size_t index = 0;
		//Main_printBlocks(); //������д���
		if (Vector_getLength(argv) >= 2)
		{
			sscanf(Vector_getPointer(argv, 1), "%u", &index);
			index = index >= Dict_getWordCount() ? Dict_getWordCount() - 1 : index;
		}
		Main_chooseFrom(Dict_getWordList(), index, status.searchRect);
	}
	else if (streq(first, "set"))
	{ //���ô��ڴ�С
		if (Vector_getLength(argv) >= 6)
		{ //�������������ô�С
			Main_changeBorder(argv);
		}
		else if (Vector_getLength(argv) >= 2)
		{ //�����������ָ�Ĭ��
			first = Vector_getPointer(argv, 1);
			if (streq(first, "default"))
			{
				Main_statusInitiailze();
			}
			else
			{ //����
				Main_confirmMessage("�������԰������ǲ���Ҫ����default ");
			}
		}
		else
		{ //����
			Main_confirmMessage("�����ʲô������ֻ��һ��set�ҿɲ�֪����Ҫ�޸��ĸ����ڵĴ�СŶ�� ");
		}
	}
	else if (streq(first, "help"))
	{ //����˼��
		char* str = malloc(sizeof(Main_helpDoc));
		str = strcpy(str, Main_helpDoc);
		Rectangle_eraseContent(status.examplRect);
		Gui_printText(str, status.examplRect);
		Ctrl_readKey();
		free(str);
	}
	else if (streq(first, "about"))
	{
		char* str = malloc(sizeof(Main_aboutDoc));
		str = strcpy(str, Main_aboutDoc);
		Rectangle_eraseContent(status.examplRect);
		Gui_printText(str, status.examplRect);
		Ctrl_readKey();
		free(str);
	}
	else
	{ //����
		Main_confirmMessage("Ȼ����û������������볢��--help ");
	}
	Str_freeFragments(argv);
}
void Main_changeBorder(Vector argv)
{ //�޸ı߿��С
	char* bordername = Vector_getPointer(argv, 1);
	char str[50];
	Rectangle* border;
	mint left, top, right, bottom, width, height;

	sprintf(str, "%s %s %s %s", Vector_getPointer(argv, 2), Vector_getPointer(argv, 3), Vector_getPointer(argv, 4), Vector_getPointer(argv, 5));
	sscanf(str, "%d %d %d %d", &left, &top, &right, &bottom);
	//�����൱��atoi�������鷳�˵�
	//һ���жϴ�����
	if (streq(bordername, "input"))
	{
		border = &status.inputRect;
	}
	else if (streq(bordername, "search"))
	{
		border = &status.searchRect;
	}
	else if (streq(bordername, "explanation"))
	{
		border = &status.explanRect;
	}
	else if (streq(bordername, "example"))
	{
		border = &status.examplRect;
	}
	else if (streq(bordername, "choose"))
	{
		border = &status.chooseRect;
	}
	else if (streq(bordername, "status"))
	{
		border = &status.statusRect;
	}
	else
	{
		Main_confirmMessage("������������ˣ�����input��search��explanation��example�� ");
		return;
	}
	//������������Ĵ�С
	width = right - left;
	width = MAX(GUI_BORDEROFFSET * 2 + GUI_CHARWIDTH * 20, width);
	width = MIN(1023, width);
	left = MAX(0, left);
	left = MIN(1023, left);
	left = left + width > 1023 ? 1023 - width : left;
	right = left + width;

	height = bottom - top;
	height = MAX(GUI_BORDEROFFSET * 2 + GUI_LINEHEIGHT, height);
	height = MIN(767, height);
	top = MAX(0, top);
	top = MIN(767, top);
	top = top + height > 767 ? 767 - height : top;
	bottom = top + height;
	bottom = top + height;

	//�޸Ĵ�С
	Rectangle_erase(*border);
	border->m_left = left;
	border->m_top = top;
	border->m_right = right;
	border->m_bottom = bottom;
	//ˢ��
	Main_printBlocks();
	Rectangle_print(*border, true);
}
void Main_search(const char* input)
{ //��������
	StateGraph sg;
	FSM fsm;
	Vector result;
	TRY
	{ //�쳣����
		if (status.matchPattern == REGEX)
		{ //Ԥ��������ƥ��
			input += MAIN_REGEXOFFSET;
			AST_regexPreprocess(input);
		}
		else if (status.matchPattern == WILDCARD)
		{ //Ԥ����ͨ���ƥ��
			input += MAIN_WILDCARDOFFSET;
			AST_wildcardPreprocess(input);
		}
		else
		{ //�����ܵ���
			Main_confirmMessage("������������������������ʧЧ���������� ");
			return;
		}
		AST_convertToPostfix(); //תΪ��׺���ʽ
		sg = AST_buildStateGraph(); //����״̬ͼ
		fsm = StateGraph_getFSM(sg); //����״̬��
		result = Dict_search(fsm); //����
		Main_chooseFrom(result, 0, status.searchRect); //���û������������ѡ��
		StateGraph_free(sg); //�ͷ�״̬ͼ
		free(fsm); //�ͷ�״̬��
		Vector_free(&result); //�ͷ��������
	}
		EXCEPT(Assert_Failed)
	{ //�﷨�������쳣
		Main_confirmMessage("�������﷨�����뾡�������﷨�������߲���֤���﷨����״̬�³��򲻷����ڴ�й©�� ");
		return;
	}
	END_TRY;
}
void Main_rawstring(const char* input)
{ //��ȷ�������Ѳ����ͷ��ص�һ��
	size_t index = Dict_getIndex(input);
	if (index == (size_t)-1)
	{
		Main_confirmMessage("�Ҳ�����Ӧ�� ");
	}
	else
	{
		Main_chooseFrom(Dict_getWordList(), index, status.searchRect);
	}
}
void Main_confirmMessage(const char* message)
{ //����˼�壬�����봰����ʾmessage��Ĭ����Ϣ
	Rectangle_highlight(status.inputRect);
	Rectangle_eraseContent(status.inputRect);
	if (message)
	{
		char* text = Str_copy(message);
		Gui_printText(text, status.inputRect);
		free(text);
	}
	else
	{
		char text[] = "����������顣 ";
		Gui_printText(text, status.inputRect);
	}
	Ctrl_readKey();
}
void Main_showAt(char* word, Rectangle border)
{
	Rectangle_eraseContent(border);
	Gui_printText(word, border);
}
int Main_kind(const char* input)
{ //��ȡ��������
	if (input[0] == '-' && input[1] == '-')
	{
		return COMMAND;
	}
	if (status.matchPattern == WILDCARD)
	{
		size_t i;
		for (i = 0; i < strlen(input); i++)
		{
			if (input[i] == '*' || input[i] == '?')
			{
				return WILDCARD;
			}
		}
	}
	if (status.matchPattern == REGEX)
	{
		if (input[0] == '=')
		{
			return REGEX;
		}
	}
	return RAWSTRING;
}
void Main_statusInitiailze(void)
{ //Ĭ�ϴ��ڴ�С
	Gui_setbgColor(GUI_BLACK);
	Gui_setfgColor(GUI_GRAY);
	status.inputRect.m_left = 0;
	status.inputRect.m_top = 0;
	status.inputRect.m_right = 1023;
	status.inputRect.m_bottom = 31;

	status.explanRect.m_left = 176;
	status.explanRect.m_top = 32;
	status.explanRect.m_right = 847;
	status.explanRect.m_bottom = 255;

	status.examplRect.m_left = 176;
	status.examplRect.m_top = 256;
	status.examplRect.m_right = 847;
	status.examplRect.m_bottom = 735;

	status.searchRect.m_left = 0;
	status.searchRect.m_top = 32;
	status.searchRect.m_right = 175;
	status.searchRect.m_bottom = 735;

	status.chooseRect.m_left = 848;
	status.chooseRect.m_top = 32;
	status.chooseRect.m_right = 1023;
	status.chooseRect.m_bottom = 735;

	status.statusRect.m_left = 0;
	status.statusRect.m_top = 736;
	status.statusRect.m_right = 1023;
	status.statusRect.m_bottom = 767;

	status.matchPattern = WILDCARD;
	status.exitFlag = 0;
	Main_printBlocks();
}
void Main_printBlocks(void)
{
	Rectangle_print(status.inputRect, true);
	Rectangle_print(status.explanRect, true);
	Rectangle_print(status.examplRect, true);
	Rectangle_print(status.searchRect, true);
	Rectangle_print(status.chooseRect, true);
	Rectangle_print(status.statusRect, true);
}
void Main_initialize(void)
{ //Ĭ�ϳ�ʼ��
	if (Dict_readdic() && Dict_openLib() && Gui_openhzLib() && Deep_init())
	{
		Gui_initialize();
		Gui_setbgColor(GUI_BLACK);
		Gui_setfgColor(GUI_GRAY);
		Deep_push("INPUT:");
	}
	else
	{
		printf("Data initialize failed, please check file 'dict.IDX', 'dict.lib' and 'HZK16'.\n");
		printf("Program will quit. Press any key.");
		getchar();
	}
}
void Main_uninitialize(void)
{
	Dict_freewordlist();
	Dict_closeLib();
	Gui_closehzLib();
	Gui_close();
	Deep_pop();
	Deep_uninit();
}
void Main_showWordInfo(IDX* pidx)
{
	Dict_getInfo(pidx);
	Rectangle_eraseContent(status.examplRect);
	Rectangle_eraseContent(status.explanRect);
	Gui_printText(Dict_getExplanation(), status.explanRect);
	Gui_printText(Dict_getExample(), status.examplRect);
}
bool Main_chooseFromExample(IDX* pidx, bool listWords)
{ //��ʾһ�����ʵĽ��ͺ�����
	Main_showWordInfo(pidx);
	if (listWords)
	{
		char** words;
		int i;
		Vector wordIDXs = Vector_new(0);
		bool needRefresh;
		words = Str_extractWords(Dict_getExample());
		for (i = 0; words[i] != NULL; i++)
		{
			IDX* pidx = Dict_getIDX(Dict_getIndex(words[i]));
			if (pidx)
			{
				Vector_push_back(wordIDXs, Vector_getPointer(Dict_getWordList(), Dict_getIndex(words[i])));
			}
			Atom_delete(words[i]);
		}
		free(words);
		needRefresh = Main_chooseFrom(wordIDXs, 0, status.chooseRect);
		Vector_free(&wordIDXs);
		return needRefresh;
	}
	return false;
}

bool Main_chooseFrom(Vector result, size_t index, Rectangle border)
{ //����������б���ѡ��
	//Rectangle_eraseContent(status.searchRect);
	size_t count;
	if (Vector_getLength(result) == 0)
	{
		Rectangle_eraseContent(border);
		Ctrl_readKey();
		return true;
	}
	Rectangle_highlight(border);
	count = Vector_getLength(result);
	//Main_showWordInfo(Vector_getPointer(result, index));
	Gui_setfgColor(GUI_GRAY);
	Ctrl_showWordlist(result, index, border, true);
	INFINITE_LOOP
	{ //����ж�
		switch (Ctrl_readKey())
		{
		case CTRL_ESC:
			return true;
		case CTRL_UP:
			index = index == 0 ? index : index - 1;
			Ctrl_showWordlist(result, index, border, false);
			break;
		case CTRL_PGUP:
			index = index >= CTRL_LINECOUNT ? index - CTRL_LINECOUNT : 0;
			Ctrl_showWordlist(result, index, border, false);
			break;
		case CTRL_DOWN:
			index = index + 1 < count ? index + 1 : count - 1;
			Ctrl_showWordlist(result, index, border, false);
			break;
		case CTRL_PGDOWN:
			index = index + CTRL_LINECOUNT < count ? index + CTRL_LINECOUNT : count - 1;
			Ctrl_showWordlist(result, index, border, false);
			break;
		case CTRL_ENTER:
		{
			IDX* word;
			bool needRefresh = false;
			word = Vector_getPointer(result, index);
			Main_showAt(word->m_entry, status.inputRect);
			Deep_push(word->m_entry);
			Deep_show(status.statusRect);
			needRefresh = Main_chooseFromExample(word, true);
			Main_showWordInfo(Vector_getPointer(result, index));
			Deep_pop();
			Deep_show(status.statusRect);
			if (needRefresh)
			{
				Ctrl_showWordlist(result, index, border, true);
				Main_showAt(word->m_entry, status.inputRect);
			}
			if (!bineq(border, status.chooseRect))
			{
				Rectangle_eraseContent(status.chooseRect);
			}
			Rectangle_highlight(border);
		}
		default:
			break;
			//do nothing
		};
	}
	return true;
}
