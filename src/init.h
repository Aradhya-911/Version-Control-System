#pragma once

#include<iostream>
#include<filesystem>
using namespace std;
namespace fs = std::filesystem;
#include<fstream>

void init(){
    fs::path repoPath = ".mygit/commits";
                if(fs::exists(".mygit")){
                    cout<<"Repository already exists\n";
                } else {
                    fs::create_directories(repoPath);
                    ofstream headFile(".mygit/HEAD");
                    headFile<<0;        
                    headFile.close();
                    cout<<"Repository initialized\n";
                }
}