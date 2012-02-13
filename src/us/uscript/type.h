////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  UndeadScript Game Scripting Engine                                        //
//  by Kostas "Bad Sector" Michalopoulos of Slashstone                        //
//                                                                            //
//  File name:    type.h                                                      //
//  Description:  UScriptType class                                           //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef __USCRIPT_TYPE_H_INCLUDED__
#define __USCRIPT_TYPE_H_INCLUDED__

#define USB_INTEGER	0
#define USB_FLOAT	1
#define USB_HANDLE	2
#define USB_STRUCT	3
#define USB_STRING	4

struct UScriptTypeField
{
	String			name;
	struct UScriptType	*type;
};

struct UScriptType
{
	String			name;
	int			base;
	UScriptTypeField	*field;
	uint			fields;
	uint			size;
	
	UScriptType		*prev;
	UScriptType		*next;
	
	UScriptType(String name, int base);
	~UScriptType();
	
	void addField(String name, UScriptType *type);
	UScriptTypeField *getField(String name);
};

extern UScriptType	*usInteger;
extern UScriptType	*usFloat;
extern UScriptType	*usHandle;
extern UScriptType	*usString;

#define USTYPENAME(t)	((t)?(t)->name:"void")


void usInitTypeSubSystem();
void usShutdownTypeSubSystem();

UScriptType *usRegisterType(String name, int base);

UScriptType *usFindType(String name);

#endif

