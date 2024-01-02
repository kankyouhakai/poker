#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "debuglib.h"
#include <math.h>
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

void NextMember(MemberInfo** memberPtr, int count, MemberInfo* begin, MemberInfo* end);

bool IsBettinground(MemberInfo* members, int count);

void Pay(MemberInfo* member, int chip);

int MoveChip(int* source, /*int* dest,*/ int amount);


int main(void) {
    srand((unsigned int)time(NULL));                            //�����̏�����
    stop();
    poker();

    return 0;
}

int MakeMember(MemberInfo** memberPtr) {
    int comCount = 0;           //COM�̐l��
    int playerCount = 1;        //�v���C���[�̐l��(1�l�Œ�)
    int initChip = 0;           //�`�b�v�̏�������
    printf("com�̐l������́F");
    scanf("%d", &comCount);
    *memberPtr = (MemberInfo*)calloc(sizeof(MemberInfo), playerCount + comCount);
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

void NextMember(MemberInfo** memberPtr, int count, MemberInfo* begin, MemberInfo* end) {
    for (int i = 0; i < count; i++) {
        if ((*memberPtr + 1) == end) {
            *memberPtr = begin;
        }
        else {
            (*memberPtr)++;
        }
    }
}

void poker(void) { //�|�[�J�[
    MemberInfo* members = NULL;                                  //�����o�[���i�[����\���̔z��
    int numMembers = 0;
    MemberInfo* smallBlind = NULL;
    MemberInfo* bigBlind = NULL;                                   //�����o�[�̐�
    MemberInfo* currBetMember = NULL;
    MemberInfo* winner = NULL;
    const char suit[][15] = { "spade", "\x1b[31mheart\x1b[0m", "\x1b[31mdia\x1b[0m", "clab" }; //�G�D�̎��
    TrumpInfo trump[RANK_COUNT][SUIT_COUNT] = { 0 };            //�g�����v�J�[�h���i�[����\���̔z��
    TrumpInfo* communityCard[5] = { NULL };                     //�R�~���j�e�B�J�[�h���i�[����\���̔z��
    int round = 0;                                              //���E���h
    int game = 0;                                               //�Q�[��
    int numCommCards = 0;
    int ante = 50;
    int blind = ante*2;
    int callChip = 0;
    int pot = 0;
    BET choiceAct = 0;
    const char* action[] = { "bet", "check", "call", "raise", "fold" };
    int stake = 0;
    
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

    //n�o�C�g�Ƃ��m�o�C�g�ǂފ֐�(�\���̔z��p)
    sck((char*)((char*)&members[0].name + 2 * sizeof(MemberInfo)));
    sck(members[2].name);
    
    stop();
    smallBlind = &members[(game) % numMembers];
    bigBlind = &members[(game + 1) % numMembers];
    currBetMember = &members[(game + 2) % numMembers];

    //members[numMembers]; //�����o�[�̏I�[

    //mainloop
    for(game = 1; true; game++) {
        ick(game);
        //�A���e�B�i�������邩�͖��m��j
        printf("�A���e�B�F%d\n", ante);
        for (int i = 0; i < numMembers; i++) {
            //Pay(&members[i], ante);
            pot += MoveChip(&(members[i].chip), ante);
        }

        //�X���[���u���C���h
        //Pay(smallBlind, blind);
        smallBlind->ownPot = MoveChip(&(smallBlind->chip), blind);
        ick(smallBlind->ownPot);

        //�r�b�O�u���C���h
        //Pay(bigBlind, 2 * blind);
        bigBlind->ownPot = MoveChip(&(bigBlind->chip), 2 * blind);
        ick(bigBlind->ownPot);

        stop();
        //��D�̔z�z
        for (int i = 0; i < numMembers; i++) {                                     //�z�z����郁���o�[
            for (int j = 0; j < sizeof(members[i].deck) / sizeof(TrumpInfo*); j++) { //2���z�z����
                members[i].deck[j] = ChoiceTrump(trump);
            }
        }
        printf("%s - %d\n", suit[members[0].deck[0]->cardSuit], members[0].deck[0]->cardRank);
        printf("%s - %d\n", suit[members[0].deck[1]->cardSuit], members[0].deck[1]->cardRank);

        stop();
        //���E���h�J�n
        for (round = 1; round <= 4; round++) {
            ick(round);
            //�x�b�e�B���O���E���h
            do{
                //�x�b�h����Ă��Ȃ��Ȃ�x�b�h�C�`�F�b�N
                // �x�b�h�̍Œ�z��BB
                //�x�b�h����Ă���Ȃ�R�[���C���C�Y�C�t�H�[���h
                // �R�[���͒��O�̃����o�[�̃|�b�g�ɍ��킹��
                //�x�b�e�B���O���E���h�̏����́u�S���̓q�����̈�v�v�ȁu�����o�[��������Ă���v����
                //��for���̈ꎞ�ϐ��H

                //���ꂼ��̃|�b�g�̊m�F
                for (int i = 0; i < numMembers; i++) {
                    ick(members[i].ownPot);
                }

                //�v���C���[�̎�D�̊m�F
                for (int i = 0; i < numCommCards; i++) {
                    printf("%s - %d\n", suit[communityCard[i]->cardSuit], communityCard[i]->cardRank);
                }
                sck(currBetMember->name);   //�x�b�g����v���C���[�̊m�F

                //�R�[������z�̌���
                for (int i = 0; i < numMembers; i++) {
                    callChip = (members[i].ownPot > callChip ? members[i].ownPot : callChip);
                }
                ick(callChip);
                //�x�b�h����Ă��Ȃ����̓���
                if (callChip == 0) {
                    printf("1:%s, 2:%s", action[0], action[1]);
                    scanf("%d", &choiceAct);
                    choiceAct -= 1;
                }
                else {  //�x�b�h����Ă���Ƃ��̓���
                    printf("1:%s, 2:%s, 3:%s", action[2], action[3], action[4]);
                    scanf("%d", &choiceAct);
                    choiceAct += 1;
                }
                sck(action[choiceAct]);
                stop();
                switch (choiceAct)
                {
                case bet:
                    stake = 2 * blind;
                    break;
                    
                case check:
                    //���邱�ƂȂ�
                    break;

                case call:
                    stake = callChip - currBetMember->ownPot;
                    break;

                case raise:
                    //�v���[������
                    break;

                case fold:
                    //���˂�
                    break;
                default:

                    break;
                }
                ick(stake);
                stop();
                //Pay(currBetMember, 0);
                currBetMember->ownPot += MoveChip(&(currBetMember->chip), stake);
                NextMember(&currBetMember, 1, members, &members[numMembers]);
            } while (IsBettinground(members, numMembers));
            
            stop();

            for (int i = 0; i < numMembers; i++) {
                //pot += members[i].ownPot;
                pot += MoveChip(&(members[i].ownPot), members[i].ownPot);
            }
            callChip = 0;
            stake = 0;

            if (round == 4) {
                //�V���[�_�E��
                for (int i = 0; i < numMembers; i++) {
                    RoleJudge(&members[i], communityCard);
                }
                //���Ҕ���
                winner = members;
                winner->chip += MoveChip(&pot, pot);
                //winner->chip += pot;
                //pot = 0;
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

        NextMember(&smallBlind, 1, members, &members[numMembers]);
        NextMember(&bigBlind, 1, members, &members[numMembers]);

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

void Pay(MemberInfo* member, int chip) {
    if (chip > member->chip) {
        chip = member->chip;
    }
    //chip = member->chip >= chip ? chip : member->chip;
    member->ownPot += chip;
    member->chip -= chip;
}

int MoveChip(int* source, /*int* dest,*/ int amount) {
    if (amount > *source) {
        amount = *source;
    }
    //*dest += amount;
    *source -= amount;
    return amount;
}


bool IsBettinground(MemberInfo* members, int count) {
    for (int i = 0; i < count-1; i++) {
        if (members[i].ownPot != members[i + 1].ownPot) {
            return true;
        }
    }
    return false;
}

void RoleJudge(MemberInfo* member, TrumpInfo* communityCard[5]) {
    //sck(member->name);
    //ick(member->deck[0]->cardRank);
    //ick(member->deck[0]->cardSuit);
    //printf("\n");
}
