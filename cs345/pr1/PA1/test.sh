#!/usr/bin/env bash
lexer test.L > lexer.out
lexref test.L > lexref.out
diff -s -y lexref.out lexer.out
