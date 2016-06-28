/* Assembler code for LC3101 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXLINELENGTH 1000
#define MAXNUMLABELS 65536
#define MAXLABELNAME 7
int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);
void checkRegisterValues(char *, char *);



/*opcode global constants*/
#define ADDVAL 0
#define NANDVAL 1
#define LWVAL 2
#define SWVAL 3
#define BEQVAL 4
#define JALRVAL 5
#define HALTVAL 6
#define NOOPVAL 7

struct labelStruct
{
	char labelName[MAXLINELENGTH];
	int labelAddress;
	char labelOpcode[MAXLINELENGTH];
	char labelArg0[MAXLINELENGTH];
};

struct labelStruct labelArray[MAXNUMLABELS];

int
main(int argc, char *argv[])
{
    	char *inFileString, *outFileString;
    	FILE *inFilePtr, *outFilePtr;
    	char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
            	arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];


	int address; 
	int i;	
	int instructionVal = 0;
	int lineCount = 0;
	int temp;
	int arg0Label;
	int arg1Label;
	int arg2Label;
	int j;
	int temp2;
	int temp3;
	int temp4;
	char *tempchar;
	int labelFlag = 0;
	int duplicateFlag = 0;
	int tempInt = 0;
	

	    
    	/*check command line arguments */
    	if (argc != 3)
    	{
        	printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
            	argv[0]);
        	exit(1);
    	}

    	inFileString = argv[1];
   
    	outFileString = argv[2];

    	/*open infile to read stream */
    	inFilePtr = fopen(inFileString, "r");
   
    	if (inFilePtr == NULL)
    	{
        	printf("error in opening %s\n", inFileString);
        	exit(1);
    	}
   

    	/*open outfile to write to */
    	outFilePtr = fopen(outFileString, "w");
   
    	if (outFilePtr == NULL)
    	{
       	 	printf("error in opening %s\n", outFileString);
        	exit(1);
    	}

   




    	/*first pass to gather label names and store them in labelArray of structs*/
   
   	for(address = 0; readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2); address++)
 	{
    		
		sscanf(label, "%[^\t\n]", labelArray[address].labelName);
		sscanf(opcode, "%[^\t\n]", labelArray[address].labelOpcode);
		sscanf(arg0, "%[^\t\n]", labelArray[address].labelArg0);
		labelArray[address].labelAddress = address;		
			
		lineCount++;

			
    	}

   	/* check for duplicate labels */
	for(i = 0; i < lineCount; i++)
	{
		for(j = 0; j < lineCount; j++)
		{
			if(!strcmp(labelArray[i].labelName, labelArray[j].labelName) &&
					!strcmp(labelArray[j].labelName, "       "))
			{
				printf("Error: duplicate labels\n");
				exit(1);
			}
		}
	}
			
	
    	/* this is how to rewind the file ptr so that you start reading from the beginning of the file */
    	rewind(inFilePtr);


	for(i = 0; i < lineCount; i++)
	{
		printf("labelArray[%d].labelName = %s\n", i, labelArray[i].labelName);
	}
		

   

	/*second pass to generate machine code from assembly instructions*/
	for(address = 0; readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2); address++)
	{
		

		temp = 0;
		temp2 = 0;
		temp3 = 0;
		temp4 = 0;
		instructionVal = 0;			
		j = 0;
		
		/* check undefined opcode */
		if(strcmp(opcode, "add") && strcmp(opcode, "nand") && strcmp(opcode, "lw")
				&& strcmp(opcode, "sw") && strcmp(opcode, "beq") && strcmp(opcode, "jalr")
				&& strcmp(opcode, "halt") && strcmp(opcode, "noop") && strcmp(opcode, ".fill"))
		{
			printf("Error: unrecognized opcode\n");
			exit(1);
		}


			
		/* check for undefined label */
		labelFlag = 0;

		for(i = 0; i < lineCount - 1; i++)
		{
			if(strcmp(arg0, labelArray[i].labelName) || strcmp(arg0, "       "))
				labelFlag++;
			if(labelFlag >= lineCount)
			{	
				printf("Error: undefined label (arg0) at %d, label = %s\n", i, arg0);
				exit(1);
			}
		}
		
		labelFlag = 0;

		for(i = 0; i < lineCount - 1; i++)
		{
			if(strcmp(arg1, labelArray[i].labelName) || strcmp(arg1, "       "))
				labelFlag++;
			if(labelFlag >= lineCount)
			{
				printf("Error: undefined label (arg1) at %d, label = %s\n", i, arg1);
				exit(1);
			}
		}

		labelFlag = 0;

		for(i = 0; i < lineCount - 1; i++)
		{
			if(strcmp(arg2, labelArray[i].labelName) || strcmp(arg2, "       "))
				labelFlag++;
			if(labelFlag >= lineCount)
			{
				printf("Error: undefined label (arg2) at %d, label = %s\n", i, arg2);
				exit(1);
			}
		}
			 
		






		/* check for .fill directive */

		if(!strcmp(opcode, ".fill"))
		{

			if(isNumber(arg0))
				fprintf(outFilePtr, "%s\n", arg0);
			else
			{
				for(j = 0; j < lineCount; j++)
				{	
					if(!strcmp(arg0, labelArray[j].labelName))
						fprintf(outFilePtr, "%d\n", labelArray[j].labelAddress);
				}
			} 
		}
	
		if(!strcmp(opcode, "add"))
		{
			for(j = 0; j < lineCount; j++)
			{	
				if(!strcmp(arg0, labelArray[j].labelName))
					temp = labelArray[j].labelAddress;
				else
					temp = atoi(arg0) << 19;
				if(!strcmp(arg1, labelArray[j].labelName))
					temp2 = labelArray[j].labelAddress;
				else
					temp2 = atoi(arg1) << 16;
				if(!strcmp(arg2, labelArray[j].labelName))
					temp3 = labelArray[j].labelAddress;
				else	
					temp3 = atoi(arg2);
			}
			
			temp4 = ADDVAL << 22;
			
			instructionVal = temp4 | temp | temp2 | temp3;
	
		
			fprintf(outFilePtr, "%d\n", instructionVal);
		}	
						
		if(!strcmp(opcode, "nand"))
		{
			for(j = 0; j < lineCount; j++)
			{	
				if(!strcmp(arg0, labelArray[j].labelName))
					temp = labelArray[j].labelAddress;
				else
					temp = atoi(arg0) << 19;
				if(!strcmp(arg1, labelArray[j].labelName))
					temp2 = labelArray[j].labelAddress;
				else
					temp2 = atoi(arg1) << 16;
				if(!strcmp(arg2, labelArray[j].labelName))
					temp3 = labelArray[j].labelAddress;
				else	
					temp3 = atoi(arg2);
			}
			
			temp4 = NANDVAL << 22;
			
			instructionVal = temp4 | temp | temp2 | temp3;
	
		
			fprintf(outFilePtr, "%d\n", instructionVal);
		}	

		if(!strcmp(opcode, "jalr"))
		{
			for(j = 0; j < lineCount; j++)
			{	
				if(!strcmp(arg0, labelArray[j].labelName))
					temp = labelArray[j].labelAddress;
				else
					temp = atoi(arg0) << 19;
				if(!strcmp(arg1, labelArray[j].labelName))
					temp2 = labelArray[j].labelAddress;
				else
					temp2 = atoi(arg1) << 16;
				if(!strcmp(arg2, labelArray[j].labelName))
					temp3 = labelArray[j].labelAddress;
				else	
					temp3 = atoi(arg2);
			}
			
			temp4 = JALRVAL << 22;
			instructionVal = temp4 | temp | temp2 | temp3;
	
			fprintf(outFilePtr, "%d\n", instructionVal);
		}

		if(!strcmp(opcode, "lw"))
		{
			for(j = 0; j < lineCount; j++)
			{
				if(!strcmp(arg0, labelArray[j].labelName))
					temp = labelArray[j].labelAddress;
				else
					temp = atoi(arg0) << 19;
				if(!strcmp(arg1, labelArray[j].labelName))
					temp2 = labelArray[j].labelAddress;
				else
					temp2 = atoi(arg1) << 16;
				if(!strcmp(arg2, labelArray[j].labelName))
				{
					temp3 = labelArray[j].labelAddress;
					printf("temp3 for lw = %d\n", temp3);
					goto breaklw;
				}
				else	
					temp3 = atoi(arg2);
			}
			
breaklw:		
			temp3 = (temp3 & 65535); 
			temp4 = LWVAL << 22;
			instructionVal = temp4 | temp | temp2 | temp3;
			fprintf(outFilePtr, "%d\n", instructionVal);
		}
		
		
		if(!strcmp(opcode, "sw"))
		{
			for(j = 0; j < lineCount; j++)
			{
				if(!strcmp(arg0, labelArray[j].labelName))
					temp = labelArray[j].labelAddress;
				else
					temp = atoi(arg0) << 19;
				if(!strcmp(arg1, labelArray[j].labelName))
					temp2 = labelArray[j].labelAddress;
				else
					temp2 = atoi(arg1) << 16;
				if(!strcmp(arg2, labelArray[j].labelName))
				{
					temp3 = labelArray[j].labelAddress;
					goto breaksw;
				}	
				else	
					temp3 = atoi(arg2);
			}

breaksw:			
			temp3 = temp3 & 65535;
			temp4 = SWVAL << 22;
			instructionVal = temp4 | temp | temp2 | temp3;
			fprintf(outFilePtr, "%d\n", instructionVal);
		}
		
		
		if(!strcmp(opcode, "beq"))
		{
			for(j = 0; j < lineCount; j++)
			{
				if(!strcmp(arg0, labelArray[j].labelName))
					temp = labelArray[j].labelAddress;
				else
					temp = atoi(arg0) << 19;
				if(!strcmp(arg1, labelArray[j].labelName))
					temp2 = labelArray[j].labelAddress;
				else
					temp2 = atoi(arg1) << 16;
				if(!strcmp(arg2, labelArray[j].labelName))
				{
					temp3 = labelArray[j].labelAddress;
					if(temp3 < address)
						temp3 = temp3 - address - 1;
					goto breakbeq;					
				}
				else	
					temp3 = atoi(arg2);
			}
breakbeq:
			
										
			if(temp3 > 32767 || temp3 < -32768)
				temp3 = convertNum(temp3);
			temp3 = (temp3 & 65535);
			printf("temp 3 for beq = %d\n",temp3); 
			
			temp4 = BEQVAL << 22;
			instructionVal = temp4 | temp | temp2 | temp3;
			fprintf(outFilePtr, "%d\n", instructionVal);
		}

		if(!strcmp(opcode, "halt"))
		{
			instructionVal = HALTVAL << 22;
			fprintf(outFilePtr, "%d\n", instructionVal);
		}

		if(!strcmp(opcode, "noop"))
		{
			instructionVal = NOOPVAL << 22;
			fprintf(outFilePtr, "%d\n", instructionVal);
		}
	}    	

    return(0);
}

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */




int
readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
    char *arg1, char *arg2)
{
    char line[MAXLINELENGTH];
    char *ptr = line;

    /* delete prior values */
    label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

    /* read the line from the assembly-language file */
    if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
	/* reached end of file */
        return(0);
    }

    /* check for line too long (by looking for a \n) */
    if (strchr(line, '\n') == NULL) {
        /* line too long */
	printf("error: line too long\n");
	exit(1);
    }

    /* is there a label? */
    ptr = line;
    if (sscanf(ptr, "%[^\t\n ]", label)) {
	/* successfully read label; advance pointer over the label */
        ptr += strlen(label);
    }

    /*
     * Parse the rest of the line.  Would be nice to have real regular
     * expressions, but scanf will suffice.
     */
    sscanf(ptr, "%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]",
        opcode, arg0, arg1, arg2);
    return(1);
}






int
isNumber(char *string)
{
    /* return 1 if string is a number */
    int i;
    return( (sscanf(string, "%d", &i)) == 1);
}

int
convertNum(int num)
{
	if(num & (1<<15))
		num -= (1<<16);
	return num;
}
