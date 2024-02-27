#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <hiredis/hiredis.h>
#include <string.h>
#include <assert.h>
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

void usage(void)
{
    printf("Read output of tshark and pipe in kvrocks\n");
}

int main(int argc, char* argv[]){

    int opt;
    char* redis_server;
    char* filename;
    redisContext* ctx;

    int redis_server_port;
    redis_server = calloc(BUFSIZE,1);
    assert(redis_server);

    filename = calloc(BUFSIZE,1);
    assert(filename);

    while ((opt = getopt(argc, argv, "b:hs:p:q:")) != -1) {
        switch (opt) {
            case 's':
                strncpy(redis_server, optarg, BUFSIZE);
                break;
            case 'p':
                redis_server_port = atoi(optarg);
                break;
            case 'f':
                strncpy(filename, optarg, BUFSIZE);
                break;
            case 'h':
                usage();
                return EXIT_SUCCESS;
            default: /* '?' */
                fprintf(stderr, "[ERROR] Invalid command line was specified\n");
        }
    }

    // Connect to redis
    ctx = redisConnect(redis_server, redis_server_port);

    if (ctx != NULL && ctx->err) {
        fprintf(stderr,"[ERROR] Could not connect to redis. %s.\n", ctx->errstr);
        return EXIT_FAILURE;
    }


    //read_from_stdin(NULL);
return EXIT_SUCCESS;
}
