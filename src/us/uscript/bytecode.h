////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  UndeadScript Game Scripting Engine                                        //
//  by Kostas "Bad Sector" Michalopoulos of Slashstone                        //
//                                                                            //
//  File name:    bytecode.h                                                  //
//  Description:  UScriptByteCode class                                       //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef __USCRIPT_SCRIPTBYTECODE_H_INCLUDED__
#define __USCRIPT_SCRIPTBYTECODE_H_INCLUDED__

enum UScriptBC
{
	BC_NOP=0,
	BC_NULLA,
	BC_SETA,
	BC_SETFA,
	BC_SETSS,
	BC_PUSHA,
	BC_PUSHFA,
	BC_PUSHPA,
	BC_POPB,
	BC_POPFB,
	BC_POPPB,
	
	BC_LOADA,
	BC_LOADFA,
	BC_LOADPA,
	BC_SAVEA,
	BC_SAVEFA,
	BC_SAVEPA,
	
	BC_ADD,
	BC_SUB,
	BC_MUL,
	BC_DIV,
	BC_MOD,
	BC_FADD,
	BC_FSUB,
	BC_FMUL,
	BC_FDIV,
	BC_FMOD,
	
	BC_AND,
	BC_OR,
	BC_EQ,
	BC_NEQ,
	BC_GREAT,
	BC_LESS,
	BC_GREATEQ,
	BC_LESSEQ,
	BC_FEQ,
	BC_FNEQ,
	BC_FGREAT,
	BC_FLESS,
	BC_FGREATEQ,
	BC_FLESSEQ,
	BC_PEQ,
	BC_PNEQ,

	BC_ARGPUSH,
	BC_ARGPUSHF,
	BC_ARGPUSHP,
	BC_ARGPOP,
	BC_ARGPOPF,
	BC_ARGPOPP,
	
	BC_CALL,
	BC_RET,
	BC_NCALL,
	BC_JUMP,
	BC_JUMPIF,
	BC_JUMPIFNOT,
	
	BC_STOP
};

class UScriptByteCode
{
	friend class UScriptVM;
	friend class UScriptCompiler;
	
	unsigned char	*bc;		// Byte Code
	uint		bcl;		// Byte Code Length
	uint		dataSize;
	uint		*ssTable;	// static string table
	uint		sstSize;	// static string table size
	
	void makeRoom(uint newSize);
	
	public:
	UScriptByteCode();
	~UScriptByteCode();
	
	uint length();
		
	void putByte(uint addr, unsigned char b);
	void putInt(uint addr, int i);
	void putFloat(uint addr, float f);
	
	void addByte(unsigned char b);
	void addInt(int i);
	void addFloat(float f);
	
	unsigned char getByte(uint addr);
	int getInt(uint addr);
	float getFloat(uint addr);

	String getSS(uint index);	
};

#endif

