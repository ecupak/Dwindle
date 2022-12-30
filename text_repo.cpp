#include "text_repo.h"

#include <fstream>
#include <iostream>


namespace Tmpl8
{
	TextRepo::TextRepo() {};


	void TextRepo::LoadText(const std::string& file_location)
	{
		// Try to load file with text.
		std::ifstream if_stream{ file_location };
		if (!if_stream)
		{
			std::cerr << "Could not find file.\n";
		}

		// Read file and store text by level/book.
		std::string m_entry;
		while (if_stream)
		{
			std::getline(if_stream, m_entry);

			switch (m_entry.length())
			{
			// Delimiter for new level/book is a single number.
			case 1:
				AddBookToLibrary();
				break;
			// Otherwise group all lines into same vector.
			default:
				AddEntryToBook(m_entry);
				break;
			}
		}
	}


	void TextRepo::AddBookToLibrary()
	{
		if (m_book.size() > 0)
		{
			m_library.push_back(m_book);
			m_book.clear();
		}
	}


	void TextRepo::AddEntryToBook(std::string& entry)
	{
		m_book.push_back(entry);
	}
};