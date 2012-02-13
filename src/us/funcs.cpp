////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  UndeadScript Game Scripting Engine                                        //
//  by Kostas "Bad Sector" Michalopoulos of Slashstone                        //
//                                                                            //
//  File name:    funcs.cpp                                                   //
//  Description:  Native functions                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <uscript/uscript.h>

static UScriptNativeFunc	*funcStack = NULL;
static UScriptNativeFunc	**func = NULL;
static uint			funcs = 0;

//UScriptNativeFunc
UScriptNativeFunc::UScriptNativeFunc(String name, UScriptNativeFuncProc proc,
	UScriptType *type)
{
	this->name = strdup(name);
	this->proc = proc;
	this->type = type;
	arg = NULL;
	args = 0;
}

UScriptNativeFunc::~UScriptNativeFunc()
{
	free(name);
	if (arg)
		free(arg);
}

//Core RunTimeLibrary
static void CRTL_itof(UScriptVM *vm)
{
	vm->fa = (float)vm->popIntArg();
}

static void CRTL_ftoi(UScriptVM *vm)
{
	vm->a = (int)vm->popFloatArg();
}

static void CRTL_sin(UScriptVM *vm)
{
	vm->fa = sin(vm->popFloatArg());
}

static void CRTL_cos(UScriptVM *vm)
{
	vm->fa = cos(vm->popFloatArg());
}

static void CRTL_tan(UScriptVM *vm)
{
	vm->fa = tan(vm->popFloatArg());
}

static void CRTL_asin(UScriptVM *vm)
{
	vm->fa = asin(vm->popFloatArg());
}

static void CRTL_acos(UScriptVM *vm)
{
	vm->fa = acos(vm->popFloatArg());
}

static void CRTL_atan(UScriptVM *vm)
{
	vm->fa = atan(vm->popFloatArg());
}

static void CRTL_rand(UScriptVM *vm)
{
	vm->fa = (float)rand()/(float)RAND_MAX;
}

static void CRTL_randi(UScriptVM *vm)
{
	int	max = vm->popIntArg();
	int	min = vm->popIntArg();
	vm->a = min + (rand()%(max - min + 1));
}

static void CRTL_abs(UScriptVM *vm)
{
	vm->fa = fabs(vm->popFloatArg());
}

static void CRTL_absi(UScriptVM *vm)
{
	vm->a = abs(vm->popIntArg());
}

static void CRTL_ceil(UScriptVM *vm)
{
	vm->fa = ceil(vm->popFloatArg());
}

static void CRTL_floor(UScriptVM *vm)
{
	vm->fa = floor(vm->popFloatArg());
}

static void CRTL_sign(UScriptVM *vm)
{
	vm->fa = vm->popFloatArg() > 0.0f?1.0f:-1.0f;
}

static void CRTL_signi(UScriptVM *vm)
{
	vm->a = vm->popIntArg() > 0?1:-1;
}

static void CRTL_max(UScriptVM *vm)
{
	float	b = vm->popFloatArg();
	float	a = vm->popFloatArg();
	vm->fa = a > b?a:b;
}

static void CRTL_maxi(UScriptVM *vm)
{
	int	b = vm->popIntArg();
	int	a = vm->popIntArg();
	vm->a = a > b?a:b;
}

static void CRTL_min(UScriptVM *vm)
{
	float	b = vm->popFloatArg();
	float	a = vm->popFloatArg();
	vm->fa = a < b?a:b;
}

static void CRTL_mini(UScriptVM *vm)
{
	int	b = vm->popIntArg();
	int	a = vm->popIntArg();
	vm->a = a < b?a:b;
}

static void CRTL_sqr(UScriptVM *vm)
{
	float	a = vm->popFloatArg();
	vm->fa = a*a;
}

static void CRTL_sqrt(UScriptVM *vm)
{
	float	a = vm->popFloatArg();
	if (a < 0.0f)
		vm->fa = sqrt(-a);
	else
		vm->fa = sqrt(a);
}

static void CRTL_hypot(UScriptVM *vm)
{
	float	b = vm->popFloatArg();
	float	a = vm->popFloatArg();
	vm->fa = sqrt(a*a + b*b);
}

static void CRTL_dumpInt(UScriptVM *vm)
{
	printf("%i\n", vm->popIntArg());
}

static void CRTL_dumpFloat(UScriptVM *vm)
{
	printf("%f\n", vm->popFloatArg());
}

static void CRTL_dumpString(UScriptVM *vm)
{
	printf("%s\n", vm->popStringArg());
}

static void CRTL_dumpPtr(UScriptVM *vm)
{
	printf("%lx\n", (long)vm->popPointerArg());
}

//Globals
void usInitNativeFunctions()
{
	usRegisterNativeFunc("itof", "i", CRTL_itof, usFloat); 
	usRegisterNativeFunc("ftoi", "f", CRTL_ftoi, usInteger); 
	usRegisterNativeFunc("sin", "f", CRTL_sin, usFloat); 
	usRegisterNativeFunc("cos", "f", CRTL_cos, usFloat); 
	usRegisterNativeFunc("tan", "f", CRTL_tan, usFloat); 
	usRegisterNativeFunc("asin", "f", CRTL_asin, usFloat); 
	usRegisterNativeFunc("acos", "f", CRTL_acos, usFloat); 
	usRegisterNativeFunc("atan", "f", CRTL_atan, usFloat); 
	usRegisterNativeFunc("rand", "", CRTL_rand, usFloat); 
	usRegisterNativeFunc("randi", "ii", CRTL_randi, usInteger); 
	usRegisterNativeFunc("abs", "f", CRTL_abs, usFloat); 
	usRegisterNativeFunc("absi", "i", CRTL_absi, usInteger); 
	usRegisterNativeFunc("ceil", "f", CRTL_ceil, usFloat); 
	usRegisterNativeFunc("floor", "f", CRTL_floor, usFloat); 
	usRegisterNativeFunc("sign", "f", CRTL_sign, usFloat); 
	usRegisterNativeFunc("signi", "i", CRTL_signi, usInteger); 
	usRegisterNativeFunc("max", "ff", CRTL_max, usFloat); 
	usRegisterNativeFunc("maxi", "ii", CRTL_maxi, usInteger); 
	usRegisterNativeFunc("min", "ff", CRTL_min, usFloat); 
	usRegisterNativeFunc("mini", "ii", CRTL_mini, usInteger);
	usRegisterNativeFunc("sqr", "f", CRTL_sqr, usFloat);
	usRegisterNativeFunc("sqrt", "f", CRTL_sqrt, usFloat);
	usRegisterNativeFunc("hypot", "ff", CRTL_hypot, usFloat);
	
	usRegisterNativeFunc("dumpInt", "i", CRTL_dumpInt, NULL); 
	usRegisterNativeFunc("dumpFloat", "f", CRTL_dumpFloat, NULL); 
	usRegisterNativeFunc("dumpString", "s", CRTL_dumpString, NULL); 
	usRegisterNativeFunc("dumpHandle", "h", CRTL_dumpPtr, NULL); 
}

void usShutdownNativeFunctions()
{
	usCreateFunctionArray();
	for (uint i=0;i<funcs;i++)
		delete func[i];
	free(func);
}

static void usAddNativeFuncArg(UScriptNativeFunc *func, UScriptType *type)
{
	func->arg = (UScriptNativeFuncArg*)realloc(func->arg,
		sizeof(UScriptNativeFuncArg)*(func->args + 1));
	func->arg[func->args++].type = type;
}

UScriptNativeFunc *usRegisterNativeFunc(String name, String args,
	UScriptNativeFuncProc proc, UScriptType *type)
{
	UScriptNativeFunc	*fnc = new UScriptNativeFunc(name, proc, type);
	for (uint i=0;i<strlen(args);i++)
	{
		UScriptType	*type = NULL;
		if (args[i] == 'i')
			type = usInteger;
		else if (args[i] == 'f')
			type = usFloat;
		else if (args[i] == 'h')
			type = usHandle;
		else if (args[i] == 's')
			type = usString;
		else if (args[i] == '{')
		{
			char	buff[256];
			int	l = 0;
			for (i++;args[i]&&args[i] != '}';i++,l++)
				buff[l] = args[i];
			buff[l] = 0;
			type = usFindType(buff);
		}
		if (type)
			usAddNativeFuncArg(fnc, type);
	}

	fnc->next = funcStack;
	funcStack = fnc;
	
	fnc->index = funcs++;
	
	return fnc;
}

UScriptNativeFunc *usFindNativeFunc(String name)
{
	for (uint i=0;i<funcs;i++)
		if (!strcmp(name, func[i]->name))
			return func[i];
	return NULL;
}

void usCreateFunctionArray()
{
	if (func)
		free(func);
	func = (UScriptNativeFunc**)malloc(sizeof(UScriptNativeFunc*)*funcs);
	for (UScriptNativeFunc *fnc=funcStack;fnc;fnc=fnc->next)
		func[fnc->index] = fnc;
}

void usCallNativeFunction(uint index, UScriptVM *vm)
{
	func[index]->proc(vm);
}

