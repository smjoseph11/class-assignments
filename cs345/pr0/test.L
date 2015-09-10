fun addToList with list, elem =
let li = list in 
if isNil list then elem else
let hd = !list in
let tl = #list in
if #hd < #elem then elem@li else
(hd@(addToList tl elem))
in
let l = (("stan"@5)@("stan"@4)@("stan"@2)@("stan"@1))
in
(addToList l ("stan"@3))
