////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  UndeadScript Game Scripting Engine                                        //
//  by Kostas "Bad Sector" Michalopoulos of Slashstone                        //
//                                                                            //
//  File name:    compiler.cpp                                                //
//  Description:  UScriptCompiler class                                       //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <stdarg.h>
#include <uscript/uscript.h>

using namespace UScriptUtils;

struct FuncDecArgument
{
	String		name;
	UScriptType	*type;
};

//UScriptCompiler
UScriptCompiler::UScriptCompiler(UScript *script)
{
	this->script = script;
	code = script->code;
	codeLen = strlen(script->code);
	if (script->bc)
		delete script->bc;
	bc = new UScriptByteCode();
	script->bc = bc;
	identifiers = NULL;
	functions = NULL;
	error = false;
	errorString = NULL;
	staticStrings = NULL;
}

UScriptCompiler::~UScriptCompiler()
{
	UScriptIdentifier	*nexti;
	while (identifiers)
	{
		nexti = identifiers->next;
		free(identifiers->name);
		delete identifiers;
		identifiers = nexti;
	}
	
	UScriptFunction		*nextf;
	while (functions)
	{
		nextf = functions->next;
		free(functions->name);
		if (functions->arg)
		{
			for (uint i=0;i<functions->args;i++)
				free(functions->arg[i].name);
			free(functions->arg);
		}
		delete functions;
		functions = nextf;
	}
	
	UScriptStaticString	*nexts;
	while (staticStrings)
	{
		nexts = staticStrings->next;
		free(staticStrings->string);
		delete staticStrings;
		staticStrings = nexts;
	}
	
	if (errorString)
		free(errorString);
}

void UScriptCompiler::setError(String errorString, ...)
{
	char	buffer[16384];
	va_list	ap;
	
	if (error)
		return;
	
	va_start(ap, errorString);
	vsprintf(buffer, errorString, ap);
	va_end(ap);
	
	error = true;
	if (this->errorString)
		free(this->errorString);
	this->errorString = strdup(buffer);
}

void UScriptCompiler::registerIdentifier(String name, UScriptType *type)
{
	UScriptIdentifier	*idf = new UScriptIdentifier;
	idf->name = strdup(name);
	idf->type = type;
	idf->address = nextIdentifierAddress;
	idf->next = identifiers;
	identifiers = idf;
	nextIdentifierAddress += type->size;
}

UScriptIdentifier *UScriptCompiler::findIdentifier(String name)
{
	for (UScriptIdentifier *idf=identifiers;idf;idf = idf->next)
		if (!strcmp(idf->name, name))
			return idf;
	return NULL;
}

UScriptFunction *UScriptCompiler::registerFunction(String name,
	UScriptType *type, int address)
{
	UScriptFunction		*func = new UScriptFunction;
	func->name = strdup(name);
	func->type = type;
	func->address = address;
	func->arg = NULL;
	func->args = 0;
	func->next = functions;
	functions = func;
	
	return func;
}

UScriptFunction *UScriptCompiler::findFunction(String name)
{
	for (UScriptFunction *func=functions;func;func = func->next)
		if (!strcmp(func->name, name))
			return func;
	return NULL;
}

void UScriptCompiler::registerStaticString(String string, uint refAddr)
{
	UScriptStaticString	*sstr = new UScriptStaticString;
	sstr->string = strdup(string);
	sstr->address = staticStrings?(staticStrings->address + 1):0;
	sstr->next = staticStrings;
	staticStrings = sstr;
	bc->sstSize++;
}

String UScriptCompiler::getToken()
{
	String	token = NULL;
	uint	tokenLen = 0;
	
	skipWhitespace();
	
	while (head < codeLen && !isspace(code[head]))
	{
		if (code[head] != '.' && code[head] != '_' &&
			ispunct(code[head]))
			break;
		token = (String)realloc(token, tokenLen + 1);
		token[tokenLen++] = code[head++];
	}
	
	token = (String)realloc(token, tokenLen + 1);
	token[tokenLen] = 0;
	
	skipWhitespace();

	return lastToken = token;
}

String UScriptCompiler::getString()
{
	String	token = NULL;
	uint	tokenLen = 0;
	char	ch = code[head++];
	
	while (head < codeLen && code[head] != ch)
	{
		token = (String)realloc(token, tokenLen + 1);
		token[tokenLen++] = code[head++];
	}
	
	if (code[head] == ch)
		head++;
	
	token = (String)realloc(token, tokenLen + 1);
	token[tokenLen] = 0;
	
	return lastToken = token;
}

void UScriptCompiler::skipWhitespace()
{
	while (head < codeLen && (isspace(code[head]) || code[head] == '#'))
	{
		if (code[head] == '#')
		{
			while (head < codeLen && !(code[head] == '\r' ||
				code[head] == '\n'))
				head++;
		}
		head++;
	}
}

bool UScriptCompiler::compatibleTypes(UScriptType *wanted, UScriptType *got)
{
	if (!wanted)
		return true;
	
	if (!got)
		return false;

	if ((wanted->base == USB_STRUCT && got->base == USB_STRUCT) ||
		(wanted->base == USB_HANDLE && got->base == USB_HANDLE))
		return !strcmp(wanted->name, got->name); 
	else
		return wanted->base == got->base;
}

void UScriptCompiler::parseFunctionCall(UScriptType *&type, bool toplevel)
{
	// Native function call?
	UScriptNativeFunc	*nfunc = usFindNativeFunc(lastToken);
	if (nfunc)
	{
		if (!nfunc->type && !toplevel)
		{
			setError("The function '%s' returns nothing, but it is",
				" used in an expression.", lastToken);
			return;
		}
		
		if (!compatibleTypes(type, nfunc->type))
		{
			setError("The function '%s' returns a value of type '%s"
				"', but a value of type '%s' is wanted.",
				lastToken, USTYPENAME(nfunc->type),
				USTYPENAME(type));
			return;
		}

		type = nfunc->type;

		for (uint i=0;i<nfunc->args;i++)
		{
			UScriptType	*argType = nfunc->arg[i].type;
			parseExpression(argType);
			switch (argType->base)
			{
				case USB_INTEGER:
					bc->addByte(BC_ARGPUSH);
					break;
				case USB_FLOAT:
					bc->addByte(BC_ARGPUSHF);
					break;
				case USB_HANDLE:
				case USB_STRUCT:
				case USB_STRING:
					bc->addByte(BC_ARGPUSHP);
					break;
			}
			skipWhitespace();
			if (i == nfunc->args - 1 && code[head] != ')')
			{
				setError("Function call closing parenthesis "
					"(')') for function '%s' was expected "
					"here, but '%c' was found.",
					nfunc->name, code[head]);
				return;
			}
			if (i < nfunc->args - 1 && code[head] != ',')
			{
				setError("Argument separation comma (',') for "
					"function '%s' was expected here, but "
					"'%c' was found.", nfunc->name,
					code[head]);
				return;
			}
			head++;
			skipWhitespace();
		}
		
		if (!nfunc->args)
		{
			if (code[head] != ')')
			{
				setError("Function call closing parenthesis "
					"(')') for function '%s' was expected "
					"here, but '%c' was found.",
					nfunc->name, code[head]);
				return;
			}
			head++;
		}

		bc->addByte(BC_NCALL);
		bc->addInt(nfunc->index);
		
		return;
	}

	// Script function call?
	UScriptFunction		*func = findFunction(lastToken);
	if (func)
	{
		if (!func->type && !toplevel)
		{
			setError("The function '%s' returns nothing, but it is",
				" used in an expression.", lastToken);
			return;
		}
		
		if (!compatibleTypes(type, func->type))
		{
			setError("The function '%s' returns a value of type '%s"
				"', but a value of type '%s' is wanted.",
				lastToken, USTYPENAME(nfunc->type),
				USTYPENAME(type));
			return;
		}

		type = func->type;

		for (uint i=0;i<func->args;i++)
		{
			UScriptType	*argType = func->arg[i].type;
			parseExpression(argType);
			switch (argType->base)
			{
				case USB_INTEGER:
					bc->addByte(BC_ARGPUSH);
					break;
				case USB_FLOAT:
					bc->addByte(BC_ARGPUSHF);
					break;
				case USB_HANDLE:
				case USB_STRUCT:
				case USB_STRING:
					bc->addByte(BC_ARGPUSHP);
					break;
			}
			skipWhitespace();
			if (i == func->args - 1 && code[head] != ')')
			{
				setError("Function call closing parenthesis "
					"(')') for function '%s' was expected "
					"here, but '%c' was found.",
					func->name, code[head]);
				return;
			}
			if (i < func->args - 1 && code[head] != ',')
			{
				setError("Argument separation comma (',') for "
					"function '%s' was expected here, but "
					"'%c' was found.", func->name,
					code[head]);
				return;
			}
			head++;
			skipWhitespace();
		}
		
		if (!func->args)
		{
			if (code[head] != ')')
			{
				setError("Function call closing parenthesis "
					"(')') for function '%s' was expected "
					"here, but '%c' was found.",
					func->name, code[head]);
				return;
			}
			head++;
		}

		bc->addByte(BC_CALL);
		bc->addInt(func->address);
		
		return;
	}

	setError("'%s' is not a known function.", lastToken);
}

void UScriptCompiler::parseElement(UScriptType *&type)
{
	String	token;

	skipWhitespace();	
	if (code[head] == '"' || code[head] == '\'')
	{
		token = getString();
		skipWhitespace();
		
		if (type && type->base != USB_STRING)
		{
			setError("A value of type '%s' was expected, but the "
				"string '%s' given.", USTYPENAME(type), token);
			return;
		}

		type = usString;
		
		registerStaticString(token, bc->length());
		bc->addByte(BC_SETSS);
		bc->addInt(staticStrings->address);
		return;
	}

	token = getToken();
	if (!token[0])
	{
		if (ispunct(code[head]))
			setError("A syntactical error found in the code before "
				"the '%c' symbol.", code[head]);
		else
			setError("A syntactical error found in the code.");
		return;
	}
	
	if (!strcmp(token, "null"))
	{
		if (type && type->base != USB_HANDLE)
		{
			setError("Cannot use 'null' where the type '%s' is "
				"expected.", USTYPENAME(type));
			return;
		}
		bc->addByte(BC_NULLA);
		return;
	}
		
	UScriptConst		*sconst = script->findConst(token);
	if (!sconst)
		sconst = usFindConst(token);
	if (sconst)
	{
		if (!compatibleTypes(type, sconst->type))
		{
			setError("Constant '%s' is of type '%s', but a value of"
				" type '%s' was expected.", sconst->name,
				USTYPENAME(sconst->type), USTYPENAME(type));
			return;
		}
		type = sconst->type;
		switch (type->base)
		{
			case USB_INTEGER:
				bc->addByte(BC_SETA);
				bc->addInt(sconst->i);
				break;
			case USB_FLOAT:
				bc->addByte(BC_SETFA);
				bc->addFloat(sconst->f);
				break;
		}
		return;
	}
	
	UScriptIdentifier	*identifier = findIdentifier(token);
	if (identifier)
	{
		if (!compatibleTypes(type, identifier->type))
		{
			setError("Variable '%s' is of type '%s', but a value of"
				"type '%s' was expected.", identifier->name,
				USTYPENAME(identifier->type), USTYPENAME(type));
			return;
		}
		
		type = identifier->type;
		switch (type->base)
		{
			case USB_INTEGER:
				bc->addByte(BC_LOADA);
				break;
			case USB_FLOAT:
				bc->addByte(BC_LOADFA);
				break;
			case USB_HANDLE:
			case USB_STRUCT:
			case USB_STRING:
				bc->addByte(BC_LOADPA);
				break;
		}
		bc->addInt(identifier->address);
		
		return;
	}
	
	if (code[head] == '(')
	{
		head++;
		parseFunctionCall(type, false);
		return;
	}

	// check for number
	bool	number = true, real = false;
	for (int i=0;token[i];i++)
	{
		if (token[i] == '.')
		{
			if (real)
			{
				number = false;
				break;
			}
			real = true;
		}
		else
		{
			if (!isdigit(token[i]))
			{
				number = false;
				break;
			}
		}
	}
	
	if (number)
	{
		if (real && type && type->base != USB_FLOAT)
		{
			setError("A value of type '%s' was expected, but the "
				"real number '%s' given.", USTYPENAME(type),
				token);
			return;
		}
		else if (!real && type && type->base != USB_INTEGER)
		{
			setError("A value of type '%s' was expected, but the "
				"integer number '%s' given.", USTYPENAME(type),
				token);
			return;
		}
		if (real)
		{
			bc->addByte(BC_SETFA);
			bc->addFloat(atof(token));
		}
		else
		{
			bc->addByte(BC_SETA);
			bc->addInt(atoi(token));
		}
		return;
	}
	
	setError("I don't know what to do with '%s' - it is not a known "
		"variable or function. Is it spelled correctly?",
		token);
}

void UScriptCompiler::parseParentheses(UScriptType *&type)
{
	skipWhitespace();
	if (code[head] == '(')
	{
		head++;
		parseExpression(type);
		if (error)
			return;
		skipWhitespace();
		if (code[head] != ')')
		{
			setError("A closing parenthesis (')') was expected, but"
				" a '%c' found.", code[head]);
			return;
		}
		head++;
	}
	else
		parseElement(type);
}

void UScriptCompiler::parseMulDivMod(UScriptType *&type)
{
	parseParentheses(type);
	if (error)
		return;
	skipWhitespace();
	while (code[head] == '*' || code[head] == '/' || code[head] == '%')
	{
		if (!type)
			type = usInteger;
		if (type->base != USB_INTEGER && type->base != USB_FLOAT)
		{
			setError("The type '%s' cannot be used with the '%c'"
				" operator.", USTYPENAME(type), code[head]);
			return;
		}
		if (type->base == USB_INTEGER)
			bc->addByte(BC_PUSHA);
		else
			bc->addByte(BC_PUSHFA);
		switch (code[head++])
		{
			case '*':
			{
				parseParentheses(type);
				if (error)
					return;
				if (type->base == USB_INTEGER)
				{
					bc->addByte(BC_POPB);
					bc->addByte(BC_MUL);
				}
				else
				{
					bc->addByte(BC_POPFB);
					bc->addByte(BC_FMUL);
				}
				break;
			}
			case '/':
			{
				parseParentheses(type);
				if (error)
					return;
				if (type->base == USB_INTEGER)
				{
					bc->addByte(BC_POPB);
					bc->addByte(BC_DIV);
				}
				else
				{
					bc->addByte(BC_POPFB);
					bc->addByte(BC_FDIV);
				}
				break;
			}
			case '%':
			{
				parseParentheses(type);
				if (error)
					return;
				if (type->base == USB_INTEGER)
				{
					bc->addByte(BC_POPB);
					bc->addByte(BC_MOD);
				}
				else
				{
					bc->addByte(BC_POPFB);
					bc->addByte(BC_FMOD);
				}
				break;
			}
			default:
				setError("The '*', '/' or '%' symbols were "
					"expected, but the symbol '%c' found.",
					code[head]);
				return;
		}
		skipWhitespace();
	}
}

void UScriptCompiler::parsePlusMinus(UScriptType *&type)
{
	parseMulDivMod(type);
	if (error)
		return;
	skipWhitespace();
	while (code[head] == '+' || code[head] == '-')
	{
		if (!type)
			type = usInteger;
		if (type->base != USB_INTEGER && type->base != USB_FLOAT)
		{
			setError("The type '%s' cannot be used with the '%c'"
				" operator.", USTYPENAME(type), code[head]);
			return;
		}
		if (type->base == USB_INTEGER)
			bc->addByte(BC_PUSHA);
		else
			bc->addByte(BC_PUSHFA);
		switch (code[head++])
		{
			case '+':
			{
				parseMulDivMod(type);
				if (error)
					return;
				if (type->base == USB_INTEGER)
				{
					bc->addByte(BC_POPB);
					bc->addByte(BC_ADD);
				}
				else
				{
					bc->addByte(BC_POPFB);
					bc->addByte(BC_FADD);
				}
				break;
			}
			case '-':
			{
				parseMulDivMod(type);
				if (error)
					return;
				if (type->base == USB_INTEGER)
				{
					bc->addByte(BC_POPB);
					bc->addByte(BC_SUB);
				}
				else
				{
					bc->addByte(BC_POPFB);
					bc->addByte(BC_FSUB);
				}
				break;
			}
			default:
				setError("The '+' or '-' symbols were expected,"
					" but the symbol '%c' found.",
					code[head]);
				return;
		}
		skipWhitespace();
	}
}

void UScriptCompiler::parseComparisons(UScriptType *&type)
{
	parsePlusMinus(type);
	if (error)
		return;
	skipWhitespace();
	while (code[head] == '>' || code[head] == '<' || (code[head] == '=' &&
		code[head + 1] == '=') || (code[head] == '!' && code[head + 1]
		== '='))
	{
		if (!type)
			type = usInteger;
		if (!((code[head] == '=' && code[head + 1] == '=') ||
			(code[head] == '!' && code[head + 1] == '=')) &&
			type->base != USB_INTEGER && type->base != USB_FLOAT)
		{
			setError("The type '%s' cannot be used with comparison"
				" operators.", USTYPENAME(type), code[head]);
			return;
		}
		
		if (type->base == USB_INTEGER)
			bc->addByte(BC_PUSHA);
		else if (type->base == USB_FLOAT)
			bc->addByte(BC_PUSHFA);
		else
			bc->addByte(BC_PUSHPA);
		
		if (code[head] == '>' && code[head + 1] != '=')
		{
			head++;
			parsePlusMinus(type);
			if (error)
				return;
			if (type->base == USB_INTEGER)
			{
				bc->addByte(BC_POPB);
				bc->addByte(BC_GREAT);
			}
			else
			{
				bc->addByte(BC_POPFB);
				bc->addByte(BC_FGREAT);
			}
		}
		else if (code[head] == '<' && code[head + 1] != '=')
		{
			head++;
			parsePlusMinus(type);
			if (error)
				return;
			if (type->base == USB_INTEGER)
			{
				bc->addByte(BC_POPB);
				bc->addByte(BC_LESS);
			}
			else
			{
				bc->addByte(BC_POPFB);
				bc->addByte(BC_FLESS);
			}
		}
		else if (code[head] == '=' && code[head + 1] == '=')
		{
			head += 2;
			parsePlusMinus(type);
			if (error)
				return;
			if (type->base == USB_INTEGER)
			{
				bc->addByte(BC_POPB);
				bc->addByte(BC_EQ);
			}
			else if (type->base == USB_FLOAT)
			{
				bc->addByte(BC_POPFB);
				bc->addByte(BC_FEQ);
			}
			else
			{
				bc->addByte(BC_POPPB);
				bc->addByte(BC_PEQ);
			}
		}
		else if (code[head] == '!' && code[head + 1] == '=')
		{
			head += 2;
			parsePlusMinus(type);
			if (error)
				return;
			if (type->base == USB_INTEGER)
			{
				bc->addByte(BC_POPB);
				bc->addByte(BC_NEQ);
			}
			else if (type->base == USB_FLOAT)
			{
				bc->addByte(BC_POPFB);
				bc->addByte(BC_FNEQ);
			}
			else
			{
				bc->addByte(BC_POPPB);
				bc->addByte(BC_PNEQ);
			}
		}
		else if (code[head] == '>' && code[head + 1] == '=')
		{
			head += 2;
			parsePlusMinus(type);
			if (error)
				return;
			if (type->base == USB_INTEGER)
			{
				bc->addByte(BC_POPB);
				bc->addByte(BC_GREATEQ);
			}
			else
			{
				bc->addByte(BC_POPFB);
				bc->addByte(BC_FGREATEQ);
			}
		}
		else if (code[head] == '<' && code[head + 1] == '=')
		{
			head += 2;
			parsePlusMinus(type);
			if (error)
				return;
			if (type->base == USB_INTEGER)
			{
				bc->addByte(BC_POPB);
				bc->addByte(BC_LESSEQ);
			}
			else
			{
				bc->addByte(BC_POPFB);
				bc->addByte(BC_FLESSEQ);
			}
		}
		else
		{
			setError("A comparison operator was expected, but the"
				" symbol '%c' was found.", code[head]);
			return;
		}
		
		type = usInteger;
		
		skipWhitespace();
	}
}

void UScriptCompiler::parseAndOr(UScriptType *&type)
{
	parseComparisons(type);
}

void UScriptCompiler::parseExpression(UScriptType *&type)
{
	parseAndOr(type);
}

void UScriptCompiler::parseIfStructure()
{
	UScriptType	*type = NULL;
	
	if (code[head] != '(')
	{
		setError("Opening parenthesis missing after 'if'.");
		return;
	}
	head++;
	parseExpression(type);
	if (!type || type->base != USB_INTEGER)
	{
		setError("Only integer types can be used with the 'if' "
			"structure.");
		return;
	}
	skipWhitespace();
	if (code[head] != ')')
	{
		setError("Closing parenthesis missing after the expression in "
			"the 'if' structure.");
		return;
	}
	head++;
	
	bc->addByte(BC_JUMPIFNOT);
	uint	fix1 = bc->length();
	bc->addInt(0);
	
	compileCommand();
	
	uint	saveHead = head;
	if (!strcmp(getToken(), "else"))
	{
		bc->addByte(BC_JUMP);
		uint	fix2 = bc->length();
		bc->addInt(0);
		bc->putInt(fix1, bc->length());
		compileCommand();
		bc->putInt(fix2, bc->length());
	}
	else
	{
		bc->putInt(fix1, bc->length());
		head = saveHead;
	}
}

void UScriptCompiler::parseWhileStructure()
{
	UScriptType	*type = NULL;
	
	uint		exprAddress = bc->length();
	
	if (code[head] != '(')
	{
		setError("Opening parenthesis missing after 'while'.");
		return;
	}
	head++;
	parseExpression(type);
	if (!type || type->base != USB_INTEGER)
	{
		setError("Only integer types can be used with the 'while' "
			"structure.");
		return;
	}
	skipWhitespace();
	if (code[head] != ')')
	{
		setError("Closing parenthesis missing after the expression in "
			"the 'while' structure.");
		return;
	}
	head++;
	
	bc->addByte(BC_JUMPIFNOT);
	uint	fix1 = bc->length();
	bc->addInt(0);
	
	compileCommand();
	
	bc->addByte(BC_JUMP);
	bc->addInt(exprAddress);
	
	bc->putInt(fix1, bc->length()); 
}

void UScriptCompiler::parseAssignment()
{
	UScriptIdentifier	*idf = findIdentifier(lastToken);
	UScriptType		*type;
	head++;
	if (!idf)
	{
		if (!lastToken[0])
			setError("An empty string found before the assignment",
				" ('=') symbol. You must give an identifier if",
				" you want to assign something to something "
				"else.");
		setError("The '%s' identifier is unknown, therefore you cannot"
			" assign something to it.", lastToken); 
		return;
	}
	
	type = idf->type;
	parseExpression(type);
	if (error)
		return;
	
	switch (type->base)
	{
		case USB_INTEGER:
			bc->addByte(BC_SAVEA);
			break;
		case USB_FLOAT:
			bc->addByte(BC_SAVEFA);
			break;
		case USB_HANDLE:
		case USB_STRUCT:
		case USB_STRING:
			bc->addByte(BC_SAVEPA);
			break;
	}
	bc->addInt(idf->address);
}

void UScriptCompiler::parseVariableDeclaration(UScriptType *type)
{
	do
	{
		registerIdentifier(lastToken, type);
		skipWhitespace();
		if (code[head] == '=')
			parseAssignment();
		if (code[head] != ',')
			break;
		else
			head++;
		getToken();
	}
	while (head < codeLen);
}

void UScriptCompiler::parseFunctionDeclaration(UScriptType *type)
{
	Stack<FuncDecArgument>	args;
	Stack<int>		returnFixes;
	String			funcName = strdup(lastToken);
	UScriptFunction		*func = NULL;
	
	head++;	// skip '('
	
	
	/*
	while (head < codeLen)
	{
		//UScriptType	*argType = getToken();
		//if (!argType)
		{
			setError("Unknown type '%s' in the declaration of the "
				"function '%s'.", lastToken, funcName);
			free(funcName);
			return;
		}
	}
	*/

	skipWhitespace();
	if (code[head] != ')')
	{
		setError("The character ')' was expected after the argument "
			"list in the declaration of the function '%s'.",
			funcName);
		free(funcName);
		return;
	}
	head++; // skip ')'
	
	bc->addByte(BC_JUMP);
	uint	skipFuncFix = bc->length();
	bc->addInt(0);
	
	func = registerFunction(funcName, type, bc->length());
	
	// Write data save code 
	
	compileCommand();
	
	// Write data retrieve code
	
	
	bc->addByte(BC_RET);
	bc->putInt(skipFuncFix, bc->length());
	
	free(funcName);
	
	// "forget" functions declared inside this function
	while (functions)
	{
		UScriptFunction	*next = functions->next;
		if (functions == func)
			break;
		free(functions->name);
		if (functions->arg)
		{
			for (uint i=0;i<functions->args;i++)
				free(functions->arg[i].name);
			free(functions->arg);
		}
		functions = next;
	}
}

void UScriptCompiler::parseDeclaration(UScriptType *type)
{
	getToken();
	skipWhitespace();
	if (code[head] == '(')
		parseFunctionDeclaration(type);
	else
		parseVariableDeclaration(type);
}

void UScriptCompiler::compileCommand()
{
	String		token;
	UScriptType	*type;
	
	skipWhitespace();
	
	if (code[head] == '{')
	{
		head++;
		compileCommandSet();
		return;
	}
	
	token = getToken();
	
	if (!token[0] && !code[head])
		return;
	
	skipWhitespace();
	
	if (!strcmp(token, "if"))
	{
		parseIfStructure();
		return;
	}
	else if (!strcmp(token, "while"))
	{
		parseWhileStructure();
		return;
	}
	else if (!strcmp(token, "void"))
	{
		getToken();
		skipWhitespace();
		if (code[head] != '(')
		{
			setError("The character '(' was expected after the "
				"function name in the declaration of the '%s' "
				"function.", lastToken);
			return;
		}
		parseFunctionDeclaration(NULL);
		return;
	}
	
	type = usFindType(token);
	if (type)
		parseDeclaration(type);
	else if (code[head] == '=')
		parseAssignment();
	else if (code[head] == '(')
	{
		head++;
		type = NULL;
		parseFunctionCall(type, true);
	}
	else
	{
		if (token[0])
			setError("I don't know what to do with the symbol '%c'"
				" after the '%s' word.", code[head], token);
		else
			setError("I don't know what to do with the symbol '%c'"
				" at the beginning of the command.",
				code[head]);
	}
	
	skipWhitespace();
	if (code[head] == ';')
		head++;
}

void UScriptCompiler::compileCommandSet()
{
	while (head < codeLen)
	{
		skipWhitespace();
		if (code[head] == '}')
		{
			head++;
			break;
		}
		compileCommand();
		if (error)
			break;
	}
}

bool UScriptCompiler::compile()
{
	head = 0;
	nextIdentifierAddress = 0;
	
	usCreateFunctionArray();
	
	compileCommandSet();
	bc->addByte(BC_STOP);
	
	if (!error)
	{
		bc->dataSize = nextIdentifierAddress;
		
		// build static string table
		bc->ssTable = (uint*)malloc(sizeof(uint)*bc->sstSize);
		for (UScriptStaticString *sstr=staticStrings;sstr;
			sstr=sstr->next)
		{
			bc->ssTable[sstr->address] = bc->length();
			for (char *s=sstr->string;*s;s++)
				bc->addByte(*s);
			bc->addByte(0);
		}
	}
	
	if (error)
		printf("error: %s\n", errorString);
	
	return !error;
}

