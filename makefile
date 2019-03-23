all:
	@gcc *.c -DDEBUG=\"off\" -o ubershell
debug:
	@gcc *.c -DDEBUG=\"all\" -o ubershell
