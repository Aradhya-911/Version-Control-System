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



void commit(){
    if(!fs::exists(".mygit")){
        cout<<"Repository not initialized. Please run init first.\n";
        return;
    }
    int commitcount;
    ifstream headFile(".mygit/HEAD");
    headFile>>commitcount;
    headFile.close();
    

    Commit newCommit;
    
    if(commitcount==0){
        newCommit.parentId=-1;
    }else{
        newCommit.parentId=commitcount;
    }

    commitcount++;

    newCommit.id = commitcount;

    string commitName = "commit" + to_string(commitcount) ;

    fs::path commitPath=fs::path (".mygit/commits")/commitName;
    fs::create_directory(commitPath);
    
        // Create new commit from previous commit
    if(newCommit.parentId != -1){

    fs::path previousCommit =
        fs::path(".mygit/commits") /
        ("commit" + to_string(newCommit.parentId));
        cout << "Copying previous commit: " << previousCommit << '\n';

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
            fs::create_directories(destinationPath.parent_path());

            fs::copy_file(
                entry.path(),
                destinationPath,
                fs::copy_options::overwrite_existing
            );
        }
    }
}

    // Apply staged changes
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
            fs::create_directories(destinationPath.parent_path());

            fs::copy_file(
                entry.path(),
                destinationPath,
                fs::copy_options::overwrite_existing
            );
        }
    }
}
    // Clear the staging area after commit
    fs::remove_all(".mygit/staging");
    //Creating a log file for the commit
    cout<<"Enter commit message: ";
    cin.ignore();
    getline(cin, newCommit.message);

    //getting current date and time
    auto now = std::chrono::system_clock::now();

    time_t currentTime=chrono::system_clock::to_time_t(now);
    tm* localTime=localtime(&currentTime);

    // Convert formatted time into a string
    stringstream ss;
    ss << put_time(localTime, "%d-%m-%Y %H:%M:%S");

    newCommit.timestamp = ss.str();

    //opening log in append mode, file will be created if it doesn't exist
    ofstream LogFile(".mygit/log.txt", ios::app);

    LogFile
        << "commit " << newCommit.id << "|"
        << newCommit.parentId << "|"
        << newCommit.message << "|"
        << newCommit.timestamp
        <<'\n';

        LogFile.close();

        cout<<"Commit created succesfully: "
            <<commitName<< '\n';

    //updating the HEAD file with the new commit count
    ofstream headFileout(".mygit/HEAD");
    headFileout<<commitcount;
    headFileout.close();
}

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

int main(int argc,char* argv[]) {
    if(argc<2){
        cout<<"Please provide a command\n";
        return 0;
    }
    string command=argv[1];

    if(command=="init"){
        init();
    }else if(command=="add"){
        if(argc<3){
            cout<<"Please provide a filename to add\n";
            return 0;
        }
        add(argv[2]);
    }
    else if(command=="commit"){
        commit();
    }else if(command=="log"){
        Log();
    }else if(command=="revert"){
        revert();
    }else{
        cout<<"Invalid command\n";
    }   
    return 0;
    
}