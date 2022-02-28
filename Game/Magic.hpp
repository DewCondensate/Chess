#include <iostream>
#include <cstdio>

#ifndef MAGIC_HPP
#define MAGIC_HPP

class Magic {
public:
    Magic(){};
    uint64_t mask;
    uint16_t shift;
    uint64_t magic;

    void readFromFile(FILE * file) {
        fread(&mask, sizeof(uint64_t), 1, file);
        fread(&shift, sizeof(uint16_t), 1, file);
        fread(&magic, sizeof(uint64_t), 1, file);
    }

    void writeToFile(FILE * file) {
        fwrite(&mask, sizeof(uint64_t), 1, file);
        fwrite(&shift, sizeof(uint16_t), 1, file);
        fwrite(&magic, sizeof(uint64_t), 1, file);
    }
};

#endif