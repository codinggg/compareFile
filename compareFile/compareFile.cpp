#pragma comment(lib, "cryptlib.lib")
#pragma warning(disable: 4996)

#include <iostream>
#include "MMFIODef.h"
#include <cstdlib>
#include <cstdio>

#include "cryptopp/sha3.h"

using namespace std;
using namespace CryptoPP;

#define _DEBUG	

// #ifdef _DEBUG
// #define new DEBUG_NEW
// #endif

#define SOURCEFILEPAHT		"E:\\log.txt"		// july1.nyf
#define DESTINATIONPATH     "E:\\log1.txt"	


void saveFile(string st1, string st2);


int main()
{
//	goto copy;

	// SOURCE create file
	HANDLE hofile = CreateFile(SOURCEFILEPAHT, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (INVALID_HANDLE_VALUE == hofile)
	{
		cout << endl;
	}
	LARGE_INTEGER lInteger;
	DWORD hfSize = GetFileSizeEx(hofile, &lInteger);

	CloseHandle(hofile);

	// DESTINATION if not exist create file, otherwise set file size is equal source file
	HANDLE hFile = CreateFile(DESTINATIONPATH, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	suint64 nLength = lInteger.LowPart;

	LONG nLengthHigh = (nLength >> 32);
	DWORD dwPtrLow = SetFilePointer(hFile, (LONG)(nLength & 0xFFFFFFFF),
		&nLengthHigh, FILE_BEGIN);
	SetEndOfFile(hFile);
	CloseHandle(hFile);


copy:

	CWinMMFIO mmf, mmfwrite;
	bool bRet = mmf.Open(SOURCEFILEPAHT, OPN_READWRITE);
	mmf.Seek(0, SP_BEGIN);
	if (!bRet)return false;
	suint64 fileLength1 = mmf.GetLength();

	bRet = mmfwrite.Open(DESTINATIONPATH, OPN_READWRITE);
	if (!bRet)	// file is not exist
	{
		HANDLE hFile = CreateFile(DESTINATIONPATH, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
		suint64 nLength = fileLength1;

		LONG nLengthHigh = (nLength >> 32);
		DWORD dwPtrLow = SetFilePointer(hFile, (LONG)(nLength & 0xFFFFFFFF),
			&nLengthHigh, FILE_BEGIN);
		SetEndOfFile(hFile);
		CloseHandle(hFile);

		bRet = mmfwrite.Open(DESTINATIONPATH, OPN_READWRITE);
		if (!bRet)
		{
			printf("open file failed.");
			return false;
		}
	}
	mmfwrite.Seek(0, SP_BEGIN);
	suint64 fileLength2 = mmfwrite.GetLength();



	if (fileLength1 != fileLength2)
	{
		mmfwrite.Close();

		HANDLE hFile = CreateFile(DESTINATIONPATH, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
		suint64 nLength = fileLength1;

		LONG nLengthHigh = (nLength >> 32);
		DWORD dwPtrLow = SetFilePointer(hFile, (LONG)(nLength & 0xFFFFFFFF),
			&nLengthHigh, FILE_BEGIN);
		SetEndOfFile(hFile);
		CloseHandle(hFile);

		// reopen
		bRet = mmfwrite.Open(DESTINATIONPATH, OPN_READWRITE);
		mmfwrite.Seek(0, SP_BEGIN);
		if (!bRet)return false;
	}

	

	char* pBuf = new char[4097];
	char* pBuf1 = new char[4097];
	ZeroMemory(pBuf, 11);
	ZeroMemory(pBuf1, 11);

	//copy file
	int ncount = 0, ncount1 = 0, nwrite = 0, lcount = 0, fsek = 0;
	suint64 curSeek2 = 0;

	int iseq = 0, notiseq = 0;

	while (1)
	{
		ncount = mmf.Read(pBuf, 4096);
		if (ncount)
		{
			ncount1 = mmfwrite.Read(pBuf1, 4096);

			if (ncount1)
			{

				// 1:
				/*
				int cmp = memcmp(pBuf, pBuf1, 4096);
				if (cmp != 0)
				{
				cout << "cmp:" << cmp << endl;

				mmfwrite.Seek(curSeek2, SP_BEGIN);
				nwrite = mmfwrite.Write(pBuf, ncount);
				}
				*/

				// 2:

				int cmp = memcmp(pBuf, pBuf1, 4096);

				if (ncount == ncount1)
				{
					SHA3_512 sha3;
					unsigned char output1[1024] = { '\0' }, output2[1024] = { '\0' };
					sha3.CalculateDigest(output1, (unsigned char*)pBuf, ncount);

					sha3.Restart();
					sha3.CalculateDigest(output2, (unsigned char*)pBuf1, ncount);

					int iseql = memcmp(output1, output2, strlen((char*)output1));
					if (iseql != 0)
					{
						string st1, st2;
						for (int i = 0; i < strlen((char*)output1); i++)
						{
							char ctmp[10] = { '\0' };
							sprintf(ctmp, "%02X", output1[i]);
							st1.append(ctmp);
							sprintf(ctmp, "%02X", output2[i]);
							st2.append(ctmp);
						}

						cout << "st1:" << st1 << endl;
						cout << "st2:" << st2 << endl;

						mmfwrite.Seek(curSeek2, SP_BEGIN);
						nwrite = mmfwrite.Write(pBuf, ncount);

						saveFile(st1, st2);
						++notiseq;
					}
					else
						++iseq;
				}

			}
			curSeek2 += ncount;
		}
		if (ncount == 0)
			break;
		++lcount;
	}



	while (0)
	{
		ncount = mmf.Read(pBuf, 4096);
		if (ncount)
		{
			ncount1 = mmfwrite.Read(pBuf1, 4096);

			if (ncount1)
			{

				// 1:
				/*
				int cmp = memcmp(pBuf, pBuf1, 4096);
				if (cmp != 0)
				{
					cout << "cmp:" << cmp << endl;

					mmfwrite.Seek(curSeek2, SP_BEGIN);
					nwrite = mmfwrite.Write(pBuf, ncount);
				}
				*/

				// 2:

				int cmp = memcmp(pBuf, pBuf1, 4096);

				if (ncount == ncount1)
				{
					SHA3_512 sha3;
					unsigned char output1[1024] = { '\0' }, output2[1024] = { '\0' };
					sha3.CalculateDigest(output1, (unsigned char*)pBuf, ncount);
					
					sha3.Restart();
					sha3.CalculateDigest(output2, (unsigned char*)pBuf1, ncount);
					
					int iseql = memcmp(output1, output2, strlen((char*)output1));
					if (iseql != 0)
					{
						string st1, st2;
						for (int i = 0; i < strlen((char*)output1); i++)
						{
							char ctmp[10] = { '\0' };
							sprintf(ctmp, "%02X", output1[i]);
							st1.append(ctmp);
							sprintf(ctmp, "%02X", output2[i]);
							st2.append(ctmp);
						}

						cout << "st1:" << st1 << endl;
						cout << "st2:" << st2 << endl;

						mmfwrite.Seek(curSeek2, SP_BEGIN);
						nwrite = mmfwrite.Write(pBuf, ncount);

						saveFile(st1, st2);
						++notiseq;
					}else 
						++iseq;
				}

			}
			curSeek2 += ncount;
		}
		if (ncount == 0)
			break;
		++lcount;
	}

	suint64 nWriteFileLength = mmfwrite.GetLength();

	mmf.Close();
	mmfwrite.Close();

	delete[] pBuf;



	::system("pause");
	return 0;
}


void saveFile(string st1, string st2)
{
	FILE* pfile;
	pfile = fopen("E:\\abc.txt", "a");
	if (pfile == NULL)
	{
		printf("open file failed.");
		system("pause");
	}

	fputs(st1.c_str(), pfile);
	fputs("\n\n\n--------------\n\n\n", pfile);
	fputs(st2.c_str(), pfile);
	fputs("\n\n\n--------------\n\n\n", pfile);

	fclose(pfile);
}