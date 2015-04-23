/* TextReader.cpp */

#include "TextReader.h"



TextReader::TextReader(string_slice text)
	: p(text.begin()), stopper(text.end()), start(text.begin())
{
}


bool TextReader::AtEOF()
{
	return (p >= stopper);
}


string_slice TextReader::NextLine()
{
	const char* lineStart = p;
	while (p < stopper && *p != '\n')
		p++;
	string_slice line(lineStart, p);
	if (p < stopper)
		p++;	// skip '\n'
	return line;
}


string_slice TextReader::NextTabField()
{
	// find the field
	const char* fieldStart = p;
	for (; p < stopper; p++) {
		char c = *p;
		if (c == '\t' || c == '\r' || c == '\n')
			break;
		}
	string_slice result(fieldStart, p);

	// skip the tab
	if (p < stopper && *p == '\t')
		p++;

	return result;
}


int TextReader::Position()
{
	return p - start;
}


