#pragma once

#include <iostream>

#include "Tile.h"
#include "Player.h"

class Map
{
public:
	/**
	 * �غc�禡�G��l�Ʀa��
	 */
	Map();

	/**
	 * �q�ɮ׸��J�a�ϰt�m�]����@�ΫO�d�����X�R�^
	 * @param path �ɮ׸��|
	 */
	void LoadMap(std::string path);

	/**
	 * ��X��i�a�Ϫ����A�A�]�t���a��m
	 * @param players ���a�C��
	 */
	void PrintMap(std::vector<Player*>& players);

	/**
	* ���o�C���a�Ϫ���T
	* @return ��^�]�t��ӦV�q�� `std::pair`�G
	*         - �Ĥ@�ӦV�q�O�]�t�a�Ϯ�l�]`Tile*`�^�����СA�N��C�������U�خ�l�]�Ҧp�_�I�B�a���B�ө����^
	*         - �ĤG�ӦV�q�O�]�t������l�W�٪��r��]`std::string`�^�A�C�Ӯ�l�����������W�٩μ���
	*/
	std::pair<std::vector<Tile*>, std::vector<std::string>> getMap();

	/**
	 * �N�a�Ϯ�[�J�a��
	 * @param tile �a�Ϯ����]�~�Ӧ� Tile�^
	 * @param name �a�W�]�Ω���ܡ^
	 */
	void addTile(Tile* tile, std::string name);

	/**
	 * ���o���w��m���a�Ϯ檫��
	 * @param pos �y�Ц�m�]��ơ^
	 * @return ���w��m�W�� Tile*
	 */
	Tile* GetTileAt(int pos);

	/**
	 * ���o�a�Ϥj�p�]��l�`�ơ^
	 */
	int getSize();

private:
	std::vector<Tile*> map;               // �a�Ϯ�}�C
	std::vector<std::string> names;       // �����C�檺�a�W�]��ܥΡ^

	/**
	 * ��l�ƹw�]�a�ϰt�m
	 */
	void InitMap();
};

