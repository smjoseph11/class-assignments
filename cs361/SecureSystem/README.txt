UTEID: sj9557
FIRSTNAME: Stanlin
LASTNAME: Joseph
CSACCOUNT: stanlin
EMAIL: smjoseph11@utexas.edu

My program reads from a text file lines of read and write commands. 
It takes these commands line by line and parses them in the InstructionObject class.
The class then makes some checks to see if the command was READ, WRITE, or BAD and stores 
this information in a enumerator. Each line is then sent to the ReferenceMonitor which also
contains the subjects and objects lists created by the system.
The ReferenceMonitor then does a permission() check on the command and if it passes 
(determined via the SecurityLevel class), it allows the ObjectMonitor to perform write or read
and update the appropriate Subject or Object. This information is then loaded into a private variable
and the system can then update its Subjects and Objects based on the changes. 

FINISHED ALL REQUIREMENTS
I created 4 test cases. 
test.txt
test2.txt
test3.txt
test4.txt