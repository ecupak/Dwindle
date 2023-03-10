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
		MessageType m_ending{ MessageType::ENDING };

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
				"It's safe to touch a glowing surface! You will not lose light."
			},
			Entry{m_tutorial, vec2{13, 36}, 4,
				"This is a Spark. It will restore some of your lost light."
			},
			Entry{m_tutorial, vec2{18, 36}, 5,
				"If you touch a dark surface and have no light left, the level will restart."
			},
			Entry{m_tutorial, vec2{22, 44}, 1,
				"Keep moving!"
			},
			Entry{m_tutorial, vec2{16, 44}, 4,
				"Some surfaces will absorb the glow you make."
			},			
			Entry{m_tutorial, vec2{10, 44}, 5,
				"Find an exit to each stage. Good luck!"
			},
			Entry{m_guide, vec2{9, 29}, 3,
				"Press F1 to toggle collision mode."
			},
		};


		std::vector<Entry> m_level_1_entries
		{
			Entry{m_guide, vec2{5, 14}, 1,
				"Hello, Dear."
			},
			Entry{m_guide, vec2{9, 12}, 2,
				"Don't be afraid."
			},
			Entry{m_guide, vec2{14, 14}, 3,
				"We'll get through this together."
			},
			Entry{m_guide, vec2{22, 14}, 2,
				"Follow the light."
			},			
			Entry{m_guide, vec2{17, 8}, 3,
				"Mind the gap. Our memories are full of them."
			},
			Entry{m_guide, vec2{19, 2}, 4,
				"You're doing wonderful."
			},
			Entry{m_guide, vec2{13, 6}, 2,
				"Curious as ever."
			},
			Entry{m_guide, vec2{8, 4}, 3,
				"I miss watching the clouds with you."
			},
			Entry{m_guide, vec2{4, 7}, 2,
				"You can do this."
			},
		};


		std::vector<Entry> m_level_2_entries
		{
			Entry{m_guide, vec2{4, 9}, 3,
				"I'm not fond of the memories ahead."
			},
			Entry{m_guide, vec2{4, 15}, 3,
				"They kept you so busy."
			},
			Entry{m_guide, vec2{9, 15}, 3,
				"...all the time."
			},
			Entry{m_guide, vec2{10, 7}, 3,
				"But we found our time."
			},			
			Entry{m_guide, vec2{11, 1}, 3,
				"This is our moutain climb, isn't it?"
			},
			Entry{m_guide, vec2{20, 1}, 4,
				"Fall down. I'll catch you."
			},
			Entry{m_guide, vec2{22, 8}, 2,
				"Safe!"
			},
			Entry{m_guide, vec2{16, 8}, 2,
				"Almost there."
			},
			Entry{m_guide, vec2{15, 15}, 3,
				"You were always working on something new."
			},
			Entry{m_guide, vec2{21, 15}, 3,
				"Did you do everthing you wanted to?"
			},
			Entry{m_guide, vec2{20, 25}, 3,
				"Ahead.. is not a good memory."
			},
			Entry{m_guide, vec2{4, 23}, 2,
				"It wasn't your fault."
			},
		};


		std::vector<Entry> m_level_3_entries
		{
			Entry{m_guide, vec2{10, 30}, 5,
				"I always wondered what happened to that piano."
			},
			Entry{m_guide, vec2{19, 30}, 4,
				"And to those raccoons from the storm."
			},
			Entry{m_guide, vec2{22, 26}, 2,
				"They were adorable."
			},
			Entry{m_guide, vec2{11, 26}, 4,
				"You'll have to tell me everything I missed."
			},					
			Entry{m_guide, vec2{4, 13}, 2,
				"Oh. This is when I..."
			},
			Entry{m_guide, vec2{25, 14}, 2,
				"You got some cats?"
			},
			Entry{m_guide, vec2{21, 10}, 3,
				"A lot of cats!"
			},
			Entry{m_guide, vec2{15, 9}, 3,
				"That's too many cats."
			},
			Entry{m_guide, vec2{5, 5}, 2,
				"I've missed you."
			},			
			Entry{m_guide, vec2{15, 1}, 4,
				"It's almost time"
			},
			Entry{m_guide, vec2{4, 1}, 2,
				"At last."
			},
		};


		std::vector<Entry> m_game_over_entries
		{
			Entry{m_ending, vec2{3, 1}, 4,
				"Hello, Dear."
			},
			Entry{m_ending, vec2{3, 3}, 4,
				"It's been a while."
			},
			Entry{m_ending, vec2{2, 5}, 6,
				"Rest now. Come watch the clouds with me."
			},			
			Entry{m_tutorial, vec2{4, 8}, 2,
				"-End-"
			},

		};
	};
};

