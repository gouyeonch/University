#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <bitset>
#include <fstream>

using namespace std;

class Memory
{
	string *mem;
	static unsigned int mpt;
public:
	Memory();
	~Memory();
	string getMem(unsigned int pt = mpt);
	void setMem(string hex, unsigned int pt = mpt);
	void initMem(unsigned int pt = 0, string binary = "0");
	void initPtr();
	void setPtr(unsigned int pt);
	bool isPtrValid();
	void printMemBin(unsigned int pt1 = 0, unsigned int pt2 = 0);
	void printMemHex(unsigned int pt1 = 0, unsigned int pt2 = 0);
};

template <typename T>
class Register
{
	T v;
public:
	T getValue();
	void setValue(T v);
};

class AC : public Register<int>
{

};

class PC : public Register<int>
{

};

class IR : public Register<string>
{

};

class ALU
{
	AC *ac;
public:
	ALU(AC *c) : ac(c) {}
	void add(int v) { ac->setValue(ac->getValue() + v); }
	void mul(int v) { ac->setValue(ac->getValue() * v); }
	void div(int v) { ac->setValue(ac->getValue() / v); }
	void mod(int v) { ac->setValue(ac->getValue() % v); }
};

class CU
{
	ALU *alu;
	Memory *mem;
	AC *ac;
	IR *ir;
public:
	CU(ALU *a, Memory *m, AC *c, IR *i) : alu(a), mem(m), ac(c), ir(i) {}

	void LDA(int v);
	void STA(int v);
	void ADD(int v);
	void MUL(int v);
	void DIV(int v);
	void MOD(int v);
	void SEA(int v);

	void control(string ic);
};

class CPU16
{
	Memory *mem;
	ALU alu;
	CU cu;
	AC ac;
	PC pc;
	IR ir;
public:
	CPU16(Memory *m) : mem(m), alu(&ac), cu(&alu, mem, &ac, &ir) {}
	void run(int len);
	void input(string str, int pt);
};
