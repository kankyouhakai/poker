#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "debuglib.h"
#define RANK_COUNT 13 //トランプの数字
#define SUIT_COUNT 4 //トランプの絵札の数

typedef enum {
	spade,
	clab,
	heart,
	dia
}SUIT;

typedef struct { //トランプの構造体
	int cardRank; //数字
	//SUIT cardSuit;
	char cardSuit[6]; //絵札
	bool isDeploy; //配置されているか

}TrumpInfo;

typedef struct { //プレイヤーの構造体
	char name[20]; //名前
	int point; //ポイント
	int id;
	TrumpInfo* deck[5]; //手札
	//int numOfDeck; 
	//type role; //役
}PlayerInfo;

int MakePlayer(PlayerInfo**, bool); //プレイヤー作成

void poker(void); //ポーカー実行

void RoleJudge(PlayerInfo*); //役判定

int main(void) {
	poker();

	return 0;
}

int MakePlayer(PlayerInfo** memberPtr, bool isMultPlayer) {
	int cpuCount = 0;
	int playerCount = 1;
	if (isMultPlayer) {
		printf("プレイヤーの数を入力：");
		scanf("%d", &playerCount);
	}
	printf("comの人数を入力：");
	scanf("%d", &cpuCount);

	*memberPtr = (PlayerInfo*)calloc(sizeof(PlayerInfo), playerCount + cpuCount);
	if (*memberPtr != NULL) { //メンバーが正しく作成されたか
		for (int i = 0; i < playerCount; i++) {
			printf("プレイヤー%dの名前を入力：", i + 1);

			scanf("%s", (*memberPtr)[i].name);
			//sprintf((*memberPtr)[i].name, "\x1b[31mPLAYER%d\x1b[0m", i + 1);
			(*memberPtr)[i].id = i;
		}
		for (int i = playerCount; i < playerCount + cpuCount; i++) {
			sprintf((*memberPtr)[i].name, "COM%d", i - playerCount + 1);
			(*memberPtr)[i].id = 100 + i;

		}
		/*for (int i = 0; i < playerCount + cpuCount; i++) {
			printf("member[%d].name is %s\n", i, (*memberPtr)[i].name);
			printf("member[%d].id is %03d\n", i, (*memberPtr)[i].id);
		}*/
	}
	return playerCount + cpuCount;
}

void poker(void) { //ポーカー
	srand((unsigned int)time(NULL));
	PlayerInfo* member = NULL;
	int numOfMember = 0;
	const char suit[][6] = { "spade", "clab", "heart", "dia" };
	TrumpInfo trump[RANK_COUNT][SUIT_COUNT] = { 0,(SUIT)0 };
	TrumpInfo* deck[RANK_COUNT * SUIT_COUNT] = { NULL };
	TrumpInfo** currMasterDeck = deck;

	//トランプカードの作成
	for (int i = 0; i < RANK_COUNT; i++) {
		for (int j = 0; j < SUIT_COUNT; j++) {
			trump[i][j].cardRank = i+1;
			//trump[i][j].cardSuit = (SUIT)j;
			strcpy(trump[i][j].cardSuit, suit[j]);
			trump[i][j].isDeploy = false;
		}
	}
	
	//山札の作成
	for (int i = 0; i < RANK_COUNT * SUIT_COUNT; i++) {
		int randRank = 0;
		int randSuit = 0;
		do {
			randRank = rand() % RANK_COUNT;
			randSuit = rand() % SUIT_COUNT;
		} while (trump[randRank][randSuit].isDeploy == true);
		deck[i] = &trump[randRank][randSuit];
		deck[i]->isDeploy = true;
	}

	//山札の確認
	for (int i = 0; i < RANK_COUNT * SUIT_COUNT; i++) {
		ick(i);
		ick(deck[i]->cardRank);
		sck(deck[i]->cardSuit);
		printf("\n");
	}

	numOfMember = MakePlayer(&member, false); //プレイヤーの作成
	for (int i = 0; i < numOfMember; i++) { //初期ポイントの設定
		member[i].point = 1000;
	}

	//プレイヤーの確認
	//for (int i = 0; i < numOfMember; i++) {
	//	sck(member[i].name);
	//	ick(member[i].id)
	//}

	bool testflag = true;
	//mainloop
	while (testflag) {
		//手札の配布
		for (int i = 0; i < numOfMember; i++) {
			for (int j = 0; j < 5; j++) {
				member[i].deck[j] = *currMasterDeck;
				currMasterDeck++;
			}
			RoleJudge(&member[i]); //役の判定
		}
		scanf("%s");

	}
}

void RoleJudge(PlayerInfo* player) {
	sck(player->name);
	ick(player->deck[0]->cardRank);
	sck(player->deck[0]->cardSuit);
}


