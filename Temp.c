#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*---------------- PHMM Modelling Section  ----------------*/
struct state
{
    // M for Match, I for Insertion, D for Deletion, U for Unstable
    char type;
    // Match probability, Insertion probability, Deletion Probability
    float probabilities[3];
    //Every state have connection with other states
    struct state *matchstate;
    struct state *insertionstate;
    struct state *deletionstate;
};

// will use in ReadFile()
int calculateprobablity(int count, int row)
{
    return count / row;
}

/*---------------- PHMM Modelling Ends  ----------------*/

/*---------------- Read File Section ----------------*/

void ReadFile(char *filename)
{

    // read dna sequence and get n row, m column
    // check columns and define match states ( if row data is '-' more than 2; set column to unstable)
    FILE *fp;
    if ((fp = fopen(filename, "r")) == NULL)
    {
        printf("An error occured.");
        exit(1);
    }
    else
    {

        fclose(fp);
    }
}
/*---------------- Read File Section Ends ----------------*/

int main()
{
    //char a[7][6] = {{'T', 'C', '_', 'T', 'G', 'G'}, {'G', 'G', 'A', 'A', 'G', 'A'}, {'A', '_', '_', 'T', '_', 'T'}, {'C', 'T', 'C', 'C', '_', 'T'}, {'_', 'T', 'G', 'C', 'G', '_'}, {'T', 'T', 'T', 'C', 'A', 'G'}};

    //checkColumns(a);

    ReadFile("data.txt");
}