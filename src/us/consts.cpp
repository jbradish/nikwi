////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  UndeadScript Game Scripting Engine                                        //
//  by Kostas "Bad Sector" Michalopoulos of Slashstone                        //
//                                                                            //
//  File name:    consts.cpp                                                  //
//  Description:  Constant values                                             //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <uscript/uscript.h>

static UScriptConst	*constStack = NULL;

void usInitConsts()
{
	usRegisterFloatConst("PI", M_PI);
	usRegisterFloatConst("RADIAN", M_PI/180.0f);
	usRegisterIntConst("true", 1);
	usRegisterIntConst("false", 0);
}

void usShutdownConsts()
{
	UScriptConst	*next;
	while (constStack)
	{
		next = constStack->next;
		delete constStack;
		constStack = next;
	}
}

static UScriptConst *usRegisterConst(String name, UScriptType *type)
{
	UScriptConst	*c = new UScriptConst;
	c->name = strdup(name);
	c->type = type;
	c->next = constStack;
	constStack = c;
	return c;
}

void usRegisterIntConst(String name, int value)
{
	usRegisterConst(name, usInteger)->i = value;
}

void usRegisterFloatConst(String name, float value)
{
	usRegisterConst(name, usFloat)->f = value;
}

UScriptConst *usFindConst(String name)
{
	for (UScriptConst *c=constStack;c;c=c->next)
		if (!strcmp(c->name, name))
			return c;
	return NULL;
}


