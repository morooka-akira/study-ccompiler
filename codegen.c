#include "9cc.h"

void gen(Node *node) {
    if (node->kind == ND_NUM) {
        printf("    push %d\n", node->val);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch (node->kind) {
        case ND_ADD:
            printf("    add rax, rdi\n");
            break;
        case ND_SUB:
            printf("    sub rax, rdi\n");
            break;
        case ND_MUL:
            printf("    imul rax, rdi\n");
            break;
        case ND_DIV:
            //  RAXに入っている64ビットの値を128ビットに拡張してRDXとRAXセットするための命令
            printf("    cqo\n");
            // 暗黙のうちにRDXとRAXをとって、それを合わせたものを128ビット整数とみなして、引数のレジストアの64ビットの値で
            // 割り、商をRAXに、あまりをRDXにセットする
            printf("    idiv rdi\n");
            break;
        case ND_EQ:
            // raxとrdiを比較
            printf("    cmp rax, rdi\n");
            // alにフラグレジスタの値を格納
            printf("    sete al\n");
            // raxにフラグをコピー&余剰ビットを0クリア
            printf("    movzb rax, al\n");
            break;
         case ND_NE:
            printf("    cmp rax, rdi\n");
            printf("    setne al\n");
            printf("    movzb rax, al\n");
            break;
         case ND_LT:
            printf("    cmp rax, rdi\n");
            printf("    setl al\n");
            printf("    movzb rax, al\n");
            break;
         case ND_LE:
            printf("    cmp rax, rdi\n");
            printf("    setle al\n");
            printf("    movzb rax, al\n");
            break;
    }

    printf("    push rax\n");
}
