#include "functions.h"
// newwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
void get_all_users()
{
    int fd = open("./database/client_info.dat",O_RDONLY);
    lseek(fd,0,SEEK_SET);
    struct account_holder_info buffer;
    while(read(fd,&buffer,sizeof(buffer)))
    {
        printf("acc num %ld , name = %s, username = %s\n",buffer.account_number,buffer.name,buffer.username);
    }
    close(fd);
}

int get_user(char * username, struct account_holder_info * buffer)
{
    int fd = open("./database/client_info.dat",O_RDONLY);

    // getting full file read lock
    struct flock flk;
    flk.l_type =F_RDLCK;//read flk is shared
	flk.l_whence=SEEK_SET;
	flk.l_start=0;
	flk.l_len=0;
	flk.l_pid=getpid();
    int state = fcntl(fd,F_SETLKW,&flk);

    if(state < 0)
    {
        return file_busy;
    }

    lseek(fd,0,SEEK_SET);
    int len;
    while(read(fd,buffer,sizeof(struct account_holder_info)))
    {   
        printf("%s || %s\n",buffer->username,username);
        len = strlen(buffer->username);
        if(strncmp(buffer->username,username,len)==0)
        {
            flk.l_type= F_UNLCK;
	        int j=fcntl(fd,F_SETLK,&flk);
            close(fd);
            return success;
        }
    }

    flk.l_type= F_UNLCK;
	int j=fcntl(fd,F_SETLK,&flk);
    close(fd);
    return fail;
}

void get_account_info(long int account_number)
{
    char account_info[100];
    sprintf(account_info, "./database/%ld/account_info.dat", account_number);
    int fd = open(account_info,O_RDONLY) ;

    lseek(fd,0,SEEK_SET);
    struct account_info buffer;
    while(read(fd,&buffer,sizeof(buffer))>0)
    {
        printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
        printf("account number = %ld\n",buffer.account_number);
        printf("number of account holders = %d\n",buffer.number_of_holders);
        for(int i= 0 ; i<buffer.number_of_holders; i++)
        {
            printf("account holder %d = %s\n",i+1,buffer.account_holders[i].name);
        }
        printf("account balence = %d\n",buffer.balence);
    }
    printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");

    close(fd);
}

void get_transactions(long int account_number)
{
    char account_info[100];
    sprintf(account_info, "./database/%ld/transactions.dat", account_number);
    
    int fd = open(account_info,O_RDONLY) ;
    lseek(fd,0,SEEK_SET);
    struct transaction buffer;

    while(read(fd,&buffer,sizeof(buffer))>0)
    {
        printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
        if(buffer.transaction_type == withdraw)
        {
            printf("withdraw,  inititator = %s,  amount = -%d, balence = %d, time = %s\n",buffer.initiator.name,buffer.amount,buffer.balence,ctime(&buffer.time));
        }
        else if(buffer.transaction_type == deposit)
        {
            printf("deposit,  initiator = %s,amount = +%d, balence = %d,  time = %s\n",buffer.initiator.name ,buffer.amount, buffer.balence, ctime(&buffer.time));
        }
        else if(buffer.transaction_type == transfer)
        {
            printf("transfer,  initiator = %s,from_account = %ld  ,to_account = %ld,  amount = %d,  balence = %d,  time = %s\n",buffer.initiator.name, buffer.from_account.account_number,buffer.to_account.account_number ,buffer.amount ,buffer.balence, ctime(&buffer.time));
        }
    }
    printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
    close(fd);
}

int create_single_account(char* name, char* address, int age, char* username, char* password, int is_admin)
{
    int fd = open("./database/current_accnum.dat",O_RDWR);
    int pos = lseek(fd,0,SEEK_END);
    long int acc_num = 1;
    if(pos == 0)
    {
        write(fd,&acc_num,sizeof(acc_num));
    }
    else
    {

        lseek(fd,0,SEEK_SET);
        read(fd,&acc_num,sizeof(acc_num));
        acc_num = acc_num + 1;
        lseek(fd,0,SEEK_SET);
        write(fd,&acc_num,sizeof(acc_num));    
    }
    // flk.l_type= F_UNLCK;
	// j=fcntl(fd,F_SETLK,&flk);
    close(fd);
    // =================got acc number===========


    struct account_holder_info new_user;
    strcpy(new_user.name,name);
    strcpy(new_user.address,address);
    strcpy(new_user.username,username);
    strcpy(new_user.password,password);
    new_user.age = age;
    new_user.isadmin = is_admin;
    new_user.account_number = acc_num;

    fd = open("./database/client_info.dat",O_RDWR);

    // flk.l_type =F_WRLCK;//read flk is shared
	// flk.l_whence=SEEK_END;
	// flk.l_start=0;
	// flk.l_len=sizeof(struct account_holder_info);
	// flk.l_pid=getpid();
    
    // state = fcntl(fd,F_SETLKW,&flk);
    // if(state < 0)
    // {
    //     // printf("unable to create new uses beacuse of system is not able to aquire lock on current acc num file\n");
    //     return file_busy;
    // }

    lseek(fd,0,SEEK_SET);
    
    struct account_holder_info new_buffer;
    while(read(fd,&new_buffer,sizeof(new_buffer)))
    {
        if(strcmp(new_buffer.username,username)==0)
        {
            // flk.l_type= F_UNLCK;
            // j=fcntl(fd,F_SETLK,&flk);
            close(fd);
            return username_exsisting;
        }
    }


    lseek(fd,0,SEEK_END);
    write(fd,&new_user,sizeof(new_user));
    // flk.l_type= F_UNLCK;
    // j=fcntl(fd,F_SETLK,&flk);
    close(fd);

    // now create account folder

    char account_dir[100];
    sprintf(account_dir, "./database/%ld", new_user.account_number); 
    mkdir(account_dir, 0777);

    struct account_info new_account_info;
    new_account_info.account_number = new_user.account_number;
    new_account_info.account_type = single_account;
    new_account_info.number_of_holders = 1;
    new_account_info.balence = 0;
    new_account_info.account_holders[0] = new_user;


    char account_info[100];
    strcpy(account_info,account_dir);
    strcat(account_info,"/account_info.dat");
    printf("addr %s\n",account_info);
    
    fd = open(account_info,O_CREAT|O_RDWR,0777);

    // flk.l_type =F_WRLCK;//read flk is shared
	// flk.l_whence=SEEK_SET;
	// flk.l_start=0;
	// flk.l_len=0;
	// flk.l_pid=getpid();
    // state = fcntl(fd,F_SETLKW,&flk);

    // if(state < 0)
    // {
    //     // printf("unable to create new uses beacuse of system is not able to aquire lock on current acc num file\n");
    //     return file_busy;
    // }

    lseek(fd,0,SEEK_SET);
    write(fd,&new_account_info,sizeof(new_account_info));
    // flk.l_type= F_UNLCK;
    // j=fcntl(fd,F_SETLK,&flk);
    close(fd);


    char transactions[100];
    strcpy(transactions,account_dir);
    strcat(transactions,"/transactions.dat");
    fd = open(transactions,O_CREAT|O_RDWR,0777);
    close(fd);
    return new_account_info.account_number;
}

int join_user(char* name, char* address, int age, char* username, char* password, int is_admin, long int account_number)
{
    char account_info[100];
    sprintf(account_info, "./database/%ld/account_info.dat", account_number);
    int fd = open(account_info,O_RDWR) ;
    
    //get a file lock
    // struct flock flk;
    // flk.l_type =F_WRLCK;//read flk is shared
	// flk.l_whence=SEEK_SET;
	// flk.l_start=0;
	// flk.l_len=0;
	// flk.l_pid=getpid();

    
    // int state = fcntl(fd,F_SETLKW,&flk);
    // if(state < 0)
    // {
    //     close(fd);
    //     return file_busy;
    // }

    lseek(fd,0,SEEK_SET);

    struct account_info buffer;
    read(fd,&buffer,sizeof(buffer));
    if(buffer.number_of_holders >= 5)
    {
        // flk.l_type= F_UNLCK;
        // int j=fcntl(fd,F_SETLK,&flk);
        close(fd);
        return max_number_of_users_exeeded;
    }
    else
    {
        struct account_holder_info new_user;
        strcpy(new_user.name,name);
        strcpy(new_user.address,address);
        strcpy(new_user.username,username);
        strcpy(new_user.password,password);
        new_user.age = age;
        new_user.isadmin = is_admin;
        new_user.account_number = account_number;

        // add in user info also
        int fd1 = open("./database/client_info.dat",O_RDWR);

        // get a file lock
        // struct flock flk1;
        // flk1.l_type =F_WRLCK;//read flk1 is shared
        // flk1.l_whence=SEEK_END;
        // flk1.l_start=0;
        // flk1.l_len=sizeof(struct account_holder_info);
        // flk1.l_pid=getpid();
        // state = fcntl(fd1,F_SETLKW,&flk1);

        // if(state < 0 )
        // {
        //     // flk.l_type= F_UNLCK;
        //     // int j=fcntl(fd,F_SETLK,&flk);
        //     close(fd);
        //     close(fd1);
        //     return file_busy;
        // }


        struct account_holder_info new_buffer;
        while(read(fd1,&new_buffer,sizeof(new_buffer)))
        {
            if(strcmp(new_buffer.username,username)==0)
            {
                // flk.l_type= F_UNLCK;
                // int j=fcntl(fd,F_SETLK,&flk);
                // close(fd);
                // flk1.l_type= F_UNLCK;
                // j=fcntl(fd1,F_SETLK,&flk1);
                // close(fd1);
                return username_exsisting;
            }
        }
        lseek(fd1,0,SEEK_END);
        write(fd1,&new_user,sizeof(new_user));
        // flk1.l_type= F_UNLCK;
        // int j=fcntl(fd1,F_SETLK,&flk1);
        close(fd1);

        buffer.account_type = joint_account;
        buffer.account_holders[buffer.number_of_holders] = new_user;
        buffer.number_of_holders += 1;
        lseek(fd,0,SEEK_SET);
        write(fd,&buffer,sizeof(buffer));
    }

    // flk.l_type= F_UNLCK;
	// int j=fcntl(fd,F_SETLK,&flk);
    close(fd);
    return success;
}

int update_info_user (int array[6],int new_is_admin, char* new_name, char* new_address, int new_age, char* username, char* new_username,char* new_password)
{
    int fd = open("./database/client_info.dat",O_RDWR);
    
    // struct flock flk;
    // flk.l_type =F_RDLCK;//read flk is shared
	// flk.l_whence=SEEK_SET;
	// flk.l_start=0;
	// flk.l_len=0;
	// flk.l_pid=getpid();
    // int state = fcntl(fd,F_SETLKW,&flk);
    // if(state<0)
    // {
    //     close(fd);
    //     return file_busy;
    // }
    lseek(fd,0,SEEK_SET);
    struct account_holder_info buffer;
    int found = 0;
    // use username to fing the user;
    while(read(fd,&buffer,sizeof(buffer)))
    {
        if(strcmp(buffer.username,username)==0)
        {
            found = 1; 
            break;
        }
    }
    // flk.l_type= F_UNLCK;
	// int j=fcntl(fd,F_SETLK,&flk);
    if(found == 1)
    {
        if(array[0]==1)
        {
            buffer.isadmin = new_is_admin;
        }
        if(array[1]==1)
        {
            strcpy(buffer.name,new_name);
        }
        if(array[2]==1)
        {
            strcpy(buffer.address,new_address);
        }
        if(array[3]==1)
        {
            buffer.age = new_age;
        }
        if(array[4]==1)
        {
            strcpy(buffer.username,new_username);
        }
        if(array[5]==1)
        {
            strcpy(buffer.password,new_password);
        }

        // struct flock flk;
        // flk.l_type =F_WRLCK;//read flk is shared
        // flk.l_whence=SEEK_CUR;
        // flk.l_start=-sizeof(buffer);
        // flk.l_len=sizeof(struct account_holder_info);
        // flk.l_pid=getpid();

        // int state = fcntl(fd,F_SETLKW,&flk);

        lseek(fd,-sizeof(buffer),SEEK_CUR);
        write(fd,&buffer,sizeof(buffer));
        
        // flk.l_type= F_UNLCK;
        // j=fcntl(fd,F_SETLK,&flk);
        close(fd);

        char account_info[100];
        sprintf(account_info, "./database/%ld/account_info.dat", buffer.account_number);
        int fd = open(account_info,O_RDWR) ;

        // struct flock flk1;
        // flk1.l_type =F_WRLCK;//read flk1 is shared
        // flk1.l_whence=SEEK_SET;
        // flk1.l_start=0;
        // flk1.l_len=0;
        // flk1.l_pid=getpid();
        // state = fcntl(fd,F_SETLKW,&flk1);

        lseek(fd,0,SEEK_SET);
        struct account_info buffer1;
        read(fd,&buffer1,sizeof(buffer1));
        for(int i = 0 ; i<buffer1.number_of_holders ; i++ )
        {
            if(strcmp(buffer1.account_holders[i].username,username)==0)
            {
                buffer1.account_holders[i] = buffer;
                break;
            }
        }
        lseek(fd,0,SEEK_SET);
        write(fd,&buffer1,sizeof(buffer1));
        // flk1.l_type= F_UNLCK;
        // j=fcntl(fd,F_SETLK,&flk1);
        close(fd);
        return success;
    }
    else
    {

        return fail;
    }
    
}

int deposit_money (int amount, char* initiator_username)
{
    struct account_holder_info initiator;
    get_user(initiator_username,&initiator);
    long int account_number = initiator.account_number;
    char account_info[100];
    sprintf(account_info, "./database/%ld/account_info.dat", account_number);
    
    
    int fd = open(account_info,O_RDWR) ;
     struct flock flk;
    flk.l_type =F_WRLCK;//read flk is shared
    flk.l_whence=SEEK_SET;
     flk.l_start=0;
     flk.l_len=0;
     flk.l_pid=getpid();
     int state = fcntl(fd,F_SETLKW,&flk);

    lseek(fd,0,SEEK_SET);
    struct account_info buffer;
    read(fd,&buffer,sizeof(buffer));

    buffer.balence += amount;
    lseek(fd,0,SEEK_SET);
    write(fd,&buffer,sizeof(buffer));
    
     flk.l_type= F_UNLCK;
     int j=fcntl(fd,F_SETLK,&flk);
    close(fd);

    struct transaction new_transaction;
    new_transaction.transaction_type = deposit;
    // printf("%d\n", new_transaction.transaction_type);

    new_transaction.amount = amount;
    new_transaction.balence = buffer.balence;
    new_transaction.initiator = initiator;
    time(&new_transaction.time);


    sprintf(account_info, "./database/%ld/transactions.dat", account_number);
    fd = open(account_info,O_RDWR) ;

    struct flock flk1;
    flk1.l_type =F_WRLCK;//read flk1 is shared
    flk1.l_whence=SEEK_END;
    flk1.l_start=0;
    flk1.l_len=sizeof(struct transaction);
    flk1.l_pid=getpid();
     state = fcntl(fd,F_SETLKW,&flk1);

    lseek(fd,0,SEEK_END);
    write(fd,&new_transaction,sizeof(new_transaction));
     flk1.l_type= F_UNLCK;
     j=fcntl(fd,F_SETLK,&flk1);
    close(fd);

    return success;
}

int withdraw_money (int amount, char* initiator_username)
{
    struct account_holder_info initiator;
    get_user(initiator_username,&initiator);
    long int account_number = initiator.account_number;
    char account_info[100];
    sprintf(account_info, "./database/%ld/account_info.dat", account_number);
    int fd = open(account_info,O_RDWR) ;

     struct flock flk;
     flk.l_type =F_WRLCK;//read flk is shared
     flk.l_whence=SEEK_SET;
     flk.l_start=0;
     flk.l_len=0;
     flk.l_pid=getpid();
     int state = fcntl(fd,F_SETLKW,&flk);

    lseek(fd,0,SEEK_SET);
    struct account_info buffer;
    read(fd,&buffer,sizeof(buffer));

    if(buffer.balence < amount)
    {
         flk.l_type= F_UNLCK;
         int j=fcntl(fd,F_SETLK,&flk);
        return fail;
    }

    buffer.balence -= amount;
    lseek(fd,0,SEEK_SET);
    write(fd,&buffer,sizeof(buffer));
     flk.l_type= F_UNLCK;
     int j=fcntl(fd,F_SETLK,&flk);
    close(fd);

    struct transaction new_transaction;
    new_transaction.transaction_type = withdraw;
    // printf("%d\n", new_transaction.transaction_type);
    new_transaction.amount = amount;
    new_transaction.balence = buffer.balence;
    new_transaction.initiator = initiator;
    time(&new_transaction.time);


    sprintf(account_info, "./database/%ld/transactions.dat", account_number);
    fd = open(account_info,O_RDWR) ;

     struct flock flk1;
     flk1.l_type =F_WRLCK;//read flk1 is shared
     flk1.l_whence=SEEK_END;
     flk1.l_start=0;
     flk1.l_len=sizeof(struct transaction);
     flk1.l_pid=getpid();
     state = fcntl(fd,F_SETLKW,&flk1);

    lseek(fd,0,SEEK_END);
    write(fd,&new_transaction,sizeof(new_transaction));
		// printf("hello:  \n");
		// getchar();
     flk1.l_type= F_UNLCK;
     j=fcntl(fd,F_SETLK,&flk1);
    close(fd);

    return success;
}


int transfer_money(int amount, char* initiator_username, long int to_account)
{
    struct account_holder_info initiator;
    get_user(initiator_username,&initiator);
    long int account_number = initiator.account_number;
    char account_info[100];
    sprintf(account_info, "./database/%ld/account_info.dat", account_number);
    int fd = open(account_info,O_RDWR) ;

    // struct flock flk;
    // flk.l_type =F_WRLCK;//read flk is shared
    // flk.l_whence=SEEK_SET;
    // flk.l_start=0;
    // flk.l_len=0;
    // flk.l_pid=getpid();
    // int state = fcntl(fd,F_SETLKW,&flk);

    lseek(fd,0,SEEK_SET);
    
    struct account_info buffer;
    read(fd,&buffer,sizeof(buffer));

    buffer.balence -= amount;
    lseek(fd,0,SEEK_SET);
    write(fd,&buffer,sizeof(buffer));

    // flk.l_type= F_UNLCK;
    // int j=fcntl(fd,F_SETLK,&flk);

    close(fd);

// ##############################################################################

    sprintf(account_info, "./database/%ld/account_info.dat", to_account);
    fd = open(account_info,O_RDWR) ;

    // flk.l_type =F_WRLCK;//read flk is shared
    // flk.l_whence=SEEK_SET;
    // flk.l_start=0;
    // flk.l_len=0;
    // flk.l_pid=getpid();
    // state = fcntl(fd,F_SETLKW,&flk);

    lseek(fd,0,SEEK_SET);
    struct account_info buffer1;
    read(fd,&buffer1,sizeof(buffer1));

    if(buffer.balence < amount)
    {
        return fail;
    }

    buffer1.balence += amount;
    lseek(fd,0,SEEK_SET);
    write(fd,&buffer1,sizeof(buffer1));
    
    // flk.l_type= F_UNLCK;
    // j=fcntl(fd,F_SETLK,&flk);
    close(fd);
// ###############################################################################

    struct transaction new_transaction;
    new_transaction.transaction_type = transfer;
    // printf("%d\n", new_transaction.transaction_type);

    new_transaction.amount = amount;
    new_transaction.balence = buffer.balence;
    new_transaction.initiator = initiator;
    new_transaction.from_account = buffer;
    new_transaction.to_account = buffer1;
    time(&new_transaction.time);
// ################################################################################
    
    sprintf(account_info, "./database/%ld/transactions.dat", account_number);
    fd = open(account_info,O_RDWR) ;

    // struct flock flk1;
    // flk1.l_type =F_WRLCK;//read flk1 is shared
    // flk1.l_whence=SEEK_END;
    // flk1.l_start=0;
    // flk1.l_len=sizeof(struct transaction);
    // flk1.l_pid=getpid();
    // state = fcntl(fd,F_SETLKW,&flk1);

    lseek(fd,0,SEEK_END);
    write(fd,&new_transaction,sizeof(new_transaction));
    
    // flk1.l_type= F_UNLCK;
    // j=fcntl(fd,F_SETLK,&flk1);

    close(fd);


// ################################################################################

    new_transaction.balence = buffer1.balence;
    sprintf(account_info, "./database/%ld/transactions.dat", to_account);
    fd = open(account_info,O_RDWR) ;


    // flk1.l_type =F_WRLCK;//read flk1 is shared
    // flk1.l_whence=SEEK_END;
    // flk1.l_start=0;
    // flk1.l_len=sizeof(struct transaction);
    // flk1.l_pid=getpid();
    // state = fcntl(fd,F_SETLKW,&flk1);

    lseek(fd,0,SEEK_END);
    write(fd,&new_transaction,sizeof(new_transaction));
    
    // flk1.l_type= F_UNLCK;
    // j=fcntl(fd,F_SETLK,&flk1);
    
    close(fd);

    return success;
}



// int main()
// {
    // int x = create_single_account("user1","address1",21,"username1","password1",1);
    // printf("status %d\n",x);

    // printf("new account details\n");
    // get_account_info(x);
    // printf("printing all users \n");
    // get_all_users();

    // printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
    // printf("update user\n");

    // int update [] = {1,1,1,1,1,1};
    // int status = join_user("sai2","vzm2",22,"username2","password2",0,x);
    // printf("status %d\n",status);


    // get_account_info(2);
    // get_all_users();
    // struct account_holder_info buffer;
    // int x = get_user("username",&buffer);
    // printf("pass = %s\n",buffer.password);

    // update_info_user(update,1,"new_sai","new_addrss",222,"username","new_username","new_password");
    // get_account_info(x);

    // withdraw_money(1,"username2");
    // withdraw_money(2,"new_username");
    // deposit_money(2,"username2");

    // transfer_money(500, "new_username", 2);
    // get_account_info(1);
    // get_account_info(2);

    // get_transactions(2);
    // get_transactions(2);

    // long int x = create_single_account("username1","address1",21,"username1","password1",1);

    // long int y = create_single_account("username2","address2",22,"username2","password2",0);
    // get_all_users();
    // get_account_info(1);
    // get_account_info(2);
    // deposit_money(1000,"username1");
    // get_transactions(1);
    // transfer_money(500,"username1",2);
    // get_transactions(1);
    // get_transactions(2);
    // get_account_info(1);
    // get_account_info(2);
    // get_transactions(y);
    
    // printf("success\n");
    // long int x2 = create_single_account("sai4","vzm4",24,"username4","password4",0);

    // transfer_money(500,"username3",x2);

    // get_account_info(x2);

    // get_transactions(x);


//     get_transactions(5);
// }
