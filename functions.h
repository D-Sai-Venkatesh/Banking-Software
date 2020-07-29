#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdlib.h>
#include<string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "defs.h"


void get_all_users();

int get_user(char*, struct account_holder_info *);

void get_account_info(long int);

void get_transactions(long int);

int create_single_account(char*,char*,int,char*,char*,int);

int join_user(char*,char*,int,char*,char*,int,long int);

int update_info_user(int [] , int,char*,char*,int,char*,char*,char*);

int deposit_money (int,char*);

int withdraw_money(int, char*);

int transfer_money(int, char* , long int);