#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>

void fail(char *string, ...);

int main(int argc, char **argv)
{
	FILE *asm_file;
	char filename[256];
	char buffer[BUFSIZ];
	int line_num = 1;
	const char *delimiter = " ,\t\n\r";
	char *p;
	enum { PARSE_NEUTRAL, PARSE_ARGS } parse_state;
	enum { INST_STORE, INST_LOAD, INST_DIV, INST_MUL, INST_SET, INST_ADD, INST_EQ, INST_NEQ,
		INST_LT, INST_GT, INST_BR, INST_WR, INST_RAND, INST_LOOP, INST_END } inst_state;
	int parse_end;
	char *inst_str[] = { "store", "load", "div", "mul", "set", "add", "eq", "neq", "lt", "gt",
		"br", "wr", "rand", "loop", "end" };
	int inst_argc[] = { 1, 1, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 0, 1, 0 };
	int x;
	int arg[2];
	int args_remaining;
	enum { ARG_REG, ARG_CONST } arg_type;
	unsigned char inst_buf[256];
	unsigned char *inst_p;
	unsigned char *w_p;

	/* Initialize file */
	if (argc > 2)
		printf("Usage: zas [filename]\n");
	if (argc == 2)
	{
		strcpy(filename, argv[1]);
		asm_file = fopen(filename, "rt");
		if (asm_file == NULL)
			fail("fopen");
	}
	else
		asm_file = stdin;

	inst_p = inst_buf;
	parse_state = PARSE_NEUTRAL;
	/* Read the assembly file */
	while (fgets(buffer, BUFSIZ, asm_file) != NULL)
	{
		for (p = strtok(buffer, delimiter); p != NULL; p = strtok(NULL, delimiter))
		{
			/* Comment */
			if (p[0] == ';')
				break;
			switch (parse_state)
			{
				case PARSE_NEUTRAL:
					parse_end = 0;
					for (x = INST_STORE; x <= INST_END; x++)
					{
						if (strcmp(p, inst_str[x]) == 0)
						{
							inst_state = x;
							args_remaining = inst_argc[x];
							parse_state = PARSE_ARGS;
							break;
						}
					}
					if (x > INST_END)
						fail("Invalid instruction on line %i: %s\n", line_num, p);
					break;
				case PARSE_ARGS:
					if (args_remaining == 0)
					{
						parse_end = 1;
						break;
					}
					/* Register arguments */
					if (p[0] >= 'a' && p[0] <= 'd' && strlen(p) == 1)
					{
						arg_type = ARG_REG;
						arg[2 - args_remaining] = p[0] - 'a';
					}
					else if (args_remaining == 1 && p[0] == 'r' && strlen(p) == 1)
					{
						arg_type = ARG_REG;
						arg[2 - args_remaining] = 4;
					}
					else if (args_remaining == 1 && isdigit(p[0]) || p[0] == '-')
					{
						arg_type = ARG_CONST;
						arg[2 - args_remaining] = atoi(p);
					}
					else
						fail("Invalid argument on line %i: %s\n", line_num, p);
					args_remaining--;
					break;
			}
			if (args_remaining == 0 || parse_end)
			{
				if (arg_type == ARG_REG)
				{
					if (arg[1] == 4)
						arg[1] = arg[0];
				}
				else if (arg_type == ARG_CONST && (inst_state == INST_SET || inst_state == INST_ADD ||
					inst_state == INST_MUL || inst_state == INST_DIV))
				{
					*inst_p++ = 36;
					*inst_p++ = arg[1];
					arg[1] = arg[0];
				}
printf("%s %i %i %i\n",inst_str[inst_state],arg[0],arg[1],arg_type);
				switch (inst_state)
				{
					case INST_STORE:
						if (arg_type == ARG_REG)
						{
							*inst_p++ = 20 | arg[1]<<6;/*Write next inst.*/
							*inst_p++ = 3;/*Blank instruction*/
						}
						else
							*inst_p++ = 3 | arg[1]<<2;
						break;
					case INST_LOAD:
						*inst_p++ = 1 | arg[1]<<2;
						break;
					case INST_DIV:
						*inst_p++ = 14 | arg[0]<<4 | arg[1]<<6;
						break;
					case INST_MUL:
						*inst_p++ = 6 | arg[0]<<4 | arg[1]<<6;
						break;
					case INST_SET:
						*inst_p++ = 2 | arg[0]<<4 | arg[1]<<6;
						break;
					case INST_ADD:
						*inst_p++ = 10 | arg[0]<<4 | arg[1]<<6;
						break;
					case INST_EQ:
						*inst_p++ = 61 | arg[1]<<6;
						break;
					case INST_NEQ:
						*inst_p++ = 28 | arg[1]<<6;
						break;
					case INST_LT:
						*inst_p++ = 44 | arg[1]<<6;
						break;
					case INST_GT:
						*inst_p++ = 12 | arg[1]<<6;
						break;
					case INST_BR:
						*inst_p++ = 52 | arg[1]<<6;
						break;
					case INST_WR:
						*inst_p++ = 20 | arg[1]<<6;
						break;
					case INST_RAND:
						*inst_p++ = 4;
						break;
					case INST_LOOP:
						*inst_p++ = arg[1]<<3;
						break;
					case INST_END:
						*inst_p++ = 0;
						break;
				}
				parse_state = PARSE_NEUTRAL;
				break;
			}
		}
		line_num++;
	}
	if (args_remaining)
		fail("Not enough args\n");
	putchar('"');
	/* Print out instruction sequence */
	for (w_p = inst_buf; w_p < inst_p; w_p++)
	{
		if (*w_p > 32 && *w_p < '~' && *w_p != 34)
			putchar(*w_p);
		else
			printf("\\%03o",*w_p);
	}
	putchar('"');
	putchar('\n');

	return 0;
}

void fail(char *string, ...)
{
	va_list args;

	if (errno)
		perror(string);
	else
	{
		va_start(args, string);
		vprintf(string, args);
		va_end(args);
	}
	exit(1);
}
