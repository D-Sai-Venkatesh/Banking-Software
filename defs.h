#include <stdio.h>
#include <time.h>

#define single_account 0
#define joint_account 1 

#define withdraw 0
#define deposit 1
#define transfer 2


#define success 0
#define fail -1
#define username_exsisting -2
#define file_busy -3
#define max_number_of_users_exeeded -4

#define not_enough_balence -1
#define account_not_found -2

struct account_holder_info
{
    long int account_number;
    int isadmin;
    char name[100];
    char address[200];
    int age;
    char username[20];
    char password[20];
};

struct account_info
{
    long int account_number;
    int account_type;
    int number_of_holders;
    struct account_holder_info account_holders[5];
    int balence;
};

struct transaction
{
    int transaction_type;
    time_t time;
    struct account_holder_info initiator;
    struct account_info to_account;
    struct account_info from_account;
    int balence;
    int amount;
};