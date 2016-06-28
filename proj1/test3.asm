	lw	0	0	undef	load word from undefined label
start	lw	1	1	3	load word from address 3
	beq	3	3	done	branch to done
done	halt
