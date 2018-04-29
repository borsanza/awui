/**
 * awui/Scripting/JavaScript.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "JavaScript.h"

using namespace awui::Scripting::JavaScript;

int JavaScript::JavaScript::instances = 0;

/*
ls [<pattern>]                         - ver items de una lista
cd [<path>]                            - acceder a una lista
add <item> [<property>=<value> ...]    - Crear un item integro
cp <item> <itemnew>                    - duplicar item
rm <item>                              - eliminar un item de la lista
mv <olditem> <newitem>                 - renombrar un item de la lista

set <item>.<property> <value>          - establecer una propiedad de un elemento
unset <item>.<property>                - vaciar propiedad de un elemento
get <item>.<property>                  - obtener una propiedad de un elemento
cat <item>                             - ver todas las propiedades de un elemento

ln <item>.<property> <item>.<property> - enlazar una propiedad a otra

backup                                 - exportar todos los elementos
*/

v8::Handle<v8::Value> js_ls(const v8::Arguments& args) {
	printf("ls");
	for (int i = 0; i < args.Length(); i++) {
		v8::HandleScope handle_scope();
		v8::String::Utf8Value str(args[i]);
		const char* cstr = str.operator *();
		printf(" %s", cstr);
	}

	//	printf("ls [<pattern>]\n");
	printf("\n");
	return v8::Null();
}

v8::Handle<v8::Value> js_cd(const v8::Arguments& args) {
	printf("cd [<path>]\n");
	return v8::Null();
}

v8::Handle<v8::Value> js_add(const v8::Arguments& args) {
	printf("add <item> [<property>=<value> ...]\n");
	return v8::Null();
}

v8::Handle<v8::Value> js_cp(const v8::Arguments& args) {
	printf("cp <item> <itemnew>\n");
	return v8::Null();
}

v8::Handle<v8::Value> js_rm(const v8::Arguments& args) {
	printf("rm <item>\n");
	return v8::Null();
}

v8::Handle<v8::Value> js_mv(const v8::Arguments& args) {
	printf("mv <olditem> <newitem>\n");
	return v8::Null();
}

v8::Handle<v8::Value> js_set(const v8::Arguments& args) {
	printf("set <item>.<property> <value>\n");
	return v8::Null();
}

v8::Handle<v8::Value> js_unset(const v8::Arguments& args) {
	printf("unset <item>.<property>\n");
	return v8::Null();
}

v8::Handle<v8::Value> js_get(const v8::Arguments& args) {
	printf("get <item>.<property>\n");
	return v8::Null();
}

v8::Handle<v8::Value> js_cat(const v8::Arguments& args) {
	printf("cat <item>\n");
	return v8::Null();
}

v8::Handle<v8::Value> js_ln(const v8::Arguments& args) {
	printf("ln\n");
	return v8::Null();
}

v8::Handle<v8::Value> js_backup(const v8::Arguments& args) {
	printf("backup\n");
	return v8::Null();
}

JavaScript::JavaScript() {
	if (JavaScript::instances == 0)
		JavaScript::Initialize();

	JavaScript::instances++;

	v8::HandleScope handle_scope;
	v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New();
	global->Set("ls",     v8::FunctionTemplate::New(js_ls));
	global->Set("cd",     v8::FunctionTemplate::New(js_cd));
	global->Set("add",    v8::FunctionTemplate::New(js_add));
	global->Set("cp",     v8::FunctionTemplate::New(js_cp));
	global->Set("rm",     v8::FunctionTemplate::New(js_rm));
	global->Set("mv",     v8::FunctionTemplate::New(js_mv));
	global->Set("set",    v8::FunctionTemplate::New(js_set));
	global->Set("unset",  v8::FunctionTemplate::New(js_unset));
	global->Set("get",    v8::FunctionTemplate::New(js_get));
	global->Set("cat",    v8::FunctionTemplate::New(js_cat));
	global->Set("ln",     v8::FunctionTemplate::New(js_ln));
	global->Set("backup", v8::FunctionTemplate::New(js_backup));
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
