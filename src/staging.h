#pragma once

#include<iostream>
#include<filesystem>
#include<fstream>
#include<chrono>
#include<ctime>
#include<iomanip>
#include<vector>
#include"commit.h"
#include<sstream>

using namespace std;
namespace fs = std::filesystem;

void add(string path){
    if(!fs::exists(".mygit")){
        cout << "Repository not initialized. Please run init first.\n";
        return;
    }

    fs::path source = path;

    if(!fs::exists(source)){
        cout << "Path does not exist: " << path << '\n';
        return;
    }

    fs::path stagingPath = ".mygit/staging";

    if(fs::is_regular_file(source)){

        if(source.filename() == "mygit.exe"){
            cout << "Cannot stage mygit.exe\n";
            return;
        }

        fs::path destination = stagingPath / source;

        fs::create_directories(destination.parent_path());

        fs::copy_file(
            source,
            destination,
            fs::copy_options::overwrite_existing
        );

        cout << "Added " << path << " to staging area.\n";
        return;
    }

    fs::recursive_directory_iterator it(source);
    fs::recursive_directory_iterator end;

    while(it != end){

        fs::path current = it->path();

        if(current.filename() == ".mygit" ||
           current.filename() == ".git"){

            if(it->is_directory()){
                it.disable_recursion_pending();
            }

            ++it;
            continue;
        }

        if(current.filename() == "mygit.exe"){
            ++it;
            continue;
        }

        fs::path relativePath =
            fs::relative(current, ".");

        fs::path destination =
            stagingPath / relativePath;

        if(it->is_directory()){
            fs::create_directories(destination);
        }
        else if(it->is_regular_file()){
            fs::create_directories(destination.parent_path());

            fs::copy_file(
                current,
                destination,
                fs::copy_options::overwrite_existing
            );
        }

        ++it;
    }

    cout << "Added " << path << " to staging area.\n";
}