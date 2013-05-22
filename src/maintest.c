#include "common.h"

main()
{
    int count = 0;
    count = hash_target_file("/home/apple/workplace/rpg-0.0.1/src/config.txt");
    print_all_hash_entry();
    if(del_all_hash_entry() != 0)
	printf("err\n");
    else
	printf("sucess delete the Targets list\n");
    printf("count = %d\n",count);
}
