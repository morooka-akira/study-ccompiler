#include "9cc.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        error("%s %d: invalid number of argments", argv[0], argc);
        return 1;
    }
    // 入力を保持しておく
    user_input = argv[1];
    // トークナイズする
    token = tokenize();
    Node *node = expr();

    // 抽象構文木を下りながらコードを生成
    codegen(node);
    return 0;
}
