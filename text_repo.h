#pragma once

#include <string>
#include <vector>
#include <array>

#include "template.h"


namespace Tmpl8
{
	struct Entry
	{
		Entry(vec2 position, int span, std::string text) :
			m_position{ position },
			m_span{ span },
			m_text{ text }
		{	}
				
		vec2 m_position{ 0.0f, 0.0f };
		int m_span{ 1 };
		std::string m_text;		
	};


	class TextRepo
	{ 
	public:
		TextRepo() { };

		std::vector<Entry>& GetEntriesForLevel(int level_id);

	private:
		std::vector<Entry> m_level_0_entries
		{
			Entry{vec2{3, 8}, 6,
				"Use WASD or the arrow keys to move. Touch a wall to stick to it, then press 'up' or away from the wall ('left' or 'right') to wall jump."
			},
			Entry{vec2{9, 5}, 5,
				"Use multiple 'up' wall jumps to climb a wall."
			},
			Entry{vec2{15, 1}, 3,
				"Press 'down' to drop off a wall without doing a wall jump."
			},
			Entry{vec2{14, 12}, 5,
				"While on a wall, press 'up' to to jump higher without moving left or right a lot."
			},
			Entry{vec2{22, 3}, 2,
				"Be brave."
			},
			Entry{vec2{25, 20}, 3,
				"While on a wall, press away from it to jump more sideways than up."
			},
			Entry{vec2{15, 22}, 4,
				"Landing on a corner will knock you away."
			},
			Entry{vec2{6, 25}, 7,
				"Try to land where there is light."
			},
			Entry{vec2{3, 25}, 2,
				"Time to be brave again."
			},
			Entry{vec2{1, 36}, 5,
				"When you touch a dark surface, you will light it up and lose some of your light."
			},
			Entry{vec2{7, 36}, 5,
				"You can touch a spot you have already lit without losing more light."
			},
			Entry{vec2{13, 36}, 5,
				"This is a Spark. It will restore some of your lost light."
			},
			Entry{vec2{19, 36}, 4,
				"Everything will fade away if you touch a dark surface with no light remaining."
			},
			Entry{vec2{24, 36}, 5,
				"Find an exit to each stage. Good luck!"
			},
		};

		std::vector<Entry> m_level_1_entries
		{
			Entry{vec2{1, 6}, 1,
				"Use WASD or the arrow keys to move. Hit a wall to stick to it, then press up or away to wall jump."
			},
		};
	};
};

