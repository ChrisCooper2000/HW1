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

// Function to find base L levels down
int base(int BP, int L) {
    int arb = BP; // Start at the current base pointer
    while (L > 0) { // Traverse L levels down
        arb = PAS[arb]; // Follow the static link (SL)
        L--;
    }
    return arb; // Return the base pointer of the desired level
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
        PAS[ind++] = op; // Store operation code
        PAS[ind++] = l;  // Store lexical level
        PAS[ind++] = m;  // Store modifier
    }
    fclose(file);
}

// Execute instructions
void execute_cycle() {
    const char *opcodes[] = {
        "", "LIT", "OPR", "LOD", "STO", "CAL",
        "INC", "JMP", "JPC", "SYS"
    };
    
    // Print initial values of PC, BP, SP, and stack
    printf("PC BP  SP  stack\n");
    printf("Initial values: %2d %3d %3d\n", PC, BP, SP);

    while (halt) {
        // Fetch cycle: Load instruction into IR
        IR[0] = PAS[PC];
        IR[1] = PAS[PC + 1];
        IR[2] = PAS[PC + 2];
        PC += 3; // Increment PC by 3 to fetch the next instruction

        // Print the current instruction
        printf("%-3s %d %2d ", opcodes[IR[0]], IR[1], IR[2]);

        // Execute cycle: Perform operation based on opcode
        switch (IR[0]) {
            case 1: // LIT 0, M
                SP--;
                PAS[SP] = IR[2]; // Push literal M onto the stack
                break;
            case 2: // OPR 0, M
                switch (IR[2]) {
                    case 0: // RTN
                        SP = BP + 1; // Reset stack pointer
                        BP = PAS[SP - 2]; // Restore base pointer
                        PC = PAS[SP - 3]; // Return to the caller
                        break;
                    case 1: // ADD
                        PAS[SP + 1] += PAS[SP]; // Add top two stack values
                        SP++;
                        break;
                    case 3: // MUL
                        PAS[SP + 1] *= PAS[SP]; // Multiply top two stack values
                        SP++;
                        break;
                    case 5: // EQL
                        PAS[SP + 1] = (PAS[SP + 1] == PAS[SP]); // Compare equality
                        SP++;
                        break;
                }
                break;
            case 3: // LOD L, M
                SP--;
                PAS[SP] = PAS[base(BP, IR[1]) - IR[2]]; // Load value from stack
                break;
            case 4: // STO L, M
                PAS[base(BP, IR[1]) - IR[2]] = PAS[SP]; // Store value to stack
                SP++;
                break;
            case 6: // INC 0, M
                SP -= IR[2]; // Allocate M memory words
                break;
            case 7: // JMP 0, M
                PC = IR[2]; // Jump to instruction at address M
                break;
            case 8: // JPC 0, M
                if (PAS[SP] == 0) // If top stack value is 0
                    PC = IR[2]; // Jump to instruction at address M
                SP++;
                break;
            case 9: // SYS
                if (IR[2] == 1) { // Output top stack value
                    printf("\nOutput result is: %d\n", PAS[SP]);
                    SP++;
                } else if (IR[2] == 2) { // Read input from user
                    printf("\nPlease enter an integer: ");
                    scanf("%d", &PAS[SP - 1]);
                } else if (IR[2] == 3) { // Halt the program
                    halt = 0;
                }
                break;
        }

        // Print current state of PC, BP, SP, and stack
        printf("%2d %3d %3d ", PC, BP, SP);
        for (int i = 499; i >= SP; i--) {
            if (i == BP) printf("| "); // Mark base pointer
            printf("%d ", PAS[i]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <input file>\n", argv[0]);
        return 1;
    }
    
    load_program(argv[1]); // Load the program from file
    execute_cycle(); // Execute the loaded program
    
    return 0;
}


