#define _CRT_SECURE_NO_WARNINGS
//#define ESC 0x1b
//#define CSI 0x5b
//#define VN "38;2;156;220;254m"
//#define VV "38;2;181;206;168m"
//#define FN "38;2;220;220;170m"
//#define ick(value) printf("line %3d: \x1b[%s%s\x1b[0m | \x1b[%s%d\x1b[0m | \x1b[%s%s\x1b[0m\n", __LINE__, VN, #value, VV, value, FN, __func__);
//#define cck(character) printf("line %3d: \x1b[%s%s\x1b[0m | \x1b[%s%c\x1b[0m | \x1b[%s%s\x1b[0m\n", __LINE__, VN, #character, VV, character, FN, __func);
//#define sck(string) printf("line %3d: \x1b[%s%s\x1b[0m | \x1b[%s%s\x1b[0m | \x1b[%s%s\x1b[0m\n", __LINE__, VN, #string, VV, string, FN, __func__);
//#define pck(pointer) printf("line %3d: \x1b[%s%s\x1b[0m | \x1b[%s%p\x1b[0m | \x1b[%s%s\x1b[0m\n", __LINE__, VN, #pointer, VV, pointer, FN, __func__);

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "debugMacro.h"
#include "trumpGameBase.h"
#include "memryGame.h"
//#define RANK_COUNT 3
//#define SUIT_COUNT 4

//typedef enum {
//	spade,
//	clab,
//	heart,
//	dia,
//	joker
//}SUIT;
//
//typedef struct {
//	int cardRank;
//	//SUIT cardSuit;
//	char cardSuit[6];
//	bool isDeploy;
//
//}TrumpInfo;

//typedef struct {
//	char name[20];
//	int point;
//	int id;
//	TrumpInfo* haveTrump[SUIT_COUNT * RANK_COUNT];
//	int numOfhaveTrump;
//}PlayerInfo;

int main(void) {
	//printf("\x1b[31m");
	//トランプカードの作成
	
	char suit[][6] = { "spade", "clab", "heart", "dia" };
	TrumpInfo trump[RANK_COUNT][SUIT_COUNT] = { 0,(SUIT)0 };
	for (int i = 0; i < RANK_COUNT; i++) {
		for (int j = 0; j < SUIT_COUNT; j++) {
			trump[i][j].cardRank = i;
			//trump[i][j].cardSuit = (SUIT)j;
			strcpy(trump[i][j].cardSuit, suit[j]);
			trump[i][j].isDeploy = false;
		}
	}

	//神経衰弱
	MemoryGame(trump);
	
	return 0;
}


//int MakePlayer(PlayerInfo** memberPtr) {
//	int cpuCount = 0;
//	int playerCount = 0;
//
//	printf("プレイヤーの数を入力：");
//	scanf("%d", &playerCount);
//	printf("comの人数を入力：");
//	scanf("%d", &cpuCount);
//
//	*memberPtr = (PlayerInfo*)calloc(sizeof(PlayerInfo), playerCount + cpuCount);
//	//*memberPtr == member
//	//member[i] == (*member)[i]
//	//member[i].name == ((*member)[i]).name
//	//a[i] == *(a + i)
//	//(*member)[i].name == (*(*member + i)).name
//	//(*a).mem == a->mem * は内側
//	// *(a + i) == a[i]      * は外側
//	//(*(*member + i)).name == (*(member + i))->name
//
//	if (memberPtr != NULL) {
//		for (int i = 0; i < playerCount; i++) {
//			printf("プレイヤー%dの名前を入力：", i + 1);
//
//			scanf("%s", (*(*memberPtr + i)).name);
//			//sprintf((*(*memberPtr + i)).name, "\x1b[31mPLAYER%d\x1b[0m", i + 1);
//			(*(*memberPtr + i)).id = i;
//			//ick((*(*memberPtr + i)).id); //OK
//			//ick(( (*memberPtr) + i)->id); //OK  member[i]のアドレス＋アロー
//			//ick((*memberPtr)[i].id);     //OK   実体member[i]+ドット
//			//ick((&((*memberPtr)[i]))->id);
//			//ick(((memberPtr[0]) + i)->id);
//			//ick((memberPtr[0])[i].id);
//			//ick(memberPtr[0][i].id);
//			//member == *memberPtr
//			// memberPtr == &member
//			//member[i].id
//			//(*(member + i)).id
//			//(*((*memberPtr) + i)).id
//			//(*(( memberPtr) + i))->id
//			// a[i][j] == *(a[i] + j)
//			//*(a[i] + j) == *(*(a + i) + j);;;
//
//			//演算子の優先順位は　'[]' = '.' = '->' > '*'
//			// ick(memberPtr[i]->id)     NG (優先度共に1のため左から評価される)
//			//ick((*memberPtr[i]).id);   NG ( '*' < '[]' だから)
//			//ick((*(memberPtr[i])).id); NG
//
//		}
//		for (int i = playerCount; i < playerCount + cpuCount; i++) {
//			sprintf((*(*memberPtr + i)).name, "COM%d", i - playerCount + 1);
//			(*(*memberPtr + i)).id = 100 + i;
//		}
//		for (int i = 0; i < playerCount + cpuCount; i++) {
//			printf("member[%d].name is %s\n", i, (*(*memberPtr + i)).name);
//			printf("member[%d].id is %03d\n", i, (*(*memberPtr + i)).id);
//		}
//	}
//	return playerCount + cpuCount;
//}


//void MemoryGame(TrumpInfo trump[RANK_COUNT][SUIT_COUNT]) {
//	srand((unsigned int)time(NULL));
//
//	int currRank = 0;
//	int currSuit = 0;
//	int x = 0;
//	int y = 0;
//	int sucCount = 0;
//	int gameTurn = 0;
//	int numOfPlayers = 0;
//	PlayerInfo* currPlayer;
//	PlayerInfo** playerRank = NULL;
//
//	PlayerInfo* member = NULL; //= MakePlayer();
//
//	numOfPlayers = MakePlayer(&member);
//	playerRank = (PlayerInfo**)calloc(numOfPlayers, sizeof(PlayerInfo**));
//	pck(playerRank);
//	//memcpy(playerRank, &member, sizeof(PlayerInfo**));
//	for (int i = 0; i < sizeof(PlayerInfo**); i++) {
//		playerRank[i] = &member[i];
//	}
//
//	if (member != NULL) {
//		TrumpInfo* field[RANK_COUNT][SUIT_COUNT] = { {0} };
//		TrumpInfo* choiceTrump[2] = { 0 };
//
//		for (x = 0; x < RANK_COUNT; x++) {
//			for (y = 0; y < SUIT_COUNT; y++) {
//				do {
//					currRank = rand() % RANK_COUNT;
//					currSuit = rand() % SUIT_COUNT;
//				} while (trump[currRank][currSuit].isDeploy == true);
//				field[x][y] = &trump[currRank][currSuit];
//				field[x][y]->isDeploy = true;
//				printf("(%2d, %2d) is %5s of %2d\n", x, y, field[x][y]->cardSuit, field[x][y]->cardRank);
//			}
//		}
//
//		//mainloop
//		while (RANK_COUNT * SUIT_COUNT / 2 > sucCount) {
//			currPlayer = &member[gameTurn % numOfPlayers];
//			ick(gameTurn);
//			sck(currPlayer->name);
//			for (int i = 0; i < 2;) {
//				scanf("%d", &x);
//				scanf("%d", &y);
//				if (field[x][y]->isDeploy == true) {
//					choiceTrump[i] = field[x][y];
//					choiceTrump[i]->isDeploy = false;
//					printf("(%2d, %2d) is %5s of %2d\n", x, y, choiceTrump[i]->cardSuit, choiceTrump[i]->cardRank);
//					i++;
//				}
//				else {
//					printf("再入力\n");
//				}
//			}
//
//			if (choiceTrump[0]->cardRank == choiceTrump[1]->cardRank) {
//				for (int i = 0; i < 2; i++) {
//					choiceTrump[i]->isDeploy = false;
//					currPlayer->haveTrump[currPlayer->numOfhaveTrump] = choiceTrump[i];
//					currPlayer->numOfhaveTrump++;
//					ick(currPlayer->numOfhaveTrump);
//				}
//				sucCount++;
//			}
//			else {
//				printf("残念!\n");
//				choiceTrump[0]->isDeploy = choiceTrump[1]->isDeploy = true;
//				gameTurn++;
//
//			}
//		}
//		for (int i = 0; i < numOfPlayers - 1; i++) {
//			for (int j = i + 1; j < numOfPlayers; j++) {
//				if (playerRank[i]->numOfhaveTrump < playerRank[j]->numOfhaveTrump) {
//					PlayerInfo* tmp = playerRank[i];
//					playerRank[i] = playerRank[j];
//					playerRank[j] = tmp;
//					
//				}
//			}
//		}
//		/*for (int i = 0; i < numOfPlayers; i++) {
//			sck(playerRank[i]->name);
//		}*/
//		RankOfGame();
//		ick(__LINE__);
//		free(member);
//		ick(__LINE__);
//		//free(playerRank);
//		ick(__LINE__);
//	}
//}
