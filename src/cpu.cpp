#include "types.cpp"
#include <stdexcept>

#define ZERO_FLAG_BYTE_POSITION 7
#define SUBTRACT_FLAG_BYTE_POSITION 6
#define HALF_CARRY_FLAG_BYTE_POSITION 5
#define CARRY_FLAG_BYTE_POSITION 4

class cpu {
    byte cpuRegisters[8];
    byte flagsRegister;
    
    enum Instruction {
        ADD,
        ADDHL,
        ADC,
        SUB,
        SBC,
        AND,
        OR,
        XOR,
        CP,
        INC,
        DEC,
        CCF,
        SCF,
        RRA,
        RLA,
        RRCA,
        RRLA,
        CPL,
        BIT,
        RESET,
        SET,
        SRL,
        RR,
        RL,
        RRC,
        RLC,
        SRA,
        SWAP
    };

    void setFlags(bool value, byte position) {
        if (value) {
            flagsRegister |= (byte)(1 << position);
        } else {
            flagsRegister &= (byte)(0 << position);
        }
    }

    void add(char target) {
        // Read current from target
        byte value = cpuRegisters[target - ASCII_A_LOWER];
        // Add with register a
        word newValue = (word)value + (word)cpuRegisters[0];
        
        // Zero
        setFlags(newValue == 0, ZERO_FLAG_BYTE_POSITION);
        // Subtract
        setFlags(false, SUBTRACT_FLAG_BYTE_POSITION);
        // Half Carry 
        setFlags((cpuRegisters[0] & 0x0f) + (value & 0x0f) > 0x0f, HALF_CARRY_FLAG_BYTE_POSITION);
        // Overflow (CARRY_FLAG)
        setFlags(newValue > 255, CARRY_FLAG_BYTE_POSITION);
        
    }

    
    

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
        return (word)cpuRegisters[a] << 8 | (word)cpuRegisters[b];
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
        cpuRegisters[a] = (byte)((value & 0xff00) >> 8);
        cpuRegisters[b] = (byte)(value & 0x00ff);
    }
};