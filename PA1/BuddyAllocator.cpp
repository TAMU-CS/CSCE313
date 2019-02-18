/* 
    File: my_allocator.cpp
*/
#include "BuddyAllocator.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstdint>
using namespace std;

//private functions
BlockHeader *BuddyAllocator::getBuddy(BlockHeader *block)
{
	//perform XOR to get buddy memory address
	//buddyAddress = XOR(currentblock - startAddress, memoryblock size)

	//convert hex string to number
	stringstream ss;
	uintptr_t addrNum, startNum;
	//char *addr = (char *)((int *)(block) - (int *)startMemory);
	ss << hex << (int *)block << endl
	   << (int *)startMemory;
	ss >> addrNum >> startNum;

	//perform xor on both bitsets and then convert result back to hex
	uintptr_t temp = ((addrNum-startNum) ^ block->blocksize) + startNum;

	/* debugging memory addresses
	cout << "add1: " << addrNum << endl;
	cout << "startNum: " << startNum << endl;
	cout << "size: " << block->blocksize << endl;
	cout << "RESULT: " << (int *)result << endl;
	*/
	return (BlockHeader*)(temp);
}

bool BuddyAllocator::arebuddies(BlockHeader *block1, BlockHeader *block2)
{
	return getBuddy(block2) == block1 && getBuddy(block1) == block2;
}

BlockHeader *BuddyAllocator::merge(BlockHeader *block1, BlockHeader *block2)
{
	if (arebuddies(block1, block2))
	{

		//check if both are open to merge
		if (block1->free && block2->free)
		{
			//change the size and information of block 1
			//get index
			int blockind = block1->blocksize / bbs;
			int indCount = 0;
			while (blockind >>= 1)
				++indCount;

			//remove from linked lists
			Mem[indCount].remove(block1);
			Mem[indCount].remove(block2);

			//get the left most block before merging
			uintptr_t blockaddr = (uintptr_t)block1 < (uintptr_t)block2 ? (uintptr_t)block1 : (uintptr_t)block2;
			BlockHeader *block = (BlockHeader *)(blockaddr);
			block->free = true;
			block->blocksize = block1->blocksize * 2;

			//insert the block into new linked list
			Mem[indCount + 1].insert(block);

			//check if block is max sized
			if(block->blocksize >= tml){
				return block;
			}

			//return the merged version of block and it's buddy
			return merge(block, getBuddy(block));
		}
	}

	return block1;
}

BlockHeader *BuddyAllocator::split(BlockHeader *block)
{
	//cout << "\n\n" << block->blocksize << endl;
	//debug();
	//get proper index of the linked lists
	int blockind = block->blocksize / bbs;
	int newBlockSize = block->blocksize/2;
	int indCount = 0;
	while (blockind >>= 1)
		++indCount;

	//create a new block header at center of block
	uintptr_t blockaddr = (uintptr_t)block + (uintptr_t)(newBlockSize);

	//set the block to new size
	BlockHeader *nBlock = (BlockHeader *)blockaddr;
	nBlock->blocksize = newBlockSize;
	nBlock->free = true;

	//update old block
	block->blocksize = newBlockSize;
	//cout <<"SComp:" << block->blocksize << " " << nBlock->blocksize << endl;

	//update the linked list for nBlock and block
	Mem[indCount].remove(block);
	Mem[indCount - 1].insert(block);
	Mem[indCount - 1].insert(nBlock);

	//return one of the blocks after split
	//debug();
	return block;
}

//public functions
BuddyAllocator::BuddyAllocator(int _basic_block_size, int _total_memory_length)
{

	//setting bbs and tml
	bbs = _basic_block_size;
	tml = _total_memory_length;

	//round up the basic block size to a power of 2 and initiate the linked lists
	int base = _basic_block_size;
	while (base <= _total_memory_length)
	{
		LinkedList temp;
		Mem.push_back(temp);
		base *= 2;
	}

	//create the entire memory block
	startMemory = new char[_total_memory_length];
	BlockHeader *first = (BlockHeader *)startMemory;
	first->blocksize = _total_memory_length;
	first->free = true;

	//set the linked list
	Mem[Mem.size() - 1].insert(first);

	//getBuddy(first);
}

BuddyAllocator::~BuddyAllocator()
{
	//free the memory back to the heap
	delete[] startMemory;
}

char *BuddyAllocator::alloc(int _length)
{
	//get the least amount that _length will fit into a block
	int trueSize = _length + sizeof(BlockHeader);
	int base = bbs;

	//check if out of bounds or if not enough memory is left to allocate
	if(trueSize > tml){
		return NULL;
	}

	//iterate through the smallest block size needed
	int i;
	for (i = 0; i < Mem.size(); i++)
	{
		if (base >= trueSize)
		{
			break;
		}

		base *= 2;
	}

	//with the smallest block size needed, find the
	//iterate through memory until a open block is found
	for (int j = i; j < Mem.size(); j++)
	{

		BlockHeader *tempBlock = Mem[j].getHead();
		while (tempBlock != NULL)
		{

			//see if the block is open
			//split the block until the minimum size is reached
			if (tempBlock->free)
			{
				while (j > i)
				{
					tempBlock = split(tempBlock);
					j--;
				}

				//update tempblock information
				tempBlock->free = false;

				return (char *)((uintptr_t)tempBlock + sizeof(BlockHeader)); //return the final split block
			}

			tempBlock = tempBlock->Next;
		}
	}

	//no block was found so return null pointer because size issues
	return NULL;
}

int BuddyAllocator::free(char *_a)
{
	//define block header and find buddy
	BlockHeader *block = (BlockHeader *)((uintptr_t)_a - sizeof(BlockHeader));
	BlockHeader *buddy = getBuddy(block);

	//merge block and buddy if they are both open and if within the block size
	block->free = true;
	block = merge(block, buddy); //recursively merges

	return 0;
}

void BuddyAllocator::debug()
{

	//print title
	cout << "Block Size : 1-free block, 0-used block" << endl;

	//iterate through all of the vectors containing the linked lists
	int base = bbs;
	for (int i = 0; i < Mem.size(); i++)
	{
		//output format: memorysize ll1 ll2 ll3 ...
		cout << base << " ";

		//loop through linked list
		BlockHeader *temp = Mem[i].getHead();
		while (temp != NULL)
		{
			cout << (uintptr_t)temp - (uintptr_t)startMemory << " " << temp->blocksize << ", " << temp->free << " : ";
			//cout << getBuddy(temp)->blocksize << ", " << getBuddy(temp)->free << " ;";

			temp = temp->Next;
		}

		cout << endl;

		base *= 2;
	}

}
