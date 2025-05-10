#pragma once

#include "Player.h"
#include "Map.h"
#include <fstream>
#include <string>

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

	/**
	 * �O�s�C���i�ר��ɮ�
	 * @param filename �O�s�ɮת����|
	 * @return �O�_���\�O�s
	 */
	bool SaveGame(const std::string& filename = "save.json");

	/**
	 * �q�ɮ׸��J�C���i��
	 * @param filename ���J�ɮת����|
	 * @return �O�_���\���J
	 */
	bool LoadGame(const std::string& filename = "save.json");

	/**
	 * �ˬd�O�_���O�s���C���i��
	 * @param filename �O�s�ɮת����|
	 * @return �O�_���O�s���i��
	 */
	bool HasSavedGame(const std::string& filename = "save.json");

	/**
	 * �R���O�s���C���i��
	 * @param filename �O�s�ɮת����|
	 * @return �O�_���\�R��
	 */
	bool DeleteSaveGame(const std::string& filename = "save.json");

	/**
	 * �B�z���a�}�������p
	 * @param bankruptPlayer �}�������a
	 * @param creditor ���v���a�]�ɭP�}�������a�^
	 */
	void HandlePlayerBankruptcy(Player* bankruptPlayer, Player* creditor);

	Player* getCurrentPlayer();

	Map* gameMap;					// �a�Ϫ���
private:
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

