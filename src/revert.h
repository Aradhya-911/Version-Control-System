#pragma once

#include<iostream>
#include<filesystem>
using namespace std;
namespace fs = std::filesystem;


void revert(){
    if(!fs::exists(".mygit")){
    cout << "Repository not initialized.\n";
    return;
    }
    int commitNumber;
    cout<<"Enter the commit number: ";
    cin>>commitNumber;

    string commitName="commit"+to_string(commitNumber);

    fs::path commitPath=fs::path(".mygit/commits")/commitName;

    if(!fs::exists(commitPath)){
    cout << "Commit does not exist.\n";
    return;
    }
    for(const auto&entry: fs::recursive_directory_iterator(commitPath)){
        //get path relative to commit folder
        fs::path relativePath =
            fs::relative(entry.path(), commitPath);
        //where this file/folder belongs in the working directory
        fs::path destinationPath =
            fs::path(".") / relativePath;
        //recreating directories
        if(entry.is_directory()){
            fs::create_directories(destinationPath);
        }
        //restoring files
        else if(entry.is_regular_file()){
            fs::copy_file(
                entry.path(),
                destinationPath,
                fs::copy_options::overwrite_existing
            );
    }
    }
    cout<<"Successfully reverted to commit: "<<commitName<<'\n';
}