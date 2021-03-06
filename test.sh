#!/bin/bash
assert() {
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    cc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 0 "0;"
assert 42 "42;"
assert 21 "5+20-4;"
# スペース
assert 41 " 12 + 34 - 5;"
# 四則演算
assert 47 "5+6*7;"
assert 15 "5*(9-6);"
assert 4 "(3+5)/2;"
# 単項演算子
assert 8 "-2+10;"
assert 10 "- -10;"
assert 10 "- - +10;"
# 比較演算子
assert 0 "0==1;"
assert 1 "30==30;"
assert 1 "0!=1;"
assert 0 "23 != 23;"

assert 1 '0<1;'
assert 0 '1<1;'
assert 0 '2<1;'
assert 1 '0<=1;'
assert 1 '1<=1;'
assert 0 '2<=1;'

assert 1 '1>0;'
assert 0 '1>1;'
assert 0 '1>2;'
assert 1 '1>=0;'
assert 1 '1>=1;'
assert 0 '1>=2;'

# 変数
assert 42 '42;'
assert 3 'a=3;'
assert 12 'a=b=12;'
assert 123 'foo = 123;'

# リターン
assert 5 'return 5;'
assert 3 'a=3; return a;'
assert 8 'foo123=3; bar=5; return foo123+bar;'

echo OK
