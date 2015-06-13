#include "basic.h"
#include "regex.h"
#include "assert.h"
#include "except.h"
#include "parser.h"

enum tag_oprator {
	REPEAT = 1,
	REPEAT1 = 2,
	OPTIONAL = 3,
	CONNECT = 4,
	PARALLEL = 5,
	LBRACE = 6,
	RBRACE = 7
};

static const int piority[] = {0, 5, 5, 5, 4, 3, 2, 2}; //各运算符优先级
static signed char infix[PARSER_BUFFERLEN]; //中缀表达式
static signed char oprator[PARSER_BUFFERLEN]; //运算符栈
static signed char operand[PARSER_BUFFERLEN]; //运算数栈
static signed char postfix[PARSER_BUFFERLEN]; //后缀表达式
static StateGraph SGStack[PARSER_BUFFERLEN]; //状态图栈
#define if_not_clean_and_throw(condition, pstack, stack)				\
	{																	\
		if (!(condition))												\
		{																\
			while (pstack > stack)										\
			{															\
				StateGraph_free(*--pstack);								\
			}															\
			RAISE(Assert_Failed);										\
		}																\
	}
//抛异常前要先释放资源

void AST_wildcardPreprocess(const char* ustrinput)
{ //用于将通配符的*和+展开为正则语法
	const static char allletter[] = "(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|,|\\-|\\(|\\)|')";
	const signed char* input = (const signed char*)ustrinput;
	size_t len = strlen((const char*)input);
	size_t i;
	signed char infix[PARSER_BUFFERLEN] = {0};
	signed char* pinfix = infix;
	char lastone = '\0';
	for (i = 0; i < len; i++)
	{
		if (input[i] == '?')
		{ //可选字符
			memcpy(pinfix, allletter, sizeof(allletter));
			pinfix += sizeof(allletter) - 1;
		}
		else if (input[i] == '*')
		{ //0次及以上重复
			if (lastone == '*')
			{ //小小的优化一下，跳过重复的*
				continue;
			}
			memcpy(pinfix, allletter, sizeof(allletter));
			pinfix += sizeof(allletter) - 1;
			*pinfix++ = '*';
		}
		else
		{ //其他字符照抄
			*pinfix++ = input[i];
		}
		lastone = input[i];
	}
	*pinfix = '\0';
	AST_convertToInfix(infix);
}
void AST_regexPreprocess(const char* input)
{ //用于展开regex中的字符区间
	signed char infix[PARSER_BUFFERLEN] = {'\0'};
	signed char* pinfix = infix;
	size_t i;
	for (i = 0; i < strlen(input); i++)
	{
		if (input[i] == '[')
		{
			pinfix = AST_range(pinfix, input, &i);
		}
		else
		{
			*pinfix++ = input[i];
		}
	}
	*pinfix = '\0';
	AST_convertToInfix(infix);
}
signed char* AST_range(signed char* pinfix, const char* input, size_t* index)
{ //字符区间展开
	const static char allletter[] = "a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z";
	size_t i = *index;
	*pinfix++ = '(';
	i++;
	while (input[i] && input[i] != ']')
	{
		if (isalpha(input[i]))
		{ //字母
			if (input[i - 1] != '[')
			{ //检查是否需要加‘|’
				*pinfix++ = '|';
			}
			*pinfix++ = input[i];
		}
		else if (input[i] == '-')
		{ //字符区间的标记
			CHECK_RAISE(i > 0, Assert_Failed);//构不成区间
			CHECK_RAISE(isalpha(input[i - 1]), Assert_Failed);//构不成区间
			CHECK_RAISE(isalpha(input[i + 1]), Assert_Failed);
			CHECK_RAISE(input[i + 1] > input[i - 1], Assert_Failed);
			//以上检查是否合语法
			pinfix--;
			*pinfix++ = '(';
			strncpy((char*)pinfix, allletter + (input[i - 1] - 'a') * 2, (input[i + 1] - input[i - 1]) * 2 + 1);
			pinfix += (input[i + 1] - input[i - 1]) * 2 + 1;
			*pinfix++ = ')';
			i++;
		}
		else
		{ //非法字符
			RAISE(Assert_Failed);
		}
		i++;
	}
	CHECK_RAISE(input[i] == ']', Assert_Failed);//封口的']'
	*pinfix++ = ')';
	*index = i;
	return pinfix;
}
void AST_convertToInfix(const signed char* ustrinput)
{ //转为中缀，运算符用常量替代
	const signed char* input = (const signed char*)ustrinput;
	size_t len = strlen((const char*)input);
	size_t i;
	signed char* pinfix = infix;
	bool needConnect = false;
	for (i = 0; i < len; i++)
	{
		if (i > 0
			&& (isalpha(input[i]) || input[i] == '(' || input[i] == '\\')
			&& needConnect)
		{
			*pinfix++ = CONNECT;
		}
		needConnect = false;
		switch (input[i])
		{
		case '\\':	//转义字符
			*pinfix++ = input[++i];
			needConnect = true;
			break;
		case '+':	//>=1次重复，a+
			*pinfix++ = REPEAT1;
			needConnect = true;
			break;
		case '*':	//>=0次重复，a*
			*pinfix++ = REPEAT;
			needConnect = true;
			break;
		case '?':	//0或1次重复
			*pinfix++ = OPTIONAL;
			break;
		case '|':	//或，a|b
			*pinfix++ = PARALLEL;
			break;
		case '(':	//左括号
			*pinfix++ = LBRACE;
			break;
		case ')':	//右括号
			*pinfix++ = RBRACE;
			needConnect = true;
			break;
		default:
			*pinfix++ = input[i];
			needConnect = true;
			break;
		}
	}
	*pinfix = '\0';
}
void AST_convertToPostfix(void)
{ //转为后缀，不啰嗦了
	signed char* poperand = operand;
	signed char* poprator = oprator;
	signed char* ppostfix = postfix;
	size_t i;
	for (i = 0; infix[i]; i++)
	{
		if (infix[i] <= 7)
		{
			if (infix[i] == LBRACE)
			{
				*poprator++ = infix[i];
			}
			else if (infix[i] == RBRACE)
			{
				while (poprator > oprator && poprator[-1] != LBRACE)
				{
					*ppostfix++ = *--poprator;
				}
				CHECK_RAISE(*--poprator == LBRACE, Assert_Failed);
			}
			else if (infix[i] == REPEAT || infix[i] == REPEAT1 || infix[i] == OPTIONAL)
			{
				*ppostfix++ = infix[i];
			}
			else if (poprator > oprator)
			{
				if (piority[infix[i]] > piority[poprator[-1]])
				{
					*poprator++ = infix[i];
				}
				else
				{
					do
					{
						*ppostfix++ = *--poprator;
					} while (poprator > oprator && piority[poprator[-1]] > piority[infix[i]]);
					*poprator++ = infix[i];
				}
			}
			else
			{
				*poprator++ = infix[i];
			}
		}
		else
		{
			*ppostfix++ = infix[i];
		}
	}
	while (poprator > oprator)
	{
		*ppostfix++ = *--poprator;
	}
	*ppostfix = '\0';
}
StateGraph AST_buildStateGraph(void)
{
	size_t i;
	StateGraph* psgstack = SGStack;
	for (i = 0; postfix[i]; i++)
	{
		if (postfix[i] <= 7)
		{
			switch (postfix[i])
			{
			case REPEAT:
				if_not_clean_and_throw(psgstack > SGStack, psgstack, SGStack);
				psgstack--;
				*psgstack = StateGraph_repeat(*psgstack);
				psgstack++;
				break;
			case REPEAT1:
				if_not_clean_and_throw(psgstack > SGStack, psgstack, SGStack);
				psgstack--;
				*psgstack = StateGraph_repeat1(*psgstack);
				psgstack++;
				break;
			case OPTIONAL:
				if_not_clean_and_throw(psgstack > SGStack, psgstack, SGStack);
				psgstack--;
				*psgstack = StateGraph_optional(*psgstack);
				psgstack++;
				break;
			case CONNECT:
				if_not_clean_and_throw(psgstack > SGStack + 1, psgstack, SGStack);
				psgstack -= 2;
				*psgstack = StateGraph_connect(psgstack[0], psgstack[1]);
				psgstack++;
				break;
			case PARALLEL:
				if_not_clean_and_throw(psgstack > SGStack + 1, psgstack, SGStack);
				psgstack -= 2;
				*psgstack = StateGraph_parallel(psgstack[0], psgstack[1]);
				psgstack++;
				break;
			}
		}
		else
		{
			*psgstack++ = StateGraph_new(postfix[i]);
		}
	}
	if_not_clean_and_throw(psgstack == SGStack + 1, psgstack, SGStack);
	//如果开启REGEX_DEBUG开关，那么会在文本模式下输出状态机转换的过程
	//否则debugOutput就什么也不做
	StateGraph_debugOutput(SGStack[0]);
	SGStack[0] = StateGraph_mark(SGStack[0]);
	StateGraph_debugOutput(SGStack[0]);
	SGStack[0] = StateGraph_removeEpsilon(SGStack[0]);
	StateGraph_debugOutput(SGStack[0]);
	SGStack[0] = StateGraph_NFAtoDFA(SGStack[0]);
	StateGraph_debugOutput(SGStack[0]);
	return SGStack[0];
}
