#include "glow_socket.h"

namespace Tmpl8
{
	GlowSocket::GlowSocket() {};


	void GlowSocket::SendMessage(vec2& orb_position, bool is_from_ricochet)
	{
		message = Message(orb_position, is_from_ricochet);
		m_has_new_message = true;
	}


	Message GlowSocket::ReceiveMessage()
	{
		m_has_new_message = false;
		return (m_has_new_message ? message : Message{});
	}

};