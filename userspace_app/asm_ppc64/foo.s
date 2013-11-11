# this program just return 9
	.global  main
	.section	".opd","aw"
main:
	.quad	.L.main,.TOC.@tocbase
	.previous
	.type	main, @function
.L.main:
	li 3, 9
	blr

