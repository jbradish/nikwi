////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  UndeadScript Game Scripting Engine                                        //
//  by Kostas "Bad Sector" Michalopoulos of Slashstone                        //
//                                                                            //
//  File name:    script.cpp                                                  //
//  Description:  UScript class                                               //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <uscript/uscript.h>

//UScript
UScript::UScript()
{
	code = strdup("");
	bc = NULL;
	vm = NULL;
	lconst = NULL;
}

UScript::UScript(String code)
{
	this->code = strdup(code);
	bc = NULL;
	vm = NULL;
	lconst = NULL;
}

UScript::~UScript()
{
	while (lconst)
	{
		UScriptConst	*next = lconst->next;
		delete lconst;
		lconst = next;
	}
	
	if (vm)
		delete vm;
	if (bc)
		delete bc;
	free(code);
}

bool UScript::compile()
{
	UScriptCompiler	*compiler = new UScriptCompiler(this);
	if (compiler->compile())
	{
		delete compiler;
		return true;
	}
	else
	{
		delete compiler;
		return false;
	}
}

void UScript::run()
{
	if (!bc)
	{
		if (!compile())
			return;
	}
	
	if (!vm)
		vm = new UScriptVM(this);
	
	vm->run();
}

void UScript::runExclusive()
{
	if (!bc)
	{
		if (!compile())
			return;
	}
	
	if (!vm)
		vm = new UScriptVM(this);
	
	vm->runExclusive();
}

UScriptConst *UScript::createConst(String name, UScriptType *type)
{
	UScriptConst	*c = new UScriptConst;
	c->name = strdup(name);
	c->type = type;
	c->next = lconst;
	lconst = c;
	return c;
}

UScriptConst *UScript::findConst(String name)
{
	for (UScriptConst *c=lconst;c;c=c->next)
		if (!strcmp(name, c->name))
			return c;
	return NULL;
}

void UScript::setIntConst(String name, int value)
{
	createConst(name, usInteger)->i = value;
}

void UScript::setFloatConst(String name, float value)
{
	createConst(name, usFloat)->f = value;
}

void UScript::setStringConst(String name, String value)
{
	createConst(name, usString)->p = (void*)value;
}

void UScript::setHandleConst(String name, void *value)
{
	createConst(name, usHandle)->p = value;
}

