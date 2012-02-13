////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  UndeadScript Game Scripting Engine                                        //
//  by Kostas "Bad Sector" Michalopoulos of Slashstone                        //
//                                                                            //
//  File name:    funcs.h                                                     //
//  Description:  Native functions                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef __USCRIPT_FUNCS_H_INCLUDED__
#define __USCRIPT_FUNCS_H_INCLUDED__

typedef void	(*UScriptNativeFuncProc)(UScriptVM*);

struct UScriptNativeFuncArg
{
	UScriptType	*type;
};

struct UScriptNativeFunc
{
	uint			index;
	String			name;
	UScriptNativeFuncProc	proc;
	UScriptType		*type;
	UScriptNativeFuncArg	*arg;
	uint			args;
	
	UScriptNativeFunc	*next;
	
	UScriptNativeFunc(String name, UScriptNativeFuncProc proc,
		UScriptType *type);
	~UScriptNativeFunc();
};

void usInitNativeFunctions();
void usShutdownNativeFunctions();

UScriptNativeFunc *usRegisterNativeFunc(String name, String args,
	UScriptNativeFuncProc proc, UScriptType *type);
UScriptNativeFunc *usFindNativeFunc(String name);

void usCreateFunctionArray();
void usCallNativeFunction(uint index, UScriptVM *vm);

#endif

