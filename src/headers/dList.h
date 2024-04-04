/*
double sided linked list
*/
#pragma once

#include "screen.h"

struct DList
{
	DList* next;
	DList* prev;
}__attribute__((packed));

void DListNodeInit(DList* node);
void DListPushFront(DList** head, DList* newNode);
void DListPushBack(DList** head, DList* newNode);
void DListRemove(DList** head, DList* node);
// void DListInsertAfter(DList* node, DList* newNode);
// void DListInsertBefore(DList** head, DList* node, DList* newNode);

// void printDList(DList* head);