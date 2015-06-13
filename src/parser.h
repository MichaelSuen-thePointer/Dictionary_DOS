#ifndef PARSER_H
#define PARSER_H


#define PARSER_BUFFERLEN 1536

StateGraph	AST_buildStateGraph		(void);
void		AST_convertToPostfix	(void);
void		AST_convertToInfix		(const signed char* input);
void		AST_wildcardPreprocess	(const char* ustrinput);
void		AST_regexPreprocess		(const char* input);
signed char*AST_range				(signed char* pinfix, const char* input, size_t* index);

#endif
