#int bar(int a)
#{
#	return a+3;
#}
#
#int main()
#{
#	return bar(1);
#}
	.global bar
	.section        ".opd","aw"
bar:
	.quad	.L.bar,.TOC.@tocbase
	.previous
	.type	bar, @function
.L.bar:
	# int bar(int a)
	# save r31 on stack
	std 31,-8(1)       # *(r1-8) = r31;
	# increase stack and save stack pointer
	stdu 1,-64(1)      # *(r1-64) = r1; r1 = r1-64;
	mr 31,1            # r31 = r1;
	mr 9,3             # r9 = r3;
	# a + 3
	addi 9,9,3         # r9 = r9 + 3;
	extsw 9,9
	mr 3,9             # r3 = r9;
	# restore r1, the stack
	addi 1,31,64       # r1 = r31 +  64;
	# restore r31
	ld 31,-8(1)        # r31 = *(r1-8);
	blr
	
	.global  main
	.section	".opd","aw"
main:
	.quad	.L.main,.TOC.@tocbase
	.previous
	.type	main, @function
.L.main:
	# save return address to stack
	mflr 0             # r0 = LR;
	std 0,16(1)        # *(r1+16) = r0;
	# save r31, since we will use it
	std 31,-8(1)       # *(r1-8) = r31;
	# increase stack and save origin stack pointer
	stdu 1,-128(1)     # *(r1-128) = r1; r1 = r1-128;
	# r3 point to the stack
	mr 31,1            # r31 = r1;
	# prepare parameters for bar()
	li 3,1             # r3 = 1;
	bl bar             # bar(r3);
	mr 9,3             # r9 = r3;
	mr 3,9             # r3 = r9;
	# restore the stack before function call
	addi 1,31,128      # r1 = r31 + 128;
	# restore the return address to LR
	ld 0,16(1)         # r0 = *(r1+16);
	mtlr 0             # LR = r0;
	# restore r31
	ld 31,-8(1)        # r31 = *(r1-8);
	blr
