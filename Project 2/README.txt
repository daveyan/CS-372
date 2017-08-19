David Yan (Yanda)
As per the specifications of the project, you must run server first and then run the client to connect. 
Directory list and Files sent and writtedn should be located on the same the directory of the application

>>> ftserver.c <<<

COMPILE:
gcc ftserver.c -o ftserver
USAGE:
 ./ftserver [PORT NUMBER]
 EXAMPLE TESTED - Ubuntu Bash WSL and Flip3
 ./ftserver 9001

>>>> ftclient.py <<<

COMPILE/USAGE:
python3 ftclient.py [SERVERHOST] [SERVERPORT] [COMMAND] [FILENAME] [DATAPORT]

EAMPLE TESTED - Ubuntu Bash WSL and Flip3
Requesting Directory
python3 ftclient.py 127.0.0.1 9001 -l 9002 
OR
python3 ftclient.py 127.0.0.1 9001 -l textfile.txt 9002

Requesting File Transfer
python3 ftclient.py 127.0.0.1 9001 -g textfile.txt 9002



