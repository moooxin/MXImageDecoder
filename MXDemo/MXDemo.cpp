// MXDemo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "windows.h"
#include <io.h>
#include <direct.h>  
#include <iostream>
#include <string>
#include <vector>
#include "../MXImageDecoder/imximagedecoder.h"


#define TEST_C_API 0

using namespace std;
using namespace mx;

void DecodeFiles(const vector<string>& files);
void OnResult(const DecodeInfo *info);
int UTF8ToGBK(const char * lpUTF8Str, char * lpGBKStr, int nGBKStrLen);

void GetFiles(const string& path, vector<string>& files)
{
	//文件句柄  
	long   hFile = 0;
	//文件信息  
	struct _finddata_t fileinfo;  
	string p;  
	if((hFile = _findfirst(p.assign(path).append("\\*").c_str(),&fileinfo)) !=  -1)  
	{  
		do  
		{  
			//如果是目录,迭代之  
			//如果不是,加入列表  
			if((fileinfo.attrib &  _A_SUBDIR))  
			{
				if(strcmp(fileinfo.name,".") != 0  &&  strcmp(fileinfo.name,"..") != 0)  
					GetFiles( p.assign(path).append("\\").append(fileinfo.name), files );  
			}  
			else  
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name) );  
			}  
		}while(_findnext(hFile, &fileinfo)  == 0);

		_findclose(hFile);  
	}  
} 

int _tmain(int argc, _TCHAR* argv[])
{
	HINSTANCE hnst = LoadLibrary(L"MXImageDecoder");//得到动态链接库的句柄 

	char *buffer; 
	if ((buffer = _getcwd(NULL, 0)) == NULL)
	{
		perror("getcwd error");  

		return 0;
	}

#ifdef _DEBUG
	string path = "C:\\Users\\mx-home\\Desktop\\MXImageDecoder\\MXImageDecoder\\Debug";
#else
	string path = buffer;
	//string path = "C:\\Users\\mx-home\\Desktop\\MXImageDecoder\\MXImageDecoder\\Release";
#endif
	path += "\\img";
	cout << "图片目录：" << path << endl;

	vector<string> files;
	GetFiles(path, files);
	cout << "文件数量：" << files.size() << endl;

	if(files.size())
		DecodeFiles(files);

	getchar();

	return 0;
}

void DecodeFiles(const vector<string>& files)
{
	HINSTANCE hnst = LoadLibrary(L"MXImageDecoder");//得到动态链接库的句柄 

	if (hnst == NULL)
	{
		cout << "加载MXImageDecoder失败！" << GetLastError() << endl;

		return ;
	}
#if TEST_C_API
	typedef DecodeResult(__cdecl *decoderfunction)(const char *, DecodeInfo **);
	typedef void(__cdecl *destroyfunction)(DecodeInfo *);

	decoderfunction dec = (decoderfunction)GetProcAddress(hnst, "DecodeImageFile");//得到动态链接库中add导出函数的地址 
	destroyfunction des = (destroyfunction)GetProcAddress(hnst, "DestoryDecodeInfo");

	if (!dec || !des)
	{
		cout << "获得函数地址失败!" << endl;

		return ;
	}
	
#else

	AllocDecoderFunction allocfunc = (AllocDecoderFunction)GetProcAddress(hnst, "AllocDecoder");
	DestroyDecoderFunction destroyfunc = (DestroyDecoderFunction)GetProcAddress(hnst, "DestroyDecoder");

	if (!allocfunc || !destroyfunc)
	{
		cout << "获得函数地址失败!" << endl;

		return ;
	}

	IMXImageDecoder *decoder = NULL;
	allocfunc(&decoder);

	if (!decoder)
	{
		cout << "获得解码器失败!" << endl;

		return;
	}
#endif

	int failcount = 0;
	for (int i = 0; i < files.size(); i++)
	{
		cout << "解码文件：" << files[i] << endl;

#if TEST_C_API

		DecodeInfo *info = NULL;
		DecodeResult dr = dec(files[i].c_str(), &info);

		if (dr == DR_OK && info)
		{
			OnResult(info);
		}
#else
		if (decoder->DecodeFile(files[i]) == DR_OK)
		{
			const DecodeInfo *info = decoder->GetResult();

			if (info)
				OnResult(info);
			else
			{
				failcount++;
				cout << "解码失败!" << endl;
			}
		}
#endif

		cout << "-----------------------------------" << endl;
	}

	cout << "解码结束！\n一共：" << files.size()
		<< "；成功：" << files.size() - failcount
		<< "；失败：" << failcount
		<< "；失败比：%" << (failcount / (float)files.size()) * 100 << endl;



#if !TEST_C_API
	destroyfunc(decoder);
#endif

	FreeLibrary(hnst);//释放动态链接库 
}

void OnResult(const DecodeInfo *info)
{
	cout << "解码数量：" << info->count << endl;

	if (info->count)
	{
		char szResult[1024] = {0};
		for (int i = 0; i < info->count; i++)
		{
			DataInfo data = info->data[i];

			UTF8ToGBK(data.data, szResult, 1024);
			cout << "解码内容【" << i + 1 << "】：" << szResult << endl;
		}
	}

}

int UTF8ToGBK(const char * lpUTF8Str, char * lpGBKStr, int nGBKStrLen)
{
	if (!lpUTF8Str)  //如果UTF8字符串为NULL则出错退出
		return 0;

	wchar_t * lpUnicodeStr = NULL;
	int nRetLen = 0;

	nRetLen = ::MultiByteToWideChar(CP_UTF8, 0, (char *)lpUTF8Str, -1, NULL, NULL);  //获取转换到Unicode编码后所需要的字符空间长度
	lpUnicodeStr = new WCHAR[nRetLen + 1];  //为Unicode字符串空间
	nRetLen = ::MultiByteToWideChar(CP_UTF8, 0, (char *)lpUTF8Str, -1, lpUnicodeStr, nRetLen);  //转换到Unicode编码
	if (!nRetLen)  //转换失败则出错退出
		return 0;

	nRetLen = ::WideCharToMultiByte(CP_ACP, 0, lpUnicodeStr, -1, NULL, NULL, NULL, NULL);  //获取转换到GBK编码后所需要的字符空间长度

	if (!lpGBKStr)  //输出缓冲区为空则返回转换后需要的空间大小
	{
		if (lpUnicodeStr)
			delete[]lpUnicodeStr;
		return nRetLen;
	}

	if (nGBKStrLen < nRetLen)  //如果输出缓冲区长度不够则退出
	{
		if (lpUnicodeStr)
			delete[]lpUnicodeStr;
		return 0;
	}

	nRetLen = ::WideCharToMultiByte(CP_ACP, 0, lpUnicodeStr, -1, (char *)lpGBKStr, nRetLen, NULL, NULL);  //转换到GBK编码

	if (lpUnicodeStr)
		delete[]lpUnicodeStr;

	return nRetLen;
}
