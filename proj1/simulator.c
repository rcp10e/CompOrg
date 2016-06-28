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

void printState(stateType *);
int convertNum(int);
int
main(int argc, char *argv[])
{
   	 char line[MAXLINELENGTH];
    	stateType state;
    	FILE *filePtr;

   	int i =0;
	int opcode;
	int arg0;
	int arg1;
	int arg2;
	int regA;
	int regB;
	int destReg;
	int offset;
	int loopcount;



    	if (argc != 2) {
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
		printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    	}
	
	for(i = 0; i < NUMREGS; i++)
		state.reg[i] = 0;
	
	state.reg[1] = 1;

	printState(&state);

	loopcount = state.pc + 1;
	
	
	for(state.pc; state.pc < state.numMemory; state.pc++) 		
	{
		
		opcode = state.mem[state.pc] >> 22;
		
		
		
		//add
		if(opcode == 0)
		{
							
			regA = (state.mem[state.pc] >> 19) & 7;
			regB = (state.mem[state.pc] >> 16) & 7;
			destReg = (state.mem[state.pc]) & 7;						
			state.reg[destReg] = state.reg[regA] + state.reg[regB];
			state.pc++;
			printState(&state);
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
			printState(&state);
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
			printState(&state);	
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
			
						
			state.reg[regB] = state.mem[state.reg[regA] + offset];
			state.pc++;
			printState(&state);
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
			printState(&state);
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
			
			state.mem[offset + state.reg[regA]] = state.reg[regB];
			state.pc++;
			printState(&state);
			state.pc--;
		}

		//halt
		if(opcode == 6)
		{
			
			state.pc++;
			printf("machine halted\ntotal of %d instructions executed\nfinal state of machine:", loopcount);
			printState(&state);
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

void
printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate:\n");
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
