#pragma once

#include <vector>

namespace Tmpl8
{
	template<typename T>
	class Socket
	{
	public:
		Socket() {}


		void SendMessage(T& message)
		{
			m_messages.push_back(message);
			m_has_new_messages = true;
		}


		bool HasNewMessage() { return m_has_new_messages; }


		std::vector<T>& ReadMessages()
		{ 
			m_has_new_messages = false;
			return m_messages;
		}


		void ClearMessages() 
		{
			m_has_new_messages = false;
			m_messages.clear();
		}


	private:
		std::vector<T> m_messages;
		bool m_has_new_messages{ false };
	};
};