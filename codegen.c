#include "9cc.h"

void gen_lval(Node *node) {
    if (node->kind != ND_LVAR) {
        error("not an lvalue");
        return;
    }
    printf("    mov rax, rbp\n");
    printf("    sub rax, %d\n", node->var->offset);
    printf("    push rax\n");
}

void load() {
    printf("    pop rax\n");
    printf("    mov rax, [rax]\n");
    printf("    push rax\n");
}

void store() {
    printf("    pop rdi\n");
    printf("    pop rax\n");
    printf("    mov [rax], rdi\n");
    printf("    push rdi\n");
}

void gen(Node *node) {
    switch (node->kind) {
        case ND_NUM:
//            printf("gen: ND_NUM\n");
            printf("    push %d\n", node->val);
            return;
        case ND_LVAR:
//            printf("gen: ND_LVAR\n");
            gen_lval(node);
            load();
            return;
        case ND_ASSIGN:
//            printf("gen: ND_ASSIGN\n");
            gen_lval(node->lhs);
            gen(node->rhs);
            store();
            return;
        case ND_EXPR_STMT:
//            printf("gen: ND_EXPR_STMT\n");
            gen(node->lhs);
            printf("    add rsp, 8\n");
            return;
        case ND_RETURN:
//            printf("gen: ND_RETURN\n");
            gen(node->lhs);
            printf("    pop rax\n");
            printf("    jmp .Lreturn\n");
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

void codegen(Node *node) {
    // アセンブリの前半部分を出力
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    // プロローグ
    printf("    push rbp\n");
    printf("    mov rbp, rsp\n");
    // 変数分の領域を確保
    int offset = 0;
    for (LVar *var = locals; var; var = var->next) {
        offset += 8;
        var->offset = offset;
    }
    printf("    sub rsp, %d\n", offset);

    for (Node *n = node; n; n = n->next) {
        gen(n);
        printf("    pop rax\n");
    }

    // プロローグ
    // 最後の式の結果がRAXに残っているのでそれが戻り値になる
    printf(".Lreturn:\n");
    printf("    mov rsp, rbp\n");
    printf("    pop rbp\n");
    // RAXにロードして関数からの戻り値とする
    printf("    ret\n");
}
