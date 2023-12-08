#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "debuglib.h"
#define RANK_COUNT 13 //�g�����v�̐���
#define SUIT_COUNT 4 //�g�����v�̊G�D�̐�

typedef enum {
	spade,
	clab,
	heart,
	dia
}SUIT;


typedef struct { //�g�����v�̍\����
	int cardRank;
	//SUIT cardSuit;
	char cardSuit[6];
	bool isDeploy;

}TrumpInfo;

typedef struct { //�v���C���[�̍\����
	char name[20];
	int point;
	int id;
	TrumpInfo* deck[2];
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
		printf("�v���C���[�̐�����́F");
		scanf("%d", &playerCount);
	}
	printf("com�̐l������́F");
	scanf("%d", &cpuCount);

	*memberPtr = (PlayerInfo*)calloc(sizeof(PlayerInfo), playerCount + cpuCount);

	if (memberPtr != NULL) {
		for (int i = 0; i < playerCount; i++) {
			printf("�v���C���[%d�̖��O����́F", i + 1);

			scanf("%s", (*memberPtr)[i].name);
			//sprintf((*memberPtr)[i].name, "\x1b[31mPLAYER%d\x1b[0m", i + 1);
			(*memberPtr)[i].id = i;
		}
		for (int i = playerCount; i < playerCount + cpuCount; i++) {
			sprintf((*memberPtr)[i].name, "COM%d", i - playerCount + 1);
			(*memberPtr)[i].id = 100 + i;

		}
		for (int i = 0; i < playerCount + cpuCount; i++) {
			printf("member[%d].name is %s\n", i, (*memberPtr)[i].name);
			printf("member[%d].id is %03d\n", i, (*memberPtr)[i].id);
		}
	}
	return playerCount + cpuCount;
}

void poker(void) { //�|�[�J�[
	srand((unsigned int)time(NULL));
	PlayerInfo* member = NULL;
	int numOfMember = 0;
	char suit[][6] = { "spade", "clab", "heart", "dia" };
	TrumpInfo trump[RANK_COUNT][SUIT_COUNT] = { 0,(SUIT)0 };
	TrumpInfo* deck[RANK_COUNT * SUIT_COUNT] = { NULL };

	//�g�����v�J�[�h�̍쐬
	for (int i = 0; i < RANK_COUNT; i++) {
		for (int j = 0; j < SUIT_COUNT; j++) {
			trump[i][j].cardRank = i;
			//trump[i][j].cardSuit = (SUIT)j;
			strcpy(trump[i][j].cardSuit, suit[j]);
			trump[i][j].isDeploy = false;
		}
	}
	
	//�R�D�̍쐬
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

	//�R�D�̊m�F
	for (int i = 0; i < RANK_COUNT * SUIT_COUNT; i++) {
		ick(i);
		ick(deck[i]->cardRank);
		sck(deck[i]->cardSuit);
		ick(deck[i]->isDeploy);
		printf("\n");
	}

	numOfMember = MakePlayer(&member, false); //�v���C���[�̍쐬
	for (int i = 0; i < numOfMember; i++) { //�����|�C���g�̐ݒ�
		member[i].point = 1000;
	}

	//�v���C���[�̊m�F
	for (int i = 0; i < numOfMember; i++) {
		sck(member[i].name);
		ick(member[i].id)
	}

	bool testflag = true;

	//mainloop
	while (testflag) {
		TrumpInfo** currMasterDeck = &deck;
		//ick((*(currMasterDeck + 1))->cardRank);
		for (int i = 0; i < numOfMember; i++) {
			for (int j = 0; j < 2; j++) {
				member[i].deck[j] = *currMasterDeck;
				ick(i);
				sck(member[i].name);
				ick(member[i].deck[j]->cardRank);
				sck(member[i].deck[j]->cardSuit);
				printf("\n");
				currMasterDeck++;
			}
		}
		scanf("%s");
	}
}

