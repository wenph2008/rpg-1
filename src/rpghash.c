/****************************************************************************
   Program:     $Id: rpghash.c,v 0.0.1 2013/04/29
   Author:      $Author: wenph@bupt $
   Date:        $Date: 2013/04/29
   Description: RPG target hash table routines
****************************************************************************/

#include "common.h"
#include "rpg.h"



extern target_t *Targets;
extern target_t *current;

/* Add an entry to hash if it is unique, otherwise free() it */
int add_hash_entry(target_t *new)
{
    target_t *p = NULL;
    p = new;
    p->next = Targets;
    Targets = p;
    return 1;
}

int del_all_hash_entry(void)
{
    target_t *p = NULL;
    if(Targets == NULL)
    {
        printf("There are no Targets to delete\n");
        return -1;
    }
    else
    {
        while(Targets != NULL)
        {
            p = Targets;
            Targets = Targets->next;
            free(p);
        }
    }
    return 0;
}

int print_all_hash_entry(void)
{
    target_t *p = Targets;
    while(p != NULL)
    {
        printf("host = %s,community = %s\n",p->host,p->community);
        p = p->next;
    }
    return 0;
}

/* Read a target file into a target_t hash table.  Our hash algorithm
   roughly randomizes targets, easing SNMP load on end devices during
   polling.  hash_target_file() can be called again to update the target
   hash.  If hash_target_file() finds new target entries in the file, it
   adds them to the hash.  If hash_target_file() finds entries in hash
   but not in file, it removes said entries from hash.  */
void *hash_target_file(void *arg)
{
    target_session_t *target_session = (target_session_t *)arg;
    char *file = target_session->file;
    FILE *fp;
    target_t *new = NULL;
    char buffer[BUFSIZE];
    int entries = 0;
    int count = 1000;

    printf("start hash the pack:");
    print_cur_time();
    /* Open the target file */
    if ((fp = fopen(file, "r")) == NULL)
    {
        fprintf(stderr, "\nCould not open file for reading '%s'.\n", file);
        exit (1);
    }

    while (count && !feof(fp))
    {
        fgets(buffer, BUFSIZE, fp);
        if (!feof(fp) && buffer[0] != '#' && buffer[0] != ' ' && buffer[0] != '\n')
        {
            new = (target_t *) malloc(sizeof(target_t));
            if (!new)
            {
                printf("Fatal target malloc error!\n");
                exit(-1);
            }
            sscanf(buffer, "%64s", new->host);
            strcpy(new->community, "public");
            new->next = NULL;
            entries += add_hash_entry(new);
            count--;
        }

    }
    printf("hash tell you can go\n");
    PT_COND_SIGNAL(&(target_session->go));
    //lock(mutex);cond_wait();unlock();

    fclose(fp);
    printf("Successfully read [%d] new targets\n", entries);

    printf("end hash the pack:");
    print_cur_time();

}


void *hash_target_file2(void *arg)
{
    crew_t *crew = (crew_t *)arg;
    char *file = "/home/apple/workplace/rpg/src/config.txt";
    FILE *fp;
    target_t *new = NULL;
    char buffer[BUFSIZE];
    int entries = 0;
    int count = 1000;

    printf("start hash the pack:");
    print_cur_time();
    /* Open the target file */
    if ((fp = fopen(file, "r")) == NULL)
    {
        fprintf(stderr, "\nCould not open file for reading '%s'.\n", file);
        exit (1);
    }

    while (count && !feof(fp))
    {
        fgets(buffer, BUFSIZE, fp);
        if (!feof(fp) && buffer[0] != '#' && buffer[0] != ' ' && buffer[0] != '\n')
        {
            new = (target_t *) malloc(sizeof(target_t));
            if (!new)
            {
                printf("Fatal target malloc error!\n");
                exit(-1);
            }
            sscanf(buffer, "%64s", new->host);
            strcpy(new->community, "public");
            new->next = NULL;
            entries += add_hash_entry(new);
            count--;
        }

    }
    printf("hash tell you can go\n");
    current = Targets;
    //PT_COND_BROAD(&(crew->go));
    //lock(mutex);cond_wait();unlock();

    fclose(fp);
    printf("Successfully read [%d] new targets\n", entries);

    printf("end hash the pack:");
    print_cur_time();
    PT_COND_BROAD(&(crew->go));

}
