(* Binary operators and if then statements *)
let _ = if((1+2+3*4)/2 = 7) then print "yeah!" else print "no" in 
let _ = if("duck"="fish") then print "yeah!" else print "no" in 
let _ = if("duck"<>"fish") then print "yeah!" else print "no" in 
let _ = if(2<4) then print "yeah!" else print "no" in 
let _ = if(2>4) then print "yeah!" else print "no" in 
let _ = if(2<=4) then print "yeah!" else print "no" in 
let _ = if(2>=4) then print "yeah!" else print "no" in 
(*List operations*)
let x = 1@2@3@4 in
let _ =  print !x in 
let _ = print #x in 
let y = 2 in
let _ =  print !y in
let _ = if(isNil #y) then print "isNil" else print "notNil" in 
print 1@2@3(#y)  
(*Lambda operations*)
let _ = print (lambda x,y,z.x+y+z 3 4 5) in 
let _ = print (lambda x,y,z.x+y+z 3 4) in 
let _ = print (lambda x,y,z,q.x+y*z/q 3 4 5 6) in
let _ = print (lambda x,y,z,q.x-y*z*q 3 4 5) in 
let _ = print (lambda x,y,z,q.x+y+z-q 3 4) in 
0
(*Function Operations*)
fun fact with n = if n=0 then 1 else n* (fact (n-1)) in (fact 6)
fun even with x = if x=0 then 1 else (odd x-1) in
fun odd with x = if x=0 then 0 else (even x-1) in 
(odd 7)
fun hi with x,y,z = x+y+z in (hi 3 4)
fun hi with x,y,z = x+y+z in (hi 3 4 5)
(*Run time errors*)
let x = 1@2@3 in print x+3
2+"duck"
"duck">="fish"
(2 3+4)
2+x
if("hello") then 0 else 1
let x = 1 in 
let y = #x in x+y
