#include <iostream>

#include "Game.h"
#include "Monopoly.h"

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
		Monopoly::sleepMS(300);
	}
	Monopoly::sleepMS(1000);
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

		int choice = Monopoly::GetUserChoice(question, options);

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
