(*Stanlin Joseph smjoseph11@gmail.com*)
let _ = print "how many assignments are there?" in 
let assignmentNo = readInt in
fun createList with len = 
if len = 0 then 0 else
0@(createList(len-1))
in 
let lenList = (createList assignmentNo - 1)
in
let gradeStr = "what are the respective grades for each assignment?"
in
let _ = print gradeStr
in 
fun repeat with weight = 
if (weight >= 0) & (weight <= 100) then weight else
let _ = print "input out of range" in 
let x = readInt in
(repeat x) in 
fun weights with list = 
let f = readInt in
if isNil #list then !list+(repeat f) else
let hd = !list in
let tl = #list in
let weight = (repeat f) in
(hd+weight)@(weights tl)
in
let weightsList = (weights lenList)
in
fun addListValues with list = 
if isNil #list then !list else
let hd = !list in
let tl = #list in
hd + (addListValues tl)
in
let totalWeight = (addListValues weightsList) in
if totalWeight <> 100 then print "Weights do not add up to 100" else
fun gradeEntries with list =
let f = readInt in
if isNil #list then !list*(repeat f) else
let hd = !list in
let tl = #list in 
let grade = (repeat f) in
(hd) * grade + (gradeEntries tl) in
fun addToList with list, elem =
let li = list in 
if isNil list then elem else
let hd = !list in
let tl = #list in
if #hd < #elem then (elem@li) else
(hd@(addToList tl elem))
in
let grlist = ("test"@1) in
fun studentAverages with list =  
let _ = print "Enter student's name: " in 
let name = readString in
let _ = print "Enter student's grades: " in
let ga = ((gradeEntries(list))/100) in
let _ = print "Would you like to enter another student's grades?"
in
let strRead = readString in 
if (strRead = "y") | (strRead = "Y") then name@ga@(studentAverages weightsList)
else
name@ga in
let prList = (studentAverages weightsList) in
fun printl with list = 
if isNil #list then print !list else
let hd = !list in
let tl = #list in 
let _ = print hd in
(lambda x. x 3)
