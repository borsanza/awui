/**
 * awui/Scripting/JavaScript.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "JavaScript.h"

using namespace awui::Scripting::JavaScript;

int JavaScript::JavaScript::instances = 0;

JavaScript::JavaScript() {
	if (JavaScript::instances == 0)
		JavaScript::Initialize();

	JavaScript::instances++;

	this->context = v8::Context::New();
}

JavaScript::~JavaScript() {
	JavaScript::instances--;
	if (JavaScript::instances == 0)
		JavaScript::Shutdown();
}

void JavaScript::Initialize() {
//	v8::V8::InitializeICUDefaultLocation("");
//	v8::V8::InitializeExternalStartupData("");
//	std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
//	v8::V8::InitializePlatform(platform.get());
	v8::V8::Initialize();
}

void JavaScript::Run(const char * code) {
	v8::HandleScope handle_scope;
	v8::Context::Scope context_scope(this->context);
	v8::Handle<v8::String> source = v8::String::New(code);
	v8::Handle<v8::Script> script = v8::Script::Compile(source);
	v8::Handle<v8::Value> result = script->Run();
	v8::String::AsciiValue ascii(result);
	printf("%s\n", *ascii);
}

void JavaScript::Shutdown() {
	v8::V8::Dispose();
//	v8::V8::ShutdownPlatform();
}
