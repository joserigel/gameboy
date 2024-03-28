#include "types.cpp"
#include <stdexcept>

class cpu {
    byte cpuRegister[8];

    word getCPURegisterAsWord(char* selection) {
        if (sizeof(selection) != 2) {
            throw std::invalid_argument("selection does not consist of two chars");
        } else if (
            selection[0] < ASCII_A_LOWER || selection[0] > ASCII_H_LOWER || 
            selection[1] < ASCII_A_LOWER || selection[1] > ASCII_H_LOWER
        ) {
            throw std::invalid_argument("selection is not bounded by a <= selection[i] <= h");
        }
        byte a = selection[0] - ASCII_A_LOWER;
        byte b = selection[1] - ASCII_H_LOWER;
        return (word)cpuRegister[a] << 8 | (word)cpuRegister[b];
    }

    void setCPURegisterAsWord(char* selection, word value) {
        if (sizeof(selection) != 2) {
            throw std::invalid_argument("selection does not consist of two chars");
        } else if (
            selection[0] < ASCII_A_LOWER || selection[0] > ASCII_H_LOWER || 
            selection[1] < ASCII_A_LOWER || selection[1] > ASCII_H_LOWER
        ) {
            throw std::invalid_argument("selection is not bounded by a <= selection[i] <= h");
        }
        byte a = selection[0] - ASCII_A_LOWER;
        byte b = selection[1] - ASCII_H_LOWER;
        cpuRegister[a] = (byte)((value & 0xff00) >> 8);
        cpuRegister[b] = (byte)(value & 0x00ff);
    }
};