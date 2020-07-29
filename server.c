#include<stdio.h>
#include<string.h>    
#include<stdlib.h>    
#include<sys/socket.h>
#include<arpa/inet.h> 
#include<unistd.h>    
#include<pthread.h> 
#include "functions.h"

//the thread function
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int n;
    char    sendBuff[20], client_message[20];
    int status;
    char temp[20];
    struct account_holder_info holder;
    // check username
    while(1)
    {
      n=recv(sock,client_message,20,0);
      if(n==0 || n<0)
      {
        puts("Client Disconnected");
        break;
      }
      strcpy(temp,client_message);
      status = get_user(temp,&holder);
      printf("recvd s %s\n",client_message);
      printf("holder %s\n",holder.username);
      if(status == success)
      {
        sprintf(sendBuff, "success");
        send(sock,sendBuff,n,0);
        break;
      }
      else
      {
        sprintf(sendBuff, "fail");
        send(sock,sendBuff,n,0);
      }
    }
    
  // check password
    while(1)
    {
      n=recv(sock,client_message,20,0);
      if(n==0 || n<0)
      {
        puts("Client Disconnected");
        break;
      }
      strcpy(temp,client_message);
      if(strncmp(holder.password,client_message,strlen(holder.password))==0)
      {
        sprintf(sendBuff, "success");
        send(sock,sendBuff,n,0);
        break;
      }
      else
      {
        sprintf(sendBuff, "fail");
        send(sock,sendBuff,n,0);
      }

    }

    if(n==0)
    {
      puts("Client Disconnected");
    }
    else
    {
      perror("recv failed");
    }

// check is admin

    while(1)
    {
      n=recv(sock,client_message,20,0);
      strcpy(temp,client_message);
      if(holder.isadmin == 1)
      {
        sprintf(sendBuff, "success");
        send(sock,sendBuff,n,0);
        break;
      }
      else
      {
        sprintf(sendBuff, "fail");
        send(sock,sendBuff,n,0);
        break;
      }
    }
// start interface

  char account_info[100];

  while(1)
  {
    char rbuffer[1024],sbuffer[1024];
    n=recv(sock,rbuffer,1024,0);

    if(n==0 || n<0)
    {
      puts("Client Disconnected\n");
      break;
    }

    printf("recvd %s\n",rbuffer);
    int temp_temp  = strncmp("option11",rbuffer,strlen("option11"));
    // printf("XXX   %d XXXXXXXX\n",temp_temp);
    if(strncmp("option1",rbuffer,strlen("option1"))==0)
    {
      sprintf(sbuffer,"name = %s, address = %s, age = %d, username = %s, password = %s",holder.name,holder.address,holder.age,holder.username,holder.password);
      send(sock,sbuffer,1024,0);
    }

    if(temp_temp==0)
    {
      // printf("XXXXXXXXXX\n");
      char name[200],address[200],username[20],password[20],age_str[10],is_admin_str[10],acc_no_str[20];
      int age,is_admin;
      long int acc_no;

      n=recv(sock,name,200,0);
      // strcpy(name,rbuffer);

      n=recv(sock,address,200,0);
      // printf("XXXXXXXXXX\n");

      // strcpy(address,rbuffer);

      n=recv(sock,username,20,0);
      // strcpy(username,rbuffer);

      n=recv(sock,password,20,0);
      // strcpy(password,rbuffer);

      n=recv(sock,age_str,10,0);
      strcpy(age_str,rbuffer);
      age = atoi(age_str);

      n=recv(sock,rbuffer,10,0);
      is_admin = atoi(is_admin_str);

      long int status = create_single_account(name,address,age,username,password,is_admin);
      if(status != username_exsisting && status != fail)
      {
        send(sock,"fail",strlen("fail"),0);
      }
      else
      {
        sprintf(acc_no_str,"%ld",status);
        send(sock,acc_no_str,strlen(acc_no_str),0);
      }

    }


    else if(strncmp("option2",rbuffer,strlen("option2"))==0)
    {      
      sprintf(account_info, "./database/%ld/account_info.dat", holder.account_number);
      int fd = open(account_info,O_RDONLY) ;
      lseek(fd,0,SEEK_SET);
      struct account_info buffer;
      while(read(fd,&buffer,sizeof(buffer))>0)
      {
    
          sprintf(sbuffer,"account number = %ld\nnumber of account holders = %d\n",buffer.account_number,buffer.number_of_holders);
          for(int i= 0 ; i<buffer.number_of_holders; i++)
          {
              strcat(sbuffer,"account holder = ");
              strcat(sbuffer,buffer.account_holders[i].name);
          }
          char balence_temp[10];
          sprintf(balence_temp,"%d",buffer.balence);
          strcat(sbuffer,"\naccount balence = ");
          strcat(sbuffer,balence_temp);
      }
      close(fd);
      send(sock,sbuffer,1024,0);
    }
    else if(strncmp("option3",rbuffer,strlen("option3"))==0)
    {
      sprintf(account_info, "./database/%ld/transactions.dat", holder.account_number);
      int fd = open(account_info,O_RDONLY) ;
      lseek(fd,0,SEEK_SET);
      struct transaction buffer1;

      while(read(fd,&buffer1,sizeof(buffer1))>0)
      {
          if(buffer1.transaction_type == withdraw)
          {
              sprintf(sbuffer,"withdraw,  inititator = %s,  amount = -%d, balence = %d, time = %s",buffer1.initiator.name,buffer1.amount,buffer1.balence,ctime(&buffer1.time));
          }
          else if(buffer1.transaction_type == deposit)
          {
              sprintf(sbuffer,"deposit,  initiator = %s,amount = +%d, balence = %d,  time = %s",buffer1.initiator.name ,buffer1.amount, buffer1.balence, ctime(&buffer1.time));
          }
          else if(buffer1.transaction_type == transfer)
          {
              sprintf(sbuffer,"transfer,  initiator = %s,from_account = %ld  ,to_account = %ld,  amount = %d,  balence = %d,  time = %s",buffer1.initiator.name, buffer1.from_account.account_number,buffer1.to_account.account_number ,buffer1.amount ,buffer1.balence, ctime(&buffer1.time));
          }
          send(sock,sbuffer,1024,0);

      }
      send(sock,"success",strlen("success"),0);

      close(fd);
    }
    
    else if(strncmp("option4",rbuffer,strlen("option4"))==0)
    {
      n=recv(sock,rbuffer,1024,0);
      int amount_w;
      amount_w = atoi(rbuffer);

      int status_w = withdraw_money(amount_w,holder.username);
      if(status_w == success)
      {
        send(sock,"success",strlen("success"),0);
      }
      else
      {
        send(sock,"fail",strlen("fail"),0);
      }
    }

    else if(strncmp("option5",rbuffer,strlen("option5"))==0)
    {
      n=recv(sock,rbuffer,1024,0);
      int amount_w;
      amount_w = atoi(rbuffer);

      int status_w = deposit_money(amount_w,holder.username);
      if(status_w == success)
      {
        send(sock,"success",strlen("success"),0);
      }
      else
      {
        send(sock,"fail",strlen("fail"),0);
      }
    }

    else if(strncmp("option6",rbuffer,strlen("option6"))==0)
    {
      n=recv(sock,rbuffer,1024,0);
      int amount_w;
      amount_w = atoi(rbuffer);


      n=recv(sock,rbuffer,1024,0);
      long int to_acc;
      to_acc = atoi(rbuffer);

      int status_w = transfer_money(amount_w,holder.username,to_acc);
      if(status_w == success)
      {
        send(sock,"success",strlen("success"),0);
      }
      else
      {
        send(sock,"fail",strlen("fail"),0);
      }
    }

    else if(strncmp("option7",rbuffer,strlen("option7"))==0)
    {
      while(1)
      {
        n=recv(sock,rbuffer,1024,0);

        if(strncmp("update5",rbuffer,strlen("update5"))==0)
        {
          break;
        }

        if(strncmp("update1",rbuffer,strlen("update1"))==0)
        {
          n=recv(sock,rbuffer,1024,0);
          int status_up1;
          int arr1[] = {0,1,0,0,0,0};
          status_up1 = update_info_user(arr1,0,rbuffer,"",0,holder.username,"","");
          if(status_up1 == success)
          {
            send(sock,"success",strlen("success"),0);
          }
          else
          {
            send(sock,"fail",strlen("fail"),0);
          }
        }
        
        else if(strncmp("update2",rbuffer,strlen("update2"))==0)
        {
          n=recv(sock,rbuffer,1024,0);
          int status_up2;
          int arr1[] = {0,0,1,0,0,0};
          status_up2 = update_info_user(arr1,0,"",rbuffer,0,holder.username,"","");
          if(status_up2 == success)
          {
            send(sock,"success",strlen("success"),0);
          }
          else
          {
            send(sock,"fail",strlen("fail"),0);
          }
        }

        else if(strncmp("update3",rbuffer,strlen("update3"))==0)
        {
          n=recv(sock,rbuffer,1024,0);
          int status_up3;
          int arr1[] = {0,0,0,0,0,1};
          status_up3 = update_info_user(arr1,0,"","",0,holder.username,"",rbuffer);
          if(status_up3 == success)
          {
            send(sock,"success",strlen("success"),0);
          }
          else
          {
            send(sock,"fail",strlen("fail"),0);
          }
        }
      }
      break;
    }
    
    else if(strncmp("option8",rbuffer,strlen("option8"))==0)
    {
      int fd = open("./database/client_info.dat",O_RDONLY);
      char trncs[10000] = {'\0'};
      sprintf(trncs,"USER INFO\n");

      lseek(fd,0,SEEK_SET);
      struct account_holder_info buffer;
      while(read(fd,&buffer,sizeof(buffer)))
      {
          char bufbuf[200] = {'\0'};
          sprintf(bufbuf,"acc num %ld , name = %s, username = %s\n",buffer.account_number,buffer.name,buffer.username);
          strcat(trncs,bufbuf);
      }
      send(sock,trncs,strlen(trncs),0);
      close(fd);
    }

    else if(strncmp("option9",rbuffer,strlen("option9"))==0)
    {      
      n=recv(sock,rbuffer,1024,0);
      long int to_acc;
      to_acc = atoi(rbuffer);

      sprintf(account_info, "./database/%ld/account_info.dat", to_acc);
      int fd = open(account_info,O_RDONLY) ;
      lseek(fd,0,SEEK_SET);
      struct account_info buffer;
      while(read(fd,&buffer,sizeof(buffer))>0)
      {
    
          sprintf(sbuffer,"account number = %ld\nnumber of account holders = %d\n",buffer.account_number,buffer.number_of_holders);
          for(int i= 0 ; i<buffer.number_of_holders; i++)
          {
              strcat(sbuffer,"account holder = ");
              strcat(sbuffer,buffer.account_holders[i].name);
              strcat(sbuffer,"\n");
          }
          char balence_temp[10];
          sprintf(balence_temp,"%d",buffer.balence);
          strcat(sbuffer,"\naccount balence = ");
          strcat(sbuffer,balence_temp);
      }
      close(fd);
      send(sock,sbuffer,1024,0);
    }

    else if(strncmp("option10",rbuffer,strlen("option10"))==0)
    {
      char username_buffer[20];
      n=recv(sock,username_buffer,1024,0);
      if(n==0 || n<0)
      {
        puts("Client Disconnected\n");
        break;
      }
      while(1)
      {
        n=recv(sock,rbuffer,1024,0);
        if(n==0 || n<0)
        {
          puts("Client Disconnected\n");
          break;
        }

        if(strncmp("update5",rbuffer,strlen("update5"))==0)
        {
          break;
        }

        if(strncmp("update1",rbuffer,strlen("update1"))==0)
        {
          n=recv(sock,rbuffer,1024,0);
          int status_up1;
          int arr1[] = {0,1,0,0,0,0};
          status_up1 = update_info_user(arr1,0,rbuffer,"",0,username_buffer,"","");
          if(status_up1 == success)
          {
            send(sock,"success",strlen("success"),0);
          }
          else
          {
            send(sock,"fail",strlen("fail"),0);
          }
        }
        
        else if(strncmp("update2",rbuffer,strlen("update2"))==0)
        {
          n=recv(sock,rbuffer,1024,0);
          int status_up2;
          int arr1[] = {0,0,1,0,0,0};
          status_up2 = update_info_user(arr1,0,"",rbuffer,0,username_buffer,"","");
          if(status_up2 == success)
          {
            send(sock,"success",strlen("success"),0);
          }
          else
          {
            send(sock,"fail",strlen("fail"),0);
          }
        }

        else if(strncmp("update3",rbuffer,strlen("update3"))==0)
        {
          n=recv(sock,rbuffer,1024,0);
          int status_up3;
          int arr1[] = {0,0,0,0,0,1};
          status_up3 = update_info_user(arr1,0,"","",0,username_buffer,"",rbuffer);
          if(status_up3 == success)
          {
            send(sock,"success",strlen("success"),0);
          }
          else
          {
            send(sock,"fail",strlen("fail"),0);
          }
        }
      }
    }


    else if(strncmp("option12",rbuffer,strlen("option12"))==0)
    {
      char name[200],address[200],username[20],password[20],age_str[10],is_admin_str[10],acc_no_str[20];
      int age,is_admin;
      long int acc_no;

      n=recv(sock,acc_no_str,20,0);
      // strcpy(acc_no_str,rbuffer);
      acc_no = atoi(acc_no_str);

      n=recv(sock,name,200,0);
      // strcpy(name,rbuffer);

      n=recv(sock,address,200,0);
      // strcpy(address,rbuffer);

      n=recv(sock,username,20,0);
      // strcpy(username,rbuffer);

      n=recv(sock,password,20,0);
      // strcpy(password,rbuffer);

      n=recv(sock,age_str,10,0);
      // strcpy(age_str,rbuffer);
      age = atoi(age_str);

      n=recv(sock,is_admin_str,10,0);
      // strcpy(is_admin_str,rbuffer);
      is_admin = atoi(is_admin_str);

      long int status = join_user(name,address,age,username,password,is_admin,acc_no);
      if(status != username_exsisting && status != fail)
      {
        send(sock,"fail",strlen("fail"),0);
      }
      else
      {
        send(sock,"success",strlen("success"),0);
      }

    }

  }

    if(n==0)
    {
      puts("Client Disconnected");
    }
    else
    {
      perror("recv failed");
    }

      // while((n=recv(sock,client_message,2000,0))>0)
      // {

      //   send(sock,client_message,n,0);
      // }

    close(sock);
    return 0;
}

int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , *new_sock;
    struct sockaddr_in server , client;

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 5001);

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    //Listen
    listen(socket_desc , 3);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    while(client_sock=accept(socket_desc,(struct sockaddr*)&client,(socklen_t*)&c))
       {
        puts("Connection accepted");

        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;

        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }

        puts("Handler assigned");
    }

    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    return 0;
}

