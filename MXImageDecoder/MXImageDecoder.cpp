// MXImageDecoder.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "imximagedecoder.h"
#include "MXImageDecoderImp.h"

using namespace mx;

extern "C" _declspec(dllexport) void AllocDecoder(IMXImageDecoder **decoder)
{
	MXImageDecoderImp *decoderimp = new MXImageDecoderImp;

	if (decoderimp != NULL)
	{
		*decoder = decoderimp;
	}
}

extern "C" _declspec(dllexport) void DestroyDecoder(IMXImageDecoder *decoder)
{
	if (decoder)
	{
		MXImageDecoderImp *decoderimp = dynamic_cast<MXImageDecoderImp *>(decoder);
		if (decoderimp)
		{
			delete decoderimp;

			decoderimp = NULL;
		}
	}
}

extern "C" _declspec(dllexport) DecodeResult DecodeImageFile(const char *filepath, DecodeInfo **result)
{
	MXImageDecoderImp decoder;
	DecodeResult ret = decoder.DecodeFile(filepath);

	if (ret == DR_OK)
	{
		*result = (DecodeInfo *)decoder.GetResult();

		decoder.DisableDecodeResultTools();
	}

	return ret;
}

extern "C" _declspec(dllexport) void DestoryDecodeInfo(DecodeInfo *result)
{
	if (result)
	{
		unsigned int i = 0;
		for (; i < result->count; i++)
		{
			DataInfo info = result->data[i];
			if (info.data)
			{
				delete[] info.data;

				info.data = NULL;
			}
		}
		delete result;

		result = NULL;
	}
}