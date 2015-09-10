fun length with list =
if isNil list then 0 else (length #list) + 1
in
(length 1 @ 2)
