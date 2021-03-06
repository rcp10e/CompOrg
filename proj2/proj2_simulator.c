#include<string.h>
#include<stdio.h>
#include<stdlib.h>

#define NUMMEMORY 65536 /* maximum number of data words in memory */
#define NUMREGS 8 /* number of machine registers */

#define ADD 0
#define NAND 1
#define LW 2
#define SW 3
#define BEQ 4
#define JALR 5  /* don't implement in project 2 */
#define HALT 6
#define NOOP 7
#define MAXLINELENGTH 1000

#define NOOPINSTRUCTION 0x1c00000

typedef struct IFIDStruct {
    int instr;
    int pcPlus1;
} IFIDType;

typedef struct IDEXStruct {
    int instr;
    int pcPlus1;
    int readRegA;
    int readRegB;
    int offset;
} IDEXType;

typedef struct EXMEMStruct {
    int instr;
    int branchTarget;
    int aluResult;
    int readRegB;
} EXMEMType;

typedef struct MEMWBStruct {
    int instr;
    int writeData;
} MEMWBType;

typedef struct WBENDStruct {
    int instr;
    int writeData;
} WBENDType;

typedef struct stateStruct {
    int pc;
    int instrMem[NUMMEMORY];
    int dataMem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
    IFIDType IFID;
    IDEXType IDEX;
    EXMEMType EXMEM;
    MEMWBType MEMWB;
    WBENDType WBEND;
    int cycles; /* number of cycles run so far */
} stateType;

typedef struct hazardStruct {
	int regA[3];
	int regB[3];
	int dest[3];
}hazardType;
 
void printState(stateType *statePtr);
void run(stateType state);
int field0(int instruction);
int field1(int instruction);
int field2(int instruction);
int opcode(int instruction);
void printInstruction(int instr);
int convertNum(int num);

hazardType hazard;


int i;

int main(int argc, char *argv[])
{


    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;

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
	if (sscanf(line, "%d", state.instrMem + state.numMemory) != 1) {
	    printf("error in reading address %d\n", state.numMemory);
	    exit(1);
	}

	if(sscanf(line, "%d", state.dataMem + state.numMemory) != 1) {
	    printf("error in reading address %d\n", state.numMemory);
	    exit(1);
	}

	}

	state.dataMem[100] = 1;
	state.dataMem[101] = 5;
	state.dataMem[102] = 2;
	state.pc = 0;
	state.cycles = 0;
	state.reg[0] = 0;
	state.reg[1] = 0;
	state.reg[2] = 0;
	state.reg[3] = 0;
	state.reg[4] = 0;
	state.reg[5] = 0;
	state.reg[6] = 0;
	state.reg[7] = 0;
	
	state.IFID.instr = NOOPINSTRUCTION;
	state.IFID.pcPlus1 = 0;
	state.IDEX.instr = NOOPINSTRUCTION;
	state.IDEX.pcPlus1 = 0;
	state.IDEX.readRegA = 0;
	state.IDEX.readRegB = 0;
	state.IDEX.offset = 0;
	state.EXMEM.instr = NOOPINSTRUCTION;
	state.EXMEM.aluResult = 0;
	state.EXMEM.branchTarget= 0;
	state.EXMEM.readRegB = 0;
	state.MEMWB.instr = NOOPINSTRUCTION;
	state.MEMWB.writeData = 0; 
	state.WBEND.instr = NOOPINSTRUCTION;
	state.WBEND.writeData = 0; 


	run(state);

	return (0);
}

void run(stateType state)
{
	stateType newState;
		
 while (1) {

	printf("%d\n", state.dataMem[104]);	
	printState(&state);

	/* check for halt */
	if (opcode(state.MEMWB.instr) == HALT) {
	    printf("machine halted\n");
	    printf("total of %d cycles executed\n", state.cycles);
	    exit(0);
	}

	
	newState = state;
	newState.cycles = newState.cycles + 1;

	/* --------------------- IF stage --------------------- */

	
	newState.IFID.instr = state.instrMem[state.pc];
	newState.IFID.pcPlus1 = state.pc + 1;
	
	
	
	/* --------------------- ID stage --------------------- */

	newState.IDEX.instr = state.IFID.instr;
	newState.IDEX.readRegA = state.reg[field0(state.instrMem[state.pc - 1])];
	newState.IDEX.readRegB = state.reg[field1(state.instrMem[state.pc - 1])];
	newState.IDEX.offset = field2(state.IFID.instr);
	newState.IDEX.pcPlus1 = state.IFID.pcPlus1;
	
	hazard.regA[0] = field0(state.IFID.instr);
	hazard.regB[0] = field1(state.IFID.instr);
	hazard.dest[0] = field2(state.IFID.instr); 


	if(newState.IDEX.offset >> 15 == 1)
	 	newState.IDEX.offset = convertNum(newState.IDEX.offset);

	
	


	/* --------------------- EX stage --------------------- */

	newState.EXMEM.instr = state.IDEX.instr;
    	newState.EXMEM.readRegB = state.IDEX.readRegB;
	hazard.regA[1] = field0(state.IDEX.instr);
	hazard.regB[1] = field1(state.IDEX.instr);
	hazard.dest[1] = field2(state.IDEX.instr);

  	newState.EXMEM.readRegB = state.IDEX.readRegB;

	newState.EXMEM.aluResult = 0;
	
	if(opcode(newState.EXMEM.instr) == ADD)
	{	
		
		newState.EXMEM.aluResult = state.IDEX.readRegA + state.IDEX.readRegB;
		newState.EXMEM.branchTarget = state.IDEX.offset;

	}
	
	if(opcode(newState.EXMEM.instr) == NAND)
	{
		newState.EXMEM.aluResult = ~(state.IDEX.readRegA & state.IDEX.readRegB);
		newState.EXMEM.branchTarget = state.IDEX.offset;
 
	}
	
	if(opcode(newState.EXMEM.instr) == LW)
	{
		newState.EXMEM.aluResult = state.IDEX.readRegA + state.IDEX.offset;
		newState.EXMEM.branchTarget = field1(state.IDEX.instr);
		newState.reg[field1(newState.EXMEM.instr)] = state.dataMem[newState.EXMEM.aluResult];
	}
	
	if(opcode(newState.EXMEM.instr) == SW)
	{
		newState.EXMEM.branchTarget = field1(state.IDEX.instr);
		newState.EXMEM.aluResult = state.IDEX.readRegA + state.IDEX.offset;
		newState.dataMem[newState.EXMEM.aluResult] = state.reg[newState.EXMEM.branchTarget];
	}
	
	if(opcode(newState.EXMEM.instr) == BEQ)
	{
		newState.EXMEM.aluResult = state.IDEX.readRegA - state.IDEX.readRegB;
		if(newState.EXMEM.aluResult == 0)
		{
			newState.EXMEM.branchTarget = state.pc + state.IDEX.offset + 1;
		}
		
		
	
		
	}
	
	
	
	/* --------------------- MEM stage --------------------- */

	newState.MEMWB.instr = state.EXMEM.instr;

	hazard.regA[2] = field0(state.EXMEM.instr);
	hazard.regB[2] = field1(state.EXMEM.instr);
	hazard.dest[2] = field2(state.EXMEM.instr);
	

	if(opcode(state.EXMEM.instr) == ADD)
	{
		newState.MEMWB.writeData = state.EXMEM.aluResult;

		if(hazard.dest[2] == hazard.regA[1] && hazard.dest[2] == hazard.regB[1] && opcode(newState.EXMEM.instr) == ADD)
		{
			newState.EXMEM.aluResult = state.EXMEM.aluResult + state.EXMEM.aluResult;
		}
		if(hazard.dest[2] == hazard.regA[1] && opcode(newState.EXMEM.instr) == ADD)
		{
			
			newState.EXMEM.aluResult = state.EXMEM.aluResult + state.IDEX.readRegB; 
		}
		if(hazard.dest[2] == hazard.regB[1] && opcode(newState.EXMEM.instr) == ADD)
		{
			newState.EXMEM.aluResult = state.EXMEM.aluResult + state.IDEX.readRegA;
		}
		if(hazard.dest[2] == hazard.regA[1] && hazard.dest[2] == hazard.regB[1] && opcode(newState.EXMEM.instr) == NAND)
		{
			newState.EXMEM.aluResult = ~(state.EXMEM.aluResult & state.EXMEM.aluResult);
		} 
		if(hazard.dest[2] == hazard.regA[1] && opcode(newState.EXMEM.instr) == NAND)
		{
			newState.EXMEM.aluResult = ~(state.EXMEM.aluResult & state.IDEX.readRegB);
		}
		if(hazard.dest[2] == hazard.regB[1] && opcode(newState.EXMEM.instr) == NAND)
		{
			newState.EXMEM.aluResult = ~(state.EXMEM.aluResult & state.IDEX.readRegA);
		}
		if(hazard.dest[2] == hazard.regA[1] && opcode(newState.EXMEM.instr) == LW)
		{
			newState.EXMEM.aluResult = state.EXMEM.aluResult + state.IDEX.offset;
		}
		if(hazard.dest[2] == hazard.regB[1] && opcode(newState.EXMEM.instr) == SW)
		{
			newState.EXMEM.aluResult = state.EXMEM.aluResult + state.IDEX.offset; 
		}
		if(hazard.dest[2] == hazard.regB[1] && opcode(newState.EXMEM.instr) == SW)
		{
			newState.EXMEM.readRegB = state.EXMEM.aluResult;
		}
		


		if(hazard.dest[2] == hazard.regA[1] && hazard.dest[2] == hazard.regB[1] && opcode(newState.EXMEM.instr) == BEQ)
		{
			newState.EXMEM.aluResult = 0;
			newState.EXMEM.branchTarget = state.pc + state.IDEX.offset + 1;
		}
		if(hazard.dest[2] == hazard.regA[1] && opcode(newState.EXMEM.instr) == BEQ)
		{
			newState.EXMEM.aluResult = state.EXMEM.aluResult - state.IDEX.readRegB;
			if(newState.EXMEM.aluResult == 0)
				newState.EXMEM.branchTarget = state.pc + state.IDEX.offset + 1;
		}
		if(hazard.dest[2] == hazard.regB[1] && opcode(newState.EXMEM.instr) == BEQ)
		{
			newState.EXMEM.aluResult = state.EXMEM.aluResult - state.IDEX.readRegA;
			if(newState.EXMEM.aluResult == 0)
				newState.EXMEM.branchTarget = state.pc + state.IDEX.offset + 1;
		}



		if(hazard.dest[2] == hazard.regA[0] && opcode(newState.IDEX.instr) == ADD)
		{	
			newState.IDEX.readRegA = state.EXMEM.aluResult;
		}
		if(hazard.dest[2] == hazard.regB[0] && opcode(newState.IDEX.instr) == ADD)
		{
			newState.IDEX.readRegB = state.EXMEM.aluResult;
		}
		if(hazard.dest[2] == hazard.regA[0] && opcode(newState.IDEX.instr) == NAND)
		{
			newState.IDEX.readRegA = state.EXMEM.aluResult;
		}
		if(hazard.dest[2] == hazard.regB[0] && opcode(newState.IDEX.instr) == NAND)
		{
			newState.IDEX.readRegB = state.EXMEM.aluResult;
		}
		if(hazard.dest[2] == hazard.regA[0] && opcode(newState.IDEX.instr) == LW)
		{
			newState.IDEX.readRegA = state.EXMEM.aluResult;
		}
		if(hazard.dest[2] == hazard.regA[0] && opcode(newState.IDEX.instr) == SW)
		{
			newState.IDEX.readRegA = state.EXMEM.aluResult;
		}
		if(hazard.dest[2] == hazard.regA[0] && opcode(newState.IDEX.instr) == BEQ)
		{
			newState.IDEX.readRegA = state.EXMEM.aluResult;
		}
		if(hazard.dest[2] == hazard.regB[0] && opcode(newState.IDEX.instr) == BEQ)
		{
			newState.IDEX.readRegB = state.EXMEM.aluResult;
		}
	




	}

	if(opcode(state.EXMEM.instr) == NAND)
	{
		newState.MEMWB.writeData = state.EXMEM.aluResult;

		if(hazard.dest[2] == hazard.regA[1] && hazard.dest[2] == hazard.regB[1] && opcode(newState.EXMEM.instr) == ADD)
		{
			newState.EXMEM.aluResult = state.EXMEM.aluResult + state.EXMEM.aluResult;
		}
		if(hazard.dest[2] == hazard.regA[1] && opcode(newState.EXMEM.instr) == ADD)
		{
			newState.EXMEM.aluResult = state.EXMEM.aluResult + state.IDEX.readRegB;
		}
		if(hazard.dest[2] == hazard.regB[1] && opcode(newState.EXMEM.instr) == ADD)
		{
			newState.EXMEM.aluResult = state.EXMEM.aluResult + state.IDEX.readRegA;
		}
		if(hazard.dest[2] == hazard.regA[1] && hazard.dest[2] == hazard.regB[1] && opcode(newState.EXMEM.instr) == NAND)
		{
			newState.EXMEM.aluResult = ~(state.EXMEM.aluResult & state.EXMEM.aluResult);
		}
		if(hazard.dest[2] == hazard.regA[1] && opcode(newState.EXMEM.instr) == NAND)
		{
			newState.EXMEM.aluResult = ~(state.EXMEM.aluResult & state.IDEX.readRegB);
		}
		if(hazard.dest[2] == hazard.regB[1] && opcode(newState.EXMEM.instr) == NAND)
		{
			newState.EXMEM.aluResult = ~(state.EXMEM.aluResult & state.IDEX.readRegA);
		}
		if(hazard.dest[2] == hazard.regA[1] && opcode(newState.EXMEM.instr) == LW)
		{
			newState.EXMEM.aluResult = state.EXMEM.aluResult + state.IDEX.offset;
		}
		if(hazard.dest[2] == hazard.regA[1] && opcode(newState.EXMEM.instr) == SW)
		{
			newState.EXMEM.aluResult = state.EXMEM.aluResult + state.IDEX.offset;
		}
		



		if(hazard.dest[2] == hazard.regA[1] && hazard.dest[2] == hazard.regB[1] && opcode(newState.EXMEM.instr) == BEQ)
		{
			newState.EXMEM.aluResult = 0;
			newState.EXMEM.branchTarget = state.pc + state.IDEX.offset + 1;
		}
		if(hazard.dest[2] == hazard.regA[1] && opcode(newState.EXMEM.instr) == BEQ)
		{
			newState.EXMEM.aluResult = state.EXMEM.aluResult - state.IDEX.readRegB;
			if(newState.EXMEM.aluResult == 0)
				newState.EXMEM.branchTarget = state.pc + state.IDEX.offset + 1;
		}
		if(hazard.dest[2] == hazard.regB[1] && opcode(newState.EXMEM.instr) == BEQ)
		{
			newState.EXMEM.aluResult = state.EXMEM.aluResult - state.IDEX.readRegA;
			if(newState.EXMEM.aluResult == 0)
				newState.EXMEM.branchTarget = state.pc + state.IDEX.offset + 1;
		}





		if(hazard.dest[2] == hazard.regA[0] && opcode(newState.IDEX.instr) == ADD)
		{
			newState.IDEX.readRegA = state.EXMEM.aluResult;
		}
		if(hazard.dest[2] == hazard.regB[0] && opcode(newState.IDEX.instr) == ADD)
		{
			newState.IDEX.readRegB = state.EXMEM.aluResult;
		}
		if(hazard.dest[2] == hazard.regA[0] && opcode(newState.IDEX.instr) == NAND)
		{
			newState.IDEX.readRegA = state.EXMEM.aluResult;
		}
		if(hazard.dest[2] == hazard.regB[0] && opcode(newState.IDEX.instr) == NAND)
		{
			newState.IDEX.readRegB = state.EXMEM.aluResult;
		}
		if(hazard.dest[2] == hazard.regA[0] && opcode(newState.IDEX.instr) == LW)
		{
			newState.IDEX.readRegA = state.EXMEM.aluResult;
		}
		if(hazard.dest[2] == hazard.regA[0] && opcode(newState.IDEX.instr) == SW)
		{
			newState.IDEX.readRegA = state.EXMEM.aluResult;
		}
		if(hazard.dest[2] == hazard.regA[0] && opcode(newState.IDEX.instr) == BEQ)
		{
			newState.IDEX.readRegA = state.EXMEM.aluResult;
		}
		if(hazard.dest[2] == hazard.regB[0] && opcode(newState.IDEX.instr) == BEQ)
		{
			newState.IDEX.readRegB = state.EXMEM.aluResult;
		}


	}

	if(opcode(state.EXMEM.instr) == LW)
	{
		newState.MEMWB.writeData = state.dataMem[state.EXMEM.aluResult];
		

		if(hazard.regB[2] == hazard.regA[1] && hazard.regB[2] == hazard.regB[1] && opcode(newState.EXMEM.instr) == ADD)
		{
			newState.EXMEM.aluResult = state.dataMem[state.EXMEM.aluResult] + state.dataMem[state.EXMEM.aluResult];
		}
		if(hazard.regB[2] == hazard.regA[1] && opcode(newState.EXMEM.instr) == ADD)
		{
			newState.EXMEM.aluResult = state.reg[field0(state.IDEX.instr)] + state.reg[field1(state.IDEX.instr)];
		}
		if(hazard.regB[2] == hazard.regB[1] && opcode(newState.EXMEM.instr) == ADD)
		{
			newState.EXMEM.aluResult = state.reg[field1(state.IDEX.instr)] + state.reg[field0(state.IDEX.instr)];
		}
		if(hazard.regB[2] == hazard.regA[1] && hazard.regB[2] == hazard.regB[1] && opcode(newState.EXMEM.instr) == NAND)
		{
			newState.EXMEM.aluResult = ~(state.dataMem[state.EXMEM.aluResult] & state.dataMem[state.EXMEM.aluResult]); 		         
		}
		if(hazard.regB[2] == hazard.regA[1] && opcode(newState.EXMEM.instr) == NAND)
		{
			newState.EXMEM.aluResult = ~(state.dataMem[state.EXMEM.aluResult] & state.reg[field1(state.IDEX.instr)]);
		}
		if(hazard.regB[2] == hazard.regB[1] && opcode(newState.EXMEM.instr) == NAND)
		{
			newState.EXMEM.aluResult = ~(state.dataMem[state.EXMEM.aluResult] & state.reg[field0(state.IDEX.instr)]);
		}
		

		if(hazard.regB[2] == hazard.regA[1] && hazard.regB[2] == hazard.regB[1] && opcode(newState.EXMEM.instr) == BEQ)
		{
			newState.EXMEM.aluResult = 0;
			newState.EXMEM.branchTarget = state.pc + state.IDEX.offset + 1;
		}
		if(hazard.regB[2] == hazard.regA[1] && opcode(newState.EXMEM.instr) == BEQ)
		{
			newState.EXMEM.aluResult = state.dataMem[state.EXMEM.aluResult] - state.reg[field1(state.IDEX.instr)];
			if(newState.EXMEM.aluResult == 0)
				newState.EXMEM.branchTarget = state.pc + state.IDEX.offset + 1;
		}
		if(hazard.regB[2] == hazard.regB[1] && opcode(newState.EXMEM.instr) == BEQ)
		{
			newState.EXMEM.aluResult = state.dataMem[state.EXMEM.aluResult] - state.reg[field0(state.IDEX.instr)];
			if(newState.EXMEM.aluResult == 0)
				newState.EXMEM.branchTarget = state.pc + state.IDEX.offset + 1;
		} 





		if(hazard.regB[2] == hazard.regA[0] && opcode(newState.IDEX.instr) == ADD)
		{
			newState.IDEX.readRegA = state.dataMem[state.EXMEM.aluResult];
		}
		if(hazard.regB[2] == hazard.regB[0] && opcode(newState.IDEX.instr) == ADD)
		{
			newState.IDEX.readRegB == state.dataMem[state.EXMEM.aluResult];
		}
		if(hazard.regB[2] == hazard.regA[0] && opcode(newState.IDEX.instr) == NAND)
		{
			newState.IDEX.readRegA = state.dataMem[state.EXMEM.aluResult];
		} 
		if(hazard.regB[2] == hazard.regB[0] && opcode(newState.IDEX.instr) == NAND)
		{
			newState.IDEX.readRegB = state.dataMem[state.EXMEM.aluResult];
		}
		if(hazard.regB[2] = hazard.regA[0] && opcode(newState.IDEX.instr) == LW)
		{
			newState.IDEX.readRegA = state.dataMem[state.EXMEM.aluResult];
		}
		if(hazard.regB[2] == hazard.regA[0] && opcode(newState.IDEX.instr) == SW)
		{
			newState.IDEX.readRegA = state.dataMem[state.EXMEM.aluResult];
		}
		if(hazard.regB[2] == hazard.regA[0] && opcode(newState.IDEX.instr) == BEQ)
		{
			newState.IDEX.readRegA = state.dataMem[state.EXMEM.aluResult];
		}
		if(hazard.regB[2] == hazard.regB[0] && opcode(newState.IDEX.instr) == BEQ)
		{
			newState.IDEX.readRegB = state.dataMem[state.EXMEM.aluResult];
		}
		

	}


	if(opcode(state.EXMEM.instr) == SW)
	{
		newState.MEMWB.writeData = state.reg[state.EXMEM.branchTarget];
		newState.dataMem[state.EXMEM.aluResult] = state.EXMEM.readRegB;

		if(hazard.regB[2] == hazard.regA[1] && hazard.regB[2] == hazard.regB[1] && opcode(newState.EXMEM.instr) == ADD)
		{
			newState.EXMEM.aluResult = state.dataMem[state.EXMEM.aluResult] + state.dataMem[state.EXMEM.aluResult];
		}
		if(hazard.regB[2] == hazard.regA[1] && opcode(newState.EXMEM.instr) == ADD)
		{
			newState.EXMEM.aluResult = state.dataMem[state.EXMEM.aluResult] + state.IDEX.readRegB;
		}
		if(hazard.regB[2] == hazard.regB[1] && opcode(newState.EXMEM.instr) == ADD)
		{
			newState.EXMEM.aluResult = state.dataMem[state.EXMEM.aluResult] + state.IDEX.readRegA;
		}
		if(hazard.regB[2] == hazard.regA[1] && hazard.regB[2] == hazard.regB[1] && opcode(newState.EXMEM.instr) == NAND)
		{
			newState.EXMEM.aluResult = ~(state.dataMem[state.EXMEM.aluResult] & state.dataMem[state.EXMEM.aluResult]); 		         
		}
		if(hazard.regB[2] == hazard.regA[1] && opcode(newState.EXMEM.instr) == NAND)
		{
			newState.EXMEM.aluResult = ~(state.dataMem[state.EXMEM.aluResult] & state.IDEX.readRegB);
		}
		if(hazard.regB[2] == hazard.regB[1] && opcode(newState.EXMEM.instr) == NAND)
		{
			newState.EXMEM.aluResult = ~(state.dataMem[state.EXMEM.aluResult] & state.IDEX.readRegA);
		}
		if(hazard.regB[2] == hazard.regA[1] && hazard.regB[2] == hazard.regB[1] && opcode(newState.EXMEM.instr) == BEQ)
		{
			newState.EXMEM.aluResult = 0;
			newState.EXMEM.branchTarget = state.pc + state.IDEX.offset + 1;
		}
		if(hazard.regB[2] == hazard.regA[1] && opcode(newState.EXMEM.instr) == BEQ)
		{
			newState.EXMEM.aluResult = state.dataMem[state.EXMEM.aluResult] - state.IDEX.readRegB;
			if(newState.EXMEM.aluResult == 0)
				newState.EXMEM.branchTarget = state.pc + state.IDEX.offset + 1;
		}
		if(hazard.regB[2] == hazard.regB[1] && opcode(newState.EXMEM.instr) == BEQ)
		{
			newState.EXMEM.aluResult = state.dataMem[state.EXMEM.aluResult] - state.IDEX.readRegA;
			if(newState.EXMEM.aluResult == 0)
				newState.EXMEM.branchTarget = state.pc + state.IDEX.offset + 1;
		} 
		if(newState.EXMEM.aluResult == state.EXMEM.aluResult && opcode(newState.EXMEM.instr) == LW)
		{
			newState.EXMEM.readRegB = state.IDEX.readRegB;
			newState.dataMem[newState.EXMEM.aluResult] = state.reg[state.EXMEM.branchTarget];
			
		}


		if(hazard.regB[2] == hazard.regA[0] && opcode(newState.IDEX.instr) == ADD)
		{
			newState.IDEX.readRegA = state.dataMem[state.EXMEM.aluResult];
		}
		if(hazard.regB[2] == hazard.regB[0] && opcode(newState.IDEX.instr) == ADD)
		{
			newState.IDEX.readRegB == state.dataMem[state.EXMEM.aluResult];
		}
		if(hazard.regB[2] == hazard.regA[0] && opcode(newState.IDEX.instr) == NAND)
		{
			newState.IDEX.readRegA = state.dataMem[state.EXMEM.aluResult];
		} 
		if(hazard.regB[2] == hazard.regB[0] && opcode(newState.IDEX.instr) == NAND)
		{
			newState.IDEX.readRegB = state.dataMem[state.EXMEM.aluResult];
		}
		if(hazard.regB[2] = hazard.regA[0] && opcode(newState.IDEX.instr) == LW)
		{
			newState.IDEX.readRegA = state.dataMem[state.EXMEM.aluResult];
		}
		if(hazard.regB[2] == hazard.regA[0] && opcode(newState.IDEX.instr) == SW)
		{
			newState.IDEX.readRegA = state.dataMem[state.EXMEM.aluResult];
		}
		if(hazard.regB[2] == hazard.regA[0] && opcode(newState.IDEX.instr) == BEQ)
		{
			newState.IDEX.readRegA = state.dataMem[state.EXMEM.aluResult];
		}
		if(hazard.regB[2] == hazard.regB[0] && opcode(newState.IDEX.instr) == BEQ)
		{
			newState.IDEX.readRegB = state.dataMem[state.EXMEM.aluResult];
		}
		

	}

	
	if(opcode(state.EXMEM.instr) == BEQ && state.EXMEM.aluResult == 0)
	{

		newState.MEMWB.writeData = state.EXMEM.branchTarget;
		
		newState.IFID.instr = NOOPINSTRUCTION;	
		newState.IDEX.instr = NOOPINSTRUCTION;
		newState.IDEX.readRegA = 0;
		newState.IDEX.readRegB = 0;
		newState.IDEX.offset = 0;
		newState.EXMEM.instr = NOOPINSTRUCTION;
		newState.EXMEM.aluResult = 0;
		newState.EXMEM.branchTarget = 0;
		newState.EXMEM.readRegB = 0;
	
	}


	/* --------------------- WB stage --------------------- */

	newState.WBEND.instr = state.MEMWB.instr;
	newState.WBEND.writeData = state.MEMWB.writeData;



	if(opcode(state.MEMWB.instr) == ADD && field2(state.MEMWB.instr) != 0)
	{
		newState.reg[field2(state.MEMWB.instr)] = state.MEMWB.writeData;
		
	}


	if(opcode(state.MEMWB.instr) == NAND)
	{
		newState.reg[field2(state.MEMWB.instr)] = state.MEMWB.writeData;
	
	}

	if(opcode(state.MEMWB.instr) == LW)
	{
		newState.reg[field1(state.MEMWB.instr)] = state.MEMWB.writeData;
	}

	if(opcode(state.MEMWB.instr) == SW)
	{
		newState.dataMem[state.MEMWB.writeData] =  state.reg[field1(state.MEMWB.instr)];
	}
	
	if(opcode(state.MEMWB.instr) == BEQ)
	{
		newState.pc = state.MEMWB.writeData;
		newState.pc--;
	}
	

	
	newState.pc++;
	state = newState; /* this is the last statement before end of the loop.
			    It marks the end of the cycle and updates the
			    current state with the values calculated in this
			    cycle */

		
    }


 
}

void printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate before cycle %d starts\n", statePtr->cycles);
    printf("\tpc %d\n", statePtr->pc);

    printf("\tdata memory:\n");
	for (i=0; i<statePtr->numMemory; i++) {
	    printf("\t\tdataMem[ %d ] %d\n", i, statePtr->dataMem[i]);
	}
    printf("\tregisters:\n");
	for (i=0; i<NUMREGS; i++) {
	    printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
	}
    printf("\tIFID:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->IFID.instr);
	printf("\t\tpcPlus1 %d\n", statePtr->IFID.pcPlus1);
    printf("\tIDEX:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->IDEX.instr);
	printf("\t\tpcPlus1 %d\n", statePtr->IDEX.pcPlus1);
	printf("\t\treadRegA %d\n", statePtr->IDEX.readRegA);
	printf("\t\treadRegB %d\n", statePtr->IDEX.readRegB);
	printf("\t\toffset %d\n", statePtr->IDEX.offset);
    printf("\tEXMEM:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->EXMEM.instr);
	printf("\t\tbranchTarget %d\n", statePtr->EXMEM.branchTarget);
	printf("\t\taluResult %d\n", statePtr->EXMEM.aluResult);
	printf("\t\treadRegB %d\n", statePtr->EXMEM.readRegB);
    printf("\tMEMWB:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->MEMWB.instr);
	printf("\t\twriteData %d\n", statePtr->MEMWB.writeData);
    printf("\tWBEND:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->WBEND.instr);
	printf("\t\twriteData %d\n", statePtr->WBEND.writeData);
}

int field0(int instruction)
{
    return( (instruction>>19) & 0x7);
}

int field1(int instruction)
{
    return( (instruction>>16) & 0x7);
}

int field2(int instruction)
{
    return(instruction & 0xFFFF);
}

int opcode(int instruction)
{
    return(instruction>>22);
}

void printInstruction(int instr)
{
    char opcodeString[10];
    if (opcode(instr) == ADD) {
	strcpy(opcodeString, "add");
    } else if (opcode(instr) == NAND) {
	strcpy(opcodeString, "nand");
    } else if (opcode(instr) == LW) {
	strcpy(opcodeString, "lw");
    } else if (opcode(instr) == SW) {
	strcpy(opcodeString, "sw");
    } else if (opcode(instr) == BEQ) {
	strcpy(opcodeString, "beq");
    } else if (opcode(instr) == JALR) {
	strcpy(opcodeString, "jalr");
    } else if (opcode(instr) == HALT) {
	strcpy(opcodeString, "halt");
    } else if (opcode(instr) == NOOP) {
	strcpy(opcodeString, "noop");
    } else {
	strcpy(opcodeString, "data");
    }

    printf("%s %d %d %d\n", opcodeString, field0(instr), field1(instr),
	field2(instr));
}

int convertNum(int num)
{
	if(num & (1<<15))
	{
		num -= (1 << 16);
	}
	return num;
}


