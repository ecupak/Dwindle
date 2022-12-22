#include "camera.h"


namespace Tmpl8
{
	Camera::Camera(Collidable& subject) :
		m_subject{ subject }
	{	
		center = m_subject.center;
		UpdateBounds();
	}


	void Camera::SetPosition(vec2 position)
	{
		center = position;
		UpdateBounds();
		m_has_moved = true;
	}


	bool Camera::HasMoved()
	{
		return m_has_moved;
	
	}


	void Camera::ResolveCollision(Collidable*& collision)
	{
		/*if (collision->m_object_type == CollidableType::PLAYER)
		{
			m_focus = collision->center;
			is_refocusing = true;
		}*/
	}


	void Camera::Draw(Surface* screen)
	{
		screen->Box(left, top, right, bottom, 0xFFFF0000);
	}


	void Camera::Update(float deltaTime)
	{
		// make it so it tracks distance from center of camera to subject center. if that distance exceeds a certain limit, begin closing the gap.

		if (!is_refocusing)
		{
			if (m_subject.left < left || m_subject.right > right || m_subject.top < top || m_subject.bottom > bottom)
			{
				m_focus = m_subject.center;
				is_refocusing = true;
			}
		}

		if (is_refocusing)
		{
			BringSubjectIntoFocus(deltaTime);
			m_has_moved = true;
		}
		else
		{
			m_has_moved = false;
		}
	}

		
	void Camera::BringSubjectIntoFocus(float deltaTime)
	{
		int dist_x = m_focus.x - center.x;
		int dist_y = m_focus.y - center.y;

		if (dist_x == 0 && dist_y == 0)
		{
			is_refocusing = false;
			return;
		}

		if (abs(dist_x) < m_dead_zone)
		{
			m_focus.x = center.x;
		}
		else
		{
			center.x += dist_x * deltaTime;
		}

		if (abs(dist_y) < m_dead_zone)
		{
			m_focus.y = center.y;
		}
		else
		{
			center.y += dist_y * deltaTime;
		}

		UpdateBounds();
	}


	void Camera::UpdateBounds()
	{
		left = center.x - offset_x;
		right = center.x + offset_x;
		top = center.y - offset_y;
		bottom = center.y + offset_y;
	}
};
