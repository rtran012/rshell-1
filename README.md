This is my implementation of the rshell

Pros:
1) User name and host name have been implemented and work perfectly.

2) For connectors like && and ||, there may be spaces on both sides, on one side or no spaces at all.

3) Arguments can have any length.

4)Exit commands works perfeclty with and without connectors


Notes: The implementaion of connectors is as follows:
if the previous command was successfull, and && is detected then execute next command.
If previous command was unsucessful and || is detected then execute next command.
In case of ; allways execute next command.


Bugs
1) The max number of arguments is 30, or a constant that can be changed, will fix it later.
