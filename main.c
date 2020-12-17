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

    // アセンブリの前半部分を出力
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    // 抽象構文木を下りながらコードを生成
    gen(node);
   
    // スタックトップに式全体の値が残っているはずなので
    // それをRAXにロードして関数からの戻り値とする
    printf("    pop rax\n");
    printf("    ret\n");
    return 0;
}
