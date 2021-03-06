/* instruction-level simulator for LC3101 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000

typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

typedef struct blockStruct {
	int valid;
	int dirty;
	int tag;
	int leastRecent;
	int *block;
} blockType;

typedef struct cacheStruct {
	blockType *block;
} cacheType;

enum actionType 
	{cacheToProcessor, processorToCache, memoryToCache, cacheToMemory, cacheToNowhere};

int blockSizeInWords;
int numberOfSets;
int blocksPerSet;

void printState(stateType *);
void printAction(int , int , enum actionType);
int convertNum(int);

int lru[100];
int lrucount;
cacheType * cache;

int
main(int argc, char *argv[])
{



   	char line[MAXLINELENGTH];
    	stateType state;
    	FILE *filePtr;

      	int i = 0;
	int a = 0;
	int b = 0;
	int c = 0;
	int d = 0;
	int f = 0;
	int lwset;
	int swset;
	int inserted = 0;
	int setbegin;
	int opcode;
	int arg0;
	int arg1;
	int arg2;
	int regA;
	int regB;
	int destReg;
	int offset;
	int loopcount;
	blockSizeInWords = atoi(argv[2]);
	numberOfSets = atoi(argv[3]);
	blocksPerSet = atoi(argv[4]);
	int totalSizeOfCache = blockSizeInWords * numberOfSets * blocksPerSet;
	int tag;
	int previoustag;
	lrucount = 0;
	int temp = 0;
	int temp2 = 0;
	int temp3 = 0;
	int temp4 = 0;
	int flag = 0;
	int ifcount = 0;
	//dynamically create the arrays in the cache
	
	cache = (cacheType *) malloc(sizeof(cacheType) * numberOfSets);

	for(i = 0; i < numberOfSets; i++)
	{
		cache[i].block = (blockType *) malloc(sizeof(blockType) * blocksPerSet);
		
	}

	for(i = 0; i < numberOfSets; i++)
	{
		for(a = 0; a < blocksPerSet; a++)
		{
			cache[i].block[a].block = (int *) malloc(sizeof(int) * blockSizeInWords);
		}
	}

	//initialize cache to empty

	for(i = 0; i < numberOfSets; i++)
	{
		for(a = 0; a < blocksPerSet; a++)
		{
			cache[i].block[a].dirty = 0;
			cache[i].block[a].valid = 0;
			cache[i].block[a].leastRecent = 0;
			cache[i].block[a].tag = 0;
			for(b = 0; b < blockSizeInWords; b++)
			{
				cache[i].block[a].block[b] = 0;
			}
		}
	}
	
	lru[lrucount] = 0;

	//check to make sure cache parameters are legal

	if((numberOfSets * blocksPerSet) > NUMMEMORY)
		exit(1);
	if((blockSizeInWords % 2) != 0 && blockSizeInWords != 1) 
		exit(1);
	if((numberOfSets % 2) != 0 && numberOfSets != 1)
		exit(1);
	 if((blockSizeInWords % 2) != 0 && blockSizeInWords != 1)
		exit(1);


    	if (argc != 5) {
		printf("error: usage: %s <machine-code file>\n", argv[0]);
		exit(1);
    	}

    	filePtr = fopen(argv[1], "r");
   	 if (filePtr == NULL) {
		printf("error: can't open file %s", argv[1]);
		perror("fopen");
		exit(1);
    	}

    	/* read in the entire machine-code file into memory */
    	for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
		state.numMemory++) {
		if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
	    	printf("error in reading address %d\n", state.numMemory);
	    	exit(1);
		}
		//printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    	}

	for(i = 0; i < NUMREGS; i++)
		state.reg[i] = 0;

	state.reg[1] = 1;

//	for(i = 5; i < 100; i++)
//		state.mem[i] = i;


	//printState(&state);

	loopcount = state.pc + 1;
	

	//initial instruction fetch, compulsory miss

	cache[0].block[0].valid = 1;
	cache[0].block[0].tag = 0;
	
	for(i = 0; i < blockSizeInWords; i++)
	{
		cache[0].block[0].block[i] = state.mem[i];
	}
	
	printAction(0, blockSizeInWords, 2);
	
	for(state.pc; state.pc < state.numMemory; state.pc++)
	{

		opcode = state.mem[state.pc] >> 22;
		


		for(i = 0; i < numberOfSets; i++)
		{		
			for(a = 0; a < blocksPerSet; a++)
			{
				if(cache[i].block[a].tag == (state.pc  / blockSizeInWords))			
				{	
					flag = 1;
					printAction(state.pc, 1, 0);
					break;
				}
				else if(i == (numberOfSets - 1) && a == (blocksPerSet - 1))
				{
					temp = lru[(lrucount + numberOfSets) - (numberOfSets * blocksPerSet)];
					
					for(c = 0; c < numberOfSets; c++)
					{
						if(cache[c].block[d].tag == temp)
						{
							for(d = 0; d < blocksPerSet; d++)
							{
								i = c;
								a = d;
								if(cache[c].block[d].dirty == 1)
								{
									temp3 = cache[c].block[d].tag;
									temp2 = (cache[c].block[d].tag * blockSizeInWords);
									cache[c].block[d].dirty = 0;
									printAction(temp2, blockSizeInWords, 3);
									for(f = 0; f < blockSizeInWords; f++)
									{
										state.mem[temp2 + f] = cache[c].block[d].block[f];
									}
								}
								else
								{
									printAction((cache[c].block[d].tag * blockSizeInWords), blockSizeInWords, 4);
								}
							}
						}
					}
				}
				ifcount++;
				inserted = 1;
				lrucount++;
				lru[lrucount] = state.pc / blockSizeInWords;
				for(b = 0; b < blockSizeInWords; b++)
				{	temp4 = (state.pc / blockSizeInWords) + 1;
					cache[i].block[a].block[b] = state.mem[temp4 + b];
				}
				cache[i].block[a].valid = 1;
				cache[i].block[a].tag = state.pc / blockSizeInWords;
				if(ifcount % numberOfSets == 0)
					printAction(lru[(lrucount + 1) - (numberOfSets * blocksPerSet)], blockSizeInWords, 4);	
				else
					printAction(lru[(lrucount - numberOfSets)  - (numberOfSets * blocksPerSet)], blockSizeInWords, 4);
				printAction((state.pc / blockSizeInWords) + 1, blockSizeInWords, 2);
				printAction(state.pc, 1, 0);
			}
			if(inserted == 1)
			{	inserted = 0;
				break;
			}

			if(flag == 1)
			break;

		}
		
/*			for(i = 0; i < numberOfSets; i++)
			{
				for(a = 0; a < blocksPerSet; a++)
				{
					printf("cache[%d].block[%d].dirty = %d\n", i, a, cache[i].block[a].dirty);
					printf("cache[%d].block[%d].valid = %d\n", i, a, cache[i].block[a].valid);
					printf("cache[%d].block[%d].leastRecent = %d\n", i, a, cache[i].block[a].leastRecent);
					printf("cache[%d].block[%d].tag = %d\n", i, a, cache[i].block[a].tag);

					for(b = 0; b < blockSizeInWords; b++)
					{
						printf("cache[%d].block[%d].block[%d] = %d\n", i, a, b, cache[i].block[a].block[b]);
					}
				}
			}
			printf("\n\n\n");	
				
	
*/				
			

		//add
		if(opcode == 0)
		{

			regA = (state.mem[state.pc] >> 19) & 7;
			regB = (state.mem[state.pc] >> 16) & 7;
			destReg = (state.mem[state.pc]) & 7;
			state.reg[destReg] = state.reg[regA] + state.reg[regB];
			state.pc++;
			//printState(&state);
			state.pc--;
		}

		//nand
		if(opcode == 1)
		{

			regA = (state.mem[state.pc] >> 19) & 7;
			regB = (state.mem[state.pc] >> 16) & 7;
			destReg = (state.mem[state.pc]) & 7;
			state.reg[destReg] = ~(state.reg[regA] & state.reg[regB]);
			state.pc++;
			//printState(&state);
			state.pc--;
		}

		//jalr
		if(opcode == 5)
		{
			regA = (state.mem[state.pc] >> 19) & 7;
			regB = (state.mem[state.pc] >> 16) & 7;
			destReg = (state.mem[state.pc]) & 7;
			state.reg[regB] = state.pc + 1;
			state.pc = state.reg[regA];
			//printState(&state);
		}

		//lw
		if(opcode == 2)
		{

			regA = (state.mem[state.pc] >> 19) & 7;
			regB = (state.mem[state.pc] >> 16) & 7;
			offset = (state.mem[state.pc]) & 65535;

			if(offset > 32767 || offset < -32768)
			{
				offset = convertNum(offset);
			}

			previoustag = tag;
			lwset = ((state.reg[regA] + offset)  / blockSizeInWords);
			setbegin = lwset * blockSizeInWords;
			tag = (setbegin >> (blockSizeInWords / 2));
			

			for(i = 0; i < numberOfSets; i++)
			{
				for(a = 0; a < blocksPerSet; a++)
				{
					if(cache[i].block[a].valid == 0 || (i == (numberOfSets - 1) && a == (blocksPerSet - 1)))
					{

						if(i == (numberOfSets - 1) && a == (blocksPerSet - 1))
						{
							temp = lru[(lrucount + numberOfSets) - (numberOfSets * blocksPerSet)];
							
							for(c = 0; c < numberOfSets; c++)
							{
								for(d = 0; d < blocksPerSet; d++)
								{
									if(cache[c].block[d].tag == temp)
									{
										i = c;
										a = d;
										if(cache[c].block[d].dirty == 1)
										{	
											temp2 = (cache[c].block[d].tag * blockSizeInWords);
											cache[c].block[d].dirty = 0;
											printAction(temp2, blockSizeInWords, 3);
											for(f = 0; f < blockSizeInWords; f++)
											{
												state.mem[temp2 + f] = cache[c].block[d].block[f];
											}
										}
										else
										{	
											printAction(cache[c].block[d].tag * blockSizeInWords, blockSizeInWords, 4);  
										}
									}
								}
							}
							
							
						}


						inserted = 1;
						for(b = 0; b < blockSizeInWords; b++)
						{	 
							cache[i].block[a].block[b] = state.mem[setbegin + b];
						}
						cache[i].block[a].valid = 1;
						cache[i].block[a].tag = tag;
					}
					if(inserted == 1)
					break;
				}
				if(inserted == 1)
				{	
					lrucount++;
					lru[lrucount] = tag;
					inserted = 0;
					printAction(setbegin, blockSizeInWords, 2);
					printAction(regA + offset, 1, 0);
					
					break;

				}

			}
				
			     
			
/*	
			for(i = 0; i < numberOfSets; i++)
			{
				for(a = 0; a < blocksPerSet; a++)
				{
					printf("cache[%d].block[%d].dirty = %d\n", i, a, cache[i].block[a].dirty);
					printf("cache[%d].block[%d].valid = %d\n", i, a, cache[i].block[a].valid);
					printf("cache[%d].block[%d].leastRecent = %d\n", i, a, cache[i].block[a].leastRecent);
					printf("cache[%d].block[%d].tag = %d\n", i, a, cache[i].block[a].tag);

					for(b = 0; b < blockSizeInWords; b++)
					{
						printf("cache[%d].block[%d].block[%d] = %d\n", i, a, b, cache[i].block[a].block[b]);
					}
				}
			}
			printf("\n\n\n");	
*/	

			state.reg[regB] = state.mem[state.reg[regA] + offset];
			state.pc++;
			//printState(&state);
			state.pc--;
		}

		//beq
		if(opcode == 4)
		{

			regA = (state.mem[state.pc] >> 19) & 7;
			regB = (state.mem[state.pc] >> 16) & 7;
			offset = (state.mem[state.pc]) & 65535;

			if(state.reg[regA] == state.reg[regB])
			{
				if(offset > 32767 || offset < -32768)
					offset = convertNum(offset);
				state.pc += offset;
			}
			state.pc++;
			//printState(&state);
			state.pc--;
		}

		//sw
		if(opcode == 3)
		{
			regA = (state.mem[state.pc] >> 19) & 7;
			regB = (state.mem[state.pc] >> 16) & 7;
			offset = (state.mem[state.pc]) & 65535;
			if(offset > 32767 || offset < -32768)
				offset = convertNum(offset);

			previoustag = tag;
			swset = (regA + offset) / blockSizeInWords; 
			setbegin = swset * blockSizeInWords;
			tag = (setbegin >> (blockSizeInWords / 2));
			
		
			for(i = 0; i < numberOfSets; i++)
			{
				for(a = 0; a < blocksPerSet; a++)
				{
					if(cache[i].block[a].valid == 0 || (i == (numberOfSets - 1) && a == (blocksPerSet -1)))
					{
						if(i == (numberOfSets - 1) && a == (blocksPerSet - 1))
						{
							temp = lru[(lrucount + numberOfSets) - (numberOfSets * blocksPerSet)];
							
							for(c = 0; c < numberOfSets; c++)
							{
								for(d = 0; d < blocksPerSet; d++)
								{
									if(cache[c].block[d].tag == temp)
									{
										i = c;
										a = d;
										if(cache[c].block[d].dirty == 1)
										{
											temp2 = (cache[c].block[d].tag * blockSizeInWords);
											cache[c].block[d].dirty = 1;
											printAction(temp2, blockSizeInWords, 3);
											for(f = 0; f < blockSizeInWords; f++)
											{
												state.mem[temp2 + f] = cache[c].block[d].block[f];
											}
										}
										
									
									}
								}
							}
					}


					inserted = 1;
					for(b = 0; b < blockSizeInWords; b++)
					{	 
						cache[i].block[a].block[b] = state.mem[setbegin + b];
					}
					cache[i].block[a].valid = 1;
					cache[i].block[a].tag = tag;
				}
				if(inserted == 1)
				break;
				}

				if(inserted == 1)
				{	
					lrucount++;
					lru[lrucount] = tag;
					inserted = 0;
					printAction(setbegin, blockSizeInWords, 2);
					printAction(regA + offset, 1, 1);
					cache[i].block[a].dirty = 1;
					break;
				}
				
			}
			
/*	
			for(i = 0; i < numberOfSets; i++)
			{
				for(a = 0; a < blocksPerSet; a++)
				{
					printf("cache[%d].block[%d].dirty = %d\n", i, a, cache[i].block[a].dirty);
					printf("cache[%d].block[%d].valid = %d\n", i, a, cache[i].block[a].valid);
					printf("cache[%d].block[%d].leastRecent = %d\n", i, a, cache[i].block[a].leastRecent);
					printf("cache[%d].block[%d].tag = %d\n", i, a, cache[i].block[a].tag);

					for(b = 0; b < blockSizeInWords; b++)
					{
						printf("cache[%d].block[%d].block[%d] = %d\n", i, a, b, cache[i].block[a].block[b]);
					}
				}
			}
			printf("\n\n\n");
*/

			state.mem[offset + state.reg[regA]] = state.reg[regB];
			state.pc++;
			//printState(&state);
			state.pc--;
		}

		//halt
		if(opcode == 6)
		{

			state.pc++;
			printf("machine halted\ntotal of %d instructions executed\nfinal state of machine:", loopcount);
			//printState(&state);
			return (0);
		}

		//noop
		if(opcode == 7)
		{
			state.pc++;
		}

		loopcount++;
	}



    return(0);
}





void printAction(int address, int size,  enum actionType type)
{
	printf("@@@ transferring word [%d-%d] ", address, address + size - 1);
	if(type == cacheToProcessor){
		printf("from the cache to the processor\n");
	} else if (type == processorToCache) {
		printf("from the processor to the cache\n");
	} else if(type == memoryToCache) {
		printf("from the memory to the cache\n");
	} else if(type == cacheToMemory) {
		printf("from the cache to the memory\n");
	} else if(type == cacheToNowhere) {
		printf("from the cache to nowhere\n");
	}
}










void
printState(stateType *statePtr)
{
    int i;
    printf("\nnstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
	for (i=0; i<statePtr->numMemory; i++) {
	    printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
	}
    printf("\tregisters:\n");
	for (i=0; i<NUMREGS; i++) {
	    printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
	}
    printf("end state\n");
}

int
convertNum(int num)
{
	if(num & (1<<15))
	{
		num -= (1 << 16);
	}
	return num;
}




