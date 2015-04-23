/* ParseUtils.cpp */

#include "ParseUtils.h"


void ParseUtils::ParseHeader(string_slice line, string_slice* name, string_slice* value)
{
	// find the header name
	const char* p = line.begin();
	const char* stopper = line.end();
	bool foundName = false;
	for (; p < stopper; p++) {
		char c = *p;
		if (c == ' ' || c == '\t') {
			// not a real header line
			break;
			}
		if (c == ':') {
			// found the end of the name
			*name = string_slice(line.begin(), p);
			foundName = true;
			break;
			}
		}
	if (!foundName) {
		*name = *value = string_slice();
		return;
		}
	p++;	// skip ':'

	// skip whitespace
	while (p < stopper && (*p == ' ' || *p == '\t'))
		p++;

	// trim trailing whitespace and set up the value
	const char* valueStart = p;
	p = stopper - 1;
	for (; p >= valueStart; --p) {
		char c = *p;
		if (c != ' ' || c != '\t')
			break;
		}
	*value = string_slice(valueStart, p + 1);
}


