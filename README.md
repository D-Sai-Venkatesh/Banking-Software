To run this code 

1) run ./Makefile to complile server and client code
2) ./server and ./client to run the programs


3) as an initial state 2 userhave been created 
    1)  username = username1
        password = password1
        account type = single
        this user has admin rights and also his own account in the bank

    2)  username = username2
        password = password2
        account type = single
        this user does not have admin rights

    3)  username = username3
        password = password3
        account type = joint account
        this user does not have admin rights

    4)  username = username4
        password = password4
        account type = joint account
        this user does not have admin rights

    user 3 and 4 have joint account (account number 3)

4) all the data is being stored in databse folder. where 
    1) client_info.dat contains all the information related to account holders
    2) folders with account number as name is created for each account which contains account information and transactions
    3) current_accnum.dat stores the account number which will be assigned to next new account.
