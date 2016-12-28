typedef struct task_node {
    void *(*function)(void *arg);
    void *arg;
    struct task_node *next;
}task;

typedef struct {
    pthread_mutex_t queue_lock;
    task *queue_head;
    pthread_t *thread_id;
    int max_thread_num;
    int waiting_task_num;
}thread_pool;

int add_task (void *(*function) (void *arg), void *arg);
int pool_join(void);
void pool_init(int max_thread_num);
void pool_start(void);
void *sub_thread (void *arg);

static thread_pool *pool = NULL;
pthread_mutex_t target_list_lock;

void
pool_init (int max_thread_num)
{
    pool = (thread_pool *)malloc(sizeof (thread_pool));
    pthread_mutex_init(&(pool->queue_lock), NULL);
    pool->queue_head = NULL;
    pool->max_thread_num = max_thread_num;
    pool->waiting_task_num = 0;
    pool->thread_id = (pthread_t *)malloc(max_thread_num * sizeof (pthread_t));
}

void
pool_start()
{
    int i = 0;
    for (i = 0; i < pool->max_thread_num; i++)
        pthread_create (&(pool->thread_id[i]), NULL, sub_thread,NULL);
}

int
add_task (void *(*function) (void *arg), void *arg)
{
    task *new_task = (task *) malloc (sizeof (task));
    new_task->function = function;
    new_task->arg = arg;
    new_task->next = NULL;

    pthread_mutex_lock (&(pool->queue_lock));
    task *queue_curosr = pool->queue_head;
    if (queue_curosr != NULL){
        while (queue_curosr->next != NULL)
            queue_curosr = queue_curosr->next;
        queue_curosr->next = new_task;
    }else{
        pool->queue_head = new_task;
    }

    pool->waiting_task_num++;
    pthread_mutex_unlock (&(pool->queue_lock));
    return 0;
}

int
pool_join ()
{

    int i;
    for (i = 0; i < pool->max_thread_num; i++)
        pthread_join (pool->thread_id[i], NULL);
    free (pool->thread_id);

    task *queue_member = NULL;
    while (pool->queue_head != NULL){
        queue_member = pool->queue_head;
        pool->queue_head = pool->queue_head->next;
        free (queue_member);
    }

    pthread_mutex_destroy(&(pool->queue_lock));
    free (pool);

    pool=NULL;
    return 0;
}

void *
sub_thread(void *arg)
{
    /* printf ("Thread 0x%x is starting...\n", pthread_self ()); */
    while (1){
        pthread_mutex_lock(&(pool->queue_lock));

        if(pool->waiting_task_num == 0){
            pthread_mutex_unlock(&(pool->queue_lock));
            /* printf ("Thread 0x%x is exiting...\n", pthread_self ()); */
            pthread_exit(NULL);
        }

        pool->waiting_task_num--;
        task *new_task= pool->queue_head;
        pool->queue_head = new_task->next;
        pthread_mutex_unlock(&(pool->queue_lock));

        /* printf("New task on thread 0x%x\n", pthread_self ()); */
        (*(new_task->function))(new_task->arg);
        free(new_task);
        new_task = NULL;
    }
}
