void add(string path){

    if(!fs::exists(".mygit")){
        cout<<"Repository not initialized. Please run init first.\n";
        return;
    }

    fs::path source = path;

    fs::path stagingPath = ".mygit/staging";
    fs::path newPath = stagingPath / "newlystaged";
    fs::path deletionFile = stagingPath / "deletions";

    fs::create_directories(newPath);

    string parentHash;

    ifstream headFile(".mygit/HEAD");
    headFile >> parentHash;
    headFile.close();

    if(parentHash != "0" && !parentHash.empty()){

        fs::path previousCommit =
            fs::path(".mygit/commits") / parentHash;

        if(fs::exists(previousCommit)){

            for(const auto& entry :
                fs::recursive_directory_iterator(previousCommit)){

                if(!entry.is_regular_file()){
                    continue;
                }

                fs::path relativePath =
                    fs::relative(entry.path(), previousCommit);

                if(relativePath.filename() == "mygit.exe"){
                    continue;
                }

                fs::path currentFile =
                    fs::path(".") / relativePath;

                if(!fs::exists(currentFile)){

                    fs::create_directories(
                        deletionFile.parent_path()
                    );

                    ifstream check(deletionFile);
                    string deletedPath;
                    bool alreadyAdded = false;

                    while(getline(check, deletedPath)){
                        if(deletedPath == relativePath.string()){
                            alreadyAdded = true;
                            break;
                        }
                    }

                    check.close();

                    if(!alreadyAdded){

                        ofstream deletionLog(
                            deletionFile,
                            ios::app
                        );

                        deletionLog
                            << relativePath.string()
                            << '\n';

                        deletionLog.close();
                    }
                }
            }
        }
    }

    if(!fs::exists(source)){

        cout<<"Path does not exist: "<<path<<'\n';
        return;
    }

    if(fs::is_regular_file(source)){

        if(source.filename() == "mygit.exe"){
            cout<<"Cannot stage mygit.exe\n";
            return;
        }

        fs::path relativePath =
            fs::relative(source, ".");

        fs::path destination =
            newPath / relativePath;

        fs::create_directories(
            destination.parent_path()
        );

        fs::copy_file(
            source,
            destination,
            fs::copy_options::overwrite_existing
        );

        cout<<"Added "<<path<<" to staging area.\n";
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

        if(it->is_regular_file()){

            fs::path relativePath =
                fs::relative(current, ".");

            fs::path destination =
                newPath / relativePath;

            fs::create_directories(
                destination.parent_path()
            );

            fs::copy_file(
                current,
                destination,
                fs::copy_options::overwrite_existing
            );
        }

        ++it;
    }

    cout<<"Added "<<path<<" to staging area.\n";
}