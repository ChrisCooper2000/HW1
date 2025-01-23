#include <stdio.h>
#include <stdlib.h>

#define ARRAY_SIZE 500

// CPU Registers
int pas[ARRAY_SIZE] = {0}; // Process address space
int PC = 10;              // Program Counter
int BP = 499;             // Base Pointer
int SP = 500;             // Stack Pointer
int IR[3] = {0};          // Instruction Register

// Function to find base
int base(int BP, int L) {
    int arb = BP;
    while (L > 0) {
        arb = pas[arb];
        L--;
    }
    return arb;
}

// Fetch and Execute Cycle
void fetch_and_execute() {
    int running = 1;

    printf("Initial values: PC=%d BP=%d SP=%d\n", PC, BP, SP);

    while (running) {
        // Fetch Cycle
        IR[0] = pas[PC];      // OP
        IR[1] = pas[PC + 1];  // L
        IR[2] = pas[PC + 2];  // M
        PC += 3;

        // Execute Cycle
        switch (IR[0]) {
            case 1: // LIT: Push literal to stack
                SP--;
                pas[SP] = IR[2];
                break;
            case 2: // OPR: Perform operation or return
                switch (IR[2]) {
                    case 0: // RTN: Return from subroutine
                        SP = BP + 1;
                        BP = pas[SP - 2];
                        PC = pas[SP - 3];
                        break;
                    case 1: // ADD
                        pas[SP + 1] += pas[SP];
                        SP++;
                        break;
                    case 2: // SUB
                        pas[SP + 1] -= pas[SP];
                        SP++;
                        break;
                    case 3: // MUL
                        pas[SP + 1] *= pas[SP];
                        SP++;
                        break;
                    case 4: // DIV
                        pas[SP + 1] /= pas[SP];
                        SP++;
                        break;
                    case 5: // EQL
                        pas[SP + 1] = (pas[SP + 1] == pas[SP]);
                        SP++;
                        break;
                    case 6: // NEQ
                        pas[SP + 1] = (pas[SP + 1] != pas[SP]);
                        SP++;
                        break;
                    case 7: // LSS
                        pas[SP + 1] = (pas[SP + 1] < pas[SP]);
                        SP++;
                        break;
                    case 8: // LEQ
                        pas[SP + 1] = (pas[SP + 1] <= pas[SP]);
                        SP++;
                        break;
                    case 9: // GTR
                        pas[SP + 1] = (pas[SP + 1] > pas[SP]);
                        SP++;
                        break;
                    case 10: // GEQ
                        pas[SP + 1] = (pas[SP + 1] >= pas[SP]);
                        SP++;
                        break;
                }
                break;
            case 3: // LOD: Load value to stack
                SP--;
                pas[SP] = pas[base(BP, IR[1]) - IR[2]];
                break;
            case 4: // STO: Store value from stack
                pas[base(BP, IR[1]) - IR[2]] = pas[SP];
                SP++;
                break;
            case 5: // CAL: Call procedure
                pas[SP - 1] = base(BP, IR[1]); // Static link
                pas[SP - 2] = BP;              // Dynamic link
                pas[SP - 3] = PC;              // Return address
                BP = SP - 1;
                PC = IR[2];
                break;
            case 6: // INC: Increment stack pointer
                SP -= IR[2];
                break;
            case 7: // JMP: Jump to address
                PC = IR[2];
                break;
            case 8: // JPC: Conditional jump
                if (pas[SP] == 0)
                    PC = IR[2];
                SP++;
                break;
            case 9: // SYS: System operations
                if (IR[2] == 1) {
                    // Print top of stack
                    printf("Output result is: %d\n", pas[SP]);
                    SP++;
                } else if (IR[2] == 2) {
                    // Input value
                    SP--;
                    printf("Please Enter an Integer: ");
                    scanf("%d", &pas[SP]);
                } else if (IR[2] == 3) {
                    // End of program
                    running = 0;
                }
                break;
        }

        // Print the current state
        printf("PC=%d BP=%d SP=%d | Stack: ", PC, BP, SP);
        for (int i = SP; i < 500; i++) {
            if (i == BP)
                printf("| ");
            printf("%d ", pas[i]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <input file>\n", argv[0]);
        return 1;
    }

    // Open the input file
    FILE *file = fopen(argv[1], "r");
    if (!file) {
        printf("Error: Cannot open file %s\n", argv[1]);
        return 1;
    }

    // Load instructions into PAS starting at index 10
    int i = 10;
    while (fscanf(file, "%d %d %d", &pas[i], &pas[i + 1], &pas[i + 2]) != EOF) {
        i += 3;
    }
    fclose(file);

    // Run the fetch and execute cycle
    fetch_and_execute();

    return 0;
}
