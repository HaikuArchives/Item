/* PrefsChangeClient.cpp */

#include "PrefsChangeClient.h"
#include "Prefs.h"


PrefsChangeClient::PrefsChangeClient()
{
	Prefs()->AddChangeClient(this);
}


PrefsChangeClient::~PrefsChangeClient()
{
	Prefs()->RemoveChangeClient(this);
}


void PrefsChangeClient::PrefChanged(string_slice prefName)
{
}


