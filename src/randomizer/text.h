#pragma once

class TextMap{
public:
    TextMap(const char*);
    int get_length(int);
private:
    int cmd_lengths[255];
};
