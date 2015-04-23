/* RespBuffer.cpp */

#include "RespBuffer.h"



RespBuffer::RespBuffer(uint32 size)
	: prev(NULL), next(NULL)
{
	buf = new char[size];
	start = buf;
	end = buf + size;
}


RespBuffer::~RespBuffer()
{
	delete buf;
}


uint32 RespBuffer::Length()
{
	return end - start;
}


