#pragma once

#include<iostream>
#include<sstream>
#include<string>
#include<filesystem>
using namespace std;


string generateHash(const std::string& data) {

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