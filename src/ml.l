%{

#undef YY_DECL
#define YY_DECL							int MarLexer::Lex (void *arg, void *marc)
#define YY_SKIP_YYWRAP
#define yywrap								(dynamic_cast<MarLexer *>(this))->Wrapper

/* Parses semi-xml for BMessages */

#include <stdio.h>
#include <string.h>

#include "marlex.h"
#include "minfo.h"
#include "my.tab.h"
#include "marcontrol.h"

%}

%s ATTRIBUTE
%s VALUE
%s BOOL
%s SIMPLE
%s STRBODY
%s RAWBODY
%s COMMENT

quoted	\"[^\"\n]*[\"\n]
integer	[0-9]+
decimal	[0-9]+\.[0-9]*
word		[a-zA-Z][a-zA-Z0-9]*

%option c++
%option yyclass="MarLexer"

%%

%{

MarControl *control ((MarControl *)marc);
YYSTYPE *yylval ((YYSTYPE *)arg);

%}

<ATTRIBUTE>[\t ]									;
<ATTRIBUTE>name									{ BEGIN VALUE; return NAME_ATTR; }
<ATTRIBUTE>what									{ BEGIN VALUE; return WHAT_ATTR; }
<ATTRIBUTE>src										{ BEGIN VALUE; return SRC_ATTR; }
<ATTRIBUTE>type									{ BEGIN VALUE; return TYPE_ATTR; }
<ATTRIBUTE>\>										{
															switch (type_state)
															{
																case BOOL_TAG:
																	BEGIN BOOL;
																	break;

																case DOUBLE_TAG:
																case FLOAT_TAG:
																case INT64_TAG:
																case INT32_TAG:
																case INT16_TAG:
																case INT8_TAG:
																case MESSENGER_TAG:
																case MIME_TAG:
																case OFF_TAG:
																case POINTER_TAG:
																case POINT_TAG:
																case RECT_TAG:
																case REF_TAG:
																case COLOR_TAG:
																case SIZE_TAG:
																case SSIZE_TAG:
																case TIME_TAG:
																case UINT64_TAG:
																case UINT32_TAG:
																case UINT16_TAG:
																case UINT8_TAG:
																	BEGIN SIMPLE;
																	break;

																case RAW_TAG:
																	BEGIN RAWBODY;
																	break;

																case STRING_TAG:
																	BEGIN STRBODY;
																	break;

																case MESSAGE_TAG:
																	BEGIN INITIAL;
																	break;
															}
															return yytext[0];
														}
<ATTRIBUTE>.										{ return yytext[0]; }

<VALUE>\={quoted}									{
															int len;

															BEGIN ATTRIBUTE;

															len = strlen (yylval->string = strdup (yytext + 2));
															if (yylval->string[len - 1] != '\"')
																fprintf (stderr, "Unterminated attribute value %s\n", yytext);

															if (yylval->string[len - 1] == '\"'
															||  yylval->string[len - 1] == '\n')
																yylval->string[len - 1] = '\0';

															return VALUE_ATTR;
														}
<VALUE>.												{ fprintf (stderr, "Missing attribute assignment %c\n", yytext[0]); return -1; }

<BOOL>true											{ return TRUE_DATA; }
<BOOL>false											{ return FALSE_DATA; }
<BOOL>\<\/bool\>									{ BEGIN INITIAL; return BOOL_END; }
<BOOL>.												{ BEGIN INITIAL; return yytext[0]; }

<SIMPLE>[\n\t ]									;
<SIMPLE>\<\/double\>								{ BEGIN INITIAL; return DOUBLE_END; }
<SIMPLE>\<\/float\>								{ BEGIN INITIAL; return FLOAT_END; }
<SIMPLE>\<\/int64\>								{ BEGIN INITIAL; return INT64_END; }
<SIMPLE>\<\/int32\>								{ BEGIN INITIAL; return INT32_END; }
<SIMPLE>\<\/int16\>								{ BEGIN INITIAL; return INT16_END; }
<SIMPLE>\<\/int8\>								{ BEGIN INITIAL; return INT8_END; }
<SIMPLE>\<\/messenger\>							{ BEGIN INITIAL; return MESSENGER_END; }
<SIMPLE>\<\/mime\>								{ BEGIN INITIAL; return MIME_END; }
<SIMPLE>\<\/off\>									{ BEGIN INITIAL; return OFF_END; }
<SIMPLE>\<\/pointer\>							{ BEGIN INITIAL; return POINTER_END; }
<SIMPLE>\<\/point\>								{ BEGIN INITIAL; return POINT_END; }
<SIMPLE>\<\/rect\>								{ BEGIN INITIAL; return RECT_END; }
<SIMPLE>\<\/ref\>									{ BEGIN INITIAL; return REF_END; }
<SIMPLE>\<\/color\>								{ BEGIN INITIAL; return COLOR_END; }
<SIMPLE>\<\/size\>								{ BEGIN INITIAL; return SIZE_END; }
<SIMPLE>\<\/ssize\>								{ BEGIN INITIAL; return SSIZE_END; }
<SIMPLE>\<\/time\>								{ BEGIN INITIAL; return TIME_END; }
<SIMPLE>\<\/uint64\>								{ BEGIN INITIAL; return UINT64_END; }
<SIMPLE>\<\/uint32\>								{ BEGIN INITIAL; return UINT32_END; }
<SIMPLE>\<\/uint16\>								{ BEGIN INITIAL; return UINT16_END; }
<SIMPLE>\<\/uint8\>								{ BEGIN INITIAL; return UINT8_END; }

<SIMPLE>{decimal}									{ yylval->decimal = atof (yytext); return DECIMAL; }
<SIMPLE>{integer}									{ yylval->integer = strtoll (yytext, 0, 10); return INTEGER; }
<SIMPLE>{word}										{ yylval->string  = strdup (yytext); return WORD; }
<SIMPLE>.											{ return yytext[0]; }

<STRBODY>\<\/string\>							{ BEGIN INITIAL; return STRING_END; }
<STRBODY>[^\n<]+									{ yylval->string = strdup (yytext); return LINE; }
<STRBODY>\n											{ return yytext[0]; }
<STRBODY>.											{ return yytext[0]; }

<RAWBODY>[\n\t ]									;
<RAWBODY>\<\/raw\>								{ BEGIN INITIAL; return RAW_END; }
<RAWBODY>[^\n\t <]+								{ yylval->string = strdup (yytext); return LINE; }
<RAWBODY>.											{ return yytext[0]; }

<COMMENT>-->										{ BEGIN INITIAL; }
<COMMENT>\?>										{ BEGIN INITIAL; }
<COMMENT>\n											;
<COMMENT>.											;

[\n\t ]												;
\<bool												{ BEGIN ATTRIBUTE; return type_state = BOOL_TAG; }
\<double												{ BEGIN ATTRIBUTE; return type_state = DOUBLE_TAG; }
\<float												{ BEGIN ATTRIBUTE; return type_state = FLOAT_TAG; }
\<int64												{ BEGIN ATTRIBUTE; return type_state = INT64_TAG; }
\<int32												{ BEGIN ATTRIBUTE; return type_state = INT32_TAG; }
\<int16												{ BEGIN ATTRIBUTE; return type_state = INT16_TAG; }
\<int8												{ BEGIN ATTRIBUTE; return type_state = INT8_TAG; }
\<message											{ BEGIN ATTRIBUTE; return type_state = MESSAGE_TAG; }
\<messenger											{ BEGIN ATTRIBUTE; return type_state = MESSENGER_TAG; }
\<mime												{ BEGIN ATTRIBUTE; return type_state = MIME_TAG; }
\<off													{ BEGIN ATTRIBUTE; return type_state = OFF_TAG; }
\<pointer											{ BEGIN ATTRIBUTE; return type_state = POINTER_TAG; }
\<point												{ BEGIN ATTRIBUTE; return type_state = POINT_TAG; }
\<raw													{ BEGIN ATTRIBUTE; return type_state = RAW_TAG; }
\<rect												{ BEGIN ATTRIBUTE; return type_state = RECT_TAG; }
\<ref													{ BEGIN ATTRIBUTE; return type_state = REF_TAG; }
\<color												{ BEGIN ATTRIBUTE; return type_state = COLOR_TAG; }
\<size												{ BEGIN ATTRIBUTE; return type_state = SIZE_TAG; }
\<ssize												{ BEGIN ATTRIBUTE; return type_state = SSIZE_TAG; }
\<string												{ BEGIN ATTRIBUTE; return type_state = STRING_TAG; }
\<time												{ BEGIN ATTRIBUTE; return type_state = TIME_TAG; }
\<uint64												{ BEGIN ATTRIBUTE; return type_state = UINT64_TAG; }
\<uint32												{ BEGIN ATTRIBUTE; return type_state = UINT32_TAG; }
\<uint16												{ BEGIN ATTRIBUTE; return type_state = UINT16_TAG; }
\<uint8												{ BEGIN ATTRIBUTE; return type_state = UINT8_TAG; }

\<\/message\>										{ return MESSAGE_END; }
\<!--													{ BEGIN COMMENT; }
.														{ return yytext[0]; }
\<\?xml												{ BEGIN COMMENT; }
\<locale											{ BEGIN COMMENT; }
\<\/locale\>										{ BEGIN COMMENT; }
%%

