	lw	0	1	100	reg1 = 1
	lw	0	2	102	reg2 = 2
	lw	0	3	101	reg3 = 5
	add	1	2	4	reg4 = 3
	add	4	2	5	reg5 = 5
	nand	2	1	6	reg6 = -1
	add	1	6	7	reg7 = 0
	sw	0	5	105	dataMem[105] = reg5
	lw	0	7	105	reg7 = dataMem[105]
	beq	0	7	-8	if reg7 == 0, goto add 4 2 5
	halt
