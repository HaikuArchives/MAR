#include "LocaleApp.h"
#include <stdlib.h>
#include <Debug.h>
#include <Roster.h>
#include <Path.h>
#include <FindDirectory.h>
#include <Directory.h>
#include <Node.h>
#include <Beep.h>
#include "marcontrol.h"

/***********************************************************
 * Constructor
 ***********************************************************/
LocaleApp::LocaleApp(const char *signature)
	:BApplication(signature)
	,fStrings(NULL)
	,fDump(NULL)
{
	char *lang = getenv("LANGUAGE");
	if(lang)
		InitData(lang);
}

/***********************************************************
 * Destructor
 ***********************************************************/
LocaleApp::~LocaleApp()
{
	delete fStrings;
	if(fDump)
	{
		Dump();
		delete fDump;
	}
}

/***********************************************************
 * GetText
 ***********************************************************/
const char*
LocaleApp::GetText(const char* text)
{
	const char* p;
	if(fDump)
	{
		if( fDump->FindString(text,&p) != B_OK)
			fDump->AddString(text,text);
	}
	if(fStrings)
	{
		p = fStrings->FindString(text);
		if(p)
			return p;
	}
	return text;
}


/***********************************************************
 * InitMarc
 ***********************************************************/
void
LocaleApp::InitData(const char* lang)
{
	delete fStrings;
	fStrings = NULL;
	
	app_info ainfo;
	GetAppInfo(&ainfo);
	
	BPath path;
	::find_directory(B_USER_CONFIG_DIRECTORY,&path);
	path.Append("locale");
		
	const char* sig = ainfo.signature;
	int plen=strlen("application/x-vnd.");
	if(strstr(sig,"application/x-vnd.") )
		sig += plen;
	path.Append(sig);
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

/***********************************************************
 * ArgvReceived
 ***********************************************************/
void
LocaleApp::ArgvReceived(int32 argc,char **argv)
{
	for(int32 i = 0;i< argc;i++)
	{
		if(strcmp(argv[i],"--dump-locale") == 0)
		{
			fDump = new BMessage();
			break;
		}
	}
}

/***********************************************************
 * Dump
 ***********************************************************/
void
LocaleApp::Dump()
{
	BFile file("/boot/home/locale-dump",B_WRITE_ONLY|B_CREATE_FILE);
	
	fDump->Flatten(&file);
}