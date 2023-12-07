#pragma once
#include <stdbool.h>

#define RANK_COUNT 3
#define SUIT_COUNT 4


typedef enum {
	spade,
	clab,
	heart,
	dia,
	joker
}SUIT;

typedef struct {
	int cardRank;
	//SUIT cardSuit;
	char cardSuit[6];
	bool isDeploy;

}TrumpInfo;

typedef struct {
	char name[20];
	int point;
	int id;
	TrumpInfo* haveTrump[SUIT_COUNT * RANK_COUNT];
	int numOfhaveTrump;
}PlayerInfo;

int MakePlayer(PlayerInfo** memberPtr) {
	int cpuCount = 0;
	int playerCount = 0;

	printf("プレイヤーの数を入力：");
	scanf("%d", &playerCount);
	printf("comの人数を入力：");
	scanf("%d", &cpuCount);

	*memberPtr = (PlayerInfo*)calloc(sizeof(PlayerInfo), playerCount + cpuCount);
	//*memberPtr == member
	//member[i] == (*member)[i]
	//member[i].name == ((*member)[i]).name
	//a[i] == *(a + i)
	//(*member)[i].name == (*(*member + i)).name
	//(*a).mem == a->mem * は内側
	// *(a + i) == a[i]      * は外側
	//(*(*member + i)).name == (*(member + i))->name

	if (memberPtr != NULL) {
		for (int i = 0; i < playerCount; i++) {
			printf("プレイヤー%dの名前を入力：", i + 1);

			scanf("%s", (*(*memberPtr + i)).name);
			//sprintf((*(*memberPtr + i)).name, "\x1b[31mPLAYER%d\x1b[0m", i + 1);
			(*(*memberPtr + i)).id = i;
			//ick((*(*memberPtr + i)).id); //OK
			//ick(( (*memberPtr) + i)->id); //OK  member[i]のアドレス＋アロー
			//ick((*memberPtr)[i].id);     //OK   実体member[i]+ドット
			//ick((&((*memberPtr)[i]))->id);
			//ick(((memberPtr[0]) + i)->id);
			//ick((memberPtr[0])[i].id);
			//ick(memberPtr[0][i].id);
			//member == *memberPtr
			// memberPtr == &member
			//member[i].id
			//(*(member + i)).id
			//(*((*memberPtr) + i)).id
			//(*(( memberPtr) + i))->id
			// a[i][j] == *(a[i] + j)
			//*(a[i] + j) == *(*(a + i) + j);;;

			//演算子の優先順位は　'[]' = '.' = '->' > '*'
			// ick(memberPtr[i]->id)     NG (優先度共に1のため左から評価される)
			//ick((*memberPtr[i]).id);   NG ( '*' < '[]' だから)
			//ick((*(memberPtr[i])).id); NG

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