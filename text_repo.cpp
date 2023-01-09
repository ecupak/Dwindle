#include "text_repo.h"


namespace Tmpl8
{
	// Return list of messages to caller.
	std::vector<Entry>& TextRepo::GetEntriesForLevel(int level_id)
	{
		switch (level_id)
		{
		case 0:
			return m_level_0_entries;
		case 1:
			return m_level_1_entries;
		case 2:
			return m_level_2_entries;
		case 3:
			return m_level_3_entries;
		case 4:
			return m_game_over_entries;
		default:
			break;
		}
	}
};