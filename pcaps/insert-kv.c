#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <hiredis/hiredis.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <arpa/inet.h>
#define BUFSIZE 1024
#define PROCESSED_FILES "PROCESSED_FILES"

void read_from_stdin(char*filename, redisContext *ctx )
{
    char *buf;
    int i,j;
    int ts;
    struct in_addr addr;
    int ip_src, ip_dst,proto;
    char *ptr;
    buf = calloc(BUFSIZE,1);
    char *token;
    if (!buf) {
        fprintf(stderr,"ERROR, could not allocate memory, abort");
        abort();
    }

    while( fgets(buf, BUFSIZE , stdin) ) {
        //printf("%s",buf);
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
                    break;
                case 2:
                    if (inet_pton(AF_INET, token, &addr) < 0){
                        // Something went wrong
                        ip_src = 0;
                    }else{
                        ip_src = ntohl(addr.s_addr);
                        }
                    break;

                case 3:
                    if (inet_pton(AF_INET, token, &addr) < 0){
                        // Something went wrong
                        ip_dst = 0;
                    }else{
                        ip_dst = ntohl(addr.s_addr);
                        }
                    break;
                case 4:
                    proto = atoi(token);
                    break;
                // add here the next fields that are parsed
            }
            token = strtok(NULL, ",");
            i++;
        }
        printf("ts=%u,ip_src=%u, ip_dst=%u,proto=%u\n",ts,ip_src,ip_dst,proto);
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
    redisReply *reply;

    redis_server = calloc(BUFSIZE,1);
    assert(redis_server);

    filename = calloc(BUFSIZE,1);
    assert(filename);

    while ((opt = getopt(argc, argv, "hs:p:f:")) != -1) {
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
    // Check if the filewas already porcessed
    reply = redisCommand(ctx, "SISMEMBER %s %s", PROCESSED_FILES, filename);
    if (reply) {
       if (reply->integer == 1) {
            printf("[INFO] The filename %s was already processed, skip it\n", filename);
            return EXIT_FAILURE;
       }
       freeReplyObject(reply);
    }
    // Insert the filename in processed list
    reply = redisCommand(ctx, "SADD %s %s", PROCESSED_FILES, filename);
    if (reply){
        read_from_stdin(filename,ctx);
        freeReplyObject(reply);
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}
