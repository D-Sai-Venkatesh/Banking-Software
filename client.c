#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_SIZE 200

int main()
{
    int sock_desc;
    struct sockaddr_in serv_addr;
    char sbuff[MAX_SIZE],rbuff[MAX_SIZE];

    if((sock_desc = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        printf("Failed creating socket\n");

    bzero((char *) &serv_addr, sizeof (serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(5001);

    if (connect(sock_desc, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0) {
        printf("Failed to connect to server\n");
        return -1;
    }

    printf("Connected successfully - Please enter string\n");

    char username[20];
    char password[20];
    // check username
    printf("enter username\n");
    while(1)
    {
        fgets(username,20,stdin);
        send(sock_desc,username,strlen(username),0);

        if(recv(sock_desc,rbuff,MAX_SIZE,0)==0)
        {
            printf("Error");
        }
        else
        {
            printf("recvd %s\n", rbuff);
            if(strcmp("success",rbuff)==0)
            {
                printf("enter password\n");
                break;
            }
            else
            {
                printf("wrong username please enter again\n");
            }
            
        }
    }
    // check password
    while(1)
    {
        fgets(password,20,stdin);
        send(sock_desc,password,strlen(password),0);

        if(recv(sock_desc,rbuff,MAX_SIZE,0)==0)
        {
            printf("Error");
        }
        else
        {
            printf("recvd %s\n", rbuff);
            if(strcmp("success",rbuff)==0)
            {
                printf("authentication successfull\n");
                break;
            }
            else
            {
                printf("wrong password please enter again\n");
            }
            
        }
    }
    // check is admin 
    int is_admin = 0;
    send(sock_desc,"is_admin",strlen("is_admin"),0);

    if(recv(sock_desc,rbuff,MAX_SIZE,0)==0)
    {
        printf("Error");
    }
    else
    {
        printf("recvd %s\n", rbuff);
        if(strcmp("success",rbuff)==0)
        {
            printf("ADMIN LOGIN\n");
            is_admin = 1;
        }
        else
        {
            printf("NORMAL USER\n");
        }
        
    }
    int option;
    char sbuffer[1024],rbuffer[1024];
    while(1)
    {
        printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
        printf("1) get my profile\n");
        printf("2) get account info\n");
        printf("3) get transactions\n");
        printf("4) withdraw money\n");
        printf("5) deposit money\n");
        // printf("6) transfer money\n");
        printf("7) update profile\n");
        if(is_admin == 1)
        {
            printf("8) get all user info\n");
            printf("9) get other account info\n");
            printf("10) modify other user info\n");
            printf("11) create single account\n");
            printf("12) join user to a account\n");
        }
        printf("20) exit\n");
        printf("enter your option\n");
        scanf("%d",&option);
        printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");

        if(option == 20)
        {
            send(sock_desc,"option20",strlen("option20"),0);
            break;
        }

        if(option == 1)
        {
            printf("reciving profile info\n");
            send(sock_desc,"option1",strlen("option1"),0);
            if(recv(sock_desc,rbuffer,1024,0)==0)
            {
                printf("Error");
            }
            else
            {
                printf("profile info\n%s\n",rbuffer);   
            }
        }
        if(option == 2)
        {
            printf("reciveing account info\n");
            send(sock_desc,"option2",strlen("option2"),0);
            if(recv(sock_desc,rbuffer,1024,0)==0)
            {
                printf("Error");
            }
            else
            {
                printf("account info\n%s\n",rbuffer);   
            }
        }
        if(option == 3)
        {
            printf("reciveing transactions\n");
            send(sock_desc,"option3",strlen("option3"),0);
            
            while(1)
            {
                if(recv(sock_desc,rbuffer,1024,0)==0)
                {
                    printf("Error");
                }
                else
                {
                    if(strncmp("success",rbuffer,strlen("success"))==0)
                    {
                        printf("all transactions recived\n");
                        break;
                    }
                    else if(strncmp("fail",rbuffer,strlen("fail"))==0)
                    {
                        printf("something went worng");
                        break;
                    }
                    else
                    {
                        printf("%s\n",rbuffer);  

                    }
                }
            }
        }
        if(option == 4)
        {
           printf("withdrawing money\n");
           send(sock_desc,"option4",strlen("option4"),0);
           printf("enter amount\n");
           int amount_w;
           char amount_w_str[10];
           scanf("%d",&amount_w);
           sprintf(amount_w_str, "%d", amount_w);
           send(sock_desc,amount_w_str,strlen(amount_w_str),0);

            if(recv(sock_desc,rbuffer,1024,0)==0)
                {
                    printf("Error");
                }
                else
                {
                    if(strncmp("success",rbuffer,strlen("success"))==0)
                    {
                        printf("money withdrawn\n");
                    }
                    else
                    {
                        printf("not enough money\n");
                    }
                }

        }


        if(option == 5)
        {
           printf("depositing money\n");
           send(sock_desc,"option5",strlen("option5"),0);
           printf("enter amount\n");
           int amount_d;
           char amount_d_str[10];
           scanf("%d",&amount_d);
           sprintf(amount_d_str, "%d", amount_d);
           send(sock_desc,amount_d_str,strlen(amount_d_str),0);

            if(recv(sock_desc,rbuffer,1024,0)==0)
                {
                    printf("Error");
                }
                else
                {
                    if(strncmp("success",rbuffer,strlen("success"))==0)
                    {
                        printf("money deposited\n");
                    }
                    else
                    {
                        printf("something went worng\n");
                    }
                }

        }

        if(option == 6)
        {
            printf("transferring money\n");
            send(sock_desc,"option6",strlen("option6"),0);
           printf("enter amount\n");
           int amount_t;
           char amount_t_str[10];
           scanf("%d",&amount_t);
           sprintf(amount_t_str, "%d", amount_t);
           send(sock_desc,amount_t_str,strlen(amount_t_str),0);
           printf("enter to account number\n");
           int acc_no;
           char acc_no_str[10];
            scanf("%d",&acc_no);
            sprintf(acc_no_str, "%d", acc_no);
           send(sock_desc,acc_no_str,strlen(acc_no_str),0);

            if(recv(sock_desc,rbuffer,1024,0)==0)
            {
                printf("Error");
            }
            else
            {
                if(strncmp("success",rbuffer,strlen("success"))==0)
                {
                    printf("money transfered\n");
                }
                else
                {
                    printf("something went worng\n");
                }
            }
        }
        if(option == 7)
        {
            printf("UPDATE MENU\n");
            send(sock_desc,"option7",strlen("option7"),0);
            int update_option;
            while(1)
            {
                printf("1) upadte name\n");
                printf("2) update address\n");
                printf("3) password\n");
                printf("5) exit\n");
                printf("enter option\n");
                scanf("%d",&update_option);

                if(update_option == 5)
                {
                    send(sock_desc,"update5",strlen("update5"),0);
                    break;
                }

                else if (update_option == 1)
                {
                    char new_name[100];
                    send(sock_desc,"update1",strlen("update1"),0);
                    printf("please enter new name\n");
                    scanf("%s",new_name);
                    send(sock_desc,new_name,strlen(new_name),0);
                    if(recv(sock_desc,rbuffer,1024,0)==0)
                    {
                        printf("Error");
                    }
                    else
                    {
                        if(strncmp("success",rbuffer,strlen("success"))==0)
                        {
                            printf("name updated\n");
                        }
                        else
                        {
                            printf("something went worng\n");
                        }
                    }
                }
                else if (update_option == 2)
                {
                    char new_addr[100];
                    send(sock_desc,"update2",strlen("update2"),0);
                    printf("please enter new address\n");
                    scanf("%s",new_addr);
                    send(sock_desc,new_addr,strlen(new_addr),0);
                    if(recv(sock_desc,rbuffer,1024,0)==0)
                    {
                        printf("Error");
                    }
                    else
                    {
                        if(strncmp("success",rbuffer,strlen("success"))==0)
                        {
                            printf("addr updated\n");
                        }
                        else
                        {
                            printf("something went worng\n");
                        }
                    }
                }

                else if (update_option == 3)
                {
                    char new_pass[100];
                    send(sock_desc,"update3",strlen("update3"),0);
                    printf("please enter new pass\n");
                    scanf("%s",new_pass);
                    send(sock_desc,new_pass,strlen(new_pass),0);
                    if(recv(sock_desc,rbuffer,1024,0)==0)
                    {
                        printf("Error");
                    }
                    else
                    {
                        if(strncmp("success",rbuffer,strlen("success"))==0)
                        {
                            printf("pass updated\n");
                        }
                        else
                        {
                            printf("something went worng\n");
                        }
                    }
                }

            }

            printf("plese login again\n");
            break;
        }
        if(option == 8)
        {
            printf("reciveing all user info\n");
            send(sock_desc,"option8",strlen("option8"),0);
            char r1buffer[10000]={0};
            if(recv(sock_desc,r1buffer,10000,0)==0)
            {
                printf("Error");
            }
            else
            {
                printf("%s\n",r1buffer);  
            }
        }
        if(option == 9)
        {
            printf("reciveing account info\n");
            send(sock_desc,"option9",strlen("option9"),0);

            printf("enter to account number\n");
            int acc_no;
            char acc_no_str[10];
            scanf("%d",&acc_no);
            sprintf(acc_no_str, "%d", acc_no);
            send(sock_desc,acc_no_str,strlen(acc_no_str),0);

            if(recv(sock_desc,rbuffer,1024,0)==0)
            {
                printf("Error");
            }
            else
            {
                printf("account info\n%s\n",rbuffer);   
            }
        }

        else if(option == 10)
        {
            printf("UPDATE MENU\n");
            send(sock_desc,"option10",strlen("option10"),0);
            char username_buffer[20];
            printf("enter username of the account you want to change\n");
            scanf("%s",username_buffer);
            send(sock_desc,username_buffer,strlen(username_buffer),0);

            int update_option;
            while(1)
            {
                printf("1) upadte name\n");
                printf("2) update address\n");
                printf("3) password\n");
                printf("5) exit\n");
                printf("enter option\n");
                scanf("%d",&update_option);

                if(update_option == 5)
                {
                    send(sock_desc,"update5",strlen("update5"),0);
                    break;
                }

                else if (update_option == 1)
                {
                    char new_name[100];
                    send(sock_desc,"update1",strlen("update1"),0);
                    printf("please enter new name\n");
                    scanf("%s",new_name);
                    send(sock_desc,new_name,strlen(new_name),0);
                    if(recv(sock_desc,rbuffer,1024,0)==0)
                    {
                        printf("Error");
                    }
                    else
                    {
                        if(strncmp("success",rbuffer,strlen("success"))==0)
                        {
                            printf("name updated\n");
                        }
                        else
                        {
                            printf("something went worng\n");
                        }
                    }
                }
                else if (update_option == 2)
                {
                    char new_addr[100];
                    send(sock_desc,"update2",strlen("update2"),0);
                    printf("please enter new address\n");
                    scanf("%s",new_addr);
                    send(sock_desc,new_addr,strlen(new_addr),0);
                    if(recv(sock_desc,rbuffer,1024,0)==0)
                    {
                        printf("Error");
                    }
                    else
                    {
                        if(strncmp("success",rbuffer,strlen("success"))==0)
                        {
                            printf("addr updated\n");
                        }
                        else
                        {
                            printf("something went worng\n");
                        }
                    }
                }

                else if (update_option == 3)
                {
                    char new_pass[100];
                    send(sock_desc,"update3",strlen("update3"),0);
                    printf("please enter new pass\n");
                    scanf("%s",new_pass);
                    send(sock_desc,new_pass,strlen(new_pass),0);
                    if(recv(sock_desc,rbuffer,1024,0)==0)
                    {
                        printf("Error");
                    }
                    else
                    {
                        if(strncmp("success",rbuffer,strlen("success"))==0)
                        {
                            printf("pass updated\n");
                        }
                        else
                        {
                            printf("something went worng\n");
                        }
                    }
                }

            }

        }

        else if(option == 11)
        {
            char name[200],address[200],username[20],password[20],age_str[10],is_admin_str[10];
            int age,is_admin;
            send(sock_desc,"option11",strlen("option11"),0);

            printf("please enter name \n");
            scanf("%s",name);
            send(sock_desc,name,strlen(name),0);


            printf("please enter address \n");
            scanf("%s",address);
            send(sock_desc,address,strlen(address),0);

            printf("please enter username \n");
            scanf("%s",username);
            send(sock_desc,username,strlen(username),0);

            printf("please enter password \n");
            scanf("%s",password);
            send(sock_desc,password,strlen(password),0);

            printf("please enter age \n");
            scanf("%d",&age);
            sprintf(age_str, "%d", age);
            send(sock_desc,age_str,strlen(age_str),0);

            printf("please enter is admin \n");
            scanf("%d",&is_admin);
            sprintf(is_admin_str, "%d", is_admin);
            send(sock_desc,is_admin_str,strlen(is_admin_str),0);

            recv(sock_desc,rbuffer,1024,0);
            if(strncmp("fail",rbuffer,strlen("fail"))==0)
            {
                printf("something went wrong\n");
                break;
            }
            else
            {

                printf("account number %s\n",rbuffer);  
            }
        }

        else if(option == 12)
        {
            char name[200],address[200],username[20],password[20],age_str[10],is_admin_str[10],acc_no_str[20];
            int age,is_admin;
            long int acc_no;
            send(sock_desc,"option12",strlen("option12"),0);

            printf("please enter account number\n");
            scanf("%ld",&acc_no);
            sprintf(acc_no_str, "%ld", acc_no);
            send(sock_desc,acc_no_str,strlen(acc_no_str),0);
            
            printf("please enter name \n");
            scanf("%s",name);
            send(sock_desc,name,strlen(name),0);


            printf("please enter address \n");
            scanf("%s",address);
            send(sock_desc,address,strlen(address),0);

            printf("please enter username \n");
            scanf("%s",username);
            send(sock_desc,username,strlen(username),0);

            printf("please enter password \n");
            scanf("%s",password);
            send(sock_desc,password,strlen(password),0);

            printf("please enter age \n");
            scanf("%d",&age);
            sprintf(age_str, "%d", age);
            send(sock_desc,age_str,strlen(age_str),0);

            printf("please enter is admin \n");
            scanf("%d",&is_admin);
            sprintf(is_admin_str, "%d", is_admin);
            send(sock_desc,is_admin_str,strlen(is_admin_str),0);

            recv(sock_desc,rbuffer,1024,0);
            if(strncmp("fail",rbuffer,strlen("fail"))==0)
            {
                printf("something went wrong\n");
            }
            else if(strncmp("success",rbuffer,strlen("success")))
            {
                printf("user joined \n");  
            }
        }
    }


    // while(fgets(sbuff, MAX_SIZE , stdin)!=NULL)
    // {
    //   send(sock_desc,sbuff,strlen(sbuff),0);

    //       if(recv(sock_desc,rbuff,MAX_SIZE,0)==0)
    //        printf("Error");
    //       else
    //        fputs(rbuff,stdout);

    //    bzero(rbuff,MAX_SIZE);
    // }
    // close(sock_desc);
    return 0;

}
