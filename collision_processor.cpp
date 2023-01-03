#include "collision_processor.h"


namespace Tmpl8
{
	CollisionProcessor::CollisionProcessor(Player& player) :
		m_player{ player }
	{	}


	void CollisionProcessor::SetPointToProcess(DetectorPoint& point)
	{
		m_current_point = &point;
	}


	void CollisionProcessor::CheckPointCollisions()
	{
		CheckForLevelEnd();

		m_current_point->CheckForCollisions();
	}

};
