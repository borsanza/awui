// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Environment.h"

using namespace awui;

#ifdef __unix
	#ifndef __unix__
	#define __unix__
	#endif
#endif


String Environment::GetNewLine() {
	static String newLine;

#ifdef __unix__
	newLine = String("\n");
#else
	newLine = String("\r\n");
#endif

	return newLine;
}
