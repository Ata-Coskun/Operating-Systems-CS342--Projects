
/*Ata Coşkun 21503061 CS342 Project 1 */
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "shareddefs.h"
#include <sys/stat.h>
#include <sys/time.h>


struct node* parent_head = NULL;
char* filename;

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
 //printf("\n Insertion(to tail) is succesfull. [%d] is inserted. It takes %ld microseconds.",value,end_time.tv_usec-start_time.tv_usec);
}
}

void *myThreadFun(void *vargp){
    sleep(1);
    struct node* passing_head = (struct node*) vargp;

    FILE *fp;
    fp = fopen(filename,"r");
    char str[1024];

    if(fp == NULL){printf("Problem occur. File is not open"); pthread_exit(0);}
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
//printf("name: %s frequency: %d \n",current2->text, current2->frequency);
number_of_node++;
current2 = current2->next;
}

struct node* current3 = head;
while(current3!=NULL){
current3->no_of_node = number_of_node;
current3 = current3->next;
}
fclose(fp);

/*
send_data(head,number_of_node);


struct node* deleter = head;
while(deleter != NULL){
head = deleter->next;
free(deleter);
deleter = head;
}
current2 = NULL;
current3 =NULL;
*/
    passing_head->next = head;
    pthread_exit(0);
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
    fp = fopen ("output_twc.txt","w");

    current = parent_head;
    for(i = 0; i < number;i++){
       fprintf (fp, "%s %d\n",current->text,current->frequency);
       current = current->next;
   }

   fclose (fp);

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
             pthread_t tid;
             filename = argv[i];


             struct stat info;

             if(stat(filename,&info) < 0){printf("Problem occur. File is not open");continue;}
             if(info.st_size == 0){printf("%s is empty\n",filename);continue;}

             struct node* get_head = (struct node*) malloc(sizeof(struct node));


             pthread_create(&tid, NULL, myThreadFun, (void *)get_head);
             pthread_join(tid,NULL);

             struct node* itemptr = get_head->next;

             while(itemptr != NULL){
                 //printf("\n name: %s frequency: %d\n", itemptr->text, itemptr->frequency);
                 if(parent_head ==NULL) {create_linked_list(itemptr);}
                 else {insert(itemptr);}
                 itemptr = itemptr->next;
        }

        struct node* deleter = get_head;
        while(deleter != NULL){
        get_head = deleter->next;
        free(deleter);
        deleter = get_head;
        }
    }
}
        output_create();

        struct node* deleter = parent_head;
        while(deleter != NULL){
        parent_head = deleter->next;
        free(deleter);
        deleter = parent_head;
        }
        gettimeofday(&end_time,NULL);
        printf("\n It takes %ld microseconds.",end_time.tv_usec-start_time.tv_usec);
        pthread_exit(NULL);
        return 0;
}
