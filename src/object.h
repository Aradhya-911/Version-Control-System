#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include "hashing.h"

using namespace std;
namespace fs = std::filesystem;

string storeObject(const fs::path& filePath){

    if(!fs::exists(filePath) || !fs::is_regular_file(filePath)){
        return "";
    }

    ifstream input(filePath, ios::binary);

    if(!input){
        return "";
    }

    string content(
        (istreambuf_iterator<char>(input)),
        istreambuf_iterator<char>()
    );

    input.close();

    string hash = generateHash(content);

    fs::path objectPath =
        fs::path(".mygit/objects") / hash;

    if(!fs::exists(objectPath)){

        fs::create_directories(
            objectPath.parent_path()
        );

        ofstream output(objectPath, ios::binary);

        output.write(
            content.data(),
            content.size()
        );

        output.close();
    }

    return hash;
}