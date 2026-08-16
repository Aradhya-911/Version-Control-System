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

//commit1
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

string generateHash(const string& data) {

    uint64_t hash = 14695981039346656037ULL;

    for(char c : data){
        hash ^= static_cast<unsigned char>(c);
        hash *= 1099511628211ULL;
    }

    stringstream ss;
    ss << hex << hash;

    string result = ss.str();

    return result.substr(0, 8);
}

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
        revert();
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