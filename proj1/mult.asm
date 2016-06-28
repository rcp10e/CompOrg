	lw	0	2	mcand	mcand into reg2
	lw	0	3	mplier	mplier into reg3
	lw	0	4	one	load 1 into reg4 to create mask with
	lw	0	5	fift	load 15 into reg5
loop	nand	2	4	6	nand mcand with 1 into reg6
	nand	6	6	6	if 1 add, if 0 add 6 to 6
	add	4	4	4	shift left logical on 1
	beq	6	0	doub		
	beq	0	0	loop	reloop
doub	add	6	6	6	shift left logical
	add	4	4	4	shift left logical on 1
	nand	5	4	7			 		
done	halt				
mcand	.fill	32766		 
mplier	.fill	10383		 
one	.fill	1		 	
fift	.fill	15				
