using namespace std;
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <hiredis/hiredis.h>
#include <stdint.h>
#include <arpa/inet.h>
#define BUFSIZE 1024
#include <cstring>
#include <stdint.h>
#include <cstdlib>
#include <map>


map <uint32_t, uint32_t> ip_time_line;

void read_from_stdin(char*filename, redisContext *ctx )
{
    char *buf;
    int i,j;
    int ts;
    struct in_addr addr;
    uint32_t ip_src, ip_dst,proto;
    char *ptr;
    redisReply *reply;
    buf = (char*)calloc(BUFSIZE,1);
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
                        for (j=0; j<(int)strlen(ptr); ++j){
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

            ip_time_line[ip_src]=ts;
            token = strtok(NULL, ",");
            i++;
        }
    }
    free(buf);
}

int main() {
    read_from_stdin(NULL, NULL);
     for (auto it = ip_time_line.begin(); it != ip_time_line.end(); ++it) {
        std::cout << "IP: " << it->first << ", Timestamp: " << it->second << std::endl;
    }

    return 0;
}

