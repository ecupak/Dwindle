#pragma once

#include <vector>
#include <algorithm>
#include <SDL_keycode.h>

namespace Tmpl8
{
	struct KeyInfo
	{
		KeyInfo(int key_code) :
			m_key_code{ key_code }
		{	}

		int m_key_code{ 0 };
		bool m_is_pressed{ false };
		bool m_is_active{ false };		
	};


	class KeyManager
	{
	public:
		KeyManager() {};


		void RegisterKey(int key_code)
		{
			if (keys.end() == FindKey(key_code))
			{
				keys.emplace_back(key_code);
			}
		}


		KeyInfo& GetKey(int key_code)
		{
			auto key = FindKey(key_code);
			if (keys.end() != key)
			{
				return *key;
			}
		}


	private:
		std::vector<KeyInfo>::iterator FindKey(int key_code)
		{
			return std::find_if(keys.begin(), keys.end(),
				[key_code](KeyInfo& key_info) {return key_info.m_key_code == key_code;});
		}

		std::vector<KeyInfo> keys;
	};
};

