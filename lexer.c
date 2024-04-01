#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEXEME_LENGTH 64

// clang-format off
typedef enum {
  TOK_IDENTIFIER = 1, 
  TOK_INT_DEC, TOK_INT_OCT, TOK_INT_HEX, TOK_INT_BIN, TOK_FLOAT, 
  TOK_ASSIGN, TOK_ASSIGN_CONST, TOK_ARROW,
  TOK_COLON, TOK_SEMICOLON, TOK_COMMA,
  TOK_L_PAREN, TOK_R_PAREN, TOK_L_BRACE, TOK_R_BRACE, TOK_L_BRACKET, TOK_R_BRACKET, TOK_DOUBLE_QUOTE, TOK_SINGLE_QUOTE,
  TOK_ADD, TOK_SUB, TOK_MUL, TOK_DIV, TOK_MOD,
  TOK_TYPE,
  TOK_EOF
} TokenType ;
// clang-format on

typedef struct {
  size_t hash;
  TokenType type;
  char* literal;
} Token;

size_t Line = 0;
char Lexeme[MAX_LEXEME_LENGTH];

Token next_token() {
  Token token = {};

  char last_char = ' ';

  while (isspace(last_char)) last_char = getchar();
  if (isalpha(last_char)) {
    int lexeme_length = 0;

    do {
      Lexeme[lexeme_length++] = last_char;
    } while (isalnum(last_char = getchar()));

    Lexeme[lexeme_length] = '\0';
    lexeme_length++;

    token.type = TOK_IDENTIFIER;
    token.literal = malloc(lexeme_length);
    strncpy(token.literal, Lexeme, lexeme_length);
  } else if (isnumber(last_char)) {
    int lexeme_length = 0;

    do {
      Lexeme[lexeme_length++] = last_char;
    } while (isnumber(last_char = getchar()));

    Lexeme[lexeme_length] = '\0';
    lexeme_length++;

    token.type = TOK_INT_DEC;
    token.literal = malloc(lexeme_length);
    strncpy(token.literal, Lexeme, lexeme_length);
  } else if (last_char == ':') {
    last_char = getchar();
    if (last_char == '=') {
      token.type = TOK_ASSIGN;
    } else if (last_char == ':') {
      token.type = TOK_ASSIGN_CONST;
    } else {
      ungetc(last_char, stdin);
      token.type = TOK_COLON;
    }
  } else if (last_char == ';') {
    token.type = TOK_SEMICOLON;
  } else if (last_char == '\n') {
    Line++;
  } else if (last_char == -1) {
    token.type = TOK_EOF;
  }

  return token;
}

int main(int argc, char** argv) {
  printf("\n");
  Token t;
  while (1) {
    t = next_token();
    if (t.type == TOK_EOF) {
      printf("EOF\n");
      return 0;
    } else if (t.type < 0) {
      printf("Error parsing.\n");
      return -1;
    } else if (t.type == 0) {
      continue;
    } else {
      printf("(%d %s)", t.type, t.literal);
      free(t.literal);
    }
  }

  return 0;
}
