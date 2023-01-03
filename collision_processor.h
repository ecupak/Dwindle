#pragma once

#include "player.h"


namespace Tmpl8
{
	class CollisionProcessor
	{
	public:
		CollisionProcessor(Player& player);
		
		void SetPointToProcess(DetectorPoint& point);
		void CheckPointCollisions();

	private:
		void CheckForLevelEnd() {};

		Player& m_player;
		DetectorPoint* m_current_point;
	};
};

