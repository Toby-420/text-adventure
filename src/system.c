#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

char *readFileContents(const char *filename) {
  FILE *file = fopen(filename, "r");

  if (file == NULL) {
    perror("Error opening file");
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *file_contents = (char *)malloc(size + 1);
  if (file_contents == NULL) {
    perror("Error allocating memory");
    fclose(file);
    return NULL;
  }

  size_t read_size = fread(file_contents, 1, size, file);
  file_contents[read_size] = '\0';

  fclose(file);

  return file_contents;
}

void toLowerCase(char *str) {
  if (str == NULL) return;

  while (*str) {
    *str = tolower(*str);
    str++;
  }
}

void firstToUpper(char *str) {
  *str = toupper(*str);
}

void xorCipher(char *input, int key) {
  while (*input) {
    *input = *input ^ key;
    input++;
  }
}