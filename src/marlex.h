
#ifndef MARLEXER_H_
#define MARLEXER_H_

#include <FlexLexer.h>

class MarLexer : public yyFlexLexer
{
	unsigned long long	type_state;

	public:

								MarLexer (istream * = 0);
	virtual					~MarLexer (void);
	int						Lex (void *, void *);
	int						Wrapper (void);
};

#endif
