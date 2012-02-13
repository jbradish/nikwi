////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  UndeadScript Game Scripting Engine                                        //
//  by Kostas "Bad Sector" Michalopoulos of Slashstone                        //
//                                                                            //
//  File name:    vm.h                                                        //
//  Description:  UScriptVM class                                             //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef __USCRIPT_VM_H_INCLUDED__
#define __USCRIPT_VM_H_INCLUDED__

struct UScriptVMFuncStackEntry
{
	union
	{
		uint	i;
		float	f;
		void	*p;
	};
};

struct UScriptVM
{
	UScript			*script;
	UScriptByteCode		*bc;
	uint			addr;
	int			a, b;
	float			fa, fb;
	void			*pa, *pb;
	bool			running;
	
	uint			*stack;
	uint			stackPos;
	uint			stackSize;
	
	UScriptVMFuncStackEntry	fStack[256];	// function argument stack
	uint			fStackPos;
	
	unsigned char		*memory;

	void			*vme;		// used in UScript.cpp

	UScriptVM(UScript *script);
	~UScriptVM();
	
	void run();
	void runExclusive();
	void stop();
	
	bool runCycle();
	
	void pushInt(int i);
	int popInt();
	void pushFloat(float f);
	float popFloat();
	void pushPtr(void *p);
	void *popPtr();
	
	int popIntArg();
	float popFloatArg();
	String popStringArg();
	void *popPointerArg();
};

void usInitScriptVMSubSystem();

#endif

