#pragma once
#include <string>
using namespace std;

class Commit {
    public:
    string id;
    string message;
    string timestamp;
    string parentId;
};
