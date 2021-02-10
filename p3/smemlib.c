/*
Ata Coşkun-21503061
Ataberk Gözkaya-21501928
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <fcntl.h> 
#include <sys/shm.h> 
#include <sys/stat.h> 
#include <sys/mman.h>
#include <pthread.h> 
#include <semaphore.h> 
#include <unistd.h> 
#include <math.h>

// Define a name for your shared memory; you can give any name that start with a slash character; it will be like a filename.
int shm_fd;
const char* name = "Shared Memory";
// Define semaphore(s)
sem_t mutex;
 
// Define your stuctures and variables

int memory_size;
void* ptr;
long long int allocated_list[4194304][2]; // 4mb max
int sem_cnt =0; // it is the check for mutex is initialised when first process reached.

int smem_init(int segmentsize)
{   

    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666); // creating shared memory.
    if(shm_fd == -1) printf("shm open failed");
    ftruncate(shm_fd,segmentsize);

    FILE* fd;
    fd = fopen("size.txt","w+");
    fprintf(fd,"%d",segmentsize);
    fclose(fd);

    printf("shm init memory_size: %d\n",segmentsize);
   
    printf ("smem init called \n"); // remove all printfs when you are submitting to us.  
    return (0); 
}

int smem_remove()
{
    shm_unlink(name);
    
    FILE* fd;
    fd = fopen("size.txt","w+");
    fprintf(fd,"%d",0);
    fclose(fd);
    memory_size = 0;

    sem_destroy(&mutex); 
    return (0); 
}

int smem_open()
{   
   
    if(memory_size <= 0){
    FILE* fd;
    fd = fopen("size.txt","r");
    fscanf(fd,"%d",&memory_size);
    fclose(fd);
    }
     printf("shm open memory_size: %d\n",memory_size);

    shm_fd = shm_open(name, O_RDWR, 0600); // reading shared memory.
    if(shm_fd == -1) printf("shm open failed");
    ptr = mmap(NULL,memory_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS,shm_fd,0); // get beginning address of shared memory.
    if(ptr == MAP_FAILED){return -1;} // if too many process try to reach shared memory.
    if(sem_cnt == 0){ 
    sem_init(&mutex, 0, 1);
    sem_cnt +=1;
    }
    return 0; 
}

void *smem_alloc (int size)
{    

    sem_wait(&mutex);
    if(memory_size == 0){printf("Memory is destroy or not initialised. Memory size is zero now");}
    
    printf("size : %d\n", size);
    int block_size;
    
    // block size calculation
    if (size < 8) block_size = 8;
    int k=0;
    while(pow(2,k) < size){k++;}
    //////
    printf("k: %d", k);
    block_size = pow(2,k);
    printf("block size: %d\n",block_size);

    if(block_size > memory_size){
        printf("Requested size bigger than shared memory size");
        return NULL;
    }

    int n = 0;
    
    /*
    1.Find a available starting point(not allocated byte).
    2.Start to count empty hole: go 1 by by until finding a allocated point.
    3.If hole size reachs to block size(the block we will allocate). Allocate the block.
    4.If hole size is smaller than block size. Jump the allocated block and continue to search.
    */
    while( n < memory_size){
        printf("allocated_list[%d][0] = %lld \n", n, allocated_list[n][0] );   
        if(allocated_list[n][0] == 0){ // head adress is empty
               int tracker = n;
               int s_cnt = 0;
               // evaluate size of hole
               while(allocated_list[tracker][0] == 0 ){ // continue until the find a allocated block
                   if(allocated_list[tracker][0] == 0){
                       tracker += 1; // track the adress. tracker keeps x'th bytes from the starting point(ptr).
                       s_cnt +=1; // hole size. it starts from zero and continue to count until encountering an allocated block.
                   }
                   // if hole size reachs the block size. Allocate the area.
                   if(block_size == s_cnt){
                        allocated_list[n][0] = n;
                        allocated_list[n][1] = n+ block_size;
                        printf("Allocation is successfull\n");
                        sem_post(&mutex);
                        return ptr +n ; // allocation sucessfull return.
                   }
               }
               n = tracker; // if hole size does not reach block size. Get the head of an allocated block.
        }
        else{
        n = allocated_list[n][1]; // jump block: if we are at head of allocated block, jump.
        }
    }
    printf("Allocation is uncessfull. There is no appropriate space for this block.");
    sem_post(&mutex);
    return (NULL);
}


void *smem_alloc_worst_fit (int size)
{   
    sem_wait(&mutex);
    if(memory_size == 0){printf("Memory is destroy or not initialised. Memory size is zero now");}
     
    printf("size : %d\n", size);
    int block_size;
    
    // block size calculation
    if (size < 8) block_size = 8;
    int k=0;
    while(pow(2,k) < size){k++;}
    //////
    printf("k: %d", k);
    block_size = pow(2,k);
    printf("block size: %d\n",block_size);

    if(block_size > memory_size){
        printf("Requested size bigger than shared memory size");
        return NULL;
    }
    int n = 0;
    
    int max_hole_size = 0;
    int max_n_byte = 0;
    /*
    1.Find a available starting point(not allocated byte).
    2.Start to count empty hole: go 1 by by until finding a allocated point.
    3.If empty hole is bigger than max_hole_size, it will be the new max_hole and max_n_byte keeps the starting address of this hole.
    4.If max hole size is smaller than block_size it means there is no space for that allocation. Otherwise do allocation.
    */
    while( n < memory_size){
        printf("allocated_list[%d][0] = %lld \n", n, allocated_list[n][0] );   
        if(allocated_list[n][0] == 0){ // head adress is empty
               int tracker = n;
               int s_cnt = 0;
               // evaluate size of hole
               while(allocated_list[tracker][0] == 0 ){ // continue until the find a allocated block
                   if(allocated_list[tracker][0] == 0){
                       tracker += 1; // track the adress. tracker keeps x'th bytes from the starting point(ptr).
                       s_cnt +=1; // hole size. it starts from zero and continue to count until encountering an allocated block.
                   }
               }
               // after encounter the allocated block
               if(s_cnt > max_hole_size){
                   max_hole_size = s_cnt; // get max_hole_size.
                   max_n_byte = n; // get starting address of max_hole size.
               }
               n = tracker; // Get the head of an allocated block.
        }
        else{
        n = allocated_list[n][1]; // jump block: if we are at head of allocated block, jump.
        }
    }

    //after 1 tracing the memory area.
    if(max_hole_size < block_size) // there is no space for new allocation
    printf("There is no space for this allocation.");
    else{ // do allocation
         allocated_list[max_n_byte][0] = max_n_byte;
         allocated_list[max_n_byte][1] = max_n_byte+ block_size;
         printf("Allocation is successfull\n");
         sem_post(&mutex);
         return ptr +max_n_byte ; // allocation sucessfull return.
    }

    printf("Allocation is uncessfull. There is no appropriate space for this block.");
    sem_post(&mutex);
    return (NULL);
}


void smem_free (void *p)
{   
    sem_wait(&mutex);
    int n = p-ptr; // get the x'th byte which will be dealloceted.
    allocated_list[n][0]= 0; // go to that byte and freed the head of allocated block.
    allocated_list[n][1] =0; // go to ending point  of this block and freed the ending point.
    p = NULL; // make the pointer null because we don't want that pointer to reach this space anymore.
    sem_post(&mutex);
}

int smem_close()
{
    //sem_post(&mutex)
    munmap(ptr,memory_size);
    return (0); 
}
