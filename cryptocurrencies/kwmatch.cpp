
/*
 *   Load a file of data such and do a regexp matching with hyperscan
 *
 *   Copyright (C) 2023  Gerard Wagener
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Affero General Public License as
 *   published by the Free Software Foundation, either version 3 of the
 *   License, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Affero General Public License for more details.
 *
 *   You should have received a copy of the GNU Affero General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <hs.h>
#include <string.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <getopt.h>

using namespace std;

class kwmatch
{
    public:
        // Command line arguments
        string file;
        string name;
        string original;
        void usage(void);
        bool load_regexp_file(void);
    private:
        vector <string> regexps;
        vector <string> names;
        vector <unsigned> flags;
        vector <unsigned> ids;
};

/*
 * Load regular expressions from file with the format
 * id flag name regexp
 *
 * Returns true on success, false on failure
 */

bool kwmatch::load_regexp_file(void)
{
    fstream fp;
    int id;
    int flag;
    string regexp;
    string line;
    string name;
    int i,j;
// Expect 4 offsets first for "id" "flag", "name" all the rest goes in regexp
    int idx[3]={0,0,0};

    fp.open(this->file);
    if (fp.is_open()) {
        while (getline(fp,line)) {
            if (line.rfind("#") == string::npos) {
                //regexp could have spaces aswell all the rest goes there
                j = 0;
                for (i=0;i<line.length();i++) {
                    if (line[i] == ' '){
                        if (j < 3) {
                            idx[j]=i;
                            j++;
                        }
                    }
                }
                //cout <<line<<" "<<idx[0]<<" "<<idx[1]<<" "<<idx[2]<<" " <<idx[3]<< endl;
                id = std::stoi(line.substr(0, idx[0]).c_str());
                flag = std::stoi(line.substr(idx[0]+1, idx[1]).c_str());
                name = line.substr(idx[1]+1,idx[2]-idx[1]-1);
                regexp = line.substr(idx[2]+1,string::npos);
                //Store only items where all fields are set to avoid segfaults
                //FIXME flags are ids are not checked
                if (!(name.empty()) and !(regexp.empty())){
                    this->ids.push_back(id);
                    this->flags.push_back(flag);
                    this->names.push_back(name);
                    this->regexps.push_back(regexp);
                    cout<<"id="<<id<<",flag="<<flag<<",name="<<name<<",regexp="<<regexp <<endl;
                }
            }
       }
       fp.close();
    } else {
        cerr<<"[ERROR] Cannot open regular expression file "<< this->file <<endl;
    }
    return false;
 }

void kwmatch::usage(void)
{
    cout << "kwmatch - matches a set of regex defined in a file <file> on stdin"<<endl;
    cout << "kwmatch [-h] [-f filename] [-n name] [-o original] "<< endl;
    cout << endl<< "OPTIONS" << endl<<endl;
    cout <<"-h    --help     Shows this screen"<<endl;
    cout <<"-f    --file     File containing the regular expressions" <<endl;
    cout <<"-n    --name     Name where stdin came from i.e. compressed filename of the shard or URL" <<endl;
    cout <<"-o    --original Original name of the data" <<endl;

    cout << endl << "FORMAT of the regexp file" << endl << endl;
    cout << "One line contains a regexp with spaces seprated" << endl;
    cout << "Spaces are not forseen in names of regexp but they can be in the regexp itself" << endl;
    cout << "The following fields are expected per line" << endl;
    cout << "Format id flag name regexp" <<endl;
}

int main(int argc, char* argv[])
{
    kwmatch kw;
    int next_option = 0;
    const char* const short_options = "hf:n:t:o";
    const struct option long_options[] = {
                { "help", 0, NULL, 'h' },
                { "file", 1, NULL, 'f' },
                { "name", 1, NULL, 'n' },
                { "orginal", 1, NULL, 'o'},
                {NULL,0,NULL,0}
   };

 do {
        next_option = getopt_long (argc, argv, short_options,
                                   long_options, NULL);
        if (next_option > 0) {
            switch(next_option)
            {
            case 'h':
                kw.usage();
                return EXIT_SUCCESS;
            case 'f':
                kw.file = string(optarg);
                break;
            case 'n':
                kw.name = string(optarg);
                break;
            case 'o':
                kw.original = string(optarg);
                break;
            default:
                /* Something unexpected happended */
                return EXIT_FAILURE;
            }
        }
 } while(next_option != -1);

    if (!kw.file.empty()){
        kw.load_regexp_file();
   }
	return EXIT_SUCCESS;
}
