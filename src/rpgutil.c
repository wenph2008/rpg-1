#include "common.h"
#include "rpg.h"
extern FILE *dfp;

void print_cur_time()
{
    struct timeval now;
    struct timezone tz;
    struct tm *tm;

    gettimeofday(&now, &tz);
    tm = localtime(&now.tv_sec);
    fprintf(stdout, "%.2d:%.2d:%.2d.%.6ld \n", tm->tm_hour, tm->tm_min, tm->tm_sec, now.tv_usec);
}

void config_defaults(config_t * set)
{
//   set->interval = DEFAULT_INTERVAL;
//   set->highskewslop = DEFAULT_HIGHSKEWSLOP;
//   set->lowskewslop = DEFAULT_LOWSKEWSLOP;
//   set->out_of_range = DEFAULT_OUT_OF_RANGE;
//   set->snmp_ver = DEFAULT_SNMP_VER;
//   set->snmp_port = DEFAULT_SNMP_PORT;
//   set->threads = DEFAULT_THREADS;
   strncpy(set->dbhost, DEFAULT_DB_HOST, sizeof(set->dbhost));
   strncpy(set->dbdb, DEFAULT_DB_DB, sizeof(set->dbdb));
   strncpy(set->dbuser, DEFAULT_DB_USER, sizeof(set->dbuser));
   strncpy(set->dbpass, DEFAULT_DB_PASS, sizeof(set->dbpass));
   set->dboff = FALSE;
//   set->withzeros = FALSE;
   set->verbose = OFF;
//   strncpy(config_paths[0], CONFIG_PATH_1, sizeof(config_paths[0]));
//   snprintf(config_paths[1], sizeof(config_paths[1]), "%s/etc/", RTG_HOME);
//   strncpy(config_paths[2], CONFIG_PATH_2, sizeof(config_paths[1]));
   return;
}

void convert_dot2line()
{
    while()
    {

    }
}
