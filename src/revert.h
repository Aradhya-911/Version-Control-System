#pragma once

#include<string>
#include<set>
#include<iostream>
#include<filesystem>
using namespace std;
namespace fs = std::filesystem;


void revert(string targetHash){

    if(!fs::exists(".mygit")){
        cout << "Repository not initialized.\n";
        return;
    }

    fs::path commitPath =
        fs::path(".mygit/commits") / targetHash;

    if(!fs::exists(commitPath)){
        cout << "Commit does not exist.\n";
        return;
    }

    // Store all files that exist in the target commit
    set<fs::path> targetFiles;

    for(const auto& entry :
        fs::recursive_directory_iterator(commitPath)){

        if(entry.is_regular_file()){

            fs::path relativePath =
                fs::relative(entry.path(), commitPath);

            targetFiles.insert(relativePath);
        }
    }

    // Remove files from working directory
    // that don't exist in target commit
    for(auto it = fs::recursive_directory_iterator(".");
    it != fs::recursive_directory_iterator();
    ++it){

    fs::path current = it->path();

    string filename = current.filename().string();

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

    // Restore files from target commit
    for(const auto& entry :
        fs::recursive_directory_iterator(commitPath)){

        fs::path relativePath =
            fs::relative(entry.path(), commitPath);

        fs::path destination =
            fs::path(".") / relativePath;

        if(entry.is_directory()){

            fs::create_directories(destination);
        }
        else if(entry.is_regular_file()){

            fs::create_directories(
                destination.parent_path()
            );

            fs::copy_file(
                entry.path(),
                destination,
                fs::copy_options::overwrite_existing
            );
        }
    }

    cout << "Successfully reverted to commit: "
         << targetHash << '\n';
}