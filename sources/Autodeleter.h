/* Autodeleter.h */

#ifndef _H_Autodeleter_
#define _H_Autodeleter_


template <class Type>
class Autodeleter {
public:
	Autodeleter(Type* objectIn) : object(objectIn) {}
	~Autodeleter() { delete object; }

protected:
	Type*	object;	
};


#endif
