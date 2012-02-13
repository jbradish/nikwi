////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  UndeadScript Game Scripting Engine                                        //
//  by Kostas "Bad Sector" Michalopoulos of Slashstone                        //
//                                                                            //
//  File name:    uscript.h                                                   //
//  Description:  Main include file                                           //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef __USCRIPT_USCRIPT_H_INCLUDED__
#define __USCRIPT_USCRIPT_H_INCLUDED__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#ifndef __UNDEAD_TYPES_H_INCLUDED__
#ifndef __linux__
typedef unsigned int		uint;
#endif
typedef unsigned int		uint32;
typedef char			*String;
#endif

#include <uscript/type.h>
#include <uscript/bytecode.h>
#include <uscript/script.h>
#include <uscript/compiler.h>
#include <uscript/vm.h>
#include <uscript/funcs.h>
#include <uscript/consts.h>
#include <uscript/utils.h>

void usInitialize();
void usShutdown();

void usAddScriptVM(UScriptVM *vm);
void usRemoveScriptVM(UScriptVM *vm);

void usRunScripts();
bool usAreScriptsRunning();

#endif

