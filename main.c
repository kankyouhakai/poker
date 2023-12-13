#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "debuglib.h"
#define RANK_COUNT 13   //�g�����v�̐���
#define SUIT_COUNT 4    //�g�����v�̊G�D�̐�

typedef enum {
    spade,
    clab,
    heart,
    dia
}SUIT;

typedef struct {        //�g�����v�̍\����
    int cardRank;       //����
    //SUIT cardSuit;
    char cardSuit[6];   //�G�D
    bool isDeploy;      //�z�u����Ă��邩

}TrumpInfo;

typedef struct {        //�����o�[�̍\����
    char name[20];      //���O
    int point;          //�|�C���g
    int chip;           //�`�b�v
    int id;
    TrumpInfo* deck[2]; //��D
    //int numOfDeck; 
    //type role; //��
}PlayerInfo;

int MakeMember(PlayerInfo**); //�����o�[�쐬

void poker(void);   //�|�[�J�[���s

TrumpInfo* ChoiceTrump(TrumpInfo trump[RANK_COUNT][SUIT_COUNT]);

void RoleJudge(PlayerInfo*, TrumpInfo* communityCard[5]); //�𔻒�


int main(void) {
    poker();

    return 0;
}

int MakeMember(PlayerInfo** memberPtr) {
    int cpuCount = 0;
    int playerCount = 1;
    int initChip = 0;
    printf("com�̐l������́F");
    scanf("%d", &cpuCount);

    *memberPtr = (PlayerInfo*)calloc(sizeof(PlayerInfo), playerCount + cpuCount);
    if (*memberPtr != NULL) { //�����o�[���������쐬���ꂽ��
        printf("�v���C���[�̖��O����́F");
        scanf("%s", (*memberPtr)[0].name);
        printf("�`�b�v�̏�����������́F");
        scanf("%d", &initChip);
        (*memberPtr)[0].id = 0; //�v���C���[ID�̐ݒ�
        (*memberPtr)[0].chip = initChip;

        for (int i = playerCount; i < playerCount + cpuCount; i++) {
            sprintf((*memberPtr)[i].name, "COM%d", i - playerCount + 1);
            (*memberPtr)[i].id = 100 + i;   //COM��ID�̐ݒ�
            (*memberPtr)[i].chip = initChip;
        }
    }
    return playerCount + cpuCount;
}

void poker(void) { //�|�[�J�[
    srand((unsigned int)time(NULL));                            //�����̏�����
    PlayerInfo* member = NULL;                                  //�����o�[���i�[����\���̔z��
    int numOfMember = 0;                                        //�����o�[�̐�
    const char suit[][6] = { "spade", "clab", "heart", "dia" }; //�G�D�̎��
    TrumpInfo trump[RANK_COUNT][SUIT_COUNT] = { 0,(SUIT)0 };    //�g�����v�J�[�h���i�[����\���̔z��
    TrumpInfo* communityCard[5] = { NULL };
    int round = 1;
    int game = 1;

    //�g�����v�J�[�h�̍쐬
    for (int i = 0; i < RANK_COUNT; i++) {
        for (int j = 0; j < SUIT_COUNT; j++) {
            trump[i][j].cardRank = i+1;             //�g�����v�̐�����ݒ�
            //trump[i][j].cardSuit = (SUIT)j;
            strcpy(trump[i][j].cardSuit, suit[j]);  //�g�����v�̊G�D��ݒ�
            trump[i][j].isDeploy = false;
        }
    }

    numOfMember = MakeMember(&member);  //�����o�[�̍쐬

    //�����o�[�̊m�F
    //for (int i = 0; i < numOfMember; i++) {
    //	sck(member[i].name);
    //	ick(member[i].id)
    //}

    bool testflag = true;
    //mainloop
    while (testflag) {
        if (round == 1) {
            //��D�̔z�z
            for (int i = 0; i < numOfMember; i++) {
                for (int j = 0; j < sizeof(member[i].deck) / sizeof(TrumpInfo*); j++) {
                    member[i].deck[j] = ChoiceTrump(trump);
                }

            }
            //�v���t���b�v

        }
        //�R�~���j�e�B�J�[�h�̔z�z
        int i = 0;
        do {
            communityCard[i] = ChoiceTrump(trump);
            i++;
        } while (communityCard[2] == NULL);
       
        
        //���̔���
        RoleJudge(&member[0], communityCard);


        scanf("%s");
    }
}

TrumpInfo* ChoiceTrump(TrumpInfo trump[RANK_COUNT][SUIT_COUNT]) {
    int randRank = 0;
    int randSuit = 0;
    do {                                    //�����_���Ƀg�����v��I��
        randRank = rand() % RANK_COUNT;
        randSuit = rand() % SUIT_COUNT;
    } while (trump[randRank][randSuit].isDeploy);
    trump[randRank][randSuit].isDeploy = true;

    return &trump[randRank][randSuit];
}

void RoleJudge(PlayerInfo* member, TrumpInfo* communityCard[5]) {
    sck(member->name);              //�����o�[����\��
    ick(member->deck[0]->cardRank); //0���ڂ̎�D�̐�����\��
    sck(member->deck[0]->cardSuit); //0���ڂ̎�D�̊G����\��
    ick(member->deck[1]->cardRank);
    sck(member->deck[1]->cardSuit);
    printf("\n");
    for (int i = 0; i < 5; i++) {
        pck(communityCard[i]);
    }
    printf("\n");

}


