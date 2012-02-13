////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  UndeadScript Game Scripting Engine                                        //
//  by Kostas "Bad Sector" Michalopoulos of Slashstone                        //
//                                                                            //
//  File name:    vm.cpp                                                      //
//  Description:  UScriptVM class                                             //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <uscript/uscript.h>

typedef void	(*OpFunc)(UScriptVM*);

static OpFunc	op[256];

//Operation functions
static void opNop(UScriptVM *vm)
{
}

static void opNullA(UScriptVM *vm)
{
	vm->pa = NULL;
}

static void opSetA(UScriptVM *vm)
{
	vm->a = vm->bc->getInt(vm->addr);
	vm->addr += 4;
}

static void opSetFA(UScriptVM *vm)
{
	vm->fa = vm->bc->getFloat(vm->addr);
	vm->addr += 4;
}

static void opSetSS(UScriptVM *vm)
{
	vm->pa = vm->bc->getSS(vm->bc->getInt(vm->addr));
	vm->addr += 4;
}

static void opPushA(UScriptVM *vm)
{
	vm->pushInt(vm->a);
}

static void opPushFA(UScriptVM *vm)
{
	vm->pushFloat(vm->fa);
}

static void opPushPA(UScriptVM *vm)
{
	vm->pushPtr(vm->pa);
}

static void opPopB(UScriptVM *vm)
{
	vm->b = vm->popInt();
}

static void opPopFB(UScriptVM *vm)
{
	vm->fb = vm->popFloat();
}

static void opPopPB(UScriptVM *vm)
{
	vm->pb = vm->popPtr();
}

static void opLoadA(UScriptVM *vm)
{
	vm->a = *((int*)&vm->memory[vm->bc->getInt(vm->addr)]);
	vm->addr += 4;
}

static void opLoadFA(UScriptVM *vm)
{
	vm->fa = *((float*)&vm->memory[vm->bc->getInt(vm->addr)]);
	vm->addr += 4;
}

static void opLoadPA(UScriptVM *vm)
{
	int	i;
	vm->pa = *((void**)&vm->memory[i = vm->bc->getInt(vm->addr)]);
	vm->addr += 4;
}

static void opSaveA(UScriptVM *vm)
{
	int	*dest = (int*)&vm->memory[vm->bc->getInt(vm->addr)];
	*dest = vm->a;
	vm->addr += 4;
}

static void opSaveFA(UScriptVM *vm)
{
	float	*dest = (float*)&vm->memory[vm->bc->getInt(vm->addr)];
	*dest = vm->fa;
	vm->addr += 4;
}

static void opSavePA(UScriptVM *vm)
{
	int	i;
	void	**dest = (void**)&vm->memory[i = vm->bc->getInt(vm->addr)];
	*dest = vm->pa;
	vm->addr += 4;
}

static void opAdd(UScriptVM *vm)
{
	vm->a += vm->b;
}

static void opSub(UScriptVM *vm)
{
	vm->a = vm->b - vm->a;
}

static void opMul(UScriptVM *vm)
{
	vm->a *= vm->b;
}

static void opDiv(UScriptVM *vm)
{
	if (!vm->a)
		return;
	vm->a = vm->b / vm->a;
}

static void opMod(UScriptVM *vm)
{
	if (!vm->a)
		return;
	vm->a = vm->b % vm->a;
}

static void opFAdd(UScriptVM *vm)
{
	vm->fa += vm->fb;
}

static void opFSub(UScriptVM *vm)
{
	vm->fa = vm->fb - vm->fa;
}

static void opFMul(UScriptVM *vm)
{
	vm->fa *= vm->fb;
}

static void opFDiv(UScriptVM *vm)
{
	if (vm->fa == 0.0f)
		return;
	vm->fa = vm->fb / vm->fa;
}

static void opFMod(UScriptVM *vm)
{
	if ((int)vm->fa == 0)
		return;
	vm->fa = (float)((int)vm->fb % (int)vm->fa);
}

static void opAnd(UScriptVM *vm)
{
	vm->a = vm->a && vm->b;
}

static void opOr(UScriptVM *vm)
{
	vm->a = vm->a || vm->b;
}

static void opEq(UScriptVM *vm)
{
	vm->a = vm->a == vm->b;
}

static void opNEq(UScriptVM *vm)
{
	vm->a = vm->a != vm->b;
}

static void opGreat(UScriptVM *vm)
{
	vm->a = vm->b > vm->a;
}

static void opLess(UScriptVM *vm)
{
	vm->a = vm->b < vm->a;
}

static void opGreatEq(UScriptVM *vm)
{
	vm->a = vm->b >= vm->a;
}

static void opLessEq(UScriptVM *vm)
{
	vm->a = vm->b <= vm->a;
}

static void opFEq(UScriptVM *vm)
{
	vm->a = vm->fa == vm->fb;
}

static void opFNEq(UScriptVM *vm)
{
	vm->a = vm->fa != vm->fb;
}

static void opFGreat(UScriptVM *vm)
{
	vm->a = vm->fb > vm->fa;
}

static void opFLess(UScriptVM *vm)
{
	vm->a = vm->fb < vm->fa;
}

static void opFGreatEq(UScriptVM *vm)
{
	vm->a = vm->fb >= vm->fa;
}

static void opFLessEq(UScriptVM *vm)
{
	vm->a = vm->fb <= vm->fa;
}

static void opPEq(UScriptVM *vm)
{
	vm->a = vm->pa == vm->pb;
}

static void opPNEq(UScriptVM *vm)
{
	vm->a = vm->pa != vm->pb;
}

static void opArgPush(UScriptVM *vm)
{
	vm->fStack[vm->fStackPos++].i = vm->a;
}

static void opArgPushF(UScriptVM *vm)
{
	vm->fStack[vm->fStackPos++].f = vm->fa;
}

static void opArgPushP(UScriptVM *vm)
{
	vm->fStack[vm->fStackPos++].p = vm->pa;
}

static void opArgPop(UScriptVM *vm)
{
	vm->a = vm->fStack[--vm->fStackPos].i;
}

static void opArgPopF(UScriptVM *vm)
{
	vm->fa = vm->fStack[--vm->fStackPos].f;
}

static void opArgPopP(UScriptVM *vm)
{
	vm->pa = vm->fStack[--vm->fStackPos].p;
}

static void opCall(UScriptVM *vm)
{
	uint	addr = vm->bc->getInt(vm->addr);
	vm->pushInt(vm->addr + 4);
	vm->addr = addr;
}

static void opRet(UScriptVM *vm)
{
	vm->addr = vm->popInt();
}

static void opNCall(UScriptVM *vm)
{
	usCallNativeFunction(vm->bc->getInt(vm->addr), vm);
	vm->addr += 4;
}

static void opJump(UScriptVM *vm)
{
	vm->addr = vm->bc->getInt(vm->addr);
}

static void opJumpIf(UScriptVM *vm)
{
	uint	addr = vm->bc->getInt(vm->addr);
	if (vm->a)
		vm->addr = addr;
	else
		vm->addr += 4;
}

static void opJumpIfNot(UScriptVM *vm)
{
	uint	addr = vm->bc->getInt(vm->addr);
	if (vm->a)
		vm->addr += 4;
	else
		vm->addr = addr;
}

static void opStop(UScriptVM *vm)
{
	vm->addr = vm->bc->length();
}

//UScriptVM
UScriptVM::UScriptVM(UScript *script)
{
	this->script = script;
	bc = script->bc;
	running = false;
}

UScriptVM::~UScriptVM()
{
	stop();
}

void UScriptVM::run()
{
	static bool	functionArrayCreated = false;
	addr = 0;
	fa = fb = a = b = 0;
	pa = pb = NULL;
	stack = NULL;
	stackPos = stackSize = 0;
	running = true;
	memory = (unsigned char*)malloc(bc->dataSize);
	fStackPos = 0;
	
	if (!functionArrayCreated)
	{
		usCreateFunctionArray();
		functionArrayCreated = true;
	}
	
	usAddScriptVM(this);
}

void UScriptVM::runExclusive()
{
	run();
	while (runCycle());
	stop();
}

void UScriptVM::stop()
{
	if (!running)
		return;
	if (stack)
	{
		free(stack);
		stack = NULL;
	}
	free(memory);
	running = false;
	usRemoveScriptVM(this);
}

bool UScriptVM::runCycle()
{
	if (addr < bc->bcl)
	{
		unsigned char	code = bc->getByte(addr++);
		op[code](this);
		return true;
	}
	
	return false;
}

void UScriptVM::pushInt(int i)
{
	if (stackPos == stackSize)
	{
		stackSize += 32;
		stack = (uint*)realloc(stack, sizeof(int)*stackSize);
	}
	stack[stackPos++] = i; 
}

int UScriptVM::popInt()
{
	return stack[--stackPos];
}

void UScriptVM::pushFloat(float f)
{
	if (stackPos == stackSize)
	{
		stackSize += 32;
		stack = (uint*)realloc(stack, sizeof(uint)*stackSize);
	}
	stack[stackPos++] = *((uint*)&f); 
}

float UScriptVM::popFloat()
{
	return *((float*)&stack[--stackPos]);
}

void UScriptVM::pushPtr(void *p)
{
	unsigned long long	ll = (unsigned long long)p;
	pushInt((int)(ll & 0xFFFFFFFF));
	pushInt((int)(ll >> 32));
}

void *UScriptVM::popPtr()
{
	unsigned int		l1 = (unsigned int)popInt();
	unsigned int		l2 = (unsigned int)popInt();
	unsigned long long	ll = ((unsigned long long)l1 << 32) | l2;
	return (void*)ll;
}

int UScriptVM::popIntArg()
{
	return fStack[--fStackPos].i;
}

float UScriptVM::popFloatArg()
{
	return fStack[--fStackPos].f;
}

String UScriptVM::popStringArg()
{
	return (String)fStack[--fStackPos].p;
}

void *UScriptVM::popPointerArg()
{
	return fStack[--fStackPos].p;
}

//Global
void usInitScriptVMSubSystem()
{
	op[BC_NOP] = opNop;
	op[BC_NULLA] = opNullA;
	op[BC_SETA] = opSetA;
	op[BC_SETFA] = opSetFA;
	op[BC_SETSS] = opSetSS;
	op[BC_PUSHA] = opPushA;
	op[BC_PUSHFA] = opPushFA;
	op[BC_PUSHPA] = opPushPA;
	op[BC_POPB] = opPopB;
	op[BC_POPFB] = opPopFB;
	op[BC_POPPB] = opPopPB;
	
	op[BC_LOADA] = opLoadA;
	op[BC_LOADFA] = opLoadFA;
	op[BC_LOADPA] = opLoadPA;
	op[BC_SAVEA] = opSaveA;
	op[BC_SAVEFA] = opSaveFA;
	op[BC_SAVEPA] = opSavePA;
	
	op[BC_ADD] = opAdd;
	op[BC_SUB] = opSub;
	op[BC_MUL] = opMul;
	op[BC_DIV] = opDiv;
	op[BC_MOD] = opMod;
	op[BC_FADD] = opFAdd;
	op[BC_FSUB] = opFSub;
	op[BC_FMUL] = opFMul;
	op[BC_FDIV] = opFDiv;
	op[BC_FMOD] = opFMod;

	op[BC_AND] = opAnd;
	op[BC_OR] = opOr;
	op[BC_EQ] = opEq;
	op[BC_NEQ] = opNEq;
	op[BC_GREAT] = opGreat;
	op[BC_LESS] = opLess;
	op[BC_GREATEQ] = opGreatEq;
	op[BC_LESSEQ] = opLessEq;
	op[BC_FEQ] = opFEq;
	op[BC_FNEQ] = opFNEq;
	op[BC_FGREAT] = opFGreat;
	op[BC_FLESS] = opFLess;
	op[BC_FGREATEQ] = opFGreatEq;
	op[BC_FLESSEQ] = opFLessEq;
	op[BC_PEQ] = opPEq;
	op[BC_PNEQ] = opPNEq;

	op[BC_ARGPUSH] = opArgPush;
	op[BC_ARGPUSHF] = opArgPushF;
	op[BC_ARGPUSHP] = opArgPushP;
	op[BC_ARGPOP] = opArgPop;
	op[BC_ARGPOPF] = opArgPopF;
	op[BC_ARGPOPP] = opArgPopP;
	
	op[BC_CALL] = opCall;
	op[BC_RET] = opRet;
	op[BC_NCALL] = opNCall;
	op[BC_JUMP] = opJump;
	op[BC_JUMPIF] = opJumpIf;
	op[BC_JUMPIFNOT] = opJumpIfNot;
	
	op[BC_STOP] = opStop;
}

