my_rpgpoll : rpgutil.o my_rpgpoll.o rpghash.o rpgsnmp.o
	gcc -std=c99 -g -o $@ $^ -L /usr/local/net-snmp/lib/ -lnetsnmp -lpthread -lmysqlclient
my_rpgpoll.o : my_rpgpoll.c rpgutil.c
	gcc -std=c99 -g -c $<
rpgutil.o : rpgutil.c
	gcc -g -c $<
rpghash.o : rpghash.c rpgutil.c
	gcc -g -c $^
rpgsnmp.o : rpgsnmp.c rpgutil.c
	gcc -std=c99 -g -c $^ -I /usr/local/net-snmp/include/
rpgmysql.o : rpmysql.c
	gcc -std=c99 -g -c $< -I /usr/include/mysql/
clean:
	rm *.o my_rpgpoll
