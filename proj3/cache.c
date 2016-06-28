/* --------------------------------------------------------------- */
/* Jesse Forrest */
/* Cache Simulator */
/* Prof Chang */
/* Fall 2004 */
/* --------------------------------------------------------------- */
/* --------------------------------------------------------------- */
/* Include necessary libraries */
#include <stdlib.h>
#include <string.h> 
#include <stdio.h> 

/* My Constants */
#define NUMLINES 512			/* maximum number of lines permitted */
#define BLOCKSIZE 512			/* maximum block size permitted */
#define NUMREGS 8				/* maximum number of registers permitted */
#define MAXLINELENGTH 1000		/* maximum line length permitted */
#define NUMMEMORY 65536			/* maximum number for memory */
#define NUMWORDS 512			/* maximum number of 'words' */
#define DATASIZE 256			/* maximum data size for error checking */
#define ADD 1					/* opCode instruction 1 is for an add */
#define AND 2					/* opCode instruction 2 is for an and */
#define NOR 3					/* opCode instruction 3 is for a nor */
#define LW 17					/* opCode instruction 17 is for a lw */
#define SW 18					/* opCode instruction 18 is for an sw */
#define BNE 19					/* opCode instruction 19 is for a bne */
#define JALR 33					/* opCode instruction 33 is for a jalr */
#define HALT 49					/* opCode instruction 49 is for a halt */
#define NOOP 50					/* opCode instruction 50 is for a noop */
#define TRUE 1					/* set TRUE to be 1 */
#define FALSE 0					/* set FALSE to be 0 */

/* struct stateStruct */
typedef struct stateStruct { 
  int pc; 
  int mem[NUMMEMORY]; 
  int reg[NUMREGS]; 
  int numMemory; 
} stateType; 

/* struct cacheStruct with blockStruct inside */
struct blockStruct
{
  struct lineStruct
  {
	/*int data[DATASIZE];*/
	/*int tag;*/
	int valid;
	int dirty;
	int leastRecentlyUsed;
	int word[NUMWORDS];

  } line[DATASIZE];
} cache[DATASIZE];

/* enumerations */
/*
 * Log the specifics of each cache action.
 *
 * address is the starting word address of the range of data being transferred.
 * size is the size of the range of data being transferred.
 * type specifies the source and destination of the data being transferred.
 *     cacheToProcessor: reading data from the cache to the processor
 *     processorToCache: writing data from the processor to the cache
 *     memoryToCache: reading data from the memory to the cache
 *     cacheToMemory: evicting cache data by writing it to the memory
 *     cacheToNowhere: evicting cache data by throwing it away
 */
enum actionType {cacheToProcessor, 
                 processorToCache, 
				 memoryToCache, 
				 cacheToMemory, 
				 cacheToNowhere};

/* additional functions used */
int convertNum(int);
void printAction (int, int, enum actionType type);
void cacheSim(int, int);

/* global variables */
  int theWriteCount;
  int blockSize;
  int numSets;
  int blocksPerSet;
  stateType state;  
  char line[MAXLINELENGTH];
/* --------------------------------------------------------------- */
/* --------------------------------------------------------------- */
/* main function */
int main(int argc, char *argv[]) 
{  
  int x,y;
  int regA, regB;
  int destinationReg;
  int holdInstruction = 0;
  int opCode = 0;
  int address = 0;
  int destination = 0;
  int front = 0;
  int offset;
  int max = 5000;
  int maxIndex = 0;
  
  FILE *filePtr; 
 
  if (argc != 5)  /* error checking for reading from argv[0] */
  { 
    exit(1); 
  } 
  filePtr = fopen(argv[1], "r"); 
  if (filePtr == NULL) /* error checking, can't open file */
  { 
    perror("fopen"); 
    exit(1); 
  } 
  state.pc=0;
  blockSize = atoi(argv[2]);
  numSets = atoi(argv[3]);
  blocksPerSet = atoi(argv[4]);

 
  /* sets all registers up to NUMREGS to 0 */
  for (x=0; x < NUMREGS; x++) 
  {
    state.reg[x]=0;                   
  }

  /* sets all cache attributes to 0 */
  for (x=0; x < numSets; x++) 
  {
    for(y=0; y < blocksPerSet; y++) 
    {
      cache[x].line[y].valid = 0;
      cache[x].line[y].dirty = 0;
	  /* cache[x].line[y].dirty = 1; */
      cache[x].line[y].leastRecentlyUsed = 0;
	  /* cache[x].line[y].leastRecentlyUsed = 1; */
    }
  }

  /* out of cache range */
  if (numSets*blocksPerSet > DATASIZE) exit(1);
	  
  /* blocksize must be power of 2 */
  if (blockSize % 2 != 0 && blockSize != 1) exit(1);
	 
  /* number of sets must be power of 2 */
  if (numSets %2 != 0 && numSets != 1) exit(1);
	   
  /* blocks per set must be a power of 2 */
  if (blocksPerSet %2 != 0 && blocksPerSet != 1) exit(1);
	   
  /* read holdInstruction the entire machine-code file into memory */ 
  for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL; state.numMemory++) 
  { 
    if (sscanf(line, "%d", state.mem + state.numMemory) != 1)  /* error in reading address */
	{ 
      exit(1); 
    } 
  }
  printf(" - Total block size: %d\n", blockSize);
  printf(" - Number of sets: %d\n", numSets);
  printf(" - Each set has X number of lines: %d\n\n", blocksPerSet);

  while(TRUE)
  {
	holdInstruction = state.mem[state.pc];
	regA = 0xF & (holdInstruction >> 0x4);
    regB = 0xF & (holdInstruction >> 0x0);
    opCode = (holdInstruction >> 0x18);
	offset = 0xFFFF & (holdInstruction >> 0x8);
    destinationReg = 0xF & (holdInstruction >> 0x8);
    destination = state.pc;
    cacheSim(destination, 0);

	/* ADD INSTRUCTION */
    if(opCode == ADD)
	{
      printf(" - ADD %d %d %d\n", regA, regB, destinationReg);
      state.reg[destinationReg] = state.reg[regA] + state.reg[regB];
    }
	/* AND INSTRUCTION */
    else if(opCode == AND)
	{
      printf(" - AND %d %d %d \n", regA, regB, destinationReg);
      state.reg[destinationReg] = state.reg[regA] & state.reg[regB];
    }
	/* NOR INSTRUCTION */
    else if(opCode == NOR)
	{
      printf(" - NOR %d %d %d\n", regA, regB, destinationReg);
      state.reg[destinationReg] = ~(state.reg[regA] | state.reg[regB]);
    }
	/* JALR INSTRUCTION */
    else if(opCode == JALR)
    {
      printf(" - JALR %d %d\n", regA, regB);
      state.reg[regB] = state.pc + 1;
      state.pc = state.reg[regA] - 1;
    }
	/* LW INSTRUCTION */
    else if (opCode == LW)
    {
      printf(" - LW %d %d %d\n", regA, regB, offset);
      state.reg[regB] = state.mem[convertNum(offset) + state.reg[regA]];
      destination = state.reg[regA] + convertNum(offset);
      cacheSim(destination, opCode);
	}
	/* SW INSTRUCTION */
    else if (opCode == SW) 
    {
      printf(" - SW %d %d %d\n", regA, regB, offset);
      destination = state.reg[regA] + convertNum(offset);
      cacheSim(destination, opCode);
    }
	/* BNE INSTRUCTION */
    else if (opCode == BNE) 
    {
      printf(" - BNE %d %d %d\n", regA, regB, offset);

	  /* branches on not equal */
      if ((state.reg[regA] - state.reg[regB]) != 0)   
	  {
		  state.pc = state.pc + offset;
	  }
    }
	/* HALT INSTRUCTION */
    else if(opCode == HALT)
    {
      exit(0);
    }
    else /* ERROR OCCURED, illegal opCode */
    {
      exit(1);
    }
    state.pc++;
  }

  return(0); 
} 
/* --------------------------------------------------------------- */
/* --------------------------------------------------------------- */
/* printAction function was given from Prof Chang to test program  */
void printAction(int address, int size, enum actionType type)
{
    printf("@@@ transferring word [%d-%d] ", address, address + size - 1);
    if (type == cacheToProcessor) {
        printf("from the cache to the processor\n");
    } else if (type == processorToCache) {
        printf("from the processor to the cache\n");
    } else if (type == memoryToCache) {
        printf("from the memory to the cache\n");
    } else if (type == cacheToMemory) {
        printf("from the cache to the memory\n");
    } else if (type == cacheToNowhere) {
        printf("from the cache to nowhere\n");
    }
}
/* --------------------------------------------------------------- */
/* --------------------------------------------------------------- */
/* convert number function */
int convertNum(int number)
{
  if(number & (0x1 << 0xF))
  {
    number = number - (0x1 << 0x10);
  }
  return(number);
}
/* --------------------------------------------------------------- */
/* --------------------------------------------------------------- */
/* cache simulator for lw and sw opCode                            */
void cacheSim(int destination, int opCode)
{  
  int x, y;
  int maxleastRecentlyUsedCount = NUMMEMORY;
  int leastRecentlyUsedBlock = 0;
  int minleastRecentlyUsedCount = 0, front = 0;
  int set = (destination / blockSize) & (numSets - 1);

  /* search for a block within a set, in order to be stored to cache */
  for (x=0; x < blocksPerSet; x++)
  {
    if(cache[set].line[x].valid == 1)
	{
	  if (minleastRecentlyUsedCount)
	  {
		cache[set].line[x].leastRecentlyUsed = theWriteCount;
	  }
	  else
	  {
		minleastRecentlyUsedCount = 0;
	  }
      for (y=0; y < blockSize; y++)
	  {
        if (cache[set].line[x].word[y] == destination)
        { 
          theWriteCount++;
          cache[set].line[x].leastRecentlyUsed = theWriteCount;	  
          if((opCode == 0) || (opCode == LW))
		  {
			printAction(destination, 1, cacheToProcessor);
		  }
          else if(opCode == SW) 
          {
            printAction(destination, 1, processorToCache);
			/* cache[set].line[x].dirty = 0; */
            cache[set].line[x].dirty = 1;
          }
          return;
        }
	  }
	}
  }

  /* tries to find block in another set if miss */
  for (x=0; x < blocksPerSet; x++)
  {
    if (cache[set].line[x].valid == 0)
    { 
      theWriteCount++;
      cache[set].line[x].valid = 1;
      cache[set].line[x].dirty = 0;
	  /* cache[set].line[x].dirty = 1; */
      cache[set].line[x].leastRecentlyUsed = theWriteCount;
      front = destination - (destination % blockSize);
 
      for (y=0; y < blockSize; y++)
      {
        cache[set].line[x].word[y] = front;
        front = front + 1;
      }
      if (minleastRecentlyUsedCount)
	  {
		cache[set].line[x].leastRecentlyUsed = theWriteCount;
	  }
	  else
	  {
		minleastRecentlyUsedCount = 0;
	  }
      printAction((front - blockSize), blockSize, memoryToCache);

     if ((opCode == 0) || (opCode == LW))
	 {
        printAction(destination, 1, cacheToProcessor);
	 }
     else if (opCode == SW)
     {
		cache[set].line[x].dirty = 1;
        printAction(destination, 1, processorToCache);

     }     
     return;
    }
  }
  
  /* locates least recently used block for replacement */
  front = (destination - (destination % blockSize));
  for (x=0; x < blocksPerSet; x++)
  {
    if (maxleastRecentlyUsedCount > cache[set].line[x].leastRecentlyUsed)
    {
            maxleastRecentlyUsedCount = cache[set].line[x].leastRecentlyUsed;
			leastRecentlyUsedBlock = x;
    }
  }
 
  if (!cache[set].line[leastRecentlyUsedBlock].dirty)
  {
	  cache[set].line[leastRecentlyUsedBlock].dirty = 0;
  }

  if (cache[set].line[leastRecentlyUsedBlock].dirty)
  {
	  cache[set].line[leastRecentlyUsedBlock].dirty = 1;
  }

  if (cache[set].line[leastRecentlyUsedBlock].dirty == 0)
  {
    printAction(cache[set].line[leastRecentlyUsedBlock].word[0], blockSize, cacheToNowhere);
  }
  else if (cache[set].line[leastRecentlyUsedBlock].dirty == 1)
  {
    printAction(cache[set].line[leastRecentlyUsedBlock].word[0], blockSize, cacheToMemory);
  }

  /* changes the found least recently used block */
  theWriteCount = theWriteCount + 1;
  cache[set].line[leastRecentlyUsedBlock].valid = 1;
  cache[set].line[leastRecentlyUsedBlock].dirty = 0;
  /*cache[set].line[leastRecentlyUsedBlock].dirty = 1;*/
  cache[set].line[leastRecentlyUsedBlock].leastRecentlyUsed = theWriteCount;

  for (x=0; x < blockSize; x++)
  {
    cache[set].line[leastRecentlyUsedBlock].word[x] = front;
    front++;
  } 
  printAction((front - blockSize), blockSize, memoryToCache);
  /* printAction((front - blockSize), blockSize, cacheToProcessor); */
  if ((opCode == 0) || (opCode == LW))
  {
    printAction(destination, 1, cacheToProcessor);
  }
  else if (opCode == SW)
  {
    printAction(destination, 1, processorToCache);
    cache[set].line[leastRecentlyUsedBlock].dirty = 1;
  }  
  return;
}
/* --------------------------------------------------------------- */


