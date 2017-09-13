#include <iostream>
#include "MMFIODef.h"
#include <cstdlib>
#include <cstdio>

using namespace std;
#define _DEBUG	

// #ifdef _DEBUG
// #define new DEBUG_NEW
// #endif

#define SOURCEFILEPAHT  "E:\\july1.nyf"
#define DESTFILEPATH    "E:\\july2.nyf"	



int main()
{
	cout << "1234" << endl;


	HANDLE hofile = CreateFile(SOURCEFILEPAHT, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (INVALID_HANDLE_VALUE == hofile)
	{
		cout << endl;
	}
	LARGE_INTEGER lInteger;
	DWORD hfSize = GetFileSizeEx(hofile, &lInteger);

	cout << "low:" << lInteger.LowPart << ", height:" << lInteger.HighPart;
	CloseHandle(hofile);

	//Create a 1 byte write file, we copy the contents of readfile.txt into this
	HANDLE hFile = CreateFile(DESTFILEPATH, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
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

	bRet = mmfwrite.Open(DESTFILEPATH, OPN_READWRITE);
	if (!bRet)	// 文件不存在
	{
		HANDLE hFile = CreateFile(DESTFILEPATH, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
		suint64 nLength = fileLength1;

		LONG nLengthHigh = (nLength >> 32);
		DWORD dwPtrLow = SetFilePointer(hFile, (LONG)(nLength & 0xFFFFFFFF),
			&nLengthHigh, FILE_BEGIN);
		SetEndOfFile(hFile);
		CloseHandle(hFile);

		bRet = mmfwrite.Open(DESTFILEPATH, OPN_READWRITE);
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

		HANDLE hFile = CreateFile(DESTFILEPATH, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
		suint64 nLength = fileLength1;

		LONG nLengthHigh = (nLength >> 32);
		DWORD dwPtrLow = SetFilePointer(hFile, (LONG)(nLength & 0xFFFFFFFF),
			&nLengthHigh, FILE_BEGIN);
		SetEndOfFile(hFile);
		CloseHandle(hFile);

		// reopen
		bRet = mmfwrite.Open(DESTFILEPATH, OPN_READWRITE);
		mmfwrite.Seek(0, SP_BEGIN);
		if (!bRet)return false;
	}

	

	char* pBuf = new char[4096];
	char* pBuf1 = new char[4096];

	//copy readfile to writefile using CWinMMFIO objects
	int ncount = 0, ncount1 = 0, nwrite = 0, lcount = 0, fsek = 0;
	suint64 curSeek2 = 0;
	while (1)
	{
		ncount = mmf.Read(pBuf, 4096);
		if (ncount)
		{
			ncount1 = mmfwrite.Read(pBuf1, 4096);

			if (ncount1)
			{
				suint64 curPstion1 = mmf.GetPosition();
				suint64 curPstion2 = mmfwrite.GetPosition();

				suint64 fileLength1 = mmf.GetLength();
				suint64 fileLength2 = mmfwrite.GetLength();

				int cmp = memcmp(pBuf, pBuf1, 4096);
				if (cmp != 0)
				{
					cout << "hello world" << endl;
					cout << "cmp:" << cmp << endl;

					mmfwrite.Seek(curSeek2, SP_BEGIN);
					nwrite = mmfwrite.Write(pBuf, ncount);
				}
			}
			curSeek2 += ncount;
			//	curSeek2 = mmfwrite.Seek(0, SP_CUR);
		}
		if (ncount == 0)
			break;
		++lcount;
	}

	suint64 nWriteFileLength = mmfwrite.GetLength();

	mmf.Close();
	mmfwrite.Close();

	delete[] pBuf;



	system("pause");
	return 0;
}