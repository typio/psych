#include "lexer.h"

int main(int argc, char** argv) {
  char lexeme_buf[MAX_LEXEME_LENGTH];

  char* src;
  size_t line = 0;
  size_t cursor = 0;
  size_t bol = 0;

  if (argc <= 1) {
    printf("No argument provided, please provide a filename\n");
    return -1;
  } else if (argc == 2) {
    size_t read_file_err;
    size_t file_size;
    src = read_file(argv[1], &read_file_err, &file_size);
    if (read_file_err != 1) {
      printf("Error reading file");
      return -1;
    }
  } else {
    printf("Too many arguments provided, please only provide a filename\n");
    return -1;
  }

  Token t;
  do {
    t = next_token(&src, &line, &cursor, &bol, &lexeme_buf);
    if (t.type == TOK_EOF) {
      printf("EOF\n");
      return 0;
    } else if (t.type <= 0) {
      printf("Error parsing.\n");
      return -1;
    } else {
      if (t.literal) {
        printf("(%d %s)", t.type, t.literal);
      } else {
        printf("(%d)", t.type);
      }
      free(t.literal);
    }
  } while (t.type > 0);

  free(src);

  return 0;
}
