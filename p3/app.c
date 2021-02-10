
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "smemlib.h"

int main()
{
    int i, ret; 
    char *p;
    
    /*
    char array[5] = {0,1,2,3,4};
    char* p1 = array[0];
    char* p2 = array[4];
    printf("diff byte: %ld", p2-p1);  	
    */

    ret = smem_open(); 
    if (ret == -1)
	exit (1); 

    p = smem_alloc_worst_fit (10*sizeof(char)); // allocate space to hold 1024 characters
    for (i = 0; i < 10; ++i){

    if(i % 2 == 0){
	p[i] = 'a'; // init all chars to ‘a’
    }
    else p[i] = 'b';
    printf("i: %d, %c\n",i,p[i]);
}
    smem_free (p);

    smem_close(); 
    
    return (0); 
}
