#include "LocaleApp.h"
#include <stdlib.h>
#include <Debug.h>
#include <Roster.h>
#include <Path.h>
#include <FindDirectory.h>
#include <Directory.h>
#include <Node.h>
#include <Beep.h>
#include <string.h>

#include "LocaleUtils.h"

/***********************************************************
 * Constructor
 ***********************************************************/
LocaleApp::LocaleApp(const char *signature)
	:BApplication(signature)
{	
	fLocaleUtils = new LocaleUtils(signature);
}

/***********************************************************
 * Destructor
 ***********************************************************/
LocaleApp::~LocaleApp()
{
	delete fLocaleUtils;
}

/***********************************************************
 * GetText
 ***********************************************************/
const char*
LocaleApp::GetText(const char* text)
{
	fLocaleUtils->GetText(text);
}