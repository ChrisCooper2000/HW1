// P-Machine (HW1)
// Christopher Cooper

#include <stdio.h>
#include <stdlib.h>

#define ARR_SIZE 500
#define TEXT_START 10


// Registers
int BP = 499, SP = 500, PC = TEXT_START;
int IR[3] = {0};
int PAS[ARR_SIZE] = {0};
int halt = 1;


int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <input file>\n", argv[0]);
        return 1;
    }
    
    load_program(argv[1]);
    execute_cycle();
    
    return 0;
}

// Load instructions from input file
void load_program(char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        exit(1);
    }
    
    int op, l, m, ind = TEXT_START;
    while (fscanf(file, "%d %d %d", &op, &l, &m) != EOF) {
        PAS[ind++] = op;
        PAS[ind++] = l;
        PAS[ind++] = m;
    }
    fclose(file);
}

// Execute instructions
void execute_cycle() {

    const char *opcodes[] = {
    "", "LIT", "OPR", "LOD", "STO", "CAL",
    "INC", "JMP", "JPC", "SYS"};
    
    while (halt) {
        // Fetch cycle
        IR[0] = PAS[PC];
        IR[1] = PAS[PC + 1];
        IR[2] = PAS[PC + 2];
        PC += 3;

        // Print current instruction
        printf("%s %d %d ", opcodes[IR[0]], IR[1], IR[2]);

        // Execute cycle
        switch (IR[0]) {
            case 1: // LIT 0, M
                SP--;
                PAS[SP] = IR[2];
                break;
            case 2: // OPR 0, M
                switch (IR[2]) {
                    case 0: // RTN
                        SP = BP + 1;
                        BP = PAS[SP - 2];
                        PC = PAS[SP - 3];
                        break;
                    case 1: // ADD
                        PAS[SP + 1] += PAS[SP];
                        SP++;
                        break;
                    case 3: // MUL
                        PAS[SP + 1] *= PAS[SP];
                        SP++;
                        break;
                    case 5: // EQL
                        PAS[SP + 1] = (PAS[SP + 1] == PAS[SP]);
                        SP++;
                        break;
                }
                break;
            case 3: // LOD L, M
                SP--;
                PAS[SP] = PAS[base(BP, IR[1]) - IR[2]];
                break;
            case 4: // STO L, M
                PAS[base(BP, IR[1]) - IR[2]] = PAS[SP];
                SP++;
                break;
            case 6: // INC 0, M
                SP -= IR[2];
                break;
            case 7: // JMP 0, M
                PC = IR[2];
                break;
            case 8: // JPC 0, M
                if (PAS[SP] == 0)
                    PC = IR[2];
                SP++;
                break;
            case 9: // SYS
                if (IR[2] == 1) {
                    printf("Output result is: %d\n", PAS[SP]);
                    SP++;
                } else if (IR[2] == 2) {
                    printf("Please enter an integer: ");
                    scanf("%d", &PAS[SP - 1]);
                    SP--; // Adjust stack pointer
                } else if (IR[2] == 3) {
                    halt = 0;
                }
                break;
        }

        // Print current stack state
        printf("%2d %3d %3d ", PC, BP, SP);
        for (int i = 499; i >= SP; i--) {
            if (i == BP) printf("| "); // Separate activation records
            printf("%d ", PAS[i]);
        }
        printf("\n");
    }
}



// Function to find base L levels down
int base(int BP, int L) {
    int arb = BP;
    while (L > 0) {
        arb = PAS[arb];
        L--;
    }
    return arb;
}