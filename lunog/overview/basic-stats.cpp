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
#include <stdint.h>

using namespace std;
namespace io = boost::iostreams;


class BasicStats {
    public:
        BasicStats(void);
        // Command line parsing flags
        bool flag_help;
        bool flag_day;
        bool flag_hour;
        bool flag_minute;
        bool flag_second;
        bool flag_quiet;
        // Counting datastructures
        map<time_t, int> packet_counts;
        map<uint32_t, int> src_ips_counts;

        void showHelp(void);
        void dump_packet_count(void);
        void readGzipCSV(const string& filename);
        void process_filelist(const string& filelist);

    private:
        string getSecondsTimestamp(const string& epoch_timestamp);
        time_t deriveDay(const string &epoch_seconds);
};


BasicStats::BasicStats()
{
    this->flag_help = false;
    this->flag_day = false;
    this->flag_hour = false;
    this->flag_minute = false;
    this->flag_second = false;
    this->flag_quiet = false;
}

string BasicStats::getSecondsTimestamp(const string& epoch_timestamp) {
    // Find the position of the dot (.)
    size_t dot_position = epoch_timestamp.find('.');
    if (dot_position != string::npos) {
        // Return the part before the dot (seconds)
        return epoch_timestamp.substr(0, dot_position);
    }
        return epoch_timestamp; // Return full timestamp if there's no dot
}


time_t BasicStats::deriveDay(const string &epoch_seconds)
{
    time_t epoch_time = stol(epoch_seconds);
    struct tm* time_info = localtime(&epoch_time);
    if (this->flag_day) {
        time_info->tm_hour = 0;
        time_info->tm_min = 0;
        time_info->tm_sec = 0;
    }


    if (this->flag_hour){
        time_info->tm_min = 0;
        time_info->tm_sec = 0;
    }

    if (this->flag_minute){
        time_info->tm_sec = 0;
    }
    // Default group them by seconds
    return mktime(time_info);
}
// Function to read a gzip-compressed CSV file and store records
void BasicStats::readGzipCSV(const string& filename) {
    io::filtering_istream in;
    string line;
    string seconds_timestamp;
    stringstream ss;
    double ts;
    string str_src_ip;

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
            ss = stringstream(line);
            ss>>ts;//Skip timestamp
            ss>>str_src_ip; // Read src ip;
            //cout << line <<endl;
		    seconds_timestamp =  getSecondsTimestamp(line);
    		this->packet_counts[deriveDay(seconds_timestamp)]++;
            ss.clear();
        }
    }

}

void BasicStats::showHelp(void) {
    cout << "Usage: basic-stats" << " [options]"<<endl;
    cout << "Options:\n";
    cout << "  -u           Show this help message"<<endl;
    cout << "  -l filelist  Specify a list of files"<<endl;
    cout << "  -f filename  Specify a single file"<<endl;
    cout << "  -h           Count packets per hour"<<endl;
    cout << "  -s           Count packets per second"<<endl;
    cout << "  -m           Count packets per minutes"<<endl;
    cout << "  -d           Count packets per day"<<endl;
    cout << "  -q           Do not show progress messages" <<endl;
}

void BasicStats::dump_packet_count(void)
{
       char buffer[80];
       for (const auto& entry : this->packet_counts) {
           tm* time_info = localtime(&entry.first);

           // Default grouping is by second
           strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", time_info);

           if (flag_day) {
               strftime(buffer, sizeof(buffer), "%Y-%m-%d", time_info);
           }

           if (flag_hour) {
               strftime(buffer, sizeof(buffer), "%Y-%m-%d %H", time_info);
           }

           if (flag_hour) {
               strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", time_info);
           }
           cout << buffer<< " " << entry.second << endl;
        }
}

void BasicStats::process_filelist(const string& filelist)
{
    ifstream file(filelist);
    int i = 0;
    string line;
     if (!file.is_open()) {
        cerr << "Could not open the filelist: " << filelist << endl;
        return;
    }
    while (getline(file, line)) {
        if (!line.empty()) {
            i++;
            if (!this->flag_quiet) {
                cerr<<"[INFO] Number of files processed "<<i<<"\r";
            }
            this->readGzipCSV(line);
        }
    }
    cerr<<endl;
    this->dump_packet_count();
    file.close();
}




int main(int argc, char* argv[]){
    int opt=-1;
    string filelist;
    BasicStats bs;

    // Command line option parsing using getopt
    while ((opt = getopt(argc, argv, "msudqhl:")) != -1) {
        switch (opt) {
            case 'u':  // Help
                bs.flag_help = true;
                break;
            case 'l':  // filelist
                filelist = optarg;
                break;
            case 'q':
                bs.flag_quiet = true;
                break;
            case 'd':
                bs.flag_day = true;
                break;
            case 'h':
                bs.flag_hour = true;
                break;
            case 'm':
                bs.flag_minute = true;
                break;
            case 's':
                bs.flag_second = true;
                break;
            default:   // If an unknown option is provided
                cerr << "Unknown option: " << optopt << endl;
                bs.showHelp();
                return 1;
        }
    }

    // Show help if -h was provided
    if (bs.flag_help) {
        bs.showHelp();
        return 0;
    }


     // Check if filelist or filename options were provided and print them
    if (filelist.empty()) {
        cerr << "Filelist must be specified: " << endl;
        return 1;
    }
    bs.process_filelist(filelist);
	return 0;
}
