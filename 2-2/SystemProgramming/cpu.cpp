#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <bitset>
#include <fstream>

using namespace std;

//자릿수에 맞춰서 0을 채워주는 함수
string fill0(string str, int n)
{
	int len = n - str.length();
	string temp = "";

	for (int i = 0; i < len; i++) temp += "0";

	str = temp + str;

	return str;
}

//16진수 문자열을 정수로 바꿔줌
int hexToint(string s)
{
	unsigned int i;
	std::istringstream iss(s);
	iss >> hex >> i;

	return i;
}

//정수를 16진수 문자열로 바꿔줌
string intTohex(int i)
{
	std::ostringstream ss;
	ss << std::hex << i;
	std::string result = ss.str();

	return result;
}

// //2진수 문자열을 16진수 문자열로 바꿔줌
// string binTohex(string bin)
// {
// 	int dec = 0;
// 	string temp;
// 	string hex = "";
// 	for(int i = 0; i < 4; i++)
// 	{
// 		dec = 0;
// 		temp = bin.substr(4*i, 4);
// 		for(int j = 0; j < 4; j++) dec += temp[i] * pow(2, j);
// 		hex += intTohex(dec);
// 	}
// 	return hex;
// }

//16진수 문자열을 2진수 문자열로 바꿔줌
string hexTobin(string hex)
{
	string bin = "";
	int temp;

	for (int i = 0; i < hex.length(); i++)
	{
		string tmp = "";
		tmp += hex[i];
		temp = hexToint(tmp);

		if (temp >= 8)
		{
			bin += "1";
			temp -= 8;
		}
		else bin += "0";

		if (temp >= 4)
		{
			bin += "1";
			temp -= 4;
		}
		else bin += "0";

		if (temp >= 2)
		{
			bin += "1";
			temp -= 2;
		}
		else bin += "0";

		if (temp >= 1)
		{
			bin += "1";
			temp -= 1;
		}
		else bin += "0";
	}

	return bin;
}

class Memory
{
	string *mem;
	unsigned int mpt;
public:
	Memory();
	~Memory();
	string getMem();
	string getMem(unsigned int pt);
	void setMem(string hex);
	void setMem(string hex, unsigned int pt);
	void initMem(unsigned int pt = 0, string binary = "0");
	void initPtr();
	void setPtr(unsigned int pt);
	bool isPtrValid();
	void printMemBin(unsigned int pt1 = 0, unsigned int pt2 = 4096);
	void printMemHex(unsigned int pt1 = 0, unsigned int pt2 = 4096);
};

template <typename T>
class Register
{
	T v;
public:
	T getValue() { return v; }
	void setValue(T v) { this->v = v; }
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
	CPU16(Memory *m) : mem(m), alu(&ac), cu(&alu, mem, &ac, &ir) { pc.setValue(0); }
	void run(int len);
	void input(string str);
};

void CU::LDA(int v)
{
	ac->setValue(hexToint(mem->getMem(v)));
}

void CU::STA(int v)
{
	mem->setMem(intTohex(ac->getValue()), v);
}

void CU::ADD(int v)
{
	alu->add(v);
}

void CU::MUL(int v)
{
	alu->mul(v);
}

void CU::DIV(int v)
{
	alu->div(v);
}

void CU::MOD(int v)
{
	alu->mod(v);
}

void CU::SEA(int v)
{
	ac->setValue(v);
}

void CU::control(string ic)
{
	char opcode = ic[0];
	int operand = hexToint(ic.substr(1));

	switch (opcode)
	{
	case '0':
		LDA(operand);
		break;
	case '1':
		STA(operand);
		break;
	case '2':
		ADD(operand);
		break;
	case '3':
		MUL(operand);
		break;
	case '4':
		DIV(operand);
		break;
	case '5':
		MOD(operand);
		break;
	case 'f':
		SEA(operand);
		break;
	default:
		cout << "error : invalid opcode " << opcode << endl;
		exit(1);
		break;
	}
}

void CPU16::run(int len)
{
	pc.setValue(0);

	for (int i = 0; i < len; i++)
	{
		ir.setValue(mem->getMem(pc.getValue()));

		cu.control(ir.getValue());

		pc.setValue(pc.getValue() + 2);
	}
}

//읽어온 명령을 한 줄씩 메모리에 저장
void CPU16::input(string str)
{
	int npos;
	string opcode, operand;

	npos = str.find(' ');
	operand = fill0(str.substr(npos + 1), 3);
	str.erase(npos);
	opcode = str;

	if (opcode == "lda") opcode = "0";
	else if (opcode == "sta") opcode = "1";
	else if (opcode == "add") opcode = "2";
	else if (opcode == "mul") opcode = "3";
	else if (opcode == "div") opcode = "4";
	else if (opcode == "mod") opcode = "5";
	else if (opcode == "sea") opcode = "f";

	str = opcode + operand;

	//pc값에 따라 순차적으로 메모리 저장
	mem->setMem(str, pc.getValue());
	pc.setValue(pc.getValue() + 2);
}

string Memory::getMem()
{
	if (isPtrValid()) return mem[mpt] + mem[mpt + 1];
	else
	{
		cout << "error : invalid memory asess" << endl;
		exit(1);
	}
}

string Memory::getMem(unsigned int pt)
{
	setPtr(pt);
	if (isPtrValid()) return mem[mpt] + mem[mpt + 1];
	else
	{
		cout << "error : invalid memory asess" << endl;
		exit(1);
	}
}

void Memory::setMem(string hex)
{
	if (isPtrValid())
	{
		hex = fill0(hex, 4);
		mem[mpt] = hex.substr(0, 2);
		mem[mpt] = hex.substr(2, 2);
	}
	else
	{
		cout << "error : invalid memory asess" << endl;
		exit(1);
	}

}

void Memory::setMem(string hex, unsigned int pt)
{
	setPtr(pt);
	if (isPtrValid())
	{
		hex = fill0(hex, 4);
		mem[pt] = hex.substr(0, 2);
		mem[pt+1] = hex.substr(2, 2);
	}
	else
	{
		cout << "error : invalid memory asess" << endl;
		exit(1);
	}

}

void Memory::initMem(unsigned int pt, string binary)
{
	for (int i = pt; i < 4096; i++) mem[i] = binary;
}

void Memory::initPtr()
{
	mpt = 0;
}

void Memory::setPtr(unsigned int pt)
{
	mpt = pt;
}

bool Memory::isPtrValid()
{
	if (0 <= mpt && mpt <= 4096) return 1;
	else 0;
}

void Memory::printMemBin(unsigned int pt1, unsigned int pt2)
{
	for (int i = pt1; i <= pt2; i++) 
		cout << hexTobin(mem[i]) << endl;
}

void Memory::printMemHex(unsigned int pt1, unsigned int pt2)
{
	for (int i = pt1; i <= pt2; i++) cout << mem[i] << endl;
}

Memory::Memory()
{
	mem = new string[4096];
	//메모리 전체 0으로 초기화
	initMem();

	//메모리 포인터 시작주소(0)으로 초기화
	initPtr();
}

Memory::~Memory()
{
	delete[] mem;
}

int main()
{
	string str;
	int lineCnt = 0; // 명령어가 총 몇 줄인지 count
	ifstream readFile;
	Memory mem;
	CPU16 cpu(&mem);

	readFile.open("program.txt");

	//초기 주소부터 명령어 파일로부터 각 메모리에 저장
	if (readFile.is_open())
	{
		while (!readFile.eof())
		{
			getline(readFile, str);
			cpu.input(str);

			lineCnt++;
		}
	}

	readFile.close();

	cout << "명령어 저장 후 메모리 상태" << endl;
	mem.printMemBin(0, 10);

	cpu.run(lineCnt);

	cout << "명령어 실행 후 메모리 상태" << endl;
	mem.printMemBin(0, 20);

	return 0;
}