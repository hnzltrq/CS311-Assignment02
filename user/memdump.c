#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void memdump(char *fmt, char *data, int len);

int
main(int argc, char *argv[])
{
  if (argc == 1) {
    printf("Example 1:\n");
    int a[2] = {61810, 2026};
    memdump("ii", (char *)a, sizeof(a));

    printf("Example 2:\n");
    memdump("S", "a string", sizeof("a string"));

    printf("Example 3:\n");
    char *s = "another";
    memdump("s", (char *)&s, sizeof(s));

    struct sss {
      char *ptr;
      int num1;
      short num2;
      char byte;
      char bytes[8];
    } example;

    example.ptr = "hello";
    example.num1 = 1819438967;
    example.num2 = 100;
    example.byte = 'z';
    strcpy(example.bytes, "xyzzy");

    printf("Example 4:\n");
    memdump("pihcS", (char *)&example, sizeof(example));

    printf("Example 5:\n");
    memdump("sccccc", (char *)&example, sizeof(example));
  } else if (argc == 2) {
    // format in argv[1], up to 512 bytes of data from standard input.
    char data[512];
    int n = 0;
    memset(data, '\0', sizeof(data));
    while (n < sizeof(data)) {
      int nn = read(0, data + n, sizeof(data) - n);
      if (nn <= 0)
        break;
      n += nn;
    }
    memdump(argv[1], data, n);
  } else {
    printf("Usage: memdump [format]\n");
    exit(1);
  }
  exit(0);
}

void
memdump(char *fmt, char *data, int len)
{
  int offset = 0;

  for (; *fmt; fmt++) {
    switch (*fmt) {
    case 'c':
      if (len - offset < (int)sizeof(char))
        goto not_enough_data;
      printf("%c\n", data[offset]);
      offset += sizeof(char);
      break;
    case 'h': {
      ushort value;
      if (len - offset < (int)sizeof(value))
        goto not_enough_data;
      memmove(&value, data + offset, sizeof(value));
      printf("%d\n", (int)value);
      offset += sizeof(value);
      break;
    }
    case 'i': {
      int value;
      if (len - offset < (int)sizeof(value))
        goto not_enough_data;
      memmove(&value, data + offset, sizeof(value));
      printf("%d\n", value);
      offset += sizeof(value);
      break;
    }
    case 'p': {
      uint64 value;
      if (len - offset < (int)sizeof(value))
        goto not_enough_data;
      memmove(&value, data + offset, sizeof(value));
      printf("%lx\n", value);
      offset += sizeof(value);
      break;
    }
    case 'S': {
      int end = offset;
      while (end < len && data[end] != '\0')
        end++;
      for (; offset < end; offset++)
        printf("%c", data[offset]);
      if (offset < len)
        offset++;
      printf("\n");
      break;
    }
    case 's': {
      uint64 value;
      if (len - offset < (int)sizeof(value))
        goto not_enough_data;
      memmove(&value, data + offset, sizeof(value));
      if (value == 0)
        printf("(null)\n");
      else
        printf("%s\n", (char *)value);
      offset += sizeof(value);
      break;
    }
    default:
      printf("\n");
      break;
    }
  }
  return;

not_enough_data:
  printf("memdump: not enough data\n");
}