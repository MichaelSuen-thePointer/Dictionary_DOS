#ifndef STATUS_H
#define STATUS_H

#include "basic.h"
#include "vector.h"
#include "strext.h"
#include "gui.h"

#define STATUS_DELIMETER "> "
#define STATUS_DELIMLEN sizeof(STATUS_DELIMETER)
void Deep_show		(Rectangle border);
void Deep_pop		(void);
void Deep_push		(const char* str);
int  Deep_init		(void);
void Deep_uninit	(void);
#endif
