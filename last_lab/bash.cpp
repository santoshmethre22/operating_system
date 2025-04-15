// #include<iostream>
// using namespace std;
// int main(){
// 	string s;
// 	cin>>s;


// 	int i=0;
// 	int n=s.size();



// 	while (i<n){
// 		if(s[i]==' '){
// 			i++;
// 		}
// 	}

// 	vector<string>words
	
// 	string t="";
// 	while (true){

// 		if(s[i]=="," || "||"||"&&","|"){
			
// 			words.push_back(t)
// 			t="";

// 		}


// 		t+=s[i];

// 		i++;

// 	}



// 	for(auto i:words){

// 			if(i=="ls -l"){

// 				string s;

// 				getcwd(&s)


// 				cout<<s<<endl;

// 			}

// 			else if(i=="clear"){


// 			}

// 			else if(i=="clear"){

// 				return 0;
// 			}


// 	}




// 	return 0;

// }



#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>     // for getcwd()
#include <limits.h>     // for PATH_MAX
#include <sstream>

using namespace std;


vector<string> tokenize(string s) {

	
    while (i<n){
				if(s[i]==' '){
					i++;
				}
			}
		
			vector<string>words
			
			string t="";
			while (true){
		
				if(s[i]=="," || "||"||"&&","|"){
					
					words.push_back(t)
					t="";
		
				}
		
		
				t+=s[i];
		
				i++;
		
			}

			return s;
}



void forls {
    DIR *dir;
    struct dirent *entry;

    dir = opendir(".");
    

   
    closedir(dir);
}

int main() {
    string input;
    getline(cin, input);

    vector<string> commands = tokenize(input);

    for (string cmd : commands) {
        if (cmd == "ls" || cmd == "ls -l") {
            char cwd[PATH_MAX];
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                cout << "Current working directory: " << cwd << endl;


            } else {
                perror("getcwd() error");
            }
        } else if (cmd == "clear") {
            cout << "\033[2J\033[1;1H"; 
        } else if (cmd == "exit") {
            return 0;
        } else {
            cout << "Unknown command: " << cmd << endl;
        }
    }

    return 0;
}
