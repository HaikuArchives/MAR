
#ifndef READING_H_

#include <List.h>
#include <stdio.h>

struct ReadControl
{
	BList						raws;
	BList						strings;
	uint32					width;

								ReadControl (void);
								~ReadControl (void);

	bool						AddType (BList *, const char *);
	bool						AddRaw (const char *);
	bool						AddString (const char *);
};


void read_message_file (FILE *, const char *, ReadControl *);
void read_resource_file (FILE *, const char *, ReadControl *);
void read_attribute_file (FILE *, const char *, ReadControl *);
void string_code (char [4], uint32);

#endif
