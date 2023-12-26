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

typedef enum {
    bet,
    check,
    call,
    raise,
    fold
}BET;

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
    bool isCOM;
    TrumpInfo* deck[2]; //��D
    int ownPot;
    //int numOfDeck; 
    //type role; //��
}MemberInfo;


int MakeMember(MemberInfo**); //�����o�[�쐬

void poker(void);   //�|�[�J�[���s

TrumpInfo* ChoiceTrump(TrumpInfo trump[RANK_COUNT][SUIT_COUNT]); //��D�C�R�~���j�e�B�J�[�h�̑I��

void RoleJudge(MemberInfo*, TrumpInfo* communityCard[5]); //�𔻒�

void NextMember(MemberInfo** memberPtr, int count, MemberInfo* begin);

bool IsBettinground(MemberInfo* members, int count);

int main(void) {
    
    poker();

    return 0;
}

int MakeMember(MemberInfo** memberPtr) {
    int comCount = 0;           //COM�̐l��
    int playerCount = 1;        //�v���C���[�̐l��(1�l�Œ�)
    int initChip = 0;           //�`�b�v�̏�������
    printf("com�̐l������́F");
    scanf("%d", &comCount);
    *memberPtr = (MemberInfo*)calloc(sizeof(MemberInfo), playerCount + comCount + 1);
    if (*memberPtr != NULL) { //�����o�[���������쐬���ꂽ��
        printf("�v���C���[�̖��O����́F");
        scanf("%s", (*memberPtr)[0].name);

        printf("�`�b�v�̏�����������́F");
        scanf("%d", &initChip);
         
        (*memberPtr)[0].id = 1; //�v���C���[ID�̐ݒ�
        (*memberPtr)[0].isCOM = false;
        (*memberPtr)[0].chip = initChip;

        for (int i = playerCount; i < playerCount + comCount; i++) {
            sprintf((*memberPtr)[i].name, "COM%d", i - playerCount + 1);

            (*memberPtr)[i].id = i + 1;   //COM��ID�̐ݒ�
            (*memberPtr)[i].isCOM = true;
            (*memberPtr)[i].chip = initChip;
        }
    }
    else {
        exit(1);
    }

    return playerCount + comCount;
}

void NextMember(MemberInfo** memberPtr, int count, MemberInfo* begin) {
    for (int i = 0; i < count; i++) {
        if ((*memberPtr + 1)->id == 0) {
            *memberPtr = begin;
        }
        else {
            (*memberPtr)++;
        }
    }
}

void poker(void) { //�|�[�J�[
    srand((unsigned int)time(NULL));                            //�����̏�����
    MemberInfo* members = NULL;                                  //�����o�[���i�[����\���̔z��
    int numMembers = 0;
    MemberInfo* smallBlind;
    MemberInfo* bigBlind;                                   //�����o�[�̐�
    MemberInfo* currBetMember;
    const char suit[][15] = { "spade", "\x1b[31mheart\x1b[0m", "\x1b[31mdia\x1b[0m", "clab" }; //�G�D�̎��
    TrumpInfo trump[RANK_COUNT][SUIT_COUNT] = { 0 };            //�g�����v�J�[�h���i�[����\���̔z��
    TrumpInfo* communityCard[5] = { NULL };                     //�R�~���j�e�B�J�[�h���i�[����\���̔z��
    int round = 0;                                              //���E���h
    int game = 0;                                               //�Q�[��
    int numCommCards = 0;
    int ante = 50;
    int blind = 0;
    BET choiceAct = 0;
    const char* betAct[] = { "bet", "check", "call", "raise", "fold" };

    //�g�����v�J�[�h�̍쐬
    for (int i = 0; i < RANK_COUNT; i++) {
        for (int j = 0; j < SUIT_COUNT; j++) {
            trump[i][j].cardRank = i + 1;             //�g�����v�̐�����ݒ�
            trump[i][j].cardSuit = (SUIT)j;         //�g�����v�̊G�D��ݒ�
            trump[i][j].isDeploy = false;
            //strcpy(trump[i][j].cardSuit, suit[j]);  //�g�����v�̊G�D��ݒ�
        }
    }
    numMembers = MakeMember(&members);  //�����o�[�̍쐬

    //members[numMembers]; //�����o�[�̏I�[
    //mainloop
    while (1) {
        game++;

        //�u���C���h����l�̐ݒ�
        smallBlind = &members[(game-1) % numMembers];
        bigBlind = &members[(game) % numMembers];
        currBetMember = &members[(game + 1) % numMembers];

        //�r�b�O�u���C���h
        bigBlind->ownPot = (bigBlind->chip >= ante ? ante : bigBlind->chip);
        bigBlind->chip = (bigBlind->chip >= ante ? bigBlind->chip - ante : 0);
        ick(bigBlind->ownPot);

        //�X���[���u���C���h
        smallBlind->ownPot = (smallBlind->chip >= (ante / 2) ? ante / 2 : smallBlind->chip);
        smallBlind->chip = (smallBlind->chip >= (ante / 2) ? smallBlind->chip - (ante / 2) : 0);
        ick(smallBlind->ownPot);

        //��D�̔z�z
        for (int i = 0; i < numMembers; i++) {                                     //�z�z����郁���o�[
            for (int j = 0; j < sizeof(members[i].deck) / sizeof(TrumpInfo*); j++) { //2���z�z����
                members[i].deck[j] = ChoiceTrump(trump);
            }
        }
        //���E���h�J�n
        for (round = 1; round <= 4; round++) {
            ick(round);
            //�x�b�e�B���O���E���h
            do{
                
                currBetMember->ownPot = 20;
                ick(IsBettinground(members, numMembers));
                NextMember(&currBetMember, 1, members);
            } while (!IsBettinground(members, numMembers));
            
            
            ick(round);
            stop();
            ick(round);
            if (round == 4) {
                //�V���[�_�E��
                for (int i = 0; i < numMembers; i++) {
                    RoleJudge(&members[i], communityCard);
                }
            }
            else {
                //�R�~���j�e�B�J�[�h�̔z�z
                do {
                    communityCard[numCommCards] = ChoiceTrump(trump);
                    numCommCards++;
                } while (communityCard[2] == NULL);
                RoleJudge(&members[0], communityCard);
            }
        }
        //���E���h�I��

        //��D�̍폜
        for (int i = 0; i < numMembers; i++) {
            members[i].deck[0]->isDeploy = false;
            members[i].deck[1]->isDeploy = false;
            members[i].deck[0] = NULL;
            members[i].deck[1] = NULL;
        }
        //�R�~���j�e�B�J�[�h�̍폜
        for (;numCommCards > 0; numCommCards--) {
            communityCard[numCommCards - 1]->isDeploy = false;
            communityCard[numCommCards - 1] = NULL;
        }

        //�u���C���h��ύX
        NextMember(&smallBlind, 1, members);
        NextMember(&bigBlind, 1, members);

        stop();
    }
    
    int choice = 0;
    scanf("%d", &choice);
    ick(choice);
    switch (choice) {
    case(1):
        ick(choice);
        break;

    case(2):
        ick(choice);
        break;

    default:
        break;
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

bool IsBettinground(MemberInfo* members, int count) {
    for (int i = 0; i < count-1; i++) {
        if (members[i].ownPot != members[i + 1].ownPot) {
            return false;
        }
    }
    return true;
}

void RoleJudge(MemberInfo* member, TrumpInfo* communityCard[5]) {
    //sck(member->name);
    //ick(member->deck[0]->cardRank);
    //ick(member->deck[0]->cardSuit);
    //printf("\n");
}


void CpuMove(void) {


}