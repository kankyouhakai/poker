#pragma once
#include <stdbool.h>
#include <stdio.h>
#include "gameset.h"

bool IsOnePair(TrumpInfo* cards[7], MemberInfo* member) {
    int continuity = 0;
    int  point = ONE_PAIR;
    for (int i = 0; cards[i + 1] != NULL && i < 6; i++) {
        if (cards[i]->cardRank == cards[i + 1]->cardRank) {
            continuity++;
            if (continuity == 1) {
                point = (point << 4) + cards[i]->cardRank;
                point = (point << 4) + cards[i]->cardSuit;
            }
        }
    }
    if (continuity == 1) {
        member->point = point;
        return true;
    }
    return false;
}
bool isTwoPair(TrumpInfo* cards[7], MemberInfo* member) {
    int continuity[2] = { 0 };
    int index = 0;
    int point = TWO_PAIR;
    for (int i = 0; cards[i + 1] != NULL && i < 6 && index <= 2;i++) {
        if (cards[i]->cardRank == cards[i + 1]->cardRank) {
            continuity[index]++;
            if (continuity[index] == 1) {
                point = (point << 4) + cards[i]->cardRank;
                point = (point << 4) + cards[i]->cardSuit;
            }
        }
        else {
            if (continuity[index] != 1) {
                continuity[index] = 0;
            }
            else {
                index++;
            }
        }
    }
    if (continuity[0] == 1 && continuity[1] == 1) {
        member->point = point;
        return true;
    }
    return false;
}
bool IsThreeCard(TrumpInfo* cards[7], MemberInfo* member) {
    int continuity = 0;int point = THREE_CARD;
    for (int i = 0; cards[i + 1] != NULL && i < 5; i++) {
        if (cards[i]->cardRank == cards[i + 1]->cardRank) {
            if (cards[i + 1]->cardRank == cards[i + 2]->cardRank) {
                continuity++;

                if (continuity == 1) {
                    point = (point << 4) + cards[i]->cardRank;
                    point = (point << 4) + cards[i]->cardSuit;
                }
            }
        }
    }
    if (continuity == 1) {
        member->point = point;

        return true;
    }
    return false;
}

bool IsFlash(TrumpInfo* cards[7], MemberInfo* member) {
    int countSuit[4][2] = {0};
    int point = FLASH;
    for (int i = 0; cards[i] != NULL && i < 7;i++) {
        countSuit[cards[i]->cardSuit][0]++;
        if (countSuit[cards[i]->cardSuit][1] == 0) {
            countSuit[cards[i]->cardSuit][1] = cards[i]->cardRank;
        }
    }
    for (int i = 0; i < 4; i++) {
        if (countSuit[i][0] >= 5) {
            member->point = (point << 4) + countSuit[cards[i]->cardSuit][1];

            return true;
        }
    }
    return false;
}
bool IsStraight(TrumpInfo* cards[7], MemberInfo* member) {
    //カードはソート済み
    int continuity = 0;
    int point = STRAIGHT;
    for (int i = 0; cards[i + 1] != NULL && i < 6; i++) {
        if (cards[i]->cardRank == cards[i + 1]->cardRank + 1) {
            continuity++;

            if (continuity == 1) {
                point = (point << 4) + cards[i]->cardRank;
                point = (point << 4) + cards[i]->cardSuit;
            }
        }
        else {
            continuity = 0;
        }
        if (continuity >= 4) {
            member->point = point;

            return true;
        }
    }
    return false;
}
bool isFullHouse(TrumpInfo* cards[7],MemberInfo* member) {
    return IsOnePair(cards, member) && IsThreeCard(cards, member);
}


bool IsFourCard(TrumpInfo* cards[7], MemberInfo* member) {
    int continuity = 0;
    int point = FOUR_CARD;
    for (int i = 0; cards[i + 1] != NULL && i < 6; i++) {
        if (cards[i]->cardRank == cards[i + 1]->cardRank) {
            continuity++;
            if (continuity == 1) {
                point = (point << 4) + cards[i]->cardRank;
                point = (point << 4) + cards[i]->cardSuit;
            }
        }
        else {
            continuity = 0;
        }
        if (continuity == 3) {
            member->point = point;

            return true;
        }
    }
    return false;
}




bool IsStraightFlash(TrumpInfo* cards[7], MemberInfo* member) {
    return  IsFlash(cards, member) && IsStraight(cards, member);
}
