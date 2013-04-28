// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Process.h"
#include "string.h"

using namespace awui::Diagnostics;

Process::Process() {
	pPipe = NULL;
}

Process::~Process() {
	if (pPipe)
		pclose(pPipe);
}

void Process::Start(String process, String arguments) {
	String cmd = process + " " + arguments;
	pPipe = popen(cmd.ToCharArray(), "r");
	if (!pPipe)
		return;
}

int Process::DoSelect() {
	fd_set rfds;
	struct timeval tv;

	FD_ZERO(&rfds);
	FD_SET(fileno(pPipe), &rfds);

	tv.tv_usec = 0;
	tv.tv_sec = 0;

	return select(fileno(pPipe) + 1, &rfds, NULL, NULL, &tv);
}

bool Process::GetHasExited() {
	return (feof(pPipe));
}

bool Process::GetHasString() {
	int retval = this->DoSelect();

	if (retval == -1)
		return false;
	else if (retval)
		return true;
	else
		return false;
}

awui::String Process::GetLine() {
	char buffer[256];
	if (!feof(pPipe))
		if (fgets(buffer, 256, pPipe) != NULL) {
			strtok(buffer, "\n");
			return buffer;
		}

	return "";
}
