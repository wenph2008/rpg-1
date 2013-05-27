#include "common.h"
#include "rpg.h"

/* Yes.  Globals. */
target_t *Targets = NULL;
target_t *current = NULL;
MYSQL mysql;
/* dfp is a debug file pointer.  Points to stderr unless debug=level is set */
FILE *dfp = NULL;

int main (int argc, char **argv)
{
    target_session_t target_session;
    crew_t crew;
    //int count = 0;

    dfp = stderr;

    /* Set default environment */
    config_defaults(&set);


    print_cur_time();
    pthread_mutex_init(&(target_session.mutex), NULL);
    pthread_cond_init(&(target_session.done), NULL);
    pthread_cond_init(&(target_session.go), NULL);

    /* Attempt to connect to the MySQL Database */
    if (!(set.dboff))
    {
        if (rpg_dbconnect(set.dbdb, &mysql) < 0)
        {
            fprintf(stderr, "** Database error - check configuration.\n");
            exit(-1);
        }
        if (!mysql_ping(&mysql))
        {
            if (set.verbose >= LOW)
                printf("connected.\n");
        }
        else
        {
            printf("server not responding.\n");
            exit(-1);
        }
    }

    snmp_oid_initialize();
    target_session.file = "/home/apple/workplace/rpg/src/config.txt";

    //printf("targets count = %d\n",count);
    //print_all_hash_entry();

    /*
     *this is a synchronous thread to loop the targets
     */
    /*
    pthread_t tid;
    if (pthread_create(&tid, NULL, snmp_synchronous, (void *)(&target_session)) != 0)//use arg deliever the signal
    {
        printf("pthread_create error\n");
    }
    */

    /*
     *this is a asynchronous thread to loop the targets
     */

    pthread_t tid;
    if (pthread_create(&tid, NULL, snmp_asynchronous, (void *)(&target_session)) != 0)//use arg deliever the signal
    {
        printf("pthread_create error\n");
    }
    sleep(1);
    pthread_t tid2;
    if (pthread_create(&tid2, NULL, hash_target_file, (void *)(&target_session)) != 0)
    {
        printf("pthread_create error\n");
    }
    pthread_join(tid,NULL);
    pthread_join(tid2,NULL);


    /*
     *this is a synchronous mutil-thread to loop the targets
     */
    /*
        printf("Initializing threads (%d).\n", 10);
        pthread_mutex_init(&(crew.mutex), NULL);
        pthread_cond_init(&(crew.done), NULL);
        pthread_cond_init(&(crew.go), NULL);
        crew.work_count = 0;

        for (int i = 0; i < 3; i++)
        {
            crew.member[i].index = i;
            crew.member[i].crew = &crew;
            if (pthread_create(&(crew.member[i].thread), NULL, poller, (void *) &(crew.member[i])) != 0)
                printf("pthread_create error\n");
        }
        sleep(1);
        pthread_t tid2;
        if (pthread_create(&tid2, NULL, hash_target_file2, (void *)(&crew)) != 0)
        {
            printf("pthread_create error\n");
        }
        pthread_join(tid2,NULL);

        for (int j = 0; j < 10; j++)
        {
            pthread_join(crew.member[j].thread,NULL);
        }
    */


    if(del_all_hash_entry() != 0)
        printf("delete hash entry err\n");
    else
        printf("sucess delete the Targets list\n");
    print_cur_time();

    return 0;
}
