#pragma once

#include <vector>

#include "collidable.h"
#include "template.h"

namespace Tmpl8
{
	/// <summary>
	/// Message structure to contain the data sent to CollisionManager.
	/// </summary>
	struct CollisionMessage
	{
	public:
		CollisionMessage() { };

		CollisionMessage(std::vector<Collidable*>& collidables)	:
			m_collidables{collidables}
		{	}

		std::vector<Collidable*>& GetCollidables()
		{
			return m_collidables;
		}

	private:
		std::vector<Collidable*> m_collidables;
	};


	/// <summary>
	/// Allows external class to send data to manager.
	/// External class holds the pointer of the socket instance that the manager owns.
	/// </summary>
	class CollisionSocket
	{
	public:
		CollisionSocket() {}

		/// <summary>
		/// Called by external class to load data for the manager.
		/// </summary>
		/// <param name="collidables">New list of Collidable pointers for CollisionManager to use.</param>
		void SendMessage(std::vector<Collidable*>& collidables);


		/// <summary>
		/// Called by CollisionManager to check if there is new data to receive.
		/// </summary>
		/// <returns>True if new data has been loaded and has not yet been received.</returns>
		bool HasNewMessage() { return m_has_new_message; }


		/// <summary>
		/// Returns the loaded data.
		/// </summary>
		/// <returns>The data loaded by the external class.</returns>
		CollisionMessage ReceiveMessage();


	private:
		CollisionMessage m_message;
		bool m_has_new_message{ false };
	};
};