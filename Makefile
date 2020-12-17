# CFLAGS: makeの組み込みルールにより認識されている変数
# コンパイラにわたすコマンドラインオプションを格納する
# -std: C言語のバージョン
# -g: デバッグ情報の出力
# -static: スタティックリンクする
CFLAGS=-std=c11 -g -static

# wildcardはmakeが提供している関数
# .cファイルをすべて展開する
SRCS=$(wildcard *.c)
# SRCSで展開されたファイル名(.c)をすべて(.o)に置き換える
OBJS=$(SRCS:.c=.o)

9cc: $(OBJS)
		$(CC) -o 9cc $(OBJS) $(LDFLAGS)

# すべての.oファイルが9cc.hに依存していることを表す
$(OBJS): 9cc.h

test: 9cc
	./test.sh

clean:
	rm -f 9cc *.o *~ tmp*

# .PHONY: ダミーのターゲットを表すための特別な名前
.PHONY: test clean
