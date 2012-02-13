////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  UndeadScript Game Scripting Engine                                        //
//  by Kostas "Bad Sector" Michalopoulos of Slashstone                        //
//                                                                            //
//  File name:    type.cpp                                                    //
//  Description:  UScriptType class                                           //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <uscript/uscript.h>

static UScriptType	*first = NULL, *last = NULL;

UScriptType	*usInteger;
UScriptType	*usFloat;
UScriptType	*usHandle;
UScriptType	*usString;

//UScriptType
UScriptType::UScriptType(String name, int base)
{
	this->name = strdup(name);
	this->base = base;
	field = NULL;
	fields = 0;
	prev = NULL;
	next = NULL;
	switch (base)
	{
		case USB_INTEGER:
		case USB_FLOAT:
		case USB_STRING:
			size = 4;
			break;
		case USB_HANDLE:
			size = 8;
			break;
		case USB_STRUCT:
			size = 0;
	}
}

UScriptType::~UScriptType()
{
	for (uint i=0;i<fields;i++)
		free(field[i].name);
	free(field);
}

void UScriptType::addField(String name, UScriptType *type)
{
	field = (UScriptTypeField*)realloc(field,
		sizeof(UScriptTypeField)*(fields + 1));
	field[fields].name = strdup(name);
	field[fields].type = type;
	fields++;
	size += type->size;
}

UScriptTypeField *UScriptType::getField(String name)
{
	for (uint i=0;i<fields;i++)
		if (!strcmp(name, field[i].name))
			return &field[i];
	return NULL;
}

//Globals
void usInitTypeSubSystem()
{
	UScriptType	*type;

	usInteger = usRegisterType("int", USB_INTEGER);
	usFloat = usRegisterType("float", USB_FLOAT);
	usHandle = usRegisterType("handle", USB_HANDLE);
	usString = usRegisterType("string", USB_STRING);
	
	/*
	type = usRegisterType("Vector", USB_STRUCT);
	type->addField("x", usFloat);
	type->addField("y", usFloat);
	*/
}

void usShutdownTypeSubSystem()
{
	while (first)
	{
		last = first->next;
		delete first;
		first = last;
	}
}

UScriptType *usRegisterType(String name, int base)
{
	UScriptType	*type = new UScriptType(name, base);
	type->prev = last;
	if (last)
		last->next = type;
	else
		first = type;
	last = type;
	
	return type;
}

UScriptType *usFindType(String name)
{
	for (UScriptType *type=first;type;type = type->next)
		if (!strcmp(type->name, name))
			return type;
	return NULL;
}

