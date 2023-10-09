# C Compiler
CC=gcc

build:
	@echo "building..."
	$(CC) murl.c -o murl
	@echo "done!"

debug:
	@$(CC) murl.c -o murl -g