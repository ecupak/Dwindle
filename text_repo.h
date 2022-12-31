#pragma once

#include <string>
#include <vector>

namespace Tmpl8
{
	class TextRepo
	{
	public:
		TextRepo();

		std::string& GetText(int level_id, int entry_id);
		void LoadText(const std::string& file_location);

	private:
		void AddBookToLibrary();			
		void AddEntryToBook(std::string& entry);

		std::vector<std::vector<std::string>> m_library;
		std::vector<std::string> m_book;
	};
};

