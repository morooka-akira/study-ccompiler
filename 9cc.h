#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    ND_ADD,     // +
    ND_SUB,     // -
    ND_MUL,     // *
    ND_DIV,     // /
    ND_EQ,      // ==
    ND_NE,      // !=
    ND_LT,      // <
    ND_LE,      // <=
    ND_NUM,     // 整数
    ND_ASSIGN,  // =
    ND_LVAR,    // ローカル変数
} NodeKind;

typedef struct Node Node;

struct Node {
    NodeKind kind;   // ノードの型
    Node *next;
    Node *lhs;       // 左辺(left hand side)
    Node *rhs;       // 右辺(right hand side)
    char name;       // kindがND_LVARのときのみ(変数名を指す)
    int val;         // kindがND_NUMの場合のみ使用
    int offset;      // ベースポインタからのオフセットを表す。kindがND_LVARの場合のみ使用。
};

 // トークンの種類
 typedef enum {
     TK_RESERVED,   // 記号
     TK_IDENT,      // 識別子
     TK_NUM,        // 整数トークン
     TK_EOF,        // 入力の終わりを表すトークン
 } TokenKind;

typedef struct Token Token;

struct Token {
    TokenKind kind; // トークンの型
    Token *next;    // 次の入力トークン
    int val;        // kindがTK_NUMの場合、その数値
    char *str;      // トークン文字列
    int len;        // トークンの長さ
};

void error_at(char *loc, char *fmt, ...);
void error(char *fmt, ...);
bool consume(char *op);
void expect(char *op);
int expect_number();
bool at_eof();
Node *new_node(NodeKind kind);
Node *new_binary(NodeKind kind, Node *lhs, Node *rhs);
Node *new_num(int val);
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
bool startswith(char *p, char*q);
Token *tokenize();

Node *program();
// stmt = expr ";"
Node *stmt();
// assign = equality ("=" assign)?
Node *assign();
Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *primary();
Node *unary();

/* codegen.c */
void codegen(Node *node);

/* グローバル変数 */
char *user_input;
Token *token;
