#ifndef MAIN_H
#define MAIN_H

#define MAIN_CMDOFFSET 2
#define MAIN_WILDCARDOFFSET 0
#define MAIN_REGEXOFFSET 1
void Main_search			(const char* input);
void Main_command			(char* input);
void Main_rawstring			(const char* input);
void Main_confirmMessage	(const char* message);
int	 Main_kind				(const char* input);
void Main_statusInitiailze	(void);
bool Main_chooseFromExample		(IDX* pidx, bool listWords);
bool Main_chooseFrom		(Vector result, size_t index, Rectangle border);
void Main_changeBorder		(Vector argv);
void Main_showAt			(char* word, Rectangle border);

void Main_initialize		(void);
void Main_uninitialize		(void);
void Main_printBlocks		(void);
void Main_showWordInfo		(IDX* pidx);
char Main_helpDoc[] =
"【帮助文档】\n"
"\n"
"程序支持输出控制命令，命令均以\"--\"开头\n"
"支持的命令\n"
"--exit                         退出\n"
"--showall [index]              显示所有条目并定位于[index]位置，默认为0\n"
"--matchpattern [pattern]       更改当前的查找模式 regex:正则 wildcard:通配符，不输入则显示当前模式\n"
"--set [bordername] [left] [top] [right] [bottom]\n"
"                               更改一个显示区域的大小，支持的bordername\n"
"    search         搜索结果\n"
"    input          输入\n"
"    example        例句框\n"
"    explanation    解释框\n"
"    choose         右边栏\n"
"    status         底部状态栏\n"
"--about                        查看版本号及作者信息\n"
"\n"
"普通查找直接输入要查找的单词即可\n"
"通配符查找支持*匹配多个字母，?匹配一个字母\n"
"如需搜索含标点符号的单词，请在每个标点前添加反斜杠\\\n"
"\n"
"正则查找，请在表达式前输入等号并设置匹配模式为regex\n"
"正则表达式现在支持|,*,?,+和字符区间[a-bc-d]，^运算符和计数匹配暂不支持\n"
"    正向反向贪婪匹配太高端了，不会弄，谁能教教我。\n"
"<文档结束> ";

char Main_aboutDoc[] =
">>>>>>>>>>>>>>>>>>>>[Dictionary]<<<<<<<<<<<<<<<<<<<<\n"
"\n"
"作者：\n"
"    孙明琦 3140100674\n"
"专业：\n"
"    计算机科学与技术 \n"
"版本：\n"
"    Release v2.1\n"
"    更新v2.1：\n"
"       1：修正了程序启动时的非法内存读写\n"
"    更新v2.0：\n"
"       1：程序会抽取例句框中的单词，在右边栏显示，可以继续选择\n"
"       2：活动窗口高亮显示为绿色\n"
"       3：最底层状态栏会显示当前搜索深度\n"
"联系方式：\n"
"    crazy95sun@live.cn\n"
"<文档结束> ";
#endif
