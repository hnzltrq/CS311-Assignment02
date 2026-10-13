#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

static void process_input(int fd, const char *filename)
{
	char valid_char[] = " -\r\t\n./,";
	char c;
	int n;
	int num = 0;
	int have_digits = 0;
	int too_large = 0;
	int start_valid = 1;

	while ((n = read(fd, &c, 1)) > 0)
	{
		char* is_valid = strchr(valid_char, c);
		if (is_valid)
		{
			if (have_digits && !too_large && (num % 5 == 0 || num % 6 == 0))
				fprintf(1, "%d\n", num);

			start_valid = 1;
			num = 0;
			have_digits = 0;
			too_large = 0;
		}
		else if (c >= '0' && c <= '9' && start_valid)
		{
			int digit = c - '0';
			have_digits = 1;
			if (!too_large)
			{
				if (num > (2147483647 - digit) / 10)
					too_large = 1;
				else
					num = num * 10 + digit;
			}
		}
		else
		{
			start_valid = 0;
			num = 0;
			have_digits = 0;
			too_large = 0;
		}
	}

	if (n < 0)
	{
		if (filename)
			fprintf(2, "cannot read file %s \n", filename);
		else
			fprintf(2, "sixfive: read error \n");
	}
	else if (have_digits && !too_large && (num % 5 == 0 || num % 6 == 0))
		fprintf(1, "%d\n", num);
}

int main(int argc, char *argv[])
{
	if (argc == 1)
	{
		process_input(0, 0);
		exit(0);
	}

	for (int i = 1; i < argc; i++)
	{
		int fd = open(argv[i], O_RDONLY);
		if (fd == -1)
		{
			fprintf(2, "cannot open file %s \n", argv[i]);
			continue;
		}

		process_input(fd, argv[i]);
		close(fd);
	}

	exit(0);
}