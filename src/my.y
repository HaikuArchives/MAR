
%{

#define YYPARSE_PARAM control
#define YYLEX_PARAM control
#define FROMMARLIB

#include <Message.h>
#include <String.h>
#include <List.h>
#include <Messenger.h>
#include <Entry.h>
#include <GraphicsDefs.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fstream>

#include "marcontrol.h"
#include "minfo.h"
#include "marlex.h"

uint32 figure_type (const char *);

#undef yylex
#undef yytext
#undef yyleng
#undef yyerror
#define yylex ((MarControl *)YYPARSE_PARAM)->Lexer()->Lex
#define yytext ((MarControl *)YYPARSE_PARAM)->Lexer()->YYText()
#define yyleng ((MarControl *)YYPARSE_PARAM)->Lexer()->YYLeng()
#define yyerror ((MarControl *)YYPARSE_PARAM)->ParseError

%}

%union {
	char							*string;
	long long					integer;
	double						decimal;
	struct MINFO				minfo;
	struct SINFO				sinfo;
	struct RINFO				rinfo;
	struct RECTVALUE			rect;
	struct COLORVALUE 		color;
}

%token BOOL_TAG DOUBLE_TAG FLOAT_TAG INT64_TAG INT32_TAG INT16_TAG INT8_TAG
%token MESSAGE_TAG MESSENGER_TAG MIME_TAG OFF_TAG POINTER_TAG POINT_TAG
%token RAW_TAG RECT_TAG REF_TAG COLOR_TAG SIZE_TAG SSIZE_TAG STRING_TAG
%token TIME_TAG UINT64_TAG UINT32_TAG UINT16_TAG UINT8_TAG 

%token BOOL_END DOUBLE_END FLOAT_END INT64_END INT32_END INT16_END INT8_END
%token MESSAGE_END MESSENGER_END MIME_END OFF_END POINTER_END POINT_END
%token RAW_END RECT_END REF_END COLOR_END SIZE_END SSIZE_END STRING_END
%token TIME_END UINT64_END UINT32_END UINT16_END UINT8_END 

%token NAME_ATTR WHAT_ATTR SRC_ATTR TYPE_ATTR
%token TRUE_DATA FALSE_DATA

%token <string> VALUE_ATTR
%token <decimal> DECIMAL
%token <integer> INTEGER
%token <string> WORD
%token <string> LINE

%type <string> simple_attr
%type <integer> message_attr
%type <string> string_attr
%type <minfo> message_opener
%type <minfo> combined_message_statements
%type <integer> bool_value
%type <rect> rect_values
%type <color> color_values
%type <string> string_line
%type <string> string_body
%type <sinfo> string_opener
%type <sinfo> raw_opener
%type <rinfo> raw_line
%type <rinfo> raw_body

%pure_parser

%{
int marlex (YYSTYPE *, void *);
%}

%%

start: statements																		;

statements: statement
	| statements statement
	;

statement: bool_statement
	| double_statement
	| float_statement
	| int64_statement
	| int32_statement
	| int16_statement
	| int8_statement
	| message_statement
	| messenger_statement
	| mime_statement
	| off_statement
	| pointer_statement
	| point_statement
	| rect_statement
	| ref_statement
	| color_statement
	| size_statement
	| ssize_statement
	| time_statement
	| uint64_statement
	| uint32_statement
	| uint16_statement
	| uint8_statement
	| string_statement
	| raw_statement
	;
	

bool_statement: BOOL_TAG simple_attr '>' bool_value BOOL_END				{
																									MarStack *ms (((MarControl *)control)->CurrentMessage());
																									ms->msg.AddBool ($2, $4 != 0LL);
																								}
	;

bool_value: TRUE_DATA																	{ $$ = 1LL; }
	| FALSE_DATA																			{ $$ = 0LL; }
	;

double_statement: DOUBLE_TAG simple_attr '>' DECIMAL DOUBLE_END  			{
																									MarStack *ms (((MarControl *)control)->CurrentMessage());
																									ms->msg.AddDouble ($2, $4);
																									free ($2);
																								}
	;

float_statement: FLOAT_TAG simple_attr '>' DECIMAL FLOAT_END		 		{
																									MarStack *ms (((MarControl *)control)->CurrentMessage());
																									ms->msg.AddFloat ($2, $4);
																									free ($2);
																								}
	;

int64_statement: INT64_TAG simple_attr '>' INTEGER INT64_END				{
																									MarStack *ms (((MarControl *)control)->CurrentMessage());
																									ms->msg.AddInt64 ($2, $4);
																									free ($2);
																								}
	;

int32_statement: INT32_TAG simple_attr '>' INTEGER INT32_END				{
																									MarStack *ms (((MarControl *)control)->CurrentMessage());
																									ms->msg.AddInt32 ($2, $4);
																									free ($2);
																								}
	;

int16_statement: INT16_TAG simple_attr '>' INTEGER INT16_END				{
																									MarStack *ms (((MarControl *)control)->CurrentMessage());
																									ms->msg.AddInt16 ($2, $4);
																									free ($2)
																								}
	;

int8_statement: INT8_TAG simple_attr '>' INTEGER INT8_END					{
																									MarStack *ms (((MarControl *)control)->CurrentMessage());
																									ms->msg.AddInt8 ($2, $4);
																									free ($2);
																								}
	;

message_statement: combined_message_statements									{
																									/* Pop the top off and add it to new top */
																									MarControl *c ((MarControl *)control);
																									
																									c->PopMessage ($1.name, $1.what);
																									free ($1.name);
																								}
	;

combined_message_statements: message_opener MESSAGE_END						{ $$ = $1; }
	| message_opener statements MESSAGE_END										{ $$ = $1; }
	;

message_opener: MESSAGE_TAG simple_attr message_attr '>'						{
																									MarControl *c ((MarControl *)control);
																									MarStack *ms (new MarStack);

																									c->PushMessage (ms);
																									$$.what = $3;
																									$$.name = $2;
																								}
	;

messenger_statement: MESSENGER_TAG simple_attr '>' INTEGER ',' INTEGER MESSENGER_END	{
																									MarStack *ms (((MarControl *)control)->CurrentMessage());
																									BHandler *handler ((BHandler *)$4);
																									BLooper  *looper ((BLooper *)$6);

																									BMessenger msgr (handler, looper);
																									ms->msg.AddMessenger ($2, msgr);
																									free ($2);
																								}

	;

mime_statement: MIME_TAG simple_attr '>' WORD MIME_END						{
																									MarStack *ms (((MarControl *)control)->CurrentMessage());
																									ms->msg.AddData (
																										$2,
																										B_MIME_TYPE,
																										$4,
																										strlen ($4) + 1);
																									free ($2);
																									free ($4);
																								}
	;

off_statement: OFF_TAG simple_attr '>' INTEGER OFF_END						{
																									MarStack *ms (((MarControl *)control)->CurrentMessage());
																									ms->msg.AddData (
																										$2,
																										B_OFF_T_TYPE,
																										&$4,
																										sizeof ($4));
																									free ($2);
																								}
	;

pointer_statement: POINTER_TAG simple_attr '>' INTEGER POINTER_END		{
																									MarStack *ms (((MarControl *)control)->CurrentMessage());
																									void *pointer ((void *)$4);
																									ms->msg.AddPointer ($2, pointer);
																									free ($2);
																								}
	;

point_statement: POINT_TAG simple_attr '>' DECIMAL ',' DECIMAL POINT_END	{
																										MarStack *ms (((MarControl *)control)->CurrentMessage());
																										BPoint point ($4, $6);

																										ms->msg.AddPoint ($2, point);
																										free ($2);
																									}
	;

rect_statement: RECT_TAG simple_attr '>' rect_values RECT_END					{
																										MarStack *ms (((MarControl *)control)->CurrentMessage());
																										BRect rect ($4.left, $4.top, $4.right, $4.bottom);

																										ms->msg.AddRect ($2, rect);
																										free ($2);
																									}
	;

rect_values: DECIMAL ',' DECIMAL ',' DECIMAL ',' DECIMAL							{
																										RECTVALUE rv;

																										rv.left   = $1;
																										rv.top    = $3;
																										rv.right  = $5;
																										rv.bottom = $7;

																										$$ = rv;
																									}
	;

ref_statement: REF_TAG simple_attr '>' INTEGER ',' INTEGER ',' WORD REF_END {
																										MarStack *ms (((MarControl *)control)->CurrentMessage());
																										entry_ref ref ($4, $6, $8);

																										ms->msg.AddRef ($2, &ref);
																										free ($2);
																										free ($8);
																									}
	;

color_statement: COLOR_TAG simple_attr '>' color_values COLOR_END				{
																										MarStack *ms (((MarControl *)control)->CurrentMessage());
																										rgb_color color;

																										color.red   = $4.red;
																										color.green = $4.green;
																										color.blue  = $4.blue;
																										color.alpha = $4.alpha;

																										ms->msg.AddData (
																											$2,
																											B_RGB_COLOR_TYPE,
																											&color,
																											sizeof (rgb_color));
																										free ($2);
																									}
	;

color_values: INTEGER ',' INTEGER ',' INTEGER ',' INTEGER						{
																										COLORVALUE cv;

																										cv.red = $1;
																										cv.green = $3;
																										cv.blue  = $5;
																										cv.alpha = $7;

																										$$ = cv;
																									}
	;

size_statement: SIZE_TAG simple_attr '>' INTEGER SIZE_END						{
																										MarStack *ms (((MarControl *)control)->CurrentMessage());
																										size_t size ($4);

																										ms->msg.AddData (
																											$2,
																											B_SIZE_T_TYPE,
																											&size,
																											sizeof (size_t));
																										free ($2);
																									}
	;

ssize_statement: SSIZE_TAG simple_attr '>' INTEGER SSIZE_END					{
																										MarStack *ms (((MarControl *)control)->CurrentMessage());
																										ssize_t size ($4);

																										ms->msg.AddData (
																											$2,
																											B_SSIZE_T_TYPE,
																											&size,
																											sizeof (ssize_t));
																										free ($2);
																									}
	;

time_statement: TIME_TAG simple_attr '>' INTEGER TIME_END						{
																										MarStack *ms (((MarControl *)control)->CurrentMessage());
																										bigtime_t t ($4);

																										ms->msg.AddData (
																											$2,
																											B_TIME_TYPE,
																											&t,
																											sizeof (bigtime_t));
																										free ($2);
																									}
	;

uint64_statement: UINT64_TAG simple_attr '>' INTEGER UINT64_END				{
																										MarStack *ms (((MarControl *)control)->CurrentMessage());
																										uint64 number ($4);

																										ms->msg.AddData (
																											$2,
																											B_UINT64_TYPE,
																											&number,
																											sizeof (uint64));
																										free ($2);
																									}
	;

uint32_statement: UINT32_TAG simple_attr '>' INTEGER UINT32_END				{
																										MarStack *ms (((MarControl *)control)->CurrentMessage());
																										uint32 number ($4);

																										ms->msg.AddData (
																											$2,
																											B_UINT32_TYPE,
																											&number,
																											sizeof (uint32));
																										free ($2);
																									}
	;

uint16_statement: UINT16_TAG simple_attr '>' INTEGER UINT16_END				{
																										MarStack *ms (((MarControl *)control)->CurrentMessage());
																										uint16 number ($4);

																										ms->msg.AddData (
																											$2,
																											B_UINT16_TYPE,
																											&number,
																											sizeof (uint16));
																										free ($2);
																									}
	;

uint8_statement: UINT8_TAG simple_attr '>' INTEGER UINT8_END					{
																										MarStack *ms (((MarControl *)control)->CurrentMessage());
																										uint8 number ($4);

																										ms->msg.AddData (
																											$2,
																											B_UINT8_TYPE,
																											&number,
																											sizeof (uint8));
																										free ($2);
																									}
	;

string_statement: string_opener STRING_END											{
																										MarStack *ms (((MarControl *)control)->CurrentMessage());

																										if ($1.type) 
																										{
																											uint32 type (figure_type ($1.type));

																											ms->msg.AddData (
																												$1.name,
																												type,
																												"",
																												1);
																											free ($1.type);
																										}
																										else ms->msg.AddString ($1.name, "");

																										free ($1.name);
																									}
	| string_opener string_body STRING_END												{
																										MarStack *ms (((MarControl *)control)->CurrentMessage());

																										if ($1.type)
																										{
																											uint32 type (figure_type ($1.type));

																											ms->msg.AddData (
																												$1.name,
																												type,
																												$2,
																												strlen ($2) + 1);
																											free ($1.type);
																										}
																										else
																											ms->msg.AddString ($1.name, $2);


																										delete [] $2;
																										free ($1.name);
																									}
	;

string_opener: STRING_TAG simple_attr string_attr '>'								{
																										SINFO si;

																										si.name = $2;
																										si.type = $3;
																										$$ = si;
																									}
	;

string_body: string_line																	{ $$ = $1; }
	| string_body string_line																{
																										uint32 len[2];

																										len[0] = strlen ($1);
																										len[1] = strlen ($2);

																										char *buffer;

																										buffer = new char [len[0] + len[1] + 1];
																										strcpy (buffer, $1);
																										strcat (buffer, $2);

																										delete [] $1;
																										delete [] $2;

																										$$ = buffer;
																									}
	;

string_line: LINE '\n'																		{
																										BString str ($1);

																										free ($1);
																										str.ReplaceAll ("&lt;", "<");
																										str.ReplaceAll ("&gt;", ">");
																										str.ReplaceAll ("&quot;", "\"");
																										str.ReplaceAll ("&amp;", "&");

																										char *buffer;

																										buffer = strcpy (new char [str.Length() + 2], str.String());
																										strcat (buffer, "\n");
																										$$ = buffer;
																									}
	| LINE																						{
																										BString str ($1);

																										free ($1);
																										str.ReplaceAll ("&lt;", "<");
																										str.ReplaceAll ("&gt;", ">");
																										str.ReplaceAll ("&quot;", "\"");
																										str.ReplaceAll ("&amp;", "&");

																										char *buffer;

																										buffer = strcpy (new char [str.Length() + 1], str.String());
																										$$ = buffer;
																									}
	;

raw_statement: raw_opener RAW_END														{
																										// eat it
																										if ($1.type) free ($1.type);
																										free ($1.name);
																									}
	| raw_opener raw_body RAW_END															{
																										MarStack *ms (((MarControl *)control)->CurrentMessage());
																										uint32 type (B_RAW_TYPE);

																										if ($1.type)
																										{
																											type = figure_type ($1.type);
																											free ($1.type);
																										}

																										ms->msg.AddData (
																											$1.name,
																											type,
																											$2.buffer,
																											$2.size);

																										delete [] $2.buffer;
																									}
	;

raw_opener: RAW_TAG simple_attr string_attr '>'										{
																										SINFO si;

																										si.name = $2;
																										si.type = $3;
																										$$ = si;
																									}
	;

raw_body: raw_line																			{ $$ = $1; }
	| raw_body raw_line																		{
																										char *buffer;

																										buffer = new char [$1.size + $2.size];

																										memcpy (buffer, $1.buffer, $1.size);
																										memcpy (buffer + $1.size, $2.buffer, $2.size);
																										delete [] $1.buffer;
																										delete [] $2.buffer;
																										
																										$$.buffer = buffer;
																										$$.size   = $1.size + $2.size;
																									}
	;

raw_line: LINE																					{
																										int len (strlen ($1)), place (0);
																										RINFO info;

																										info.buffer = new char [info.size = len / 2];
																										memset (info.buffer, 0, info.size);

																										for (int i = 0; i + 1 < len; i += 2)
																										{
																											if (isdigit ($1[i]))
																												info.buffer[place] = 16 * ($1[i] - '0');
																											else
																												info.buffer[place] = 16 * (toupper($1[i]) - 'A' + 10);

																											if (isdigit ($1[i + 1]))
																												info.buffer[place] += $1[i + 1] - '0';
																											else
																												info.buffer[place] += toupper ($1[i + 1] - 'A' + 10);

																											++place;
																										}

																										$$ = info;
																									}
	;


simple_attr: NAME_ATTR VALUE_ATTR														{ $$ = $2; }
	;

message_attr: 																					{ $$ = 0LL; }
	| WHAT_ATTR VALUE_ATTR 																	{
																										$$ = atol ($2);
																										free ($2);
																									}
	;

string_attr:																					{ $$ = 0; }
	| TYPE_ATTR VALUE_ATTR																	{ $$ = $2; }
	;
	


%%

uint32
figure_type (const char *code)
{
	uint32 type (0);

	if (isdigit (*code))
	{
		const char *place (code);

		while (*place && isdigit (*place))
			type = type * 10 + *place++ - '0';
	}
	else
	{
		int len (strlen (code));

		#ifdef __INTEL__
		if (len >= 1)
			type += (0UL + code[0]) << 24;

		if (len >= 2)
			type += (0UL + code[1]) << 16;
																												
		if (len >= 3)
			type += (0UL + code[2]) << 8;

		if (len >= 4)
			type += 0UL + code[3];
		#else

		if (len >= 4)
			type += (0UL + code[3]) << 24;
																												
		if (len >= 3)
			type += (0UL + code[2]) << 16;

		if (len >= 2)
			type += (0UL + code[1]) << 8;

		if (len >= 1)
			type += 0UL + code[0];
		#endif
	}

	return type;
}

MarControl::MarControl (void)
	: lexer (0),
	  stack (0),
	  files (0),
	  error (MAR_OK)
{
	lexer = new MarLexer;
}

MarControl::MarControl (int32 count, const char **flist)
	: lexer (0),
	  stack (0),
	  files (0),
	  error (MAR_OK)
{
	files = new BList;
	for (int32 i = 0; i < count; ++i)
		files->AddItem (new BString (flist[i]));

}

MarControl::~MarControl (void)
{
	if (files)
	{
		for (int32 i = 0; i < files->CountItems(); ++i)
		{
			BString *f ((BString *)files->ItemAt (i));

			delete f;
		}

		delete files;
	}

	while (stack)
	{
		MarStack *next (stack->next);

		delete stack;
		stack = next;
	}

	if (lexer) delete lexer;
}

MarStack *
MarControl::CurrentMessage (void)
{
	if (!stack)
	{
		stack = new MarStack;
		stack->next = 0;
	}

	return stack;
}

void
MarControl::PushMessage (MarStack *ms)
{
	ms->next = stack;
	stack = ms;
}

void
MarControl::PopMessage (const char *name, uint32 what)
{
	// do this way - we may want one if none are there
	MarStack *ms (CurrentMessage());

	if (ms && ms->next)
	{
		stack = stack->next;
		ms->msg.what = what;
		stack->msg.AddMessage (name, &ms->msg);
		delete ms;
	}
}

MarLexer *
MarControl::Lexer (void) const
{
	return lexer;
}

BMessage *
MarControl::Message (void) const
{
	return stack ? &stack->msg : 0;
}

MarError
MarControl::Parse (void)
{
	// in case they call it more than once
	while (stack)
	{
		MarStack *next (stack->next);

		delete stack;
		stack = next;
	}

	if (files)
	{
		MarStack *s (new MarStack);

		s->next = 0;

		for (int32 i = 0; i < files->CountItems(); ++i)
		{
			BString *f((BString *)files->ItemAt (i));

			ifstream stream (f->String());

			if (!stream)
			{
				errorstr = "Could not open file ";
				errorstr << f->String() << ".";

				error = MAR_FILE_OPEN_FAIL;
				printf("Line:%d\n",__LINE__);
				ParseError ("file error");
				break;
			}

			if (lexer)
				lexer->switch_streams (&stream, &cout);
			else
				lexer = new MarLexer (&stream);

			if (marparse (this))
			{
				if (error == MAR_OK)
				{
					printf("Line:%d\n",__LINE__);
					ParseError ("parse error");
					error = MAR_ERROR;
				}
				break;
			}

			if (stack)
			{
				s->msg.AddMessage (f->String(), &stack->msg);

				while (stack)
				{
					MarStack *next (stack->next);

					delete stack;
					stack = next;
				}
			}
		}

		stack = s;
	}
	else
	{
		lexer = new MarLexer;

		if (marparse (this))
		{
			if (error == MAR_OK)
			{
				printf("Line:%d\n",__LINE__);
				ParseError ("parse error");
				error = MAR_ERROR;
			}
		}
	}

	if (lexer)
	{
		delete lexer;
		lexer = 0;
	}

	return error;
}

int
MarControl::ParseError (const char *text)
{
	if (error != MAR_OK)
		fprintf (stderr, "[%d]%s: %s\n", LineNumber(), text, errorstr.String());
	else
		fprintf (stderr, "[%d]%s: general error\n", LineNumber(), text);
	return 0;
}

int
MarControl::LineNumber (void)
{
	if (lexer)
		return lexer->lineno();
	else
		return 0;
}

