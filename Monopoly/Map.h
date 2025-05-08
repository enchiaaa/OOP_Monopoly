#pragma once

#include <iostream>

#include "Tile.h"
#include "Player.h"

class Map
{
public:
	/**
	 * �غc�禡�G�ئn�@�i�a��
	 */
	Map();

	/**
	 * �q�ɮ׸��J�a�ϰt�m�]���Τ���ա^
	 * @param path �ɮ׸��|
	 */
	void LoadMap(std::string path);

	/**
	 * �L�X�a�ϡA���K�Ъ`���a��m
	 * @param players ���a�C��
	 */
	void PrintMap(std::vector<Player*>& players);

	/**
	* ���o��Ӧa�Ϫ���T�]��l��W�١^
	* @return ��l���и�a�W���t��
	*/
	std::pair<std::vector<Tile*>, std::vector<std::string>> getMap();

	/**
	 * �[��l�i�a�ϡA�s�W�r�@�_��
	 * @param tile ��l����
	 * @param name �a�W
	 */
	void addTile(Tile* tile, std::string name);

	/**
	 * ��X�Y��m����l
	 * @param pos ��m
	 * @return ���檺����
	 */
	Tile* GetTileAt(int pos);

	/**
	 * �ݦa�Ϧ��X��
	 */
	int getSize();

private:
	std::vector<Tile*> map;               // �a�Ϯ�}�C
	std::vector<std::string> names;       // �����C�檺�a�W�]��ܥΡ^

	/**
	 * �إX�@�i�w�]�a��
	 */
	void InitMap();
};

