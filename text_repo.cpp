#include "text_repo.h"

#include <fstream>
#include <iostream>


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
		default:
			break;
		}
	}
};