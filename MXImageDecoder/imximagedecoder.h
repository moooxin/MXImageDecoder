	#pragma once

	#ifdef __cplusplus

	#include <list>

	namespace mx
	{
		extern "C" {
	#endif

			enum SymbolType
			{
				ST_NONE = 0,	/**< no symbol decoded */
				ST_PARTIAL = 1,	/**< intermediate status */
				ST_EAN8 = 8,	/**< EAN-8 */
				ST_UPCE = 9,	/**< UPC-E */
				ST_ISBN10 = 10,	/**< ISBN-10 (from EAN-13). @since 0.4 */
				ST_UPCA = 12,	/**< UPC-A */
				ST_EAN13 = 13,	/**< EAN-13 */
				ST_ISBN13 = 14,	/**< ISBN-13 (from EAN-13). @since 0.4 */
				ST_I25 = 25,	/**< Interleaved 2 of 5. @since 0.4 */
				ST_CODE39 = 39,	/**< Code 39. @since 0.4 */
				ST_PDF417 = 57,	/**< PDF417. @since 0.6 */
				ST_QRCODE = 64,	/**< QR Code. @since 0.10 */
				ST_CODE128 = 128,	/**< Code 128 */
				ST_SYMBOL = 0x00ff,	/**< mask for base symbol type */
				ST_ADDON2 = 0x0200,	/**< 2-digit add-on flag */
				ST_ADDON5 = 0x0500,	/**< 5-digit add-on flag */
				ST_ADDON = 0x0700,	/**< add-on flag mask */
			};

			//解码内容
			struct DataInfo
			{
				SymbolType type;	//解码类型

				unsigned int size;	//数据大小
				char *data;			//数据内容
			};

			//解码数据
			struct DecodeInfo
			{
				unsigned int count;	//解码个数

				DataInfo *data;		//解码数据集合
			};

			//解码返回值
			enum DecodeResult
			{
				DR_OK = 0,			//解码成功
				DR_PARAM_ERROR		//参数错误
			};


	#ifdef __cplusplus
		}


		class IMXImageDecoder
		{
		public:
			virtual ~IMXImageDecoder(){};

			virtual DecodeResult DecodeFile(const std::string &filepath) = 0;

			virtual const DecodeInfo *GetResult() = 0;
		};

		typedef void(__cdecl *AllocDecoderFunction)(IMXImageDecoder **);
		typedef void(__cdecl *DestroyDecoderFunction)(IMXImageDecoder *);
	}
	#endif

