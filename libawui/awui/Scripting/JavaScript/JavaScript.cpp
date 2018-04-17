/**
 * awui/Scripting/JavaScript.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "JavaScript.h"

using namespace awui::Scripting::JavaScript;

int JavaScript::JavaScript::instances = 0;

v8::Handle<v8::Value> js_cd(const v8::Arguments& args) {
/*
	bool first = true;
    for (int i = 0; i < args.Length(); i++) {
        v8::HandleScope handle_scope();
        if (first)
            first = false;
        else
            printf(" ");
        v8::String::Utf8Value str(args[i]);
        const char* cstr = str.operator *();
        printf("%s", cstr);
    }
*/
	printf("cd\n");
    return v8::Null();
}

v8::Handle<v8::Value> js_ls(const v8::Arguments& args) {
	printf("ls\n");
    return v8::Null();
}

v8::Handle<v8::Value> js_set(const v8::Arguments& args) {
	printf("set\n");
    return v8::Null();
}

v8::Handle<v8::Value> js_get(const v8::Arguments& args) {
	printf("get\n");
    return v8::Null();
}

JavaScript::JavaScript() {
	if (JavaScript::instances == 0)
		JavaScript::Initialize();

	JavaScript::instances++;

	v8::HandleScope handle_scope;
	v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New();
	global->Set("cd", v8::FunctionTemplate::New(js_cd));
	global->Set("ls", v8::FunctionTemplate::New(js_ls));
	global->Set("set", v8::FunctionTemplate::New(js_set));
	global->Set("get", v8::FunctionTemplate::New(js_get));
	this->context = v8::Context::New(NULL, global);
}

JavaScript::~JavaScript() {
	JavaScript::instances--;
	if (JavaScript::instances == 0)
		JavaScript::Shutdown();
}

void JavaScript::Initialize() {
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
}
