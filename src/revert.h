#pragma once

#include <string>
#include <set>
#include <iostream>
#include <fstream>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

void revert(string targetHash){

    if(!fs::exists(".mygit")){
        cout << "Repository not initialized.\n";
        return;
    }

    fs::path commitPath =
        fs::path(".mygit/commits") / targetHash;

    fs::path manifestPath =
        commitPath / "manifest.txt";

    if(!fs::exists(manifestPath)){
        cout << "Commit does not exist.\n";
        return;
    }

    set<fs::path> targetFiles;

    ifstream manifest(manifestPath);

    string line;

    while(getline(manifest, line)){

        size_t pos = line.find('|');

        if(pos == string::npos){
            continue;
        }

        string filePath =
            line.substr(0, pos);

        string hash =
            line.substr(pos + 1);

        if(hash == "DELETED"){
            continue;
        }

        fs::path relativePath = filePath;

        targetFiles.insert(relativePath);

        fs::path objectPath =
            fs::path(".mygit/objects") / hash;

        fs::path destination =
            fs::path(".") / relativePath;

        if(!fs::exists(objectPath)){
            cout << "Object missing: "
                 << hash << '\n';
            continue;
        }

        fs::create_directories(
            destination.parent_path()
        );

        fs::copy_file(
            objectPath,
            destination,
            fs::copy_options::overwrite_existing
        );
    }

    manifest.close();


    for(auto it = fs::recursive_directory_iterator(".");
        it != fs::recursive_directory_iterator();
        ++it){

        fs::path current = it->path();

        string filename =
            current.filename().string();

        if(filename == ".mygit" ||
           filename == ".git"){

            if(it->is_directory()){
                it.disable_recursion_pending();
            }

            continue;
        }

        if(filename == "mygit.exe"){
            continue;
        }

        if(it->is_regular_file()){

            fs::path relativePath =
                fs::relative(current, ".");

            if(!targetFiles.contains(relativePath)){
                fs::remove(current);
            }
        }
    }

    ofstream headFile(".mygit/HEAD");

    headFile << targetHash;

    headFile.close();

    cout << "Successfully reverted to commit: "
         << targetHash << '\n';
}