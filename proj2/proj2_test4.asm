	lw	0	1	100	load 1 into reg1
	lw	0	2	100	load 1 into reg2
	add	1	2	3	reg3 = 2
	add	2	3	4	reg4 = 3
	nand	1	0	5	reg5 = -1
	add	5	1	6	reg6 = 0
	beq	0	6	-5	
	halt		
