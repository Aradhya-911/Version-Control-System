#pragma once

#include<iostream>
#include<filesystem>
using namespace std;
namespace fs = std::filesystem;
#include<fstream>
#include<string>
#include<vector>

void Log(){
    //checking if repo is initialized
    if(!fs::exists(".mygit")){
        cout<<"Repository not initialized, please run the init first\n";
        return;
    }
    //Repo exists but no commits found
    if(!fs::exists(".mygit/log.txt")){
        cout<<"No commits found, please create a commit first\n";
        return;
    }
    //opening the log file
    ifstream LogFile(".mygit/log.txt");
    vector<string>logs;
    string line;

    //reading the log file line by line
    while(getline(LogFile,line)){
        logs.push_back(line);
    }
    LogFile.close();
    for(int i = static_cast<int>(logs.size()) - 1; i >= 0; i--){
        cout<<logs[i]<<'\n';
    }
}