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
	//初始化
	Main_initialize();
	Main_statusInitiailze();
	Main_confirmMessage("初次使用，请输入--help查看帮助 ");
	do
	{ //主循环
		Rectangle_highlight(status.inputRect);
		Rectangle_eraseContent(status.examplRect);
		Rectangle_eraseContent(status.explanRect);
		input = Ctrl_readline(status.inputRect);
		Str_toLowerCase(input);
		switch (Main_kind(input))
		{ //获取输入的类型
		case COMMAND:
			//命令
			Main_command(input);
			break;
		case REGEX:case WILDCARD:
			//模糊查找
			Main_search(input);
			break;
		case RAWSTRING:
			//精确查找
			Main_rawstring(input);
			break;
		default:
			//不可能到这，除非改跳转
			Main_confirmMessage("程序到达了一个不可能到达的地方，这有可能是因为内部的判断跳转被修改了，反正绝对不是作者的错。 ");
			break;
		}
	} while (!status.exitFlag);
	Main_uninitialize();
	//退出
	return 0;
}

void Main_command(char* input)
{ //处理命令
	Vector argv = Str_split(input + MAIN_CMDOFFSET, ' '); //拆分命令
	char* first = Vector_getPointer(argv, 0); //获取第一个命令
	if (streq(first, "exit"))
	{ //顾名思义
		status.exitFlag = 1;
	}
	else if (streq(first, "matchpattern"))
	{ //修改匹配模式
		if (Vector_getLength(argv) >= 2)
		{ //修改模式
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
				Main_confirmMessage("我只支持regex或wildcard搜索方式，你还想怎么搜索。 ");
			}
		}
		else
		{ //显示当前模式
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
	{ //显示整个字典
		size_t index = 0;
		//Main_printBlocks(); //清空所有窗口
		if (Vector_getLength(argv) >= 2)
		{
			sscanf(Vector_getPointer(argv, 1), "%u", &index);
			index = index >= Dict_getWordCount() ? Dict_getWordCount() - 1 : index;
		}
		Main_chooseFrom(Dict_getWordList(), index, status.searchRect);
	}
	else if (streq(first, "set"))
	{ //设置窗口大小
		if (Vector_getLength(argv) >= 6)
		{ //六个参数，设置大小
			Main_changeBorder(argv);
		}
		else if (Vector_getLength(argv) >= 2)
		{ //两个参数，恢复默认
			first = Vector_getPointer(argv, 1);
			if (streq(first, "default"))
			{
				Main_statusInitiailze();
			}
			else
			{ //错误
				Main_confirmMessage("参数不对啊，你是不是要输入default ");
			}
		}
		else
		{ //错误
			Main_confirmMessage("你输的什么参数？只有一个set我可不知道你要修改哪个窗口的大小哦。 ");
		}
	}
	else if (streq(first, "help"))
	{ //顾名思义
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
	{ //错误
		Main_confirmMessage("然而并没有这个参数。请尝试--help ");
	}
	Str_freeFragments(argv);
}
void Main_changeBorder(Vector argv)
{ //修改边框大小
	char* bordername = Vector_getPointer(argv, 1);
	char str[50];
	Rectangle* border;
	mint left, top, right, bottom, width, height;

	sprintf(str, "%s %s %s %s", Vector_getPointer(argv, 2), Vector_getPointer(argv, 3), Vector_getPointer(argv, 4), Vector_getPointer(argv, 5));
	sscanf(str, "%d %d %d %d", &left, &top, &right, &bottom);
	//以上相当于atoi函数，麻烦了点
	//一下判断窗口名
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
		Main_confirmMessage("窗口名称输错了，尝试input，search，explanation或example。 ");
		return;
	}
	//以下修正输入的大小
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

	//修改大小
	Rectangle_erase(*border);
	border->m_left = left;
	border->m_top = top;
	border->m_right = right;
	border->m_bottom = bottom;
	//刷新
	Main_printBlocks();
	Rectangle_print(*border, true);
}
void Main_search(const char* input)
{ //进入搜索
	StateGraph sg;
	FSM fsm;
	Vector result;
	TRY
	{ //异常处理
		if (status.matchPattern == REGEX)
		{ //预处理正则匹配
			input += MAIN_REGEXOFFSET;
			AST_regexPreprocess(input);
		}
		else if (status.matchPattern == WILDCARD)
		{ //预处理通配符匹配
			input += MAIN_WILDCARDOFFSET;
			AST_wildcardPreprocess(input);
		}
		else
		{ //不可能到这
			Main_confirmMessage("程序发生参数错误，搜索功能已失效，请重启。 ");
			return;
		}
		AST_convertToPostfix(); //转为后缀表达式
		sg = AST_buildStateGraph(); //构建状态图
		fsm = StateGraph_getFSM(sg); //构建状态机
		result = Dict_search(fsm); //搜索
		Main_chooseFrom(result, 0, status.searchRect); //让用户在搜索结果中选择
		StateGraph_free(sg); //释放状态图
		free(fsm); //释放状态机
		Vector_free(&result); //释放搜索结果
	}
		EXCEPT(Assert_Failed)
	{ //语法错误抛异常
		Main_confirmMessage("出现了语法错误，请尽量避免语法错误，作者不保证在语法错误状态下程序不发生内存泄漏。 ");
		return;
	}
	END_TRY;
}
void Main_rawstring(const char* input)
{ //精确搜索，搜不到就返回第一个
	size_t index = Dict_getIndex(input);
	if (index == (size_t)-1)
	{
		Main_confirmMessage("找不到对应项 ");
	}
	else
	{
		Main_chooseFrom(Dict_getWordList(), index, status.searchRect);
	}
}
void Main_confirmMessage(const char* message)
{ //顾名思义，在输入窗口显示message或默认消息
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
		char text[] = "输入错误，请检查。 ";
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
{ //获取输入类型
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
{ //默认窗口大小
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
{ //默认初始化
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
{ //显示一个单词的解释和例句
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
{ //在搜索结果列表中选择
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
	{ //光标判断
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
