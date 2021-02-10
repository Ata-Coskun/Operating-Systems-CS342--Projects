/*Ata Coşkun 21503061 CS342 Project 1 */
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <mqueue.h>
#include "shareddefs.h"
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/time.h>

struct node* parent_head = NULL;

//void delete_whole_list(struct node* pass){struct node* current = pass; while(current != NULL){pass = current->next; free(current); current = pass;}}
void create_linked_list(struct node* item){
    if(parent_head == NULL){
    parent_head = (struct node*) malloc(sizeof(struct node));
    strcpy(parent_head->text,item->text);
    parent_head->frequency = item->frequency;
}
    //printf("head is  -> name: %s freq: %d", parent_head->text,parent_head->frequency);
}

void insert(struct node* item){

    int flag =0;
    struct node* current = parent_head;
    //&& strcmp(item->text,current->text) > 0
    while(current->next!= NULL ){
        if(strcmp(current->text,item->text) == 0){ current->frequency = current->frequency + item->frequency; flag = 1;}
        current = current->next;

    }
    if(strcmp(current->text,item->text) == 0){ current->frequency = current->frequency + item->frequency; flag = 1;}

    if(flag == 0){
    struct node* new_node = (struct node*) malloc(sizeof(struct node));

    strcpy(new_node->text,item->text);
    new_node->frequency = item->frequency;

    current->next = new_node;
}
 //printf("\n Insertion(to tail) is succesfull. [%d] is inserted. It takes %ld microseconds.",value,end_time.tv_usec-start_time.tv_usec);
}

void print_list(){

    printf("\n Printing list: ");

    struct node* current = parent_head;
    while(current->next !=NULL){
        printf("\n name: %s frequency: %d \n",current->text, current->frequency);
        current = current->next;
    }
    printf("\n name: %s frequency: %d",current->text, current->frequency);

}
void output_create(){
    //print_list();

    struct node* current = parent_head;
    int number =0;
    while(current->next !=NULL){
        //printf("\n name: %s frequency: %d \n",current->text, current->frequency);
        current = current->next;
        number++;
    }
    number++;
    //printf("n = %d",number);

    FILE * fp;
    int i;
    fp = fopen ("output_pwc.txt","w");

    current = parent_head;
    for(i = 0; i < number;i++){
       fprintf (fp, "%s %d\n",current->text,current->frequency);
       current = current->next;
   }

   fclose (fp);

}

void send_data(struct node* cur, int n_of_node){
    mqd_t mq;
    //struct node node;
    int n;
    int valid = 0;
    mq = mq_open(MQNAME,O_RDWR);
    if(mq == -1){
        perror("mq_open failed\n"); exit(1);
    }
    while(valid !=  n_of_node){
        //printf("text: %s\n frequency: %d \n ",cur->text,cur->frequency);
        n= mq_send(mq,(char*) cur,sizeof(struct node),0);
        if(n ==-1){
            perror("mq_send failed\n"); exit(1);
        }
        else{
            valid++;
            cur = cur->next;
        }
        sleep(1);
    }
    mq_close(mq);

}
int main(int argc, char** argv){
    struct timeval start_time;
    struct timeval end_time;
    gettimeofday(&start_time,NULL);


    if(argc < 2){
        printf(" You need to add txt file");
    }
    else{

        for(int i =1; i < argc;i++){

            char* filename = argv[i];
            //FILE *fp;
            //fp = fopen(filename,"r");

            //int fd = open(filename,O_RDONLY);
            struct stat info;

            if(stat(filename,&info) < 0){printf("Problem occur. File is not open");continue;}
            if(info.st_size == 0){printf("%s is empty\n",filename);continue;}

            pid_t pid;
            pid = fork();

            if(pid == 0){
                //printf("Child process started.");
                // get the text from the file
                FILE *fp;
                //char* filename = argv[i];
                fp = fopen(filename,"r");
                char str[1024];

                if(fp == NULL){printf("Problem occur. File is not open"); return -1;}

                int counter = 1;
                struct node* head;

                while(fgets(str,1024,fp) !=NULL){
                    printf("\n");

                //////////////////////////////////////////////
                int ln = strlen(str);
                if(ln >0 && (str[ln-1]=='\n')){str[ln-1]='\0';}

                // substring
                char* word = strtok(str," ");

                while(word != NULL){
                    //printf ("%s\n", word);
                    /// creating new_node
                    struct node* new_node = (struct node*) malloc(sizeof(struct node));

                    //new_node->text =  (char *) malloc(strlen(word)*sizeof(char));
                    strcpy(new_node->text,word);
                    //printf("%s",new_node->text);
                    new_node->frequency = 1;

                    // if linked_list empty.
                    if(counter == 1){ new_node->next = NULL; head = new_node;counter++;}

                    else{

                        struct node* current = head;

                        while(current->next != NULL && strcmp(new_node->text,current->text) > 0){
                            current = current->next;
                        }

                        if(strcmp(new_node->text,current->text) == 0){current->frequency = current->frequency+ 1;free(new_node);}

                        else{

                        if(current == head){

                        if(strcmp(new_node->text,head->text) < 0 ){ new_node-> next = head; head = new_node;}
                        else if(strcmp(new_node->text,head->text) == 0 ){ head->frequency = head->frequency + 1;}
                        else if(strcmp(new_node->text,head->text) > 0){head->next = new_node; new_node->next = NULL;}
                    }

                        else if(current != head && current->next == NULL){
                        current->next = new_node;
                        new_node->next = NULL;
                      }

                    else{
                        struct node* tmp = current->next;
                        new_node->next = tmp;
                        current->next = new_node;
                    }
                }
                }
                    word = strtok(NULL," ");
}
}

       struct node* current2 = head;
       int number_of_node = 0;
       while(current2!=NULL){
           //printf("%s frequency: %d \n",current2->text, current2->frequency);
           number_of_node++;
           current2 = current2->next;
       }

       struct node* current3 = head;
       while(current3!=NULL){
           current3->no_of_node = number_of_node;
           current3 = current3->next;
       }
       fclose(fp);
       send_data(head,number_of_node);


       struct node* deleter = head;
       while(deleter != NULL){
           head = deleter->next;
           free(deleter);
           deleter = head;
       }
       current2 = NULL;
       current3 =NULL;

       exit(0);
       //printf("Child process ended.");
}
        else{

                mqd_t mq;
                struct mq_attr mq_attr;
                struct node* itemptr;
                int n,buflen;
                char* bufptr;
                mq= mq_open(MQNAME,O_RDWR|O_CREAT, 0666, NULL);
                if(mq==-1){
                    perror("can not create msg queue\n"); exit(1);
                }
                mq_getattr(mq,&mq_attr);

                //Sprintf("mq maximum msgsize = %d\n",(int)mq_attr.mq_msgsize);
                buflen = mq_attr.mq_msgsize;
                bufptr = (char*) malloc(buflen);
                int valid = 0;
                int no_of_node = 1;
                while(valid < no_of_node){
                    n = mq_receive(mq,(char*) bufptr,buflen,NULL);
                    if(n==-1){
                        perror("mq_receive failed\n");exit(1);
                    }
                    else{
                    itemptr = (struct node*)bufptr;
                    if(parent_head ==NULL) {create_linked_list(itemptr);}
                    else {insert(itemptr);}

                    valid++;
                }
                    no_of_node = itemptr->no_of_node;
                    //printf("name: %s frequency: %d\n",itemptr->text,itemptr->frequency);
            }
        free(bufptr);
        mq_close(mq);
        kill(pid, SIGTERM);

}
}

  output_create();
  struct node* deleter = parent_head;
  while(deleter != NULL){
      parent_head = deleter->next;
      free(deleter);
      deleter = parent_head;
  }
}
gettimeofday(&end_time,NULL);
printf("\n It takes %ld microseconds.",end_time.tv_usec-start_time.tv_usec);

return 0;
}
