


int smem_init(int segmentsize); 
int smem_remove(); 

int smem_open(); 
void *smem_alloc (int size);
void *smem_alloc_worst_fit (int size);
void smem_free(void *p);
int smem_close(); 



    
    
