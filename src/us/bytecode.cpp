////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  UndeadScript Game Scripting Engine                                        //
//  by Kostas "Bad Sector" Michalopoulos of Slashstone                        //
//                                                                            //
//  File name:    bytecode.cpp                                                //
//  Description:  UScriptByteCode class                                       //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <uscript/uscript.h>

//UScriptByteCode
UScriptByteCode::UScriptByteCode()
{
	bc = NULL;
	bcl = 0;
	dataSize = 0;
	ssTable = NULL;
	sstSize = 0;
}

UScriptByteCode::~UScriptByteCode()
{
	if (ssTable)
		free(ssTable);
	if (bc)
		free(bc);
}

void UScriptByteCode::makeRoom(uint newSize)
{
	if (newSize > bcl)
	{
		bc = (unsigned char*)realloc(bc, newSize);
		bcl = newSize;
	}
}

uint UScriptByteCode::length()
{
	return bcl;
}

void UScriptByteCode::putByte(uint addr, unsigned char b)
{
	makeRoom(addr + 1);
	bc[addr] = b;
}

void UScriptByteCode::putInt(uint addr, int i)
{
	uint	u = (uint)i;

	makeRoom(addr + 4);
	bc[addr++] = u & 0xFF;
	bc[addr++] = (u >> 8) & 0xFF;
	bc[addr++] = (u >> 16) & 0xFF;
	bc[addr] = (u >> 24) & 0xFF;
}

void UScriptByteCode::putFloat(uint addr, float f)
{
	makeRoom(addr + 4);
	bc[addr++] = ((unsigned char*)(&f))[0];
	bc[addr++] = ((unsigned char*)(&f))[1];
	bc[addr++] = ((unsigned char*)(&f))[2];
	bc[addr] = ((unsigned char*)(&f))[3];
}

void UScriptByteCode::addByte(unsigned char b)
{
	putByte(bcl, b);
}

void UScriptByteCode::addInt(int i)
{
	putInt(bcl, i);
}

void UScriptByteCode::addFloat(float f)
{
	putFloat(bcl, f);
}

unsigned char UScriptByteCode::getByte(uint addr)
{
	return bc[addr];
}

int UScriptByteCode::getInt(uint addr)
{
	return (int)(bc[addr]|
		(bc[addr + 1] << 8)|
		(bc[addr + 2] << 16)|
		(bc[addr + 3] << 24));
}

float UScriptByteCode::getFloat(uint addr)
{
	return *((float*)&bc[addr]);
}

String UScriptByteCode::getSS(uint index)
{
	if (index >= sstSize)
		return "";
	return (String)&bc[ssTable[index]];
}

