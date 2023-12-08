#define _CRT_SECURE_NO_WARNINGS
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
	int cardRank;
	//SUIT cardSuit;
	char cardSuit[6];
	bool isDeploy;

}TrumpInfo;

typedef struct { //プレイヤーの構造体
	char name[20];
	int point;
	int id;
	TrumpInfo* deck[SUIT_COUNT * RANK_COUNT];
	int numOfhaveTrump;
}PlayerInfo;

int MakePlayer(PlayerInfo**);

void poker(void);

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

	if (memberPtr != NULL) {
		for (int i = 0; i < playerCount; i++) {
			printf("プレイヤー%dの名前を入力：", i + 1);

			scanf("%s", (*(*memberPtr + i)).name);
			//sprintf((*(*memberPtr + i)).name, "\x1b[31mPLAYER%d\x1b[0m", i + 1);
			(*(*memberPtr + i)).id = i;
		}
		for (int i = playerCount; i < playerCount + cpuCount; i++) {
			sprintf((*(*memberPtr + i)).name, "COM%d", i - playerCount + 1);
			(*(*memberPtr + i)).id = 100 + i;
		}
		for (int i = 0; i < playerCount + cpuCount; i++) {
			printf("member[%d].name is %s\n", i, (*(*memberPtr + i)).name);
			printf("member[%d].id is %03d\n", i, (*(*memberPtr + i)).id);
		}
	}
	return playerCount + cpuCount;
}

void poker(void) { //ポーカー
	srand((unsigned int)time(NULL));
	PlayerInfo* member = NULL;
	int numOfMember = 0;
	char suit[][6] = { "spade", "clab", "heart", "dia" };
	TrumpInfo trump[RANK_COUNT][SUIT_COUNT] = { 0,(SUIT)0 };
	TrumpInfo* deck[RANK_COUNT * SUIT_COUNT] = { NULL };

	//トランプカードの作成
	for (int i = 0; i < RANK_COUNT; i++) {
		for (int j = 0; j < SUIT_COUNT; j++) {
			trump[i][j].cardRank = i;
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

	for (int i = 0; i < RANK_COUNT * SUIT_COUNT; i++) {
		ick(deck[i]->cardRank);
		sck(deck[i]->cardSuit);
		ick(deck[i]->isDeploy);
	}

	//プレイヤーの作成
	numOfMember = MakePlayer(&member, false);

	for (int i = 0; i < numOfMember; i++) {
		sck(member[i].name);
		ick(member[i].id)
	}

}

