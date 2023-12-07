#pragma once

void MemoryGame(TrumpInfo trump[RANK_COUNT][SUIT_COUNT]) {
	srand((unsigned int)time(NULL));

	int currRank = 0;
	int currSuit = 0;
	int x = 0;
	int y = 0;
	int sucCount = 0;
	int gameTurn = 0;
	int numOfPlayers = 0;
	PlayerInfo* currPlayer;
	PlayerInfo** playerRank = NULL;

	PlayerInfo* member = NULL; //= MakePlayer();

	numOfPlayers = MakePlayer(&member);
	playerRank = (PlayerInfo**)calloc(numOfPlayers, sizeof(PlayerInfo**));
	pck(playerRank);
	//memcpy(playerRank, &member, sizeof(PlayerInfo**));
	for (int i = 0; i < sizeof(PlayerInfo**); i++) {
		playerRank[i] = &member[i];
	}

	if (member != NULL) {
		TrumpInfo* field[RANK_COUNT][SUIT_COUNT] = { {0} };
		TrumpInfo* choiceTrump[2] = { 0 };

		for (x = 0; x < RANK_COUNT; x++) {
			for (y = 0; y < SUIT_COUNT; y++) {
				do {
					currRank = rand() % RANK_COUNT;
					currSuit = rand() % SUIT_COUNT;
				} while (trump[currRank][currSuit].isDeploy == true);
				field[x][y] = &trump[currRank][currSuit];
				field[x][y]->isDeploy = true;
				printf("(%2d, %2d) is %5s of %2d\n", x, y, field[x][y]->cardSuit, field[x][y]->cardRank);
			}
		}

		//mainloop
		while (RANK_COUNT * SUIT_COUNT / 2 > sucCount) {
			currPlayer = &member[gameTurn % numOfPlayers];
			ick(gameTurn);
			sck(currPlayer->name);
			for (int i = 0; i < 2;) {
				scanf("%d", &x);
				scanf("%d", &y);
				if (field[x][y]->isDeploy == true) {
					choiceTrump[i] = field[x][y];
					choiceTrump[i]->isDeploy = false;
					printf("(%2d, %2d) is %5s of %2d\n", x, y, choiceTrump[i]->cardSuit, choiceTrump[i]->cardRank);
					i++;
				}
				else {
					printf("Ä“ü—Í\n");
				}
			}

			if (choiceTrump[0]->cardRank == choiceTrump[1]->cardRank) {
				for (int i = 0; i < 2; i++) {
					choiceTrump[i]->isDeploy = false;
					currPlayer->haveTrump[currPlayer->numOfhaveTrump] = choiceTrump[i];
					currPlayer->numOfhaveTrump++;
					ick(currPlayer->numOfhaveTrump);
				}
				sucCount++;
			}
			else {
				printf("Žc”O!\n");
				choiceTrump[0]->isDeploy = choiceTrump[1]->isDeploy = true;
				gameTurn++;

			}
		}
		for (int i = 0; i < numOfPlayers - 1; i++) {
			for (int j = i + 1; j < numOfPlayers; j++) {
				if (playerRank[i]->numOfhaveTrump < playerRank[j]->numOfhaveTrump) {
					PlayerInfo* tmp = playerRank[i];
					playerRank[i] = playerRank[j];
					playerRank[j] = tmp;

				}
			}
		}
		/*for (int i = 0; i < numOfPlayers; i++) {
			sck(playerRank[i]->name);
		}*/

		ick(__LINE__);
		free(member);
		ick(__LINE__);
		//free(playerRank);
		ick(__LINE__);
	}
}
