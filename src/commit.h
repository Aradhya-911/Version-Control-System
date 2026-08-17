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
        newCommit.parentId = "-1";
    }
    else{
        newCommit.parentId = parentHash;
    }

    // 3. Get commit message
    cout<<"Enter commit message: ";
    cin.ignore();
    getline(cin, newCommit.message);

    // 4. Get timestamp
    auto now = chrono::system_clock::now();

    time_t currentTime =
        chrono::system_clock::to_time_t(now);

    tm* localTime = localtime(&currentTime);

    stringstream ss;
    ss << put_time(localTime, "%d-%m-%Y %H:%M:%S");

    newCommit.timestamp = ss.str();

    // 5. Generate commit hash
    string hashInput =
        newCommit.parentId + "|" +
        newCommit.message + "|" +
        newCommit.timestamp;

    newCommit.id = generateHash(hashInput);

    // 6. NOW create the commit directory
    string commitName = newCommit.id;

    fs::path commitPath =
        fs::path(".mygit/commits") / commitName;

    fs::create_directory(commitPath);


    // 7. Copy previous commit snapshot
    if(newCommit.parentId != "-1"){

        fs::path previousCommit =
            fs::path(".mygit/commits") /
            newCommit.parentId;

        for(const auto& entry :
            fs::recursive_directory_iterator(previousCommit)){

            fs::path relativePath =
                fs::relative(entry.path(), previousCommit);

            fs::path destinationPath =
                commitPath / relativePath;

            if(entry.is_directory()){

                fs::create_directories(destinationPath);

            }
            else if(entry.is_regular_file()){

                fs::create_directories(
                    destinationPath.parent_path()
                );

                fs::copy_file(
                    entry.path(),
                    destinationPath,
                    fs::copy_options::overwrite_existing
                );
            }
        }
    }


    // 8. Apply staged changes
    if(fs::exists(".mygit/staging")){

        for(const auto& entry :
            fs::recursive_directory_iterator(".mygit/staging")){

            fs::path relativePath =
                fs::relative(entry.path(), ".mygit/staging");

            fs::path destinationPath =
                commitPath / relativePath;

            if(entry.is_directory()){

                fs::create_directories(destinationPath);

            }
            else if(entry.is_regular_file()){

                fs::create_directories(
                    destinationPath.parent_path()
                );

                fs::copy_file(
                    entry.path(),
                    destinationPath,
                    fs::copy_options::overwrite_existing
                );
            }
        }
    }


    // 9. Clear staging area
    fs::remove_all(".mygit/staging");


    // 10. Write commit to log
    ofstream LogFile(".mygit/log.txt", ios::app);

    LogFile
        << "commit " << newCommit.id << "|"
        << newCommit.parentId << "|"
        << newCommit.message << "|"
        << newCommit.timestamp
        << '\n';

    LogFile.close();


    cout<<"Commit created successfully: "
        <<commitName<<'\n';


    // 11. Update HEAD LAST
    ofstream headFileout(".mygit/HEAD");

    headFileout << newCommit.id;

    headFileout.close();
}