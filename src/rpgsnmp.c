/*
 * rpgsnmp.c
 *
 */
#include "common.h"
#include "rpg.h"

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

extern target_t *Targets;
extern target_t *current;
int active_hosts;			/* hosts that we have not completed */
int non_repeaters = 0;
int max_repetitions = 2;
/*
 * a list of variables to query for
 */
struct my_oid
{
    const char *Name;
    oid Oid[MAX_OID_LEN];
    int OidLen;
} oids[] =
{
    { "ifIndex.1" },
    /*
    { "ifDescr.1" },
    { "ifType.1" },
    { "ifSpeed.1" },
    { "ifOperStatus.1" },
    { "ifInOctets.1" },
    { "ifInUcastPkts.1" },
    { "ifInErrors.1" },
    { "ifOutOctets.1" },
    { "ifOutUcastPkts.1" },
    { "ifOutErrors.1" },
    */
    { NULL }
};

netsnmp_pdu *create_pdu_preGetVersion()
{
    struct snmp_pdu *pdu;
    oid id_oid[MAX_OID_LEN];
    size_t id_len = MAX_OID_LEN;

    pdu = snmp_pdu_create (SNMP_MSG_GET);
    snmp_parse_oid ("ifNumber.0", id_oid, &id_len);
    snmp_add_null_var (pdu, id_oid, id_len);
    snmp_parse_oid ("sysDescr.0", id_oid, &id_len);
    snmp_add_null_var (pdu, id_oid, id_len);

    return pdu;
}

netsnmp_pdu *create_bulkget_pdu(int non_Repeaters, int max_Repetitions)
{
    struct snmp_pdu *pdu;
    oid id_oid[MAX_OID_LEN];
    size_t id_len = MAX_OID_LEN;

    pdu = snmp_pdu_create (SNMP_MSG_GETBULK);
    pdu->non_repeaters = non_Repeaters;
    pdu->max_repetitions = max_Repetitions;     //fill the packet
    //ifTable(2)
    snmp_parse_oid ("ifIndex", id_oid, &id_len);
    snmp_add_null_var (pdu, id_oid, id_len);
    snmp_parse_oid ("ifDescr", id_oid, &id_len);
    snmp_add_null_var (pdu, id_oid, id_len);
    snmp_parse_oid ("ifType", id_oid, &id_len);
    snmp_add_null_var (pdu, id_oid, id_len);
    snmp_parse_oid ("ifSpeed", id_oid, &id_len);
    snmp_add_null_var (pdu, id_oid, id_len);
    snmp_parse_oid ("ifOperStatus", id_oid, &id_len);
    snmp_add_null_var (pdu, id_oid, id_len);
    snmp_parse_oid ("ifInOctets", id_oid, &id_len);
    snmp_add_null_var (pdu, id_oid, id_len);
    snmp_parse_oid ("ifInUcastPkts", id_oid, &id_len);
    snmp_add_null_var (pdu, id_oid, id_len);
    snmp_parse_oid ("ifInErrors", id_oid, &id_len);
    snmp_add_null_var (pdu, id_oid, id_len);
    snmp_parse_oid ("ifOutOctets", id_oid, &id_len);
    snmp_add_null_var (pdu, id_oid, id_len);
    snmp_parse_oid ("ifOutUcastPkts", id_oid, &id_len);
    snmp_add_null_var (pdu, id_oid, id_len);
    snmp_parse_oid ("ifOutErrors", id_oid, &id_len);
    snmp_add_null_var (pdu, id_oid, id_len);
    /*
    //ifXTable(1)
    snmp_parse_oid ("ifName", id_oid, &id_len);
    snmp_add_null_var (pdu, id_oid, id_len);
    snmp_parse_oid ("ifHCInOctets", id_oid, &id_len);
    snmp_add_null_var (pdu, id_oid, id_len);
    snmp_parse_oid ("ifHCInUcastPkts", id_oid, &id_len);
    snmp_add_null_var (pdu, id_oid, id_len);
    snmp_parse_oid ("ifHCOutOctets", id_oid, &id_len);
    snmp_add_null_var (pdu, id_oid, id_len);
    snmp_parse_oid ("ifHCOutUcastPkts", id_oid, &id_len);
    snmp_add_null_var (pdu, id_oid, id_len);
    */
    return pdu;
}

/*
 * initialize
 */
void snmp_oid_initialize ()
{
    struct my_oid *op = oids;

    // initialize library
    init_snmp("asynchapp");

    // parse the oids
    while (op->Name)
    {
        op->OidLen = sizeof(op->Oid)/sizeof(op->Oid[0]);
        if (!snmp_parse_oid(op->Name, op->Oid, &op->OidLen))
        {
            snmp_perror("read_objid");
            exit(1);
        }
        op++;
    }
}

/*
 * simple printing of returned data
 */
int print_result (int status, struct snmp_session *sp, struct snmp_pdu *pdu)
{
    char buf[1024];
    struct variable_list *vp;
    int ix;

    switch (status)
    {
    case STAT_SUCCESS:
        vp = pdu->variables;
        if (pdu->errstat == SNMP_ERR_NOERROR)
        {
            while (vp)
            {
                snprint_variable(buf, sizeof(buf), vp->name, vp->name_length, vp);
                fprintf(stdout, "%s: %s\n", sp->peername, buf);
                vp = vp->next_variable;
            }
        }
        else
        {
            for (ix = 1; vp && ix != pdu->errindex; vp = vp->next_variable, ix++)
                ;
            if (vp) snprint_objid(buf, sizeof(buf), vp->name, vp->name_length);
            else strcpy(buf, "(none)");
            fprintf(stdout, "%s: %s: %s\n",
                    sp->peername, buf, snmp_errstring(pdu->errstat));
        }
        return 1;
    case STAT_TIMEOUT:
        fprintf(stdout, "%s: Timeout\n", sp->peername);
        return 1;
    case STAT_ERROR:
        snmp_perror(sp->peername);
        return 1;
    }
    return 1;
}

/*
 * simple synchronous loop
 */
void *snmp_synchronous (void *arg)
{
    target_session_t *target_session = (target_session_t *)arg;
    target_t *hp = NULL;

    printf("snmp_synchronous is waiting\n");
    PT_MUTEX_LOCK(&(target_session->mutex));
    PT_COND_WAIT(&(target_session->go), &(target_session->mutex));
    printf("snmp_synchronous can go\n");

    hp = Targets;
    if(hp == NULL)
        printf("syschronous err.\n");
    printf("start of send pack:");
    print_cur_time();

    for (; hp != NULL; hp = hp->next)
    {
        struct snmp_session ss, *sp;
        struct my_oid *op;
        struct snmp_pdu *req, *resp;
        int status;

        snmp_sess_init(&ss);			/* initialize session */
        ss.version = SNMP_VERSION_2c;
        ss.peername = hp->host;
        ss.community = hp->community;
        ss.community_len = strlen(ss.community);
        if (!(sp = snmp_open(&ss)))
        {
            snmp_perror("snmp_open");
            continue;
        }
        req = snmp_pdu_create(SNMP_MSG_GET);
        for (op = oids; op->Name; op++)
        {
            snmp_add_null_var(req, op->Oid, op->OidLen);
        }
        status = snmp_synch_response(sp, req, &resp);
        if (!print_result(status, sp, resp))
            break;
        snmp_free_pdu(resp);
        snmp_close(sp);
    }
}

/*
 * response handler
 */
int asynch_response(int operation, struct snmp_session *sp, int reqid,
                    struct snmp_pdu *pdu, void *magic)
{
    //struct snmp_pdu *req;

    if (operation == NETSNMP_CALLBACK_OP_RECEIVED_MESSAGE)
        print_result(STAT_SUCCESS, /*host->sess*/sp, pdu);
    else
        print_result(STAT_TIMEOUT, /*host->sess*/sp, pdu);

    /* something went wrong (or end of variables)
     * this host not active any more
     */
    active_hosts--;
    return 1;
}

void *snmp_asynchronous(void *arg)
{
    //sleep(10);
    target_session_t *target_session = (target_session_t *)arg;
    target_t *hp = NULL;

    printf("snmp_asynchronous is waiting\n");
    PT_MUTEX_LOCK(&(target_session->mutex));
    PT_COND_WAIT(&(target_session->go), &(target_session->mutex));
    printf("snmp_asynchronous can go\n");

    hp = Targets;
    if(hp == NULL)
        printf("asyschronous err.\n");
    printf("start of send pack:");
    print_cur_time();

    for (; hp != NULL; hp = hp->next)
    {
        struct my_oid *op = oids;
        struct snmp_session sess, *sp;
        struct snmp_pdu *req;
        snmp_sess_init(&sess);			/* initialize session */
        sess.version = SNMP_VERSION_2c;
        sess.peername = hp->host;
        sess.community = hp->community;
        sess.community_len = strlen(sess.community);
        sess.callback = asynch_response;		/* default callback */
        sess.callback_magic = NULL;
        if (!(sp = snmp_open(&sess)))         //循环这个函数把session加入sess_list链表中
        {
            snmp_perror("snmp_open");
            continue;
        }

        req = create_bulkget_pdu(non_repeaters, max_repetitions);
        if (snmp_send(sp, req))
            active_hosts++;
        else
        {
            snmp_perror("snmp_send");
            snmp_free_pdu(req);
        }
    }
    PT_MUTEX_UNLOCK(&(target_session->mutex));
    //PT_COND_SIGNAL(done);

    printf("end of send pack:");
    print_cur_time();
    /* loop while any active hosts */
    printf("start of recv pack:");
    print_cur_time();
    while (active_hosts)
    {
        int fds = 0, block = 1;
        fd_set fdset;
        struct timeval timeout;

        FD_ZERO(&fdset);
        snmp_select_info(&fds, &fdset, &timeout, &block);//for循环sess_list把sockfd加入fdset中
        fds = select(fds, &fdset, NULL, NULL, block ? NULL : &timeout);
        if (fds < 0)
        {
            perror("select failed");
            exit(1);
        }
        if (fds)
            snmp_read(&fdset);//for循环sess_list用FD_ISSET检查该sockfd是否在fdset集合中。是就读取包，不是继续扫描。
        else
            snmp_timeout();
    }

    /* cleanup */
    if(snmp_close_sessions() == 1)
    {
        printf("delete the session sucess\n");
    }
    printf("end of recv pack:");
    print_cur_time();
}

void *poller(void *thread_args)
{
    worker_t *worker = (worker_t *) thread_args;
    crew_t *crew = worker->crew;

    struct snmp_session ss, *sp;
    struct my_oid *op;
    struct snmp_pdu *req, *resp;
    struct variable_list *vars;
    int status;

    printf("Thread [%d] starting.\n", worker->index);


    while (1)
    {
        printf("Thread [%d] locking (wait on work)\n", worker->index);

        PT_MUTEX_LOCK(&crew->mutex);

        while (current == NULL)
        {
            print_cur_time();
            PT_COND_WAIT(&crew->go, &crew->mutex);
        }
        printf("Thread [%d] done waiting, received go (work cnt: %d)\n", worker->index, crew->work_count);

        if (current != NULL)
        {
            //if (set.verbose >= HIGH)
            //printf("Thread [%d] processing %s %s (%d work units remain in queue)\n", worker->index, current->host, current->objoid, crew->work_count);
            snmp_sess_init(&ss);
            ss.version = SNMP_VERSION_2c;
            ss.peername = current->host;
            ss.community = current->community;
            ss.community_len = strlen(ss.community);
            if (!(sp = snmp_sess_open(&ss)))
            {
                snmp_sess_perror("snmp_open", &ss);
                //printf("snmp_sess_error\n");
                continue;
            }
            req = snmp_pdu_create(SNMP_MSG_GET);
            current = current->next;//在任务列表被锁住到情况下把current指针指向下一个任务，高！
            //if (set.verbose >= DEVELOP)
            printf("Thread [%d] unlocking (done grabbing current):", worker->index);
            printf("%s\n",current->host);
            PT_MUTEX_UNLOCK(&crew->mutex);
            for (op = oids; op->Name; op++)
            {
                snmp_add_null_var(req, op->Oid, op->OidLen);
            }
            status = snmp_sess_synch_response(sp, req, &resp);
            printf("Thread [%d] done poller:", worker->index);
            /*
            if (!print_result(status, sp, resp))
                break;
            */
            if(!status)
            {
                for (vars = resp->variables; vars; vars = vars->next_variable)
                {
                    /*这里处理收到的包，把包里的内容制成配置文件*/
                    fprint_value (stdout, vars->name, vars->name_length, vars);
                    //fputs("\n",fp);
                }
            }
            //PT_MUTEX_UNLOCK(&crew->mutex);
            snmp_free_pdu(resp);
            snmp_sess_close(sp);

        }
        else
        {
            exit(0);
        }
    }				/* while(1) */
}
