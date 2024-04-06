#include "lexer.h"

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

int is_valid_nth_id_char(char c) {
  if (isalnum(c) || c == '_')
    return 1;
  else
    return 0;
}

int advance_lexer(char** src) {
  if (**src != '\0') {
    (*src)++;
    return 1;
  }
  return 0;
}

Token next_token(char** src, size_t* cursor, size_t* bol, size_t* line,
                 char (*lexeme_buf)[MAX_LEXEME_LENGTH]) {
  (void)cursor;
  (void)bol;

  Token token = {.type = 0};
  int lexeme_length = 0;

  while (isspace(**src) || **src == '/') {
    while (isspace(**src)) {  // Whitespace
      if (**src == '\n') (*line)++;
      if (!advance_lexer(src)) return (Token){.type = TOK_EOF};
    }

    if (**src == '/') {  // Comments
      if (*(*src + 1) == '/') {
        while (**src != '\n')
          if (!advance_lexer(src)) return (Token){.type = TOK_EOF};
        if (!advance_lexer(src)) return (Token){.type = TOK_EOF};
        (*line)++;
      }
    }
  }

  if (isalpha(**src)) {  // Identifiers
    do {
      (*lexeme_buf)[lexeme_length++] = **src;
      if (!advance_lexer(src)) break;
    } while (is_valid_nth_id_char(**src));

    (*lexeme_buf)[lexeme_length] = '\0';
    token.type = TOK_ID;
    token.literal = malloc(lexeme_length + 1);
    strncpy(token.literal, *lexeme_buf, lexeme_length + 1);

  } else if (isnumber(**src)) {  // base10 number literal
    do {
      (*lexeme_buf)[lexeme_length++] = **src;
      if (!advance_lexer(src)) break;
    } while (isnumber(**src));

    if (**src == '.') {  // Float literal
      do {
        (*lexeme_buf)[lexeme_length++] = **src;
        if (!advance_lexer(src)) break;
      } while (isnumber(**src));
      token.type = TOK_FLOAT;
    } else {
      token.type = TOK_INT_DEC;
    }
    (*lexeme_buf)[lexeme_length] = '\0';
    token.literal = malloc(lexeme_length + 1);
    strncpy(token.literal, *lexeme_buf, lexeme_length + 1);

  } else if (**src == '"') {  // String literals
    if (!advance_lexer(src)) return (Token){.type = TOK_EOF};

    while (**src != '"') {
      (*lexeme_buf)[lexeme_length++] = **src;
      if (!advance_lexer(src)) break;
    }
    if (!advance_lexer(src)) return (Token){.type = TOK_EOF};

    (*lexeme_buf)[lexeme_length] = '\0';
    token.type = TOK_STRING;
    token.literal = malloc(lexeme_length + 1);
    strncpy(token.literal, *lexeme_buf, lexeme_length + 1);

  } else if (**src == '\'') {
    // UTF-8
    // 1 byte: 0xxxxxxx (ASCII characters)
    // 2 bytes: 110xxxxx 10xxxxxx
    // 3 bytes: 1110xxxx 10xxxxxx 10xxxxxx
    // 4 bytes: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx

    if (!advance_lexer(src)) return (Token){.type = TOK_EOF};
    (*lexeme_buf)[0] = **src;
    if (!advance_lexer(src)) return (Token){.type = TOK_EOF};
    (*lexeme_buf)[1] = '\0';

    token.type = TOK_CHAR;
    token.literal = malloc(2);
    strncpy(token.literal, *lexeme_buf, 2);

  } else if (**src == '\0')
    token.type = TOK_EOF;
  else {
    if (**src == ':') {
      advance_lexer(src);
      if (**src == '=') {
        token.type = TOK_ASSIGN;
      } else if (**src == ':') {
        token.type = TOK_ASSIGN_CONST;
      } else {
        token.type = TOK_COLON;
        (*src)--;
      }
    } else if (**src == '=')
      token.type = TOK_EQUALS;
    else if (**src == ';')
      token.type = TOK_SEMICOLON;
    else if (**src == ',')
      token.type = TOK_COMMA;
    else if (**src == '.')
      token.type = TOK_DOT;
    else if (**src == '(')
      token.type = TOK_L_PAREN;
    else if (**src == ')')
      token.type = TOK_R_PAREN;
    else if (**src == '[')
      token.type = TOK_L_BRACKET;
    else if (**src == ']')
      token.type = TOK_R_BRACKET;
    else if (**src == '{')
      token.type = TOK_L_BRACE;
    else if (**src == '}')
      token.type = TOK_R_BRACE;
    else if (**src == '|') {
      advance_lexer(src);
      if (**src == '>') {
        token.type = TOK_R_PIPE;
      } else {
        token.type = TOK_BAR;
        (*src)--;
      }
    } else if (**src == '+')
      token.type = TOK_PLUS;
    else if (**src == '-') {
      advance_lexer(src);
      if (**src == '>') {
        token.type = TOK_ARROW;
      } else {
        token.type = TOK_MINUS;
        (*src)--;
      }
    } else if (**src == '*')
      token.type = TOK_STAR;
    else if (**src == '%')
      token.type = TOK_PERCENT;
    else if (**src == '@')
      token.type = TOK_AT;
    else if (**src == '&')
      token.type = TOK_AMPERSAND;
    else if (**src == '<') {
      advance_lexer(src);
      if (**src == '|')
        token.type = TOK_L_PIPE;
      else if (**src == '=') {
        token.type = TOK_LTE;
      } else {
        token.type = TOK_LT;
        (*src)--;
      }
    } else if (**src == '>') {
      advance_lexer(src);
      if (**src == '=') {
        token.type = TOK_GTE;
      } else {
        token.type = TOK_GT;
        (*src)--;
      }
    }
    advance_lexer(src);
  }

  return token;
}
