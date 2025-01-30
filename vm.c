// P-Machine (HW1)
// Christopher Cooper

#include <stdio.h>
#include <stdlib.h>

#define ARRAY_SIZE 500
#define TEXT_START 10

// Registers
int BP = 499, SP = 500, PC = TEXT_START;
int IR[3] = {0};
int PAS[ARRAY_SIZE] = {0};
int halt = 1;

// Function to find base L levels down
int base(int BP, int L) {
    int arb = BP;
    while (L > 0) {
        arb = PAS[arb];
        L--;
    }
    return arb;
}

// Load instructions from input file
void load_program(char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        exit(1);
    }
    
    int op, l, m, index = TEXT_START;
    while (fscanf(file, "%d %d %d", &op, &l, &m) != EOF) {
        PAS[index++] = op;
        PAS[index++] = l;
        PAS[index++] = m;
    }
    fclose(file);
}

// Execute instructions
void execute_cycle() {
    while (halt) {
        // Fetch cycle
        IR[0] = PAS[PC];
        IR[1] = PAS[PC + 1];
        IR[2] = PAS[PC + 2];
        PC += 3;

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
                    case 2: // SUB
                        PAS[SP + 1] -= PAS[SP];
                        SP++;
                        break;
                    case 3: // MUL
                        PAS[SP + 1] *= PAS[SP];
                        SP++;
                        break;
                    case 4: // DIV
                        PAS[SP + 1] /= PAS[SP];
                        SP++;
                        break;
                    case 5: // EQL
                        PAS[SP + 1] = (PAS[SP + 1] == PAS[SP]);
                        SP++;
                        break;
                    case 6: // NEQ
                        PAS[SP + 1] = (PAS[SP + 1] != PAS[SP]);
                        SP++;
                        break;
                    case 7: // LSS
                        PAS[SP + 1] = (PAS[SP + 1] < PAS[SP]);
                        SP++;
                        break;
                    case 8: // LEQ
                        PAS[SP + 1] = (PAS[SP + 1] <= PAS[SP]);
                        SP++;
                        break;
                    case 9: // GTR
                        PAS[SP + 1] = (PAS[SP + 1] > PAS[SP]);
                        SP++;
                        break;
                    case 10: // GEQ
                        PAS[SP + 1] = (PAS[SP + 1] >= PAS[SP]);
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
            case 5: // CAL L, M
                PAS[SP - 1] = base(BP, IR[1]);
                PAS[SP - 2] = BP;
                PAS[SP - 3] = PC;
                BP = SP - 1;
                PC = IR[2];
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
                    SP--; // Move SP down to allocate space for input
                    scanf("%d", &PAS[SP]); // Store input at correct stack location
                } else if (IR[2] == 3) {
                    halt = 0;
                }
                break;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <input file>\n", argv[0]);
        return 1;
    }
    
    load_program(argv[1]);
    execute_cycle();
    
    return 0;
}
