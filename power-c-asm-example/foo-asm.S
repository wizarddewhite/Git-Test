	.section	".text"
	.align 2
	.globl foo
	.section	".opd","aw"
	.align 3
foo:
	.quad	.foo
	.quad	.TOC.@tocbase,0
	.previous
.foo:
	lis	%r3,0xdead
	ori	%r3,%r3,0xbeef
	blr

