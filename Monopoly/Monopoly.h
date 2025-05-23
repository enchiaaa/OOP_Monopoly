#pragma once
#include <vector>
#include <string>

class Game;
class Player;


namespace Monopoly {
	extern Game* game;
	extern Player bank;

	enum class Color {
		Red = 31,
		Green = 32,
		Yellow = 33,
		Blue = 34,
		Magenta = 35,
		Cyan = 36,
		White = 37,
		Reset = 0
	};

	enum class BackgroundColor {
		Black = 40,
		Red = 41,
		Green = 42,
		Yellow = 43,
		Blue = 44,
		Magenta = 45,
		Cyan = 46,
		White = 47,
		Reset = 0
	};

	std::string GetColorCode(std::string colorName);
	std::string GetBackgroundColorCode(std::string color);
	void SetTextColor(const Color& color);
	void SetBackgroundColor(const BackgroundColor& color);
	void UpdateScreen();
	// withMap�ѼơAtrue ��ܦa�ϩM���a�A false ����ܡA�Q�n���a�ϥ�Monopoly::GetUserChoice(question, options, true);
	/**
	 * ��ܿﶵ�M��A�ھڤW�U��������ﶵ�C
	 * 
	 * \param question
	 * \param options
	 * \param withMap �O�_��ܦa��
	 * \param cmd �O�_���J���O
	 * \return 
	 */
	int GetUserChoice(const std::string& question, const std::vector<std::string>& options, bool withMap = false, bool cmd = false);
	void WaitForEnter();
	void clearScreen();
	/**
	 * ����ms�@��.
	 */
	void sleepMS(const int& ms);
	/**
	 * ����s��.
	 */
	void sleepS(const int& s);
	std::string tappingCommand();
}
