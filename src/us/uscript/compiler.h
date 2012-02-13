////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  UndeadScript Game Scripting Engine                                        //
//  by Kostas "Bad Sector" Michalopoulos of Slashstone                        //
//                                                                            //
//  File name:    compiler.h                                                  //
//  Description:  UScriptCompiler class                                       //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef __USCRIPT_COMPILER_H_INCLUDED__
#define __USCRIPT_COMPILER_H_INCLUDED__

struct UScriptIdentifier
{
	String			name;
	UScriptType		*type;
	uint			address;
	UScriptIdentifier	*next;
};

struct UScriptFunctionArg
{
	String			name;
	UScriptType		*type;
};

struct UScriptFunction
{
	String			name;
	UScriptType		*type;
	uint			address;
	UScriptFunctionArg	*arg;
	uint			args;
	UScriptFunction		*next;
};

struct UScriptStaticString
{
	String			string;
	uint			address;
	UScriptStaticString	*next;
};

class UScriptCompiler
{
	UScript			*script;
	UScriptByteCode		*bc;
	String			code;
	uint			codeLen;
	uint			head;
	bool			error;
	String			errorString;
	String			lastToken;
	UScriptIdentifier	*identifiers;
	uint			nextIdentifierAddress;
	UScriptFunction		*functions;
	UScriptStaticString	*staticStrings;
	
	void setError(String errorString, ...);
	
	public:
	UScriptCompiler(UScript *script);
	~UScriptCompiler();
	
	void registerIdentifier(String name, UScriptType *type);
	UScriptIdentifier *findIdentifier(String name);
	
	UScriptFunction *registerFunction(String name, UScriptType *type,
		int address);
	UScriptFunction *findFunction(String name);
	
	void registerStaticString(String string, uint refAddr);
	
	String getToken();
	String getString();
	void skipWhitespace();
	
	bool compatibleTypes(UScriptType *wanted, UScriptType *got);
	
	void parseFunctionCall(UScriptType *&type, bool toplevel);
	
	void parseElement(UScriptType *&type);
	void parseParentheses(UScriptType *&type);
	void parseMulDivMod(UScriptType *&type);
	void parsePlusMinus(UScriptType *&type);
	void parseComparisons(UScriptType *&type);
	void parseAndOr(UScriptType *&type);
	void parseExpression(UScriptType *&type);
	
	void parseIfStructure();
	void parseWhileStructure();
	
	void parseAssignment();	
	void parseVariableDeclaration(UScriptType *type);
	void parseFunctionDeclaration(UScriptType *type);
	void parseDeclaration(UScriptType *type);
	
	void compileCommand();
	void compileCommandSet();
	
	bool compile();
};

#endif

