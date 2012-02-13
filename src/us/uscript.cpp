////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  UndeadScript Game Scripting Engine                                        //
//  by Kostas "Bad Sector" Michalopoulos of Slashstone                        //
//                                                                            //
//  File name:    uscript.cpp                                                 //
//  Description:  UndeadScript main code                                      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <uscript/uscript.h>

struct UVM
{
	UScriptVM	*vm;
	UVM		*prev;
	UVM		*next;
};

static UVM	*first = NULL, *last = NULL;


void usInitialize()
{
	usInitScriptVMSubSystem();
	usInitTypeSubSystem();
	usInitNativeFunctions();
	usInitConsts();
}

void usShutdown()
{
	UVM	*next;
	for (UVM *vme=first;vme;vme = next)
	{
		next = vme->next;
		vme->vm->stop();
	}
	
	usShutdownConsts();
	usShutdownNativeFunctions();
	usShutdownTypeSubSystem();
}


void usAddScriptVM(UScriptVM *vm)
{
	UVM	*vme = new UVM;
	vme->vm = vm;
	vme->prev = last;
	vme->next = NULL;
	if (last)
		last->next = vme;
	else
		first = vme;
	last = vme;
	vm->vme = (void*)vme;
}

void usRemoveScriptVM(UScriptVM *vm)
{
	UVM	*vme = (UVM*)vm->vme;
	if (!vme)
		return;
	if (vme->prev)
		vme->prev->next = vme->next;
	else
		first = vme->next;
	if (vme->next)
		vme->next->prev = vme->prev;
	else
		last = vme->prev;
	delete vme;
	vm->vme = NULL;
}

void usRunScripts()
{
	UVM	*next;
	for (UVM *vme=first;vme;vme = next)
	{
		next = vme->next;
		if (!vme->vm->runCycle())
			vme->vm->stop();
		vme = next;
	}
}

bool usAreScriptsRunning()
{
	return first!=NULL;
}

