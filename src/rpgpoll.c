/****************************************************************************
   Program:     $Id: rpgpoll.c,v 0.0.1 2013/04/25 $
   Author:      $Author: wenph@bupt $
   Date:        $Date: 2013/04/25 $
   Description: RPG SNMP get dumps to MongoDB database
****************************************************************************/

#include "common.h"
#include "rtg.h"

/* Yes.  Globals. */
char *target_file = NULL;
target_t *current = NULL;

//MYSQL mysql;

/* Main rpgpoll */
int main(int argc, char *argv[]) {
    char *conf_file = NULL;
    char errstr[BUFSIZE];
    int ch, i;

    /* Read configuration file to establish local environment */
    if (conf_file) {
      if ((read_rtg_config(conf_file, &set)) < 0) {
         printf("Could not read config file: %s\n", conf_file);
         exit(-1);
      }
    }
    //else{}

    init_snmp("RPG");

    /* Attempt to connect to the MySQL Database */
	if (rtg_dbconnect(set.dbdb, &mysql) < 0) {
	    fprintf(stderr, "** Database error - check configuration.\n");
	    exit(-1);
	}
	if (!mysql_ping(&mysql)) {
	    if (set.verbose >= LOW)
		printf("connected.\n");
	} else {
	    printf("server not responding.\n");
	    exit(-1);
	}
	
	printf("\nStarting threads.\n");

    for (i = 0; i < set.threads; i++) {
	crew.member[i].index = i;
	crew.member[i].crew = &crew;
	if (pthread_create(&(crew.member[i].thread), NULL, poller, (void *) &(crew.member[i])) != 0)
	    printf("pthread_create error\n");
    }
    if (pthread_create(&sig_thread, NULL, sig_handler, (void *) &(signal_set)) != 0)
	printf("pthread_create error\n");

    /* give threads time to start up */
    sleep(1);

	printf("RTG Ready.\n");

    /* Loop Forever Polling Target List */
    while (1) {
	lock = TRUE;
	gettimeofday(&now, NULL);
	begin_time = (double) now.tv_usec / 1000000 + now.tv_sec;

	PT_MUTEX_LOCK(&(crew.mutex));
	init_hash_walk();
	current = getNext();
	crew.work_count = entries;
	PT_MUTEX_UNLOCK(&(crew.mutex));
	    
	if (set.verbose >= LOW)
        timestamp("Queue ready, broadcasting thread go condition.");
	PT_COND_BROAD(&(crew.go));
	PT_MUTEX_LOCK(&(crew.mutex));
	    
	while (crew.work_count > 0) {
		PT_COND_WAIT(&(crew.done), &(crew.mutex));
	}
	PT_MUTEX_UNLOCK(&(crew.mutex));

	gettimeofday(&now, NULL);
	lock = FALSE;
	end_time = (double) now.tv_usec / 1000000 + now.tv_sec;
	stats.poll_time = end_time - begin_time;
        stats.round++;
	sleep_time = set.interval - stats.poll_time;

	if (waiting) {
	    if (set.verbose >= HIGH)
		printf("Processing pending SIGHUP.\n");
	    entries = hash_target_file(target_file);
	    waiting = FALSE;
	}
	if (set.verbose >= LOW) {
        snprintf(errstr, sizeof(errstr), "Poll round %d complete.", stats.round);
        timestamp(errstr);
	    print_stats(stats);
    }
	if (sleep_time <= 0)
	    stats.slow++;
	else
	    sleepy(sleep_time);
    } /* while */

    /* Disconnect from the MySQL Database, exit. */
    if (!(set.dboff))
	rtg_dbdisconnect(&mysql);
    exit(0);
}


/* Signal Handler.  USR1 increases verbosity, USR2 decreases verbosity. 
   HUP re-reads target list */
void *sig_handler(void *arg)
{
    sigset_t *signal_set = (sigset_t *) arg;
    int sig_number;

    while (1) {
	sigwait(signal_set, &sig_number);
	switch (sig_number) {
            case SIGHUP:
                if(lock) {
                    waiting = TRUE;
                }
                else {
                    entries = hash_target_file(target_file);
                    waiting = FALSE;
                }
                break;
            case SIGUSR1:
                set.verbose++;
                break;
            case SIGUSR2:
                set.verbose--;
                break;
            case SIGTERM:
            case SIGINT:
            case SIGQUIT:
                if (set.verbose >= LOW)
                   printf("Quiting: received signal %d.\n", sig_number);
                rtg_dbdisconnect(&mysql);
                unlink(PIDFILE);
                exit(1);
                break;
        }
   }
}

