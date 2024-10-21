#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <unistd.h>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/device/file.hpp>

using namespace std;
namespace io = boost::iostreams;

string getSecondsTimestamp(const string& epoch_timestamp) {
    // Find the position of the dot (.)
    size_t dot_position = epoch_timestamp.find('.');
    if (dot_position != string::npos) {
        // Return the part before the dot (seconds)
        return epoch_timestamp.substr(0, dot_position);
    }
        return epoch_timestamp; // Return full timestamp if there's no dot
}


time_t deriveDay(const string &epoch_seconds)
{
    time_t epoch_time = stol(epoch_seconds);
    struct tm* time_info = gmtime(&epoch_time);
    time_info->tm_hour = 0;
    time_info->tm_min = 0;
    time_info->tm_sec = 0;
    return mktime(time_info);
}
// Function to read a gzip-compressed CSV file and store records
void readGzipCSV(const string& filename, map<time_t, int>& packet_counts) {
    io::filtering_istream in;
    string line;
    string seconds_timestamp;
    // Open the file and decompress on-the-fly
    io::file_source file_source(filename);
        if (!file_source.is_open()) {
            cerr <<"ERROR: Could not open file " <<filename <<endl;
            return;
        }
    in.push(io::gzip_decompressor());
    in.push(io::file_source(file_source));

    if (!in) {
        cerr << "Could not open the gzip-compressed file: " << filename << endl;
        return;
    }

    // Read file line by line
    while (getline(in, line)) {
        if (!line.empty()) {
            //cout << line <<endl;
		    seconds_timestamp =  getSecondsTimestamp(line);
    		packet_counts[deriveDay(seconds_timestamp)]++;
        }
    }

}

void showHelp(void) {
    cout << "Usage: basic-stats" << " [options]\n";
    cout << "Options:\n";
    cout << "  -h           Show this help message\n";
    cout << "  -l filelist  Specify a list of files\n";
    cout << "  -f filename  Specify a single file\n";
}



void process_filelist(const string& filelist)
{
    map<time_t, int> packet_counts;
    ifstream file(filelist);
    string line;
     if (!file.is_open()) {
        cerr << "Could not open the filelist: " << filelist << endl;
        return;
    }
    while (getline(file, line)) {
        if (!line.empty()) {
            cout<<line<<endl;
            readGzipCSV(line,packet_counts);
        }
    }

    file.close();


    for (const auto& entry : packet_counts) {
            cout << "epoch " << entry.first << " -> Packet Count: " << entry.second << endl;
        }
}


int main(int argc, char* argv[]){
    int opt=-1;
    string filelist;
    bool helpFlag=false;

    // Command line option parsing using getopt
    while ((opt = getopt(argc, argv, "h:l:")) != -1) {
        switch (opt) {
            case 'h':  // Help
                helpFlag = true;
                break;
            case 'l':  // filelist
                filelist = optarg;
                break;
            default:   // If an unknown option is provided
                cerr << "Unknown option: " << optopt << endl;
                showHelp();
                return 1;
        }
    }

    // Show help if -h was provided
    if (helpFlag) {
        showHelp();
        return 0;
    }


     // Check if filelist or filename options were provided and print them
    if (filelist.empty()) {
        cerr << "Filelist must be specified: " << endl;
        return 1;
    }
    process_filelist(filelist);
	return 0;
}
