# C Compiler
CC=gcc

build:
	@$(CC) murl.c -o murl

debug:
	@$(CC) murl.c -o murl -g