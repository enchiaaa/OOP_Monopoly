#include <iostream>
#include <vector>
#include <string>
#include <conio.h>
#include <thread>

#include "Game.h"
#include "Monopoly.h"

// �@��²�檺���\��A�]��Game�����٥�������l��
int SimpleUserChoice(const std::string& question, const std::vector<std::string>& options) {
	int selected = 0;

	while (true) {
		system("cls"); // �M���e��
		std::cout << question << std::endl << std::endl;

		for (size_t i = 0; i < options.size(); ++i) {
			if (static_cast<int>(i) == selected)
				std::cout << " > " << "�i" << options[i] << "�j" << "\n";
			else
				std::cout << "   " << "�i" << options[i] << "�j" << "\n";
		}

		int key = _getch(); // ���o����
		if (key == 224) {   // �S�����
			key = _getch();
			if (key == 72) selected = (selected - 1 + static_cast<int>(options.size())) % static_cast<int>(options.size()); // �W
			if (key == 80) selected = (selected + 1) % static_cast<int>(options.size()); // �U
		}
		else if (key == '\r') { // Enter
			return selected;
		}
	}
}

void ShowWaveLogo() {
	std::vector<std::string> logo[2] = {
		{
		"��������������������������������������������������������������������������������������������������������",
		"��  __  __       _   _       ____        _          ��",
		"�� |  \\/  | ___ | \\ | | ___ |  _ \\  ___ | | __   __ ��",
		"�� | |\\/| |/ _ \\|  \\| |/ _ \\| |_) |/ _ \\| | \\ \\ / / ��",
		"�� | |  | | | | | |\\  | | | |  __/| | | | |__\\ V /  ��",
		"�� |_|  |_| |_| |_| \\_| |_| |_|   | |_| |____|| |   ��",
		"��         \\___/       \\___/       \\___/      |_|   ��",
		"��                                                  ��",
		"��������������������������������������������������������������������������������������������������������"
		},
		{
		"��������������������������������������������������������������������������������������������������������",
		"��          ___         ___         ___     __   __ ��",
		"��  __  __ / _ \\ _   _ / _ \\ ____  / _ \\ _  \\ \\ / / ��",
		"�� |  \\/  | | | | \\ | | | | |  _ \\| | | | |  \\ V /  ��",
		"�� | |\\/| | |_| |  \\| | |_| | |_) | |_| | |   | |   ��",
		"�� | |  | |\\___/| |\\  |\\___/|  __/ \\___/| |__ |_|   ��",
		"�� |_|  |_|     |_| \\_|     |_|         |____|      ��",
		"��                                                  ��",
		"��������������������������������������������������������������������������������������������������������"
		}
	};

	std::string loadingText[] = {
		"                    �C�����J��   ",
		"                   �C�����J��.  ",
		"                  �C�����J��.. ",
		"                 �C�����J��..."
	};


	for (int i = 0; i < 10; i++) {
		system("cls");
		std::cout << "\n";

		if (i % 2 == 0) {
			for (int j = 0; j < logo[0].size(); j++) {
				std::cout << logo[0][j] << std::endl;
			}
		}
		else if (i % 2 == 1) {
			for (int j = 0; j < logo[1].size(); j++) {
				std::cout << logo[1][j] << std::endl;
			}
		}
		std::cout << "\n" << loadingText[i % 4] << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(300));
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

int main() {
	Game game;
	Monopoly::game = &game;

	ShowWaveLogo();
	system("cls");

	// �ˬd�O�_���s��
	if (game.HasSavedGame()) {
		std::cout << "�o�{�W�����������C���i�סI" << std::endl;

		// ���ѿﶵ
		std::string question = "�O�_�n���J�W�����C���i�סH";
		std::vector<std::string> options = {
			"���J�¹C��",
			"�}�l�s�C��"
		};

		// �ϥοW�ߪ����\��A�Ӥ��O�̿�Game������k
		int choice = SimpleUserChoice(question, options);

		if (choice == 0) {
			// ���J�¹C��
			if (game.LoadGame()) {
				std::cout << "�~��W�����C���I" << std::endl;
				game.StartGame();
			}
			else {
				std::cout << "���J���ѡA�N�}�l�s�C���C" << std::endl;
				game.InitGame();
				game.StartGame();
			}
		}
		else {
			// �}�l�s�C���e�R���¦s��
			game.DeleteSaveGame();
			game.InitGame();
			game.StartGame();
		}
	}
	else {
		// �S���s�ɡA���`�}�l�C��
		game.InitGame();
		game.StartGame();
	}

	return 0;
}