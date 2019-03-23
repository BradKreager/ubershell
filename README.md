# ubershell
CMPE 142 ASSIGNMENT 1

Author: Bradley Kreager
SID: 012326886

## Build Instructions
Run make within the main directory /ubershell:

> make //will compile program without debug stubs

> make debug //will compile program with all stubs enabled

## A Note on Debug Stubs
Each function contains a multitude of debugging stubs that are encapsulated within\
comments //debug begin and //debug end. Each function can have it's debugging stubs\
activated by changing the #define DEBUG argument to a string that is the function name.\

> Eg. DEBUG "parseArgs" , will display stubs only for function parseArgs()

All stubs can be activated by assigning #define DEBUG the string "all". The code must be\
recompiled once DEBUG is changed. Although, they make the code harder to read because\
they are encapsulated within the previously noted comments, all stubs can easily be\
removed for production versions of the code with the "sed" command from a bash terminal.
