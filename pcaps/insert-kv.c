#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <hiredis/hiredis.h>
#include <string.h>
#include <stdint.h>
#define BUFSIZE 1024

void read_from_stdin(char*filename )
{
    char *buf;
    int i,j;
    int ts;

    char *ptr;
    buf = calloc(BUFSIZE,1);
    char *token;
    if (!buf) {
        fprintf(stderr,"ERROR, could not allocate memory, abort");
        abort();
    }

    while( fgets(buf, BUFSIZE , stdin) ) {
        printf("%s",buf);
        i=1;
        token = strtok(buf, ",");
        while(token != NULL)
        {
            switch (i) {
                case 1:
                        ptr = token;
                        for (j=0; j<strlen(ptr); ++j){
                            if (ptr[j]=='.'){
                                ptr[j]=0;
                            }
                            ts = atoi(ptr);
                        }
                        printf("%d: \n", ts);
                    break;
                // add here the next fields that are parsed
                case 2:
                    break;
            }

            printf("%d %s\n",i,token);
            token = strtok(NULL, ",");
            i++;
        }
    }
    free(buf);
}

int main(int argc, char* argv[]){

    read_from_stdin(NULL);

return EXIT_SUCCESS;
}
