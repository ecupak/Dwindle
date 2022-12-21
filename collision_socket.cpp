#include "collision_socket.h"

namespace Tmpl8
{
	void CollisionSocket::SendMessage(std::vector<Collidable*>& collidables)
	{
		m_message = CollisionMessage(collidables);
		m_has_new_message = true;
	}


	CollisionMessage CollisionSocket::ReceiveMessage()
	{
		m_has_new_message = false;
		return m_message;
	}

};