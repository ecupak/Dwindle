#pragma once

#include "template.h"

namespace Tmpl8
{
	/// <summary>
	/// Message structure to contain the data sent from Player to GlowManager.
	/// </summary>
	struct GlowMessage
	{
		vec2 m_orb_position{ 0.0f, 0.0f };
		bool m_is_from_ricochet{ true };

		GlowMessage() {};

		GlowMessage(vec2& orb_position, bool is_from_ricochet) :
			m_orb_position{orb_position},
			m_is_from_ricochet{is_from_ricochet}
		{	}
	};


	/// <summary>
	/// Allows Player to communicate with GlowManager. Based on a simplified version of a server-socket relationship.
	/// Player holds the pointer of the GlowSocket instance that GlowManager owns.
	/// Player calls SetSocketMessage() to load data about a new GlowOrb that should be created.
	/// GlowManager then sees that m_new_message is true and processes the data, which sets the bool back to false.
	/// </summary>
	class GlowSocket
	{
	public:
		GlowSocket();
		
		/// <summary>
		/// Player passes in data for creation of a new GlowOrb.
		/// </summary>
		/// <param name="orb_position">Coordinates to create new glow orb at.</param>
		/// <param name="is_from_ricochet">If orb was created by player ricocheting off of a corner.</param>
		void SendMessage(vec2& orb_position, bool is_from_ricochet);


		/// <summary>
		/// Returns if a new message (data for creation of a new GlowOrb) is ready to be read.
		/// </summary>
		/// <returns>m_has_new_message</returns>
		bool HasNewMessage() { return m_has_new_message; }


		/// <summary>
		/// Returns the contents of message.
		/// </summary>
		/// <returns>If there is a new message, returns that. Otherwise, returns a default-constructed message.</returns>
		GlowMessage ReceiveMessage();


	private:
		GlowMessage m_message{};
		bool m_has_new_message{ false };
	};
};