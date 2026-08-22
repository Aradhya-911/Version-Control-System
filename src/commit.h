#pragma once

#include"commitclass.h"
#include<sstream>
using namespace std;
#include<filesystem>
#include<chrono>
#include<ctime>
#include<iomanip>
#include<vector>
#include"hashing.h"
#include"object.h"
#include<unordered_map>

void commit(){

    if(!fs::exists(".mygit")){
        cout<<"Repository not initialized. Please run init first.\n";
        return;
    }

    // 1. Read current HEAD
    string parentHash;

    ifstream headFile(".mygit/HEAD");
    headFile >> parentHash;
    headFile.close();

    // 2. Create commit object
    Commit newCommit;

    if(parentHash == "0"){
        newCommit.setParentId("-1");
    }
    else{
        newCommit.setParentId(parentHash);
    }

    // 3. Get commit message
    string message;
    cout<<"Enter commit message: ";
    cin.ignore();
    getline(cin, message);
    newCommit.setMessage(message);

    // 4. Get timestamp
    auto now = chrono::system_clock::now();

    time_t currentTime =
        chrono::system_clock::to_time_t(now);

    tm* localTime = localtime(&currentTime);

    stringstream ss;
    ss << put_time(localTime, "%d-%m-%Y %H:%M:%S");

    newCommit.setTimestamp(ss.str());

    // 5. Generate commit hash
    string hashInput =
        newCommit.getParentId() + "|" +
        newCommit.getMessage() + "|" +
        newCommit.getTimestamp();

    newCommit.setId(generateHash(hashInput));

    // 6. NOW create the commit directory
    string commitName = newCommit.getId();

    fs::path commitPath =
        fs::path(".mygit/commits") / commitName;

    fs::create_directory(commitPath);

    // 7. Build new manifest

fs::path manifestPath =
    commitPath / "manifest.txt";

unordered_map<string,string> files;

if(newCommit.getParentId() != "-1"){

    fs::path previousManifest =
        fs::path(".mygit/commits") /
        newCommit.getParentId() /
        "manifest.txt";

    ifstream previous(previousManifest);

    string line;

    while(getline(previous, line)){

        size_t pos = line.find('|');

        if(pos == string::npos){
            continue;
        }

        string path =
            fs::path(line.substr(0, pos)).generic_string();

        string hash =
            line.substr(pos + 1);

        if(hash != "DELETED"){
            files[path] = hash;
        }
    }

    previous.close();
}

fs::path stagingPath = ".mygit/staging";

fs::path newPath =
    stagingPath / "newlystaged";

fs::path deletionFile =
    stagingPath / "deletions";


// Remove deleted files

if(fs::exists(deletionFile)){

    ifstream deletions(deletionFile);

    string path;

    while(getline(deletions, path)){

        string deletedPath =
        fs::path(path).generic_string();

        files.erase(deletedPath);
    }

    deletions.close();
}


// Add new and modified files

if(fs::exists(newPath)){

    for(const auto& entry :
        fs::recursive_directory_iterator(newPath)){

        if(!entry.is_regular_file()){
            continue;
        }

        fs::path relativePath =
            fs::relative(entry.path(), newPath);

        string path =
            relativePath.generic_string();

        string hash =
            storeObject(entry.path());

        files[path] = hash;
    }
}


// Write manifest

ofstream manifest(manifestPath);

for(const auto& file : files){

    manifest
        << file.first
        << "|"
        << file.second
        << '\n';
}

manifest.close();
    
    // 8. Clear staging area
    fs::remove_all(".mygit/staging");


    // 9. Write commit to log
    ofstream LogFile(".mygit/log.txt", ios::app);

    LogFile
        << "commit " << newCommit.getId() << "|"
        << newCommit.getParentId() << "|"
        << newCommit.getMessage() << "|"
        << newCommit.getTimestamp()
        << '\n';

    LogFile.close();


    cout<<"Commit created successfully: "
        <<commitName<<'\n';


    // 10. Update HEAD LAST
    ofstream headFileout(".mygit/HEAD");

    headFileout << newCommit.getId();

    headFileout.close();
}