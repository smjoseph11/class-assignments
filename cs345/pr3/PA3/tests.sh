#!/usr/bin/env bash
l-interpreter test.L > me.out
l-dillig test.L > dillig.out
diff -ys me.out dillig.out
