// P-Machine (HW1)
// Christopher Cooper


#include <stdio.h>
#include <stdlib.h>

int PAS[500] = {0};
int BP = 499;
int SP = 500;
int PC = 10;
int IR[3] = {0};
int halt = 1;

// base function 
int base(int BP, int L) {
    int arb = BP;
    while (L > 0) {
        arb = PAS[arb];
        L--;
    }
    return arb;
}

// load program
void load_program(char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        exit(1);
    }

    int op, l, m;
    int index = 10;
    while (fscanf(file, "%d %d %d", &op, &l, &m) != EOF) {
        PAS[index++] = op;
        PAS[index++] = l;
        PAS[index++] = m;
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <input file>\n", argv[0]);
        return 1;
    }

    load_program(argv[1]);

    const char *opnames[] = {
        "", "INC", "OPR", "LOD", "STO", "CAL",
        "LIT", "JMP", "JPC", "SYS"
    };

    // aligned headers
    printf("          %-8s%-5s%-5s%-6s%s\n", "OP", "PC", "BP", "SP", "stack");
    printf("Initial values:      %3d  %3d  %4d\n", PC, BP, SP);

    while (halt) {
        // fetch
        IR[0] = PAS[PC];
        IR[1] = PAS[PC + 1];
        IR[2] = PAS[PC + 2];
        PC += 3;

        // print instruction
        printf("%-10s%-8d", opnames[IR[0]], IR[1]);
        printf("%-5d%-5d", PC, BP);
        printf("%-6d", SP);

        // execute
        if (IR[0] == 6) {
            SP--;
            PAS[SP] = IR[2];
        } else if (IR[0] == 2) {
            if (IR[2] == 0) {
                int tempBP = BP;
                PC = PAS[SP - 3];
                BP = PAS[SP - 2];
                SP = tempBP + 1;
            } else if (IR[2] == 1) PAS[SP + 1] = PAS[SP + 1] + PAS[SP], SP++;
            else if (IR[2] == 2) PAS[SP + 1] = PAS[SP + 1] - PAS[SP], SP++;
            else if (IR[2] == 3) PAS[SP + 1] = PAS[SP + 1] * PAS[SP], SP++;
            else if (IR[2] == 4) PAS[SP + 1] = PAS[SP + 1] / PAS[SP], SP++;
            else if (IR[2] == 5) PAS[SP + 1] = PAS[SP + 1] == PAS[SP], SP++;
            else if (IR[2] == 6) PAS[SP + 1] = PAS[SP + 1] != PAS[SP], SP++;
            else if (IR[2] == 7) PAS[SP + 1] = PAS[SP + 1] < PAS[SP], SP++;
            else if (IR[2] == 8) PAS[SP + 1] = PAS[SP + 1] <= PAS[SP], SP++;
            else if (IR[2] == 9) PAS[SP + 1] = PAS[SP + 1] > PAS[SP], SP++;
            else if (IR[2] == 10) PAS[SP + 1] = PAS[SP + 1] >= PAS[SP], SP++;
        } else if (IR[0] == 3) {
            SP--;
            PAS[SP] = PAS[base(BP, IR[1]) - IR[2]];
        } else if (IR[0] == 4) {
            PAS[base(BP, IR[1]) - IR[2]] = PAS[SP];
            SP++;
        } else if (IR[0] == 5) {
            PAS[SP - 1] = base(BP, IR[1]);
            PAS[SP - 2] = BP;
            PAS[SP - 3] = PC;
            BP = SP - 1;
            PC = IR[2];
        } else if (IR[0] == 1) {
            SP -= IR[2];
        } else if (IR[0] == 7) {
            PC = IR[2];
        } else if (IR[0] == 8) {
            if (PAS[SP] == 0) PC = IR[2];
            SP++;
        } else if (IR[0] == 9) {
            if (IR[2] == 1) {
                printf("\nOutput result is: %d\n", PAS[SP]);
                SP++;
            } else if (IR[2] == 2) {
                printf("\nPlease enter an integer: ");
                SP--;
                scanf("%d", &PAS[SP]);
            } else if (IR[2] == 3) {
                halt = 0;
            }
        }

        // activation record markers
        int tempBP = BP;
        int arFlags[500] = {0};
        while (tempBP >= SP && tempBP < 500) {
            arFlags[tempBP] = 1;
            tempBP = PAS[tempBP];
        }

        for (int i = 499; i >= SP; i--) {
            if (arFlags[i]) printf("| ");
            printf("%2d ", PAS[i]);
        }

        printf("\n");
    }

    return 0;
}



