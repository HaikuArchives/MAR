
#include <stdio.h>

#include "marlex.h"

MarLexer::MarLexer (istream *stream)
	: yyFlexLexer (stream)
{
}

MarLexer::~MarLexer (void)
{
}

int
MarLexer::Wrapper (void)
{
	return 1;
}


