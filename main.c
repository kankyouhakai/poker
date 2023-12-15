#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "debuglib.h"
#define RANK_COUNT 13   //�g�����v�̐���
#define SUIT_COUNT 4    //�g�����v�̊G�D�̐�

typedef enum {
    spade,      //0
    heart,      //1
    dia,        //2
    clab        //3
}SUIT;

typedef struct {        //�g�����v�̍\����
    int cardRank;       //����
    SUIT cardSuit;      //�G�D
    //char cardSuit[6];
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

TrumpInfo* ChoiceTrump(TrumpInfo trump[RANK_COUNT][SUIT_COUNT]); //��D�C�R�~���j�e�B�J�[�h�̑I��

void RoleJudge(PlayerInfo*, TrumpInfo* communityCard[5]); //�𔻒�


int main(void) {
    
    poker();
    return 0;
}

int MakeMember(PlayerInfo** memberPtr) {
    int comCount = 0;           //COM�̐l��
    int playerCount = 1;        //�v���C���[�̐l��(1�l�Œ�)
    int initChip = 0;           //�`�b�v�̏�������
    printf("com�̐l������́F");
    scanf("%d", &comCount);

    *memberPtr = (PlayerInfo*)calloc(sizeof(PlayerInfo), playerCount + comCount);
    if (*memberPtr != NULL) { //�����o�[���������쐬���ꂽ��
        printf("�v���C���[�̖��O����́F");
        scanf("%s", (*memberPtr)[0].name);

        printf("�`�b�v�̏�����������́F");
        scanf("%d", &initChip);

        (*memberPtr)[0].id = 0; //�v���C���[ID�̐ݒ�
        (*memberPtr)[0].chip = initChip;

        for (int i = playerCount; i < playerCount + comCount; i++) {
            sprintf((*memberPtr)[i].name, "COM%d", i - playerCount + 1);

            (*memberPtr)[i].id = 100 + i;   //COM��ID�̐ݒ�
            (*memberPtr)[i].chip = initChip;
        }
    }

    return playerCount + comCount;
}

void poker(void) { //�|�[�J�[
    srand((unsigned int)time(NULL));                            //�����̏�����
    PlayerInfo* member = NULL;                                  //�����o�[���i�[����\���̔z��
    int numMembers = 0;                                        //�����o�[�̐�
    const char suit[][6] = { "spade", "heart", "dia", "clab" }; //�G�D�̎��
    TrumpInfo trump[RANK_COUNT][SUIT_COUNT] = { 0 };            //�g�����v�J�[�h���i�[����\���̔z��
    TrumpInfo* communityCard[5] = { NULL };                     //�R�~���j�e�B�J�[�h���i�[����\���̔z��
    int round = 1;                                              //���E���h
    int game = 1;                                               //�Q�[��
    int numCommCards = 0;
    int ante = 0;
    int blind = 0;

    //�g�����v�J�[�h�̍쐬
    for (int i = 0; i < RANK_COUNT; i++) {
        for (int j = 0; j < SUIT_COUNT; j++) {
            trump[i][j].cardRank = i+1;             //�g�����v�̐�����ݒ�
            trump[i][j].cardSuit = (SUIT)j;         //�g�����v�̊G�D��ݒ�
            trump[i][j].isDeploy = false;
            //strcpy(trump[i][j].cardSuit, suit[j]);  //�g�����v�̊G�D��ݒ�
        }
    }
    numMembers = MakeMember(&member);  //�����o�[�̍쐬

    //�����o�[�̊m�F
    //for (int i = 0; i < numOfMember; i++) {
    //	sck(member[i].name);
    //	ick(member[i].id)
    //}

    bool testflag = true;
    //mainloop
    while (testflag) {
        while (game) {
            //��D�̔z�z
            for (int i = 0; i < numMembers; i++) {                                     //�z�z����郁���o�[
                for (int j = 0; j < sizeof(member[i].deck) / sizeof(TrumpInfo*); j++) { //2���z�z����
                    member[i].deck[j] = ChoiceTrump(trump);
                }
                sck(suit[member[0].deck[0]->cardSuit]);

            }
            for(round = 1; round <= 4; round++){

                //�`�b�v��q����

                if (round == 4) {
                    //�V���[�_�E��
                    for (int i = 0; i < numMembers; i++) {
                        ick(round);
                        RoleJudge(&member[i], communityCard);
                    }
                }
                else {
                    //�R�~���j�e�B�J�[�h�̔z�z
                    do {
                        communityCard[numCommCards] = ChoiceTrump(trump);
                        numCommCards++;
                    } while (communityCard[2] == NULL);

                    RoleJudge(&member[0], communityCard);
                }

            }
            scanf("%s");
        }
       
    }
}

TrumpInfo* ChoiceTrump(TrumpInfo trump[RANK_COUNT][SUIT_COUNT]) {
    int randomRank = 0;
    SUIT randomSuit = 0;
    do {                                            //�����_���Ƀg�����v��I��
        randomRank = rand() % RANK_COUNT;
        randomSuit = rand() % SUIT_COUNT;
    } while (trump[randomRank][randomSuit].isDeploy);   //�܂��I�΂�Ă��Ȃ��g�����v���o��܂Ń��[�v
    trump[randomRank][randomSuit].isDeploy = true;

    return &trump[randomRank][randomSuit];              //�I�΂ꂽ�g�����v�̃A�h���X��Ԃ�
}

void RoleJudge(PlayerInfo* member, TrumpInfo* communityCard[5]) {
    sck(member->name);
    ick(member->deck[0]->cardSuit);

}