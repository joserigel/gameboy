#include "types.cpp"
#include <stdexcept>

#define ZERO_FLAG_BYTE_POSITION 7
#define SUBTRACT_FLAG_BYTE_POSITION 6
#define HALF_CARRY_FLAG_BYTE_POSITION 5
#define CARRY_FLAG_BYTE_POSITION 4

/**
 * Credits:
 * https://rylev.github.io/DMG-01/public/book/introduction.html
 * https://meganesu.github.io/generate-gb-opcodes/
*/

namespace cpu {
    enum Target {
        A,
        B,
        C,
        D,
        E,
        F,
        H,
        L
    };

    enum Instructionruction {
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

    class CPU {
        byte cpuRegisters[8];
        byte flagsRegister;

        bool getFlag(byte position) {
            return flagsRegister & (1 << position);
        }

        void setFlag(bool value, byte position) {
            if (value) {
                flagsRegister |= (byte)(1 << position);
            } else {
                flagsRegister &= ~(byte)(1 << position);
            }
        }

        void addInstruction(Target target) {
            // Read current from target
            byte value = cpuRegisters[target];
            // Add with register a
            word total = (word)value + (word)cpuRegisters[A];

            cpuRegisters[A] = total;
            
            // Zero
            setFlag(total == 0, ZERO_FLAG_BYTE_POSITION);
            // Subtract
            setFlag(false, SUBTRACT_FLAG_BYTE_POSITION);
            // Half Carry 
            setFlag((cpuRegisters[A] & 0x0f) + (value & 0x0f) > 0x0f, HALF_CARRY_FLAG_BYTE_POSITION);
            // Overflow (CARRY_FLAG)
            setFlag(total > 255, CARRY_FLAG_BYTE_POSITION);
        }

        void addhlInstruction(Target targetA, Target targetB) {
            word value = getCPURegisterAsWord(targetA, targetB);
            unsigned int total = (unsigned int)value + (unsigned int)getCPURegisterAsWord(H, L);

            setCPURegisterAsWord(H, L, (word)total);

            // Zero
            setFlag(total == 0, ZERO_FLAG_BYTE_POSITION);
            // Subtract
            setFlag(false, SUBTRACT_FLAG_BYTE_POSITION);
            // Half Carry 
            setFlag((word)cpuRegisters[L] + (word)targetB > 0x00ff, HALF_CARRY_FLAG_BYTE_POSITION);
            // Overflow (CARRY_FLAG)
            setFlag(total > 0xffff, CARRY_FLAG_BYTE_POSITION);
        }

        void addcInstruction(Target target) {
            byte value = cpuRegisters[target];
            word total = (word)value + (word)cpuRegisters[A];

            if (getFlag(CARRY_FLAG_BYTE_POSITION)) {
                total++;
            }
            
            cpuRegisters[A] = (byte)(total % 256);

            // Zero
            setFlag(total == 0, ZERO_FLAG_BYTE_POSITION);
            // Subtract
            setFlag(false, SUBTRACT_FLAG_BYTE_POSITION);
            // Half Carry 
            setFlag((cpuRegisters[A] & 0x0f) + (value & 0x0f) > 0x0f, HALF_CARRY_FLAG_BYTE_POSITION);
            // Overflow (CARRY_FLAG)
            setFlag(total > 255, CARRY_FLAG_BYTE_POSITION);
        }

        void subInstruction(Target target) {
            byte value = ~cpuRegisters[target];
            word total = (word)cpuRegisters[A] + (word)value;

            cpuRegisters[A] = (byte)(total % 256);

            // Zero
            setFlag(total == 0, ZERO_FLAG_BYTE_POSITION);
            // Subtract
            setFlag(true, SUBTRACT_FLAG_BYTE_POSITION);
            // Half Carry 
            setFlag((cpuRegisters[A] & 0x0f) + (value & 0x0f) > 0x0f, HALF_CARRY_FLAG_BYTE_POSITION);
            // Overflow (CARRY_FLAG)
            setFlag(total > 255, CARRY_FLAG_BYTE_POSITION);
        }

        void subcInstruction(Target target) {
            byte value = ~cpuRegisters[target];
            word total = (word)cpuRegisters[A] + (word)value;

            if (getFlag(CARRY_FLAG_BYTE_POSITION)) {
                total--;
            }

            cpuRegisters[A] = (byte)(total % 256);

            // Zero
            setFlag(total == 0, ZERO_FLAG_BYTE_POSITION);
            // Subtract
            setFlag(false, SUBTRACT_FLAG_BYTE_POSITION);
            // Half Carry 
            setFlag((cpuRegisters[A] & 0x0f) + (value & 0x0f) > 0x0f, HALF_CARRY_FLAG_BYTE_POSITION);
            // Overflow (CARRY_FLAG)
            setFlag(total > 255, CARRY_FLAG_BYTE_POSITION);
        }

        void andInstruction(Target target) {
            byte value = cpuRegisters[target];
            cpuRegisters[A] &= value;


            setFlag(cpuRegisters[A] == 0, ZERO_FLAG_BYTE_POSITION);
            // Subtract
            setFlag(false, SUBTRACT_FLAG_BYTE_POSITION);
            // Half Carry 
            setFlag(1, HALF_CARRY_FLAG_BYTE_POSITION);
            // Overflow (CARRY_FLAG)
            setFlag(false, CARRY_FLAG_BYTE_POSITION);
        }

        void orInstruction(Target target) {
            byte value = cpuRegisters[target];
            cpuRegisters[A] |= value;

            // Zero
            setFlag(cpuRegisters[A] == 0, ZERO_FLAG_BYTE_POSITION);
            // Subtract
            setFlag(false, SUBTRACT_FLAG_BYTE_POSITION);
            // Half Carry 
            setFlag(false, HALF_CARRY_FLAG_BYTE_POSITION);
            // Overflow (CARRY_FLAG)
            setFlag(false, CARRY_FLAG_BYTE_POSITION);
        }

        void xorInstruction(Target target) {
            byte value = cpuRegisters[target];
            cpuRegisters[A] ^= value;


            setFlag(cpuRegisters[A] == 0, ZERO_FLAG_BYTE_POSITION);
            // Subtract
            setFlag(false, SUBTRACT_FLAG_BYTE_POSITION);
            // Half Carry 
            setFlag(0, HALF_CARRY_FLAG_BYTE_POSITION);
            // Overflow (CARRY_FLAG)
            setFlag(false, CARRY_FLAG_BYTE_POSITION);
        }

        void cpInstruction(Target target) {
            byte value = ~cpuRegisters[target];
            word total = (word)cpuRegisters[A] + (word)value;
            
            // Zero
            setFlag(total == 0, ZERO_FLAG_BYTE_POSITION);
            // Subtract
            setFlag(true, SUBTRACT_FLAG_BYTE_POSITION);
            // Half Carry 
            setFlag((cpuRegisters[A] & 0x0f) + (value & 0x0f) > 0x0f, HALF_CARRY_FLAG_BYTE_POSITION);
            // Overflow (CARRY_FLAG)RESET
            
            cpuRegisters[target] = value;

            // Zero
            setFlag(total == 0, ZERO_FLAG_BYTE_POSITION);
            // Subtract
            setFlag(false, SUBTRACT_FLAG_BYTE_POSITION);
            // Half Carry 
            setFlag((value & 0x0f) + 1 > 0x0f, HALF_CARRY_FLAG_BYTE_POSITION);
            // Overflow (CARRY_FLAG)
            setFlag(value == 255, CARRY_FLAG_BYTE_POSITION);
        }

        void decInstruction(Target target) {
            byte value = cpuRegisters[target];
            byte total = value - 1;
            
            cpuRegisters[target] = value;

            // Zero
            setFlag(total == 0, ZERO_FLAG_BYTE_POSITION);
            // Subtract
            setFlag(true, SUBTRACT_FLAG_BYTE_POSITION);
            // Half Carry 
            setFlag(value & 0x0f == 0, HALF_CARRY_FLAG_BYTE_POSITION);
            // Overflow (CARRY_FLAG)
            setFlag(value == 0, CARRY_FLAG_BYTE_POSITION);
        }

        void ccfInstruction() {
            bool value = getFlag(CARRY_FLAG_BYTE_POSITION);
            setFlag(!value, CARRY_FLAG_BYTE_POSITION);
        }

        void scfInstruction() {
            setFlag(true, CARRY_FLAG_BYTE_POSITION);
        }

        void cplInstruction() {
            cpuRegisters[A] = ~cpuRegisters[A];
            
            // Subtract
            setFlag(true, SUBTRACT_FLAG_BYTE_POSITION);
            // Half Carry
            setFlag(true, HALF_CARRY_FLAG_BYTE_POSITION);
        }

        void rrInstruction(Target target) {
            byte lastBit = cpuRegisters[target] & 0x01;
            cpuRegisters[target] = cpuRegisters[target] >> 1;
            if (getFlag(CARRY_FLAG_BYTE_POSITION)) {
                cpuRegisters[target] |= 0x80;
            } else {
                cpuRegisters[target] &= ~0x80;
            }
            setFlag(false, SUBTRACT_FLAG_BYTE_POSITION);
            setFlag(false, HALF_CARRY_FLAG_BYTE_POSITION);
            setFlag(lastBit, CARRY_FLAG_BYTE_POSITION);
        }

        void rlInstruction(Target target) {
            byte lastBit = cpuRegisters[A] & 0x80;
            cpuRegisters[target] = cpuRegisters[target] << 1;
            if (getFlag(CARRY_FLAG_BYTE_POSITION)) {
                cpuRegisters[target] |= 0x01;
            } else {
                cpuRegisters[target] &= ~0x01;
            }

            setFlag(false, SUBTRACT_FLAG_BYTE_POSITION);
            setFlag(false, HALF_CARRY_FLAG_BYTE_POSITION);
            setFlag(lastBit, CARRY_FLAG_BYTE_POSITION);
        }

        void rrcInstruction(Target target) {
            byte lastBit = cpuRegisters[A] & 0x01;
            cpuRegisters[target] = cpuRegisters[target] >> 1;
            if (lastBit) {
                cpuRegisters[target] |= 0x80;
            } else {
                cpuRegisters[target] &= ~0x80;
            }

            setFlag(false, SUBTRACT_FLAG_BYTE_POSITION);
            setFlag(false, HALF_CARRY_FLAG_BYTE_POSITION);
        }

        void rlcInstruction(Target target) {
            byte lastBit = cpuRegisters[A] & 0x80;
            cpuRegisters[target] = cpuRegisters[target] << 1;
            if (lastBit) {
                cpuRegisters[target] |= 0x01;
            } else {
                cpuRegisters[target] &= ~0x01;
            }

            setFlag(false, SUBTRACT_FLAG_BYTE_POSITION);
            setFlag(false, HALF_CARRY_FLAG_BYTE_POSITION);
        }

        void rraInstruction() {
            rrInstruction(A);

            // Set zero flag
            setFlag(0, ZERO_FLAG_BYTE_POSITION);
        }

        void rlaInstruction() {
            rrInstruction(A);

            // Set zero flag
            setFlag(0, ZERO_FLAG_BYTE_POSITION);
        }

        void rrcaInstruction(Target target) {
            rrcInstruction(A);

            // Set zero flag
            setFlag(0, ZERO_FLAG_BYTE_POSITION);
        }

        void rlcaInstruction(Target target) {
            rlcInstruction(A);

            // Set zero flag
            setFlag(0, ZERO_FLAG_BYTE_POSITION);
        }

        void setInstruction(Target target, byte position) {
            cpuRegisters[target] |= 1 << position;
        }

        void resetInstruction(Target target, byte position) {
            cpuRegisters[target] &= ~(1 << position);
        }

        void srlInstruction(Target target) {
            cpuRegisters[target] = cpuRegisters[target] >> 1;
            cpuRegisters[target] &= ~0x80;

            setFlag(cpuRegisters[A] == 0, ZERO_FLAG_BYTE_POSITION);
            setFlag(false, SUBTRACT_FLAG_BYTE_POSITION);
            setFlag(false, HALF_CARRY_FLAG_BYTE_POSITION);
        }

        void sraInstruction(Target target) {
            cpuRegisters[target] = cpuRegisters[target] >> 1;
            
            setFlag(cpuRegisters[A] == 0, ZERO_FLAG_BYTE_POSITION);
            setFlag(false, SUBTRACT_FLAG_BYTE_POSITION);
            setFlag(false, HALF_CARRY_FLAG_BYTE_POSITION);
        }

        void slaInstruction(Target target) {
            cpuRegisters[target] = cpuRegisters[target] << 1;

            setFlag(cpuRegisters[A] == 0, ZERO_FLAG_BYTE_POSITION);
            setFlag(false, SUBTRACT_FLAG_BYTE_POSITION);
            setFlag(false, HALF_CARRY_FLAG_BYTE_POSITION);
        }

        void swapInstruction(Target target) {
            byte temp = cpuRegisters[target] & 0x0f;
            cpuRegisters[target] = cpuRegisters[target] >> 4;
            cpuRegisters[target] &= 0x0f;
            cpuRegisters[target] |= temp << 4; 


            setFlag(cpuRegisters[A] == 0, ZERO_FLAG_BYTE_POSITION);
            setFlag(false, SUBTRACT_FLAG_BYTE_POSITION);
            setFlag(false, HALF_CARRY_FLAG_BYTE_POSITION);
        }
        

        word getCPURegisterAsWord(Target targetA, Target targetB) {
            return (word)cpuRegisters[targetA] << 8 | (word)cpuRegisters[targetB];
        }

        void setCPURegisterAsWord(Target targetA, Target targetB, word value) {
            cpuRegisters[targetA] = (byte)((value & 0xff00) >> 8);
            cpuRegisters[targetB] = (byte)(value & 0x00ff);
        }
    };
}