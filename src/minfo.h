
#ifndef MINFO_H_
#define MINFO_H_

#include <Message.h>

struct MarStack
{
	BMessage							msg;
	MarStack							*next;
};


struct MINFO
{
	unsigned long					what;
	char								*name;
};

struct SINFO
{
	char								*name;
	char								*type;
};

struct RINFO
{
	char								*buffer;
	unsigned long long			size;
};

struct RECTVALUE
{
	float								left;
	float								top;
	float								right;
	float								bottom;
};

struct COLORVALUE
{
	char								red;
	char								green;
	char								blue;
	char								alpha;
};

#endif
