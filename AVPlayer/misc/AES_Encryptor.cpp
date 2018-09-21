#include "AES_Encryptor.h"
#include <fstream>
using namespace std;

AesEncryptor::AesEncryptor(unsigned char* key)
{
	m_pEncryptor = new AES(key);
}


AesEncryptor::~AesEncryptor(void)
{
	delete m_pEncryptor;
}

void AesEncryptor::Byte2Hex(const unsigned char* src, int len, char* dest) {
	for (int i = 0; i<len; ++i) {
		sprintf_s(dest + i * 2, 3, "%02X", src[i]);
	}
}

void AesEncryptor::Hex2Byte(const char* src, int len, unsigned char* dest) {
	int length = len / 2;
	for (int i = 0; i<length; ++i) {
		dest[i] = Char2Int(src[i * 2]) * 16 + Char2Int(src[i * 2 + 1]);
	}
}

int AesEncryptor::Char2Int(char c) {
	if ('0' <= c && c <= '9') {
		return (c - '0');
	}
	else if ('a' <= c && c <= 'f') {
		return (c - 'a' + 10);
	}
	else if ('A' <= c && c <= 'F') {
		return (c - 'A' + 10);
	}
	return -1;
}

string AesEncryptor::EncryptString(string strInfor) {
	int nLength = strInfor.length();
	int spaceLength = 16 - (nLength % 16);
	unsigned char* pBuffer = new unsigned char[nLength + spaceLength];
	ZeroMemory(pBuffer, nLength + spaceLength);
	memcpy_s(pBuffer, nLength + spaceLength, strInfor.data(), nLength);
	m_pEncryptor->Cipher(pBuffer, nLength + spaceLength);

	// ������Ҫ�ѵõ����ַ�����ת����ʮ�������ַ��� 
	char* pOut = new char[2 * (nLength + spaceLength) + 1];
	ZeroMemory(pOut, 2 * (nLength + spaceLength) + 1);
	Byte2Hex(pBuffer, nLength + spaceLength, pOut);
	pOut[2 * (nLength + spaceLength)] = '\0';

	string retValue(pOut);
	delete[] pBuffer;
	delete[] pOut;
	return retValue;
}

string AesEncryptor::DecryptString(string strMessage) {
	int nLength = strMessage.length() / 2;
	unsigned char* pBuffer = new unsigned char[nLength];
	ZeroMemory(pBuffer, nLength);
	Hex2Byte(strMessage.c_str(), strMessage.length(), pBuffer);

	m_pEncryptor->InvCipher(pBuffer, nLength);
	string retValue((char*)pBuffer);
	delete[] pBuffer;
	return retValue;
}

void AesEncryptor::EncryptTxtFile(const char* inputFileName, const char* outputFileName) {
	ifstream ifs;

	// Open file:
	ifs.open(inputFileName);
	if (!ifs) {
		return;
	}

	// Read config data:
	string strInfor;
	string strLine;
	while (!ifs.eof()) {
		char temp[1024];
		memset(temp, '\0', 1024);
		ifs.read(temp, 1000);
		strInfor += temp;
	}
	ifs.close();

	// Encrypt
	strLine = EncryptString(strInfor);

	// Writefile 
	ofstream ofs;
	ofs.open(outputFileName);
	if (!ofs) {
		return;
	}
	ofs << strLine;
	ofs.close();
}

void AesEncryptor::DecryptTxtFile(const char* inputFile, const char* outputFile) {
	ifstream ifs;

	// Open file:
	ifs.open(inputFile);
	if (!ifs) {
		return;
	}

	// Read config data:
	string strInfor;
	string strLine;
	while (!ifs.eof()) {
		char temp[1024];
		memset(temp, '\0', 1024);
		ifs.read(temp, 1000);
		strInfor += temp;
	}
	ifs.close();

	// Encrypt
	strLine = DecryptString(strInfor);

	// Writefile 
	ofstream ofs;
	ofs.open(outputFile);
	if (!ofs) {
		return;
	}
	ofs << strLine;
	ofs.close();
}