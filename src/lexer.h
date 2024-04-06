#ifndef LEXER_H
#define LEXER_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unicode/ustring.h>

#define FILE_OK 1
#define FILE_DOESNT_EXIST -1
#define FILE_TOO_LARGE -2
#define FILE_READ_ERROR -3

#define MAX_LEXEME_LENGTH 64

// clang-format off
typedef enum {
  TOK_ID = 1, 
  TOK_INT_DEC, TOK_INT_OCT, TOK_INT_HEX, TOK_INT_BIN, TOK_FLOAT, TOK_STRING, TOK_CHAR,
  TOK_ASSIGN, TOK_ASSIGN_CONST, TOK_ARROW,
  TOK_COLON, TOK_EQUALS, TOK_SEMICOLON, TOK_COMMA, TOK_DOT,
  TOK_L_PAREN, TOK_R_PAREN, TOK_L_BRACE, TOK_R_BRACE, TOK_L_BRACKET, TOK_R_BRACKET, TOK_DOUBLE_QUOTE, TOK_SINGLE_QUOTE,
  TOK_L_PIPE, TOK_R_PIPE, TOK_RANGE_INCL, TOK_RANGE_EXCL,
  TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH, TOK_PERCENT, TOK_AT, TOK_AMPERSAND, TOK_BAR,
  TOK_LT, TOK_LTE, TOK_GT, TOK_GTE, TOK_COMPARE,
  TOK_TYPE,
  TOK_EOF
} TokenType ;
// clang-format on

typedef struct {
  size_t hash;
  TokenType type;
  char* literal;
} Token;

char* read_file(const char* file_name, size_t* err, size_t* file_size);
int is_valid_nth_id_char(char c);
int advance_lexer(char** src);
Token next_token(char** src, size_t* cursor, size_t* bol, size_t* line,
                 char (*lexeme_buf)[MAX_LEXEME_LENGTH]);

#endif
