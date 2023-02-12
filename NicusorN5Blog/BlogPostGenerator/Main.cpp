#include <fstream>
#include <filesystem>
#include <format>
#include <iostream>
#include <vector>
#include <algorithm>

#include <maddy/parser.h>

struct File
{
	std::string Filename;
	std::filesystem::file_time_type Time;
};

bool operator <(File& a, File& b)
{
	return a.Time < b.Time;
}

bool operator >(File& a, File& b)
{
	return a.Time < b.Time;
}

auto main(int argc, char** argv) -> int
{
	if(argc <= 1) return -1;

	auto parserConfig = std::make_shared<maddy::ParserConfig>();
	parserConfig->generateEntireHTMLDocument = true;
	parserConfig->isSyntaxHighlightningEnabled = true;
	parserConfig->isHTMLWrappedInParagraph = false;

	auto parser = std::make_shared<maddy::Parser>(parserConfig);
	std::vector<File> files;

	try
	{
		for(const auto& entry : std::filesystem::recursive_directory_iterator(std::string(argv[argc - 1])))
		{
			if(std::filesystem::is_regular_file(entry)) //Skip folders.
			{
				File f = { entry.path().string(), entry.last_write_time() };
				if(f.Filename.ends_with(".md")) //Only allow markdown files.
					files.push_back(f);
			}
		}
	}
	catch(std::filesystem::filesystem_error& e)
	{
		std::cerr << e.what();
		return -2;
	}

	std::sort(files.begin(), files.end(), 
		[](File& a, File& b) -> bool
		{
			return a < b;
		}
	);

	for(const File& f : files)
	{
		std::ifstream fstr(f.Filename);
		std::string HTMLDoc = parser->Parse(fstr);

		std::cout << HTMLDoc;
		fstr.close();
	}

	return 0;
}