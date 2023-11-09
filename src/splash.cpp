#include <cstdio>
#include <unistd.h>
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include "./../incs/splash.hpp"

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

	// Split the ASCII ascii into lines
	std::vector<std::string> lines;
	size_t startPos = 0;
	size_t endPos = 0;
	while ((endPos = ascii.find('\n', startPos)) != std::string::npos)
	{
		lines.push_back(ascii.substr(startPos, endPos - startPos));
		startPos = endPos + 1;
	}
	lines.push_back(ascii.substr(startPos)); // Add the last line

	// Construct the message
	std::string message1part1 = "   Hiya!";
	std::string message1part2 = "金をくれ";
	std::string message = message1part1 + " " + message1part2;
	std::string message2 = "    Connect to " + serverAddress + " using port " + serverPort;

	// Calculate the position to insert the message (middle of the ascii)
	std::size_t middleLine = lines.size() / 4;
	std::size_t startPosition = std::min(lines[middleLine].size(), lines[middleLine].size() + message.size());

	std::size_t middleLine2 = lines.size() / 3.5;
	std::size_t startPosition2 = std::min(lines[middleLine2].size(), lines[middleLine2].size() + message2.size());

	// Insert the message into the appropriate line
	lines[middleLine].insert(startPosition, message);
	lines[middleLine2].insert(startPosition2, message2);

	// Output the modified ASCII ascii
	for (const auto &line : lines)
	{
		std::cout << line << std::endl;
	}
}
