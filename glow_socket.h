#pragma once

#include "template.h"
#include <vector>
#include "collidable.h"

namespace Tmpl8
{
	/// <summary>
	/// Message structure to contain the data sent from Player to GlowManager.
	/// </summary>
	struct GlowMessage
	{
		vec2 m_orb_position{ 0.0f, 0.0f };
		CollidableType m_glow_orb_type{ CollidableType::UNKNOWN };

		GlowMessage() {};

		GlowMessage(vec2& orb_position, CollidableType& glow_orb_type) :
			m_orb_position{orb_position},
			m_glow_orb_type{ glow_orb_type }
		{	}
	};


	/// <summary>
	/// Allows external classes to communicate with GlowManager.
	/// The external class holds a pointer of the GlowSocket instance that GlowManager owns.
	/// The external calls SendMessage() to load data about a new GlowOrb that should be created.
	/// Multiple calls to SendMessage() before ReadMessage() is called will store all messages in a vector.
	/// When GlowManager sees that m_new_messages is true, it can receive all the messages at once and reset the bool.
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
		/// <param name="is_safe_glow">If orb was spawns by a full orb during the full phase.</param>
		void SendMessage(vec2& orb_position, CollidableType glow_orb_type);


		/// <summary>
		/// Returns if a new message (data for creation of a new GlowOrb) is ready to be read.
		/// </summary>
		/// <returns>m_has_new_message</returns>
		bool HasNewMessage() { return m_has_new_messages; }


		/// <summary>
		/// Returns the contents of message.
		/// </summary>
		/// <returns>If there is a new message, returns that. Otherwise, returns a default-constructed message.</returns>
		std::vector<GlowMessage> ReceiveMessages();


	private:
		std::vector<GlowMessage> m_messages;
		bool m_has_new_messages{ false };
	};
};