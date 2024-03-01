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
#include <fstream>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/map.hpp>

map <uint32_t, uint32_t> ip_time_line;

void read_from_stdin(char*filename )
{
    char *buf;
    int i,j;
    struct in_addr addr;
    uint32_t ts,ip_src, ip_dst,proto;
    char *ptr;
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
            // Count IP addresses
            ++ip_time_line[ip_src];
            token = strtok(NULL, ",");
            i++;
        }
    }
    free(buf);
}


/* FIMXE add clean header and say what data it is
 *format: size_t size of the map  followed by series of uint32_t ip uint32_t frequency
 */
bool store_map(const char* filename){
    ofstream file(filename, std::ios::binary);
    if (file.is_open()) {
        boost::archive::binary_oarchive oa(file);
        oa << ip_time_line;
        cout<<"Serialized map"<<endl;
        file.close();
    } else {
    cerr << "Error: Unable to open file " << filename << " for writing." << std::endl;
    }
    return false;
}
int main() {
    const char* filename = "output.bin";
    read_from_stdin(NULL);
    cout<<"Start to serailize" <<endl;
    store_map(filename);

    // Load the serialized data
    std::ifstream file(filename, std::ios::binary);
    if (file.is_open()) {
        boost::archive::binary_iarchive ia(file);
        ia >> ip_time_line;
        file.close();
        std::cout << "Data deserialized successfully." << std::endl;

        // Print the deserialized map
        for (const auto& entry : ip_time_line) {
            std::cout << "Key: " << entry.first << ", Value: " << entry.second << std::endl;
        }
    } else {
        std::cerr << "Error: Unable to open file " << filename << " for reading." << std::endl;
        return 1; // Return with an error code
    }
    return 0;
}
