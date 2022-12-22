#include "glow_socket.h"

namespace Tmpl8
{
	GlowSocket::GlowSocket()
	{	}


	void GlowSocket::SendMessage(vec2& orb_position, CollidableType glow_orb_type)
	{
		m_messages.emplace_back(orb_position, glow_orb_type);
		m_has_new_messages = true;
	}


	std::vector<GlowMessage> GlowSocket::ReceiveMessages()
	{
		std::vector<GlowMessage> messages_handoff{ m_messages };
		
		m_messages.clear();
		m_has_new_messages = false;

		return messages_handoff;
	}
};