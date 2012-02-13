////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  UndeadScript Game Scripting Engine                                        //
//  by Kostas "Bad Sector" Michalopoulos of Slashstone                        //
//                                                                            //
//  File name:    script.h                                                    //
//  Description:  UScript class                                               //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef __USCRIPT_SCRIPT_H_INCLUDED__
#define __USCRIPT_SCRIPT_H_INCLUDED__

class UScript
{
	friend class UScriptCompiler;
	friend class UScriptVM;

	String			code;
	UScriptByteCode		*bc;
	struct UScriptVM	*vm;
	
	struct UScriptConst	*lconst;
	
	struct UScriptConst *createConst(String name, UScriptType *type);
	struct UScriptConst *findConst(String name);
	
	public:
	UScript();
	UScript(String code);
	~UScript();
	
	bool compile();
	void run();
	void runExclusive();
	
	void setIntConst(String name, int value);
	void setFloatConst(String name, float value);
	void setStringConst(String name, String value);
	void setHandleConst(String name, void *value);
};

#endif

