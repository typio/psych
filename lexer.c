#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_OK 1
#define FILE_DOESNT_EXIST -1
#define FILE_TOO_LARGE -2
#define FILE_READ_ERROR -3

#define MAX_LEXEME_LENGTH 64

// clang-format off
typedef enum {
  TOK_IDENTIFIER = 1, 
  TOK_INT_DEC, TOK_INT_OCT, TOK_INT_HEX, TOK_INT_BIN, TOK_FLOAT, TOK_STRING, TOK_CHAR,
  TOK_ASSIGN, TOK_ASSIGN_CONST, TOK_ARROW,
  TOK_COLON, TOK_SEMICOLON, TOK_COMMA,
  TOK_L_PAREN, TOK_R_PAREN, TOK_L_BRACE, TOK_R_BRACE, TOK_L_BRACKET, TOK_R_BRACKET, TOK_DOUBLE_QUOTE, TOK_SINGLE_QUOTE,
  TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH, TOK_PERCENT, TOK_AT,
  TOK_TYPE,
  TOK_EOF
} TokenType ;
// clang-format on

typedef struct {
  size_t hash;
  TokenType type;
  char* literal;
} Token;

char* read_file(const char* file_name, size_t* err, size_t* file_size) {
  char* buffer;
  size_t file_length;
  FILE* f = fopen(file_name, "rb");
  size_t read_length;

  if (f) {
    fseek(f, 0, SEEK_END);
    file_length = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (file_length > 1000000) {
      *err = FILE_TOO_LARGE;

      return NULL;
    }

    buffer = (char*)malloc(file_length + 1);

    if (file_length) {
      read_length = fread(buffer, 1, file_length, f);

      if (file_length != read_length) {
        free(buffer);
        *err = FILE_READ_ERROR;

        return NULL;
      }
    }

    fclose(f);

    *err = FILE_OK;
    buffer[file_length] = '\0';
    *file_size = file_length;
  } else {
    *err = FILE_DOESNT_EXIST;

    return NULL;
  }

  return buffer;
}

Token next_token(char** src, size_t* cursor, size_t* bol, size_t* line,
                 char (*lexeme_buf)[MAX_LEXEME_LENGTH]) {
  Token token = {.type = 0};

  while (isspace(**src)) {
    if (**src == '\n') (*line)++;
    (*src)++;
  }

  if (isalpha(**src)) {
    int lexeme_length = 0;

    do {
      (*lexeme_buf)[lexeme_length++] = **src;
    } while (isalnum(*(*src)++));

    (*lexeme_buf)[lexeme_length] = '\0';
    lexeme_length++;

    token.type = TOK_IDENTIFIER;
    token.literal = malloc(lexeme_length);
    strncpy(token.literal, *lexeme_buf, lexeme_length);
  } else if (isnumber(**src)) {
    int lexeme_length = 0;

    do {
      (*lexeme_buf)[lexeme_length++] = **src;
    } while (isnumber(*(*src)++));

    (*lexeme_buf)[lexeme_length] = '\0';
    lexeme_length++;

    token.type = TOK_INT_DEC;
    token.literal = malloc(lexeme_length);
    strncpy(token.literal, *lexeme_buf, lexeme_length);
  } else if (**src == '"') {
    (*src)++;
    int lexeme_length = 0;

    while (**src != '"' && **src != '\0') {
      (*lexeme_buf)[lexeme_length++] = **src;
      (*src)++;
    }

    if (**src == '"') {
      (*src)++;
    }

    (*lexeme_buf)[lexeme_length] = '\0';
    lexeme_length++;

    token.type = TOK_STRING;
    token.literal = malloc(lexeme_length);
    strncpy(token.literal, *lexeme_buf, lexeme_length);
  } else if (**src == '\'') {
  } else if (**src == ':') {
    (*src)++;
    if (**src == '=') {
      token.type = TOK_ASSIGN;
    } else if (**src == ':') {
      token.type = TOK_ASSIGN_CONST;
    } else {
      token.type = TOK_COLON;
      (*src)--;
    }
    (*src)++;
  } else if (**src == ';') {
    token.type = TOK_SEMICOLON;
    (*src)++;
  } else if (**src == '\0') {
    token.type = TOK_EOF;
  }

  return token;
}

int main(int argc, char** argv) {
  char lexeme_buf[MAX_LEXEME_LENGTH];

  char* src;
  size_t line = 0;
  size_t cursor = 0;
  size_t bol = 0;

  if (argc <= 1) {
    printf("No argument provided, please provide a filename");
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
    printf("Too many arguments provided, please only provide a filename");
    return -1;
  }

  while (1) {
    Token t = next_token(&src, &line, &cursor, &bol, &lexeme_buf);
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
  }

  free(src);

  return 0;
}
