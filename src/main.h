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
"�������ĵ���\n"
"\n"
"����֧�������������������\"--\"��ͷ\n"
"֧�ֵ�����\n"
"--exit                         �˳�\n"
"--showall [index]              ��ʾ������Ŀ����λ��[index]λ�ã�Ĭ��Ϊ0\n"
"--matchpattern [pattern]       ���ĵ�ǰ�Ĳ���ģʽ regex:���� wildcard:ͨ���������������ʾ��ǰģʽ\n"
"--set [bordername] [left] [top] [right] [bottom]\n"
"                               ����һ����ʾ����Ĵ�С��֧�ֵ�bordername\n"
"    search         �������\n"
"    input          ����\n"
"    example        �����\n"
"    explanation    ���Ϳ�\n"
"    choose         �ұ���\n"
"    status         �ײ�״̬��\n"
"--about                        �鿴�汾�ż�������Ϣ\n"
"\n"
"��ͨ����ֱ������Ҫ���ҵĵ��ʼ���\n"
"ͨ�������֧��*ƥ������ĸ��?ƥ��һ����ĸ\n"
"���������������ŵĵ��ʣ�����ÿ�����ǰ��ӷ�б��\\\n"
"\n"
"������ң����ڱ��ʽǰ����ȺŲ�����ƥ��ģʽΪregex\n"
"������ʽ����֧��|,*,?,+���ַ�����[a-bc-d]��^������ͼ���ƥ���ݲ�֧��\n"
"    ������̰��ƥ��̫�߶��ˣ�����Ū��˭�ܽ̽��ҡ�\n"
"<�ĵ�����> ";

char Main_aboutDoc[] =
">>>>>>>>>>>>>>>>>>>>[Dictionary]<<<<<<<<<<<<<<<<<<<<\n"
"\n"
"���ߣ�\n"
"    ������ 3140100674\n"
"רҵ��\n"
"    �������ѧ�뼼�� \n"
"�汾��\n"
"    Release v2.1\n"
"    ����v2.1��\n"
"       1�������˳�������ʱ�ķǷ��ڴ��д\n"
"    ����v2.0��\n"
"       1��������ȡ������еĵ��ʣ����ұ�����ʾ�����Լ���ѡ��\n"
"       2������ڸ�����ʾΪ��ɫ\n"
"       3����ײ�״̬������ʾ��ǰ�������\n"
"��ϵ��ʽ��\n"
"    crazy95sun@live.cn\n"
"<�ĵ�����> ";
#endif
