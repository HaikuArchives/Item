/* DisplayableException.cpp */

#include "DisplayableException.h"
#include "Error.h"


DisplayableException::DisplayableException()
{
}


DisplayableException::~DisplayableException()
{
}


void DisplayableException::Display()
{
	Error::ShowError(GetMessage());
}


