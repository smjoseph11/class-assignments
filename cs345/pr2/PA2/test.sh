#!/usr/bin/env bash
parser test.L > parser.out
dparser test.L > dparser.out
diff -ys parser.out dparser.out
