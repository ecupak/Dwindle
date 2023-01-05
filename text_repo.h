#pragma once

#include <string>
#include <vector>
#include <array>

#include "template.h"
#include "message_enums.h"


namespace Tmpl8
{
	struct Entry
	{
		Entry(MessageType message_type, vec2 position, int span, std::string text) :
			m_message_type{ message_type },
			m_position{ position },
			m_span{ span },
			m_text{ text }
		{	}
		
		MessageType m_message_type;
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
		MessageType m_tutorial{ MessageType::TUTORIAL };
		MessageType m_guide{ MessageType::GUIDE };

		std::vector<Entry> m_level_0_entries
		{
			Entry{MessageType::TUTORIAL, vec2{3, 8}, 6,
				"Use WASD or the arrow keys to move. Touch a wall to stick to it, then press 'up' or away from the wall ('left' or 'right') to wall jump."
			},
			Entry{MessageType::TUTORIAL, vec2{9, 5}, 5,
				"Use multiple 'up' wall jumps to climb a wall."
			},
			Entry{m_tutorial, vec2{15, 1}, 3,
				"Press 'down' to drop off a wall without doing a wall jump."
			},
			Entry{m_tutorial, vec2{14, 12}, 5,
				"While on a wall, press 'up' to to jump higher without moving left or right a lot."
			},
			Entry{m_tutorial, vec2{22, 3}, 2,
				"Be brave."
			},
			Entry{m_tutorial, vec2{25, 20}, 3,
				"While on a wall, press away from it to jump more sideways than up."
			},
			Entry{m_tutorial, vec2{15, 22}, 4,
				"Landing on a corner will knock you away."
			},
			Entry{m_tutorial, vec2{6, 25}, 7,
				"Try to land where there is light."
			},
			Entry{m_tutorial, vec2{3, 25}, 2,
				"Time to be brave again."
			},
			Entry{m_tutorial, vec2{1, 36}, 5,
				"When you touch a dark surface, it will glow and you will lose some light."
			},
			Entry{m_tutorial, vec2{7, 36}, 5,
				"You can touch a glowing spot without losing more light."
			},
			Entry{m_tutorial, vec2{13, 36}, 4,
				"This is a Spark. It will restore some of your lost light."
			},
			Entry{m_tutorial, vec2{18, 36}, 5,
				"If you touch a dark surface and have no light left, the level will restart."
			},
			Entry{m_tutorial, vec2{24, 36}, 5,
				"Find an exit to each stage. Good luck!"
			},
			Entry{m_guide, vec2{9, 29}, 3,
				"Press F1 to toggle debug info."
			},
		};


		std::vector<Entry> m_level_1_entries
		{
			Entry{m_guide, vec2{5, 14}, 1,
				"Hello, Dear."
			},
			Entry{m_guide, vec2{9, 12}, 3,
				"...It's been a long time."
			},
			Entry{m_guide, vec2{14, 14}, 3,
				"We'll get through this together."
			},
			Entry{m_guide, vec2{22, 14}, 2,
				"You always did like climbing."
			},
			Entry{m_guide, vec2{22, 9}, 2,
				"Be careful."
			},
			Entry{m_guide, vec2{19, 5}, 2,
				"Do you see it? Almost there."
			},
			Entry{m_guide, vec2{18, 2}, 2,
				"Keep going. I'm by your side."
			},
		};
	};
};

