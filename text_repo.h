#pragma once

#include <string>
#include <vector>

namespace Tmpl8
{
	class TextRepo
	{
	public:
		TextRepo();

		void LoadText(const std::string& file_location);

	private:
		void AddBookToLibrary();			
		void AddEntryToBook(std::string& entry);

		std::vector<std::vector<std::string>> m_library;
		std::vector<std::string> m_book;
	};
};

