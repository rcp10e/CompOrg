	add	1	2	3	; reg3 = reg1 + reg2
	nand	4	5	6	; reg6 = reg4 ~& reg5
	lw	2	4	20	; reg4 = Mem[reg2 + 20]
	add	2	5	5	; reg5 = reg2 + reg5
	sw	3	7	10	; Mem[reg3 + 10] = reg7
	halt
