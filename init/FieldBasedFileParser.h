#ifndef FIELD_BASED_FILE_PARSER__H
#define FIELD_BASED_FILE_PARSER__H

#include <string>
#include <istream>
#include <vector>

class FieldBasedFileParser {
protected:
    ~FieldBasedFileParser();

public:
    FieldBasedFileParser();

    FieldBasedFileParser(const FieldBasedFileParser &other) = delete;
    FieldBasedFileParser &operator =(const FieldBasedFileParser &other) = delete;

    void parse(const std::string &filename);
    void parse(std::istream &stream);

protected:
    virtual void processLine(const std::vector<std::string> &tokens) = 0;
};

#endif

