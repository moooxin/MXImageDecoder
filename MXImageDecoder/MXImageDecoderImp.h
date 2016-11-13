#pragma once
#include "imximagedecoder.h"
#include <vector>

namespace mx
{
	class DecodeResultTools
	{
	public:

		DecodeResultTools()
		{
			Init();
		}

		~DecodeResultTools()
		{
			Destroy();
		}

		void DisableDistroy()
		{
			m_result = NULL;

			m_datas.clear();
		}

		void Reset()
		{
			Destroy();
			Init();
		}

		void Add(const DataInfo &info)
		{
			m_datas.push_back(info);
		}

		DecodeInfo *GetDecodeInfo()
		{
			if (m_datas.empty())
			{
				m_result->count = 0;
				m_result->data = NULL;

				return NULL;
			}

			m_result->count = m_datas.size();
			if (m_result->count)
			{
				m_result->data = new DataInfo[m_result->count];
				memcpy(m_result->data, &m_datas[0], sizeof(DataInfo) * m_result->count);
			}
			else
			{
				m_result->data = NULL;
			}

			return m_result;
		}
	private:

		void Init()
		{
			m_result = new DecodeInfo;

			m_result->count = 0;
			m_result->data = NULL;
		}

		void Destroy()
		{
			std::vector<DataInfo>::iterator it = m_datas.begin();
			while (it != m_datas.end())
			{
				delete[] it->data;

				it->data = NULL;

				it++;
			}

			m_datas.clear();

			if (m_result)
			{
				m_result->count = 0;
				m_result->data = NULL;

				delete m_result;

				m_result = NULL;
			}
		}

		std::vector<DataInfo> m_datas;
		DecodeInfo *m_result;
	};

	class MXImageDecoderImp : public IMXImageDecoder
	{

	public:
		MXImageDecoderImp();
		virtual ~MXImageDecoderImp();

		virtual DecodeResult DecodeFile(const std::string &filepath);

		virtual const DecodeInfo *GetResult();

		void DisableDecodeResultTools();
	private:

		DecodeResultTools m_tools;
	};

}
