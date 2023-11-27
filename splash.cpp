#include "Server.hpp"

void showSplash(const std::string &serverAddress, const std::string &serverPort)
{
	// Original ASCII ascii
	std::string ascii = R"(
	⠀⠀⠀⠀⠀ ⠀⢀⣀⡤⠴⠶⠶⠒⠲⠦⢤⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
	⠀⠀⠀⠀⢀⡠⠞⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠉⠲⠤⣄⡀⠀⠀⠀⠀
	⠀⠀⣀⡴⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣤⡿⠀⠀⠀⠀
	⢾⣅⡀⠀⠀⠀⠀⣀⠀⠀⠀⠀⠀⠀⢀⡦⠤⠄⠀⠀⢻⡀⠀⠀⠀⠀
	⠈⢹⡏⠀⠀⠐⠋⠉⠁⠀⠻⢿⠟⠁⠀⠀⢤⠀⠀⠠⠤⢷⣤⣤⢤⡄
	⠀⠀⣼⡤⠤ ⠘⣆⡀⠀⣀⡼⠦⣄⣀⡤⠊⠀⠀⠀⠤⣼⠟⠀⠀⢹⡂
	⠊⣿⡠⠆⠀⠀⠀⠈⠉⠉⠙⠤⠤⠋⠀⠀⠀⠀⠀⠀⡰⠋⠀⠀⠀⡼⠁
	⢀⡾⣧⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠜⠁⠀⠀⠀⣸⠃
	⠀⠀⡼⠙⠢⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣰⠃
	⢀⡞⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣰⠃⠀⠀
	⡼⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀
	⣾⠁⠀⢀⣠⡴⠆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀
	⠈⠛⠻⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀
    )";

	// Split de ASCII-art in regels
	std::vector<std::string> lines;
	size_t startPos = 0;
	size_t endPos = 0;
	while ((endPos = ascii.find('\n', startPos)) != std::string::npos)
	{
		lines.push_back(ascii.substr(startPos, endPos - startPos));
		startPos = endPos + 1;
	}
	lines.push_back(ascii.substr(startPos)); // Voeg de laatste regel toe

	// Pool van strings voor firstMessage1
	std::vector<std::string> firstMessage1Options = {"Hiya!",
													"YO!",
													"Whazzzaaa!!"};

	// Pool van strings voor firstMessage2
	std::vector<std::string> firstMessage2Options = {"金をくれ",
													"YuGiOh > Magic",
													"Got MILK?",
													"Squirtle wordt gewoon een dikke watertank met kanonnen",
													"Yoshi's Story is echt een !#^@*% spel"};

	// Willekeurig selecteren van strings uit de pools
	std::srand(std::time(0)); // Zaden voor de willekeurige getallengenerator
	std::string firstMessage1 = firstMessage1Options[std::rand() % firstMessage1Options.size()];
	std::string firstMessage2 = firstMessage2Options[std::rand() % firstMessage2Options.size()];

	// Construeer de gecombineerde berichten
	std::string firstMessageCombined = "   " + firstMessage1 + " " + firstMessage2;
	std::string secondMessage = "    Connect to " + serverAddress + " using port " + serverPort;

	// Bereken de positie om de berichten in te voegen (midden van de ASCII-art)
	std::size_t middleLine = lines.size() / 4;
	std::size_t startPosition = std::min(lines[middleLine].size(), lines[middleLine].size() + firstMessageCombined.size());

	std::size_t middleLine2 = lines.size() / 3.5;
	std::size_t startPosition2 = std::min(lines[middleLine2].size(), lines[middleLine2].size() + secondMessage.size());

	// Voeg de berichten in op de juiste regels
	lines[middleLine].insert(startPosition, firstMessageCombined);
	lines[middleLine2].insert(startPosition2, secondMessage);

	// Toon de aangepaste ASCII-art
	for (const auto &line : lines)
	{
		std::cout << line << std::endl;
	}
}