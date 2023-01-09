#pragma once

#include <vector>

namespace Tmpl8
{
	/// <summary>
	/// Base class that handles inter-class communications.
	/// The owning class should create only 1 instance and only distribute pointers to the Socket.
	/// </summary>
	/// <typeparam name="T">Provide the Message type the Socket will be processing.</typeparam>
	template<typename T>
	class Socket
	{
	public:
		Socket() {}

		/// <summary>
		/// Called by external classes holding a reference to the Socket.
		/// Adds the Message to a vector to be extracted by the parent class later.
		/// </summary>
		/// <param name="message">The Message type of the Socket.</param>
		void SendMessage(T& message)
		{
			m_messages.push_back(message);
			m_has_new_messages = true;
		}


		/// <summary>
		/// For the parent class to check if there are any new Messages stored in the Socket.
		/// </summary>
		/// <typeparam name="T"></typeparam>
		bool HasNewMessage() { return m_has_new_messages; }


		/// <summary>
		/// For the parent class.
		/// Gets the vector of Messages stored in the Socket.
		/// </summary>
		/// <typeparam name="T"></typeparam>
		std::vector<T>& ReadMessages()
		{ 
			m_has_new_messages = false;
			return m_messages;
		}


		/// <summary>
		/// For the parent class. Empties the Socket of stored Messages.
		/// </summary>
		/// <typeparam name="T"></typeparam>
		void ClearMessages() 
		{
			m_has_new_messages = false;
			m_messages.clear();
		}


	private:
		// Vector of Messages stored by the Socket.
		std::vector<T> m_messages;

		// Tracks if there are new Messages in the Socket.
		bool m_has_new_messages{ false };
	};
};