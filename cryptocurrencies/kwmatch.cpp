
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

using namespace std;

/*
 * Load regular expressions from file with the format
 * id flag name regexp
 *
 * Returns true on success, false on failure
 */

bool load_regexp_file(char const* filename)
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

    fp.open(filename);
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
                cout<<"id="<<id<<",flag="<<flag<<",name="<<name<<",regexp="<<regexp <<endl;
                //TODO store them in arrays such that they can be taken by hs_compile_multi
            }
       }
       fp.close();
    } else {
        cerr<<"[ERROR] Cannot open regular expression file "<<filename <<endl;
    }
    return false;
}

int main(int argc, char* argv[])
{
	return EXIT_SUCCESS;
}
