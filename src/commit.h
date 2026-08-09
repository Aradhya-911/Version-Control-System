#pragma once
#include <string>
using namespace std;

class Commit {
    public:
    int id;
    string message;
    string timestamp;
    int parentId;
};
