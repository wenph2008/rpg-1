/****************************************************************************
   Program:     $Id: rpg.h,v 0.1 2013/04/25 $
   Author:      $Author: wenph@bupt $
   Date:        $Date: 2013/04/25 $
   Description: RPG headers
****************************************************************************/

#ifndef FALSE
# define FALSE 0
#endif
#ifndef TRUE
# define TRUE !FALSE
#endif

/* Constants */
#define MAX_THREADS 10
#define BUFSIZE 512

/* pthread error messages */
#define PML_ERR "pthread_mutex_lock error\n"
#define PMU_ERR "pthread_mutex_unlock error\n"
#define PCW_ERR "pthread_cond_wait error\n"
#define PCB_ERR "pthread_cond_broadcast error\n"
#define PCS_ERR "pthread_cond_signal error\n"

/* pthread macros */
#define PT_MUTEX_LOCK(x) if (pthread_mutex_lock(x) != 0) printf(PML_ERR);
#define PT_MUTEX_UNLOCK(x) if (pthread_mutex_unlock(x) != 0) printf(PMU_ERR);
#define PT_COND_WAIT(x,y) if (pthread_cond_wait(x, y) != 0) printf(PCW_ERR);
#define PT_COND_BROAD(x) if (pthread_cond_broadcast(x) != 0) printf(PCB_ERR);
#define PT_COND_SIGNAL(x) if (pthread_cond_signal(x) != 0) printf(PCS_ERR);

typedef struct target_struct
{
    char host[64];
//    char objoid[128];
//    unsigned short bits;
    char community[64];
//    char table[64];
//    struct target_struct *prev;
    struct target_struct *next;
} target_t;

typedef struct target_session_struct
{
    char *file;
    pthread_mutex_t mutex;
    pthread_cond_t done;
    pthread_cond_t go;
} target_session_t;

/* Typedefs */
typedef struct worker_struct {
    int index;
    pthread_t thread;
    struct crew_struct *crew;
} worker_t;

typedef struct crew_struct {
    int work_count;
    worker_t member[MAX_THREADS];
    pthread_mutex_t mutex;
    pthread_cond_t done;
    pthread_cond_t go;
} crew_t;

/*Precasts: rtgsnmp.c*/
//void initialize (void);
void snmp_oid_initialize();
//int print_result (int , void *, void *);
//int asynch_response_snmpv1(int , void *, int ,void *, void *);
void *snmp_asynchronous(void *);
void *snmp_synchronous(void *);


/* Precasts: rtgpoll.c */
void *poller(void *);

/* Precasts: rtgmysql.c */
//int db_insert(char *, MYSQL *);
//int rtg_dbconnect(char *, MYSQL *);
//void rtg_dbdisconnect(MYSQL *);

/* Precasts: rtgutil.c */
//int read_rtg_config(char *, config_t *);
//int write_rtg_config(char *, config_t *);
void print_cur_time();

/* Precasts: rtghash.c */
//void init_hash();
//void init_hash_walk();
//target_t *getNext();
//void free_hash();
//unsigned long make_key(const void *);
//void mark_targets(int);
//int delete_targets(int);
//void walk_target_hash();
//void *in_hash(target_t *, target_t *);
//int compare_targets(target_t *, target_t *);
//int del_hash_entry(target_t *);
int print_all_hash_entry(void);
int del_all_hash_entry(void);
int add_hash_entry(target_t *);
void *hash_target_file(void *);
void *hash_target_file2(void *arg);

/* Globals */
//config_t set;
//int lock;
//int waiting;
//char config_paths[CONFIG_PATHS][BUFSIZE];
//hash_t hash;
