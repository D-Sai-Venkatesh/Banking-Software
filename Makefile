rm -rf database
mkdir database
touch database/client_info.dat
touch database/current_accnum.dat
gcc init.c
./a.out
gcc -pthread functions.c server.c -o server
gcc client.c -o client
