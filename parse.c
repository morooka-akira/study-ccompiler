#include "9cc.h"
char *strndup(char *p, int len) {
  char *buf = malloc(len + 1);
  strncpy(buf, p, len);
  buf[len] = '\0';
  return buf;
}

// エラー箇所の報告
void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " "); // エラー箇所までのバイト数分空白を入れる
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進めて
// 真を返す。それ以外の場合には偽を返す。
bool consume(char *op) {
    //printf("op: %s\n", op);
    //printf("kind: %d\n", token->kind);
    //printf("str: %s\n", token->str);
    //printf("len: %d\n", token->len);
    if (token->kind != TK_RESERVED ||
            strlen(op) != token->len || 
            memcmp(token->str, op, token->len))
        return false;
    token = token->next;
    return true;
}

bool consume_return() {
    if (token->kind != TK_RETURN ||
            6 != token->len || 
            memcmp(token->str, "return", token->len))
        return false;
    token = token->next;
    return true;
}
 
 // 次のトークンが期待している記号のときには、トークンを1つ読み進める。
 // それ以外の場合にはエラーを報告する
 void expect(char *op) {
    if (token->kind != TK_RESERVED || 
            strlen(op) != token->len || 
            memcmp(token->str, op, token->len))
         error_at(token->str, "expected '%c'", op);
     token = token->next;
 }

 // 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す
 // それ以外の場合にはエラーを報告する
int expect_number() {
    if (token->kind != TK_NUM)
        error_at(token->str, "expected a number");
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof() {
    return token->kind == TK_EOF;
}

Node *new_node(NodeKind kind) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    return node;
}

Node *new_binary(NodeKind kind, Node *lhs, Node *rhs) {
    Node *node = new_node(kind);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_num(int val) {
    Node *node = new_node(ND_NUM);
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

LVar *find_lvar(Token *tok) {
    for (LVar *var = locals; var; var = var->next) {
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len)) {
            return var;
        }
    }
    return NULL;
}

Node *new_lvar(Token *tok) {
    Node *node = new_node(ND_LVAR);

    LVar *lvar = find_lvar(tok);
    if (lvar) {
        node->offset = lvar->offset;
    } else {
        // トークナイズされた時点では文字列の終端\0が無いのでコピーして新しい文字列を作る
        char *str = strndup(tok->str, tok->len);
        lvar = calloc(1, sizeof(LVar));
        lvar->name = str;
        lvar->len = tok->len;
        lvar->next = locals;
        node->offset = lvar->offset;
        locals = lvar;
    }
    node->var = lvar;
    return node;
}

Token *consume_ident() {
  if (token->kind != TK_IDENT)
    return NULL;
  Token *t = token;
  token = token->next;
  return t;
}

// 文字列の比較
bool startswith(char *p, char*q) {
    return memcmp(p, q, strlen(q)) == 0;
}

// stmt*
Node* program() {
    Node head;
    head.next = NULL;
    Node *cur = &head;
    int i = 0;
    while (!at_eof()) {
        cur->next = stmt();
        cur = cur->next;
    }
    return head.next;
}

// stmt = return expr ";" | expr
Node *stmt() {
    Node *node;
    if (consume_return()) {
        node = new_node(ND_RETURN);
        node->lhs = expr();
        expect(";");
        return node;
    }
    node = expr();
    //node = new_node(ND_EXPR_STMT);
    //node->lhs = expr();
    expect(";");
    return node;
}

// expr = equality
Node *expr() {
    return assign();
}

// assign = equality ("=" assign)?
Node *assign() {
    Node *node = equality();
    if (consume("=")) {
        node = new_binary(ND_ASSIGN, node, assign());
    }
    return node;
}

// equality = relational ("==" relational | "!=" relational)*
Node *equality() {
    Node *node = relational();
    for (;;) {
        if (consume("==")) {
            node = new_binary(ND_EQ, node, relational());
        } else if (consume("!=")) {
            node = new_binary(ND_NE, node, relational());
        } else {
            return node;
        }
    }
}

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *relational() {
    Node *node = add();

    for (;;) {
        if (consume("<")) {
            node = new_binary(ND_LT, node, add());
        } else if (consume("<=")) {
            node = new_binary(ND_LE, node, add());
        } else if (consume(">")) {
            node = new_binary(ND_LT, add(), node);
        } else if (consume(">=")) {
            node = new_binary(ND_LE, add(), node);
        } else {
            return node;
        }
    }
}

// add = mul ("+" mul | "-" mul)*
Node *add() {
    Node *node = mul();

    for (;;) {
        if (consume("+")) {
            node = new_binary(ND_ADD, node, mul());
        } else if (consume("-")) {
            node = new_binary(ND_SUB, node, mul());
        } else {
            return node;
        }
    }
}

// mul = unary ("*" unary | "/" unary)*
Node *mul() {
    Node *node = unary();

    for (;;) {
        if (consume("*")) {
            node = new_binary(ND_MUL, node, unary());
        } else if (consume("/")) {
            node = new_binary(ND_DIV, node, unary());
        } else {
            return node;
        }
    }
}

// unary = ("+" | "-")? unary | primary
Node *unary() {
    if (consume("+")) {
        return unary();
    } else if (consume("-")) {
        // 0から数値を減産する
        return new_binary(ND_SUB, new_num(0), unary());
    } else {
        return primary();
    }
}

// primary = "(" expr ")" | num
Node *primary() {
    if (consume("(")) {
        Node *node = expr();
        expect(")");
        return node;
    }

    Token *tok = consume_ident();
    if (tok) {
        return new_lvar(tok);
    }
    return new_num(expect_number());
}

