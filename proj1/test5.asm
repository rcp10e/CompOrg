	lw	0	1	one	 load reg1 with 1
	lw	0	4	four	 load reg4 with 4
	add	1	1	2	 add reg1 and reg1 into reg2
	add	2	2	3	 add reg2 and reg2 into reg3
	beq	3	4	done	 if reg3 = reg4 exit
	jalr	0	5			
done	halt								 
one	.fill	1			 	
four	.fill	4				
