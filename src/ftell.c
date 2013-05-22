#include <stdio.h>
#include <stdlib.h>
#define BUFSIZE 512

int hash_target_file(char *file)
{
    FILE *fp;
    char buffer[BUFSIZE];
    long offset = 0;

    while(1)
    {
        fp = fopen(file,"r");
        fseek(fp,offset,SEEK_SET);
        int count = 3;
        while(count && !feof(fp))
        {
            //printf("offset = %ld\n",ftell(fp));
            if(fgets(buffer,BUFSIZE,fp) != NULL)
            {
                printf("%s",buffer);
                count--;
            }
        }
        if(feof(fp))//the end of file
        {
            fclose(fp);
            offset = 0;
            printf("end of the file\n");
            //wait for signal
        }
        else//the count == 0
        {
            offset = ftell(fp);
            fclose(fp);
            printf("count is zero\n");
            //wait for signal
        }

    }
}

int main()
{
    hash_target_file("/home/apple/workplace/config.txt");
}
