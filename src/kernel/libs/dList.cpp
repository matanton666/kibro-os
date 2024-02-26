#include "dList.h"

void DListNodeInit(DList* node)
{
	node->next = nullptr;
	node->prev = nullptr;
}

void DListPushFront(DList** head, DList* newNode)
{
	if ((*head) == newNode)
		return;

	newNode->next = (*head);
	newNode->prev = nullptr;

	if ((*head) != nullptr)
	{
		(*head)->prev = newNode;
	}

	(*head) = newNode;
}


void DListRemove(DList** head, DList* node)
{
	if ((*head) == nullptr || node == nullptr)
		return;

	if ((*head) == node) // if node is head set the pointer of head to next
		(*head) = node->next;
	
	node->prev->next = node->next; // set previous node to skip current node

	if (node->next == nullptr) // is last node
		return;
	
	node->next->prev = node->prev; // set next node to skip current node
}

void DListPushBack(DList** head, DList* newNode)
{
	newNode->next = nullptr;
	
	// insert to head if list is empty
	if ((*head) == nullptr) {
		newNode->prev = nullptr;
		(*head) = newNode;
		return;
	}

	// insert to end of list
	DList* last = (*head); 
	while (last->next != nullptr)
		last = last->next;

	last->next = newNode;
	newNode->prev = last;
}


// void printDList(DList* head)
// {
// 	DList* last = head;
// 	while (last != nullptr)
// 	{
// 		screen.printHex((uint64_t)last);
// 		screen.print(" -> ");
// 		last = last->next;
// 	}
// 	screen.print("nullptr");
// 	screen.newLine();
// }


// void DListInsertAfter(DList* node, DList* newNode)
// {
// 	if (node == newNode)
// 		return;

// 	newNode->next = node->next;
// 	node->next = newNode;
// 	newNode->prev = node;

// 	if (newNode->next != nullptr)
// 		newNode->next->prev = newNode;
// }

// void DListInsertBefore(DList** head, DList* node, DList* newNode)
// {
// 	if (newNode == nullptr)
// 		return;

// 	newNode->prev = node->prev;

// 	node->prev = newNode;

// 	newNode->next = node;

// 	if (newNode->prev != nullptr)
// 		newNode->prev->next = newNode;
// 	else
// 		(*head) = newNode;
// }

