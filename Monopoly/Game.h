#pragma once

#include "Player.h"
#include "Map.h"

enum class State { INIT, START, MOVED, ROUND_END, FINISH };

class Game
{
public:
	/**
	 * �غc�l�A��l�ƹC�����A�]���a���ޡB�C���O�_�����^
	 */
	Game();

	/**
	 * ��l�Ʀa�ϻP���a��T�A�ÿ�ܤH�ƻP�W��
	 */
	void InitGame();

	/**
	 * �ҰʹC���D�y�{�A�C�^�X���檽��ӧQ����F��
	 */
	void StartGame();

	/**
	 * �����^�X�y�{�]�Y��B���ʡBĲ�o��l�ƥ�B���H�^
	 */
	void NextTurn();

	/**
	 * �P�_�O�_�����a�F���ӧQ����]�p����F�С^
	 * @return �Y�����a��ӫh�^�� true
	 */
	bool CheckWinCondition();

	/**
	 * �C����������ܩҦ����a�̲׸�T
	 */
	void EndGame();


	void HandlePlayerAction(Player* p);

	/**
	 * �M���e���A�í��s�L�X�a�ϻP���a��T
	 */
	void Clear();

	/**
	 * �L�X�ثe�a�Ϫ��A�]��l�W�١B�֦��̵��^
	 */
	void PrintMapStatus();

	/**
	 * �L�X�Ҧ����a�����A�]��m�B�����B�D�㵥�^
	 */
	void PrintPlayerStatus();

private:
	Map* gameMap;					// �a�Ϫ���
	std::vector<Player*>players;	// �Ҧ����a����
	int currentPlayerIdx;			// �ثe�^�X�����a����
	bool gameOver;					// �O�_�����C��

	/**
	 * ���U�禡�G�L�X������l�� ASCII �Ϯ�
	 */
	void PrintDice(int, int);

	/**
	 * ��������ʵe�æ^���`�I��
	 */
	int RollDiceWithAsciiAnimation();

	/**
	 * ��ܿ�����ϥΪ̥H ���� ��ܿﶵ
	 * @param question ���ݤ�r
	 * @param options �i�ﶵ�ء]�r��}�C�^
	 * @return �^�ǨϥΪ̿�ܪ�����
	 */
	int GetUserChoice(const std::string question, const std::vector<std::string> options);

	/**
	 * �Ȱ��e������ϥΪ̫��U Enter
	 */
	void WaitForEnter();

	/**
	* �ھ��C��W�٨��o�������C��X�]��r�C��^
	* @param colorName �C��W�١]�p "red", "green", "blue" ���^
	* @return �������C��X
	*/
	std::string GetColorCode(std::string colorName);

	/**
	* �ھ��C��W�٨��o�������C��X�]�I���C��^
	* @param colorName �I���C��W�١]�p "red", "green", "yellow" ���^
	* @return �������I���C��X
	*/
	std::string GetBackgroundColorCode(std::string colorName);
};

