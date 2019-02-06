/**
 * Name: Kevin Tang
 * Class: 313
 * Desc: Main for buddy allocator
*/

#include "Ackerman.h"
#include "BuddyAllocator.h"
#include <unistd.h>
#include <stdlib.h>

bool getInput(int &blocksize, int &memsize, int argc, char **argv);

int main(int argc, char **argv)
{

	//use get opt
	int blocksize = 128, memsize = 128 * 1024 * 1024; //128 mB = 128 * 1024 * 1024 bytes
	bool errout = false;
	errout = getInput(blocksize, memsize, argc, argv);
	if (errout)
	{
		return 0;
	}

	//get the least memory that is still multiple two of blocksize and memsize
	int logBlockSize = 0;
	int logMemSize = 0;
	while(blocksize >>= 1) ++logBlockSize;
	while(memsize >>= 1) ++logMemSize;
	blocksize = memsize = 2;
	for(int i = 0; i < logBlockSize; i++) blocksize *= 2;
	for(int i = 0; i < logMemSize; i++) memsize *= 2;

	// create memory manager
	BuddyAllocator *allocator = new BuddyAllocator(blocksize, memsize);
	//allocator->debug();

	//ask for new value
	//char* loc = allocator->alloc(blocksize);
	//allocator->debug();

	//allocator->free(loc);
	//allocator->debug();

	// test memory manager
	Ackerman* am = new Ackerman ();
	am->test(allocator); // this is the full-fledged test.

	// destroy memory manager
	delete allocator;

	return 0;
}

/* getInput Doc:

gets input and changes the block size and memsize based off arguments.
returns true if theres an error and program needs to be stopped.
return false if everything is fine.

*/
bool getInput(int &blocksize, int &memsize, int argc, char **argv)
{
	int c;
	opterr = 0;

	//get option retrieve user input
	while ((c = getopt(argc, argv, "hb:s:")) != -1)
	{
		switch (c)
		{
		case 'b': //setting the block size
			if (optarg != NULL)
			{
				blocksize = atoi(optarg);
			}
			else
			{
				cout << "-b requires an argument!\n";
				return true;
			}
			break;
		case 's': //setting the memsize
			if (optarg != NULL)
			{
				memsize = atoi(optarg);
			}
			else
			{
				cout << "-s requires an argument!\n";
				return true;
			}
			break;
		case 'h': //ask for help
			cout << "HELP:\n"
				 << "-b arg1 : Sets the blocksize to arg1 in bytes (defaults to 128 bytes)\n"
				 << "-s arg1 : Sets the memory size to arg1 in bytes (defaults 512 kB)\n\n";
			return true;
			break;
		default:
			if (optopt == 's')
			{
				cout << "Option -s requires an argument\n";
			}
			if (optopt == 'b')
			{
				cout << "Option -b requires an argument\n";
			}
			cout << "Invalid input\n";
			return true;
		}
	}
}
