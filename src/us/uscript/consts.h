////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  UndeadScript Game Scripting Engine                                        //
//  by Kostas "Bad Sector" Michalopoulos of Slashstone                        //
//                                                                            //
//  File name:    consts.h                                                    //
//  Description:  Constant values                                             //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef __USCRIPT_CONSTS_H_INCLUDED__
#define __USCRIPT_CONSTS_H_INCLUDED__

struct UScriptConst
{
	String		name;
	union
	{
		int	i;
		float	f;
		void	*p;
	};
	UScriptType	*type;
	UScriptConst	*next;
};

void usInitConsts();
void usShutdownConsts();

void usRegisterIntConst(String name, int value);
void usRegisterFloatConst(String name, float value);
UScriptConst *usFindConst(String name);

#endif

