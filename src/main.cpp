#include<string>
#include<iostream>
#include "init.h"
#include "commit.h"
#include "revert.h"
#include "log.h"
#include "staging.h"


using namespace std;

int main(int argc, char* argv[]) {

    if(argc < 2){
        cout << "Please provide a command\n";
        return 0;
    }

    string command = argv[1];

    if(command == "init"){
        init();
    }
    else if(command == "commit"){
        commit();
    }
    else if(command == "log"){
        Log();
    }
    else if(command == "revert"){
        if(argc<3){
            cout<<"Please provide a commithash to revert to\n";
            return 0;
        }
        revert(argv[2]);
    }
    else if(command == "add"){
        if(argc < 3){
            cout << "Please provide a file or directory to add\n";
            return 0;
        }

        add(argv[2]);
    }
    else{
        cout << "Invalid command\n";
    }

    return 0;
}