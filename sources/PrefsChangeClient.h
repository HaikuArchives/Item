/* PrefsChangeClient.h */

#ifndef _H_PrefsChangeClient_
#define _H_PrefsChangeClient_

#include "string_slice.h"

class PrefsChangeClient {
public:
	PrefsChangeClient();
	virtual~PrefsChangeClient();
	virtual void	PrefChanged(string_slice prefName);
};


#endif
