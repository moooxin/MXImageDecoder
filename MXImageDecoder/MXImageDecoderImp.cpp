#include "stdafx.h"
#include "MXImageDecoderImp.h"
#include <iostream>
#include <zbar.h>
#include <opencv2/opencv.hpp>
#include <tchar.h>
#include <Windows.h>


namespace mx
{
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

	MXImageDecoderImp::MXImageDecoderImp()
	{
	}

	MXImageDecoderImp::~MXImageDecoderImp()
	{
	}
	
	DecodeResult MXImageDecoderImp::DecodeFile(const std::string &filepath)
	{
		if (filepath.empty())
			return DR_PARAM_ERROR;

		m_tools.Reset();

		// create a reader
		zbar::ImageScanner scanner;
		// configure the reader
		scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 1);

		//cv::Mat img = cv::imread("C:\\Users\\mx-home\\Desktop\\libdecodeqr-master\\img\\125.bmp");
		cv::Mat img = cv::imread(filepath);
		cv::Mat imgGray;
		imgGray.create(img.size(), CV_8UC1);
		cvtColor(img, imgGray, CV_BGR2GRAY);
		int width = imgGray.cols;   // extract dimensions
		int height = imgGray.rows;

		zbar::Image image(width, height, "Y800", imgGray.data, width * height);

		// scan the image for barcodes
		int n = scanner.scan(image);

		//TCHAR szResult[512];
		//extract results
		for (zbar::Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end();++symbol)
		{
			// do something useful with result
			const std::string name = symbol->get_type_name();
			//std::cout << "DecodeType :" << symbol->get_type() << "(" << name << ")" << std::endl;

			const std::string data = symbol->get_data();
			//std::cout << "DecodeData :" << data << std::endl;
			
			//UTF8ToGBK(data.c_str(), szResult, 512);
			//std::cout << "DecodeData :" << szResult << std::endl;

			DataInfo info;
			info.type = (SymbolType)symbol->get_type();
			info.size = data.length() + 1;
			info.data = new char[info.size];
			memset(info.data, 0, info.size);
			memcpy(info.data, data.c_str(), info.size - 1);

			m_tools.Add(info);
		}

		// clean up
		image.set_data(NULL, 0);

		return DR_OK;
	}

	const DecodeInfo * MXImageDecoderImp::GetResult()
	{
		return m_tools.GetDecodeInfo();
	}

	void MXImageDecoderImp::DisableDecodeResultTools()
	{
		m_tools.DisableDistroy();
	}

}