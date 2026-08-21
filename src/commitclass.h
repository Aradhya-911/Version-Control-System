#pragma once

#include <string>
using namespace std;

class Commit {
private:
    string id;
    string message;
    string timestamp;
    string parentId;

public:

    // Default constructor
    Commit()
        : id(""),
          message(""),
          timestamp(""),
          parentId("-1") {}

    // Parameterized constructor
    Commit(
        string id,
        string message,
        string timestamp,
        string parentId
    )
        : id(id),
          message(message),
          timestamp(timestamp),
          parentId(parentId) {}

    // Getters
    string getId() const {
        return id;
    }
    string getMessage() const {
        return message;
    }
    string getTimestamp() const {
        return timestamp;
    }
    string getParentId() const {
        return parentId;
    }

    // Setters
    void setId(string newId) {
        id = newId;
    }
    void setMessage(string newMessage) {
        message = newMessage;
    }
    void setTimestamp(string newTimestamp) {
        timestamp = newTimestamp;
    }
    void setParentId(string newParentId) {
        parentId = newParentId;
    }
};