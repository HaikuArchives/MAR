#include <stdlib.h>
#include <Message.h>
#include <Roster.h>
#include <FindDirectory.h>
#include <Path.h>
#include <Application.h>
#include <stdio.h>

#include "LocaleUtils.h"
#include "marcontrol.h"
/***********************************************************
 * Constructor
 ***********************************************************/
LocaleUtils::LocaleUtils(const char* app_sig)
	:fStrings(NULL)
{
	int plen=strlen("application/x-vnd.");
	if(strstr(app_sig,"application/x-vnd.") )
		app_sig += plen;

	char *lang = getenv("LANGUAGE");
	if(lang)
		InitData(lang,app_sig);
}


/***********************************************************
 * Destructor
 ***********************************************************/
LocaleUtils::~LocaleUtils()
{	
	delete fStrings;
}

/***********************************************************
 * GetText
 ***********************************************************/
const char*
LocaleUtils::GetText(const char* text)
{
	const char* p;
	if(fStrings)
	{
		p = fStrings->FindString(text);
		if(p)
			return p;
	}
	return text;	
}

/***********************************************************
 * InitData
 ***********************************************************/
void
LocaleUtils::InitData(const char* lang,const char* app_sig)
{
	delete fStrings;
	fStrings = NULL;
	
	BPath path;
	::find_directory(B_USER_CONFIG_DIRECTORY,&path);
	path.Append("locale");	
	path.Append(app_sig);
	
	char *filename = new char[strlen(lang)+5];
	::sprintf(filename,"%s.xml",lang);
	path.Append(filename);
	delete[] filename;
	
	if(BNode(path.Path()).InitCheck() == B_OK)
	{
		const char* p = path.Path();
		
		MarControl marc(1,&p);
		 if (marc.Parse() != MAR_OK) 
   			return; 
		
		BMessage *msg = marc.Message();
		BMessage data;
		msg->FindMessage(p,&data);
		fStrings = new BMessage(data);	
	}
}