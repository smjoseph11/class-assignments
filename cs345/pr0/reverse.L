fun cat with l1, l2 =
if isNil l1 then l2 else
!l1 @ (cat #l1 l2)
in
fun reverse with l = 
if isNil #l then l else
let hd = !l in
let t1 = #l in
(cat (reverse t1) hd) in
(reverse 1 @ 2 @ 3)
