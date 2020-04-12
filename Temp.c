#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define STRING_LIMIT 100
enum TYPE
{
    Beginning,
    Match,
    Insertion,
    Deletion,
    Unstable,
    Ending
};
int ROW = 0, COLUMN = 0;

char *OUTPUTFILENAME = "/Users/kaantecik/CLionProjects/untitled2/output.json";
char *FILENAME = "/Users/kaantecik/CLionProjects/untitled2/data2.txt";
/*---------------- PHMM Modelling Section  ----------------*/
struct state
{
    enum TYPE type;
    // Match probability, Deletion probability, Insertion Probability
    float probabilities[3];
    //Every state have connection with other states
    struct state *next;
    struct state *next_states[3];
};

struct state *beginning_state = NULL;
struct state *temp_state = NULL;

/***
 *
 * This function inserts state to beginning.
 *
 * @author Kaan Tecik
 * @param new_state
 */
void insert_beginning(struct state *new_state)
{
    new_state->type = Beginning;
    beginning_state = new_state;
}

/***
 * This function inserts state to end.
 * @author Kaan Tecik
 * @param new_state
 */
void insert_end(struct state *new_state)
{
    if (beginning_state == NULL)
    {
        insert_beginning(new_state);
    }
    else
    {
        temp_state = beginning_state;

        while (temp_state->next != NULL)
        {
            temp_state = temp_state->next;
        }
        temp_state->next = new_state;
        new_state->type = Match;
    }
}

/***
 *
 * @author Kaan Tecik
 * @param count
 * @param column
 * @return
 */
float calculate_match_probability(int count, int row)
{
    return (float)(row - count) / row;
}

/***
 * @author Kaan Tecik
 * @param match_prob
 * @return
 */
float calculate_deletion_probability(float match_prob)
{
    return 1 - match_prob;
}

/***
 * @author Kaan Tecik
 * @param match_prob
 * @return
 */
float calculate_insertion_probability(float match_prob)
{
    return 1 - match_prob;
}

/***
 * @author Kaan Tecik
 * 3
 * @param array
 * @param column
 * @return length
 */
int get_unstable_column_length(int *array, int column)
{
    int count = 0;
    for (int i = 0; i < column; ++i)
    {
        if (array[i] > 2)
        {
            count++;
        }
    }
    return count;
}

/***
 *
 * @param data
 * @return
 */
char **get_nucleotide(char *data[ROW][COLUMN])
{
    char **string = malloc(sizeof(char *) * COLUMN);
    int numA = 0;
    int numG = 0;
    int numC = 0;
    int numT = 0;
    for (int i = 0; i < COLUMN; ++i)
    {
        for (int j = 0; j < ROW; ++j)
        {

            if (strcmp(data[j][i], "A") == 0)
            {
                numA++;
            }
            else if (strcmp(data[j][i], "G") == 0)
            {
                numG++;
            }
            else if (strcmp(data[j][i], "T") == 0)
            {
                numT++;
            }
            else if (strcmp(data[j][i], "C") == 0)
            {
                numC++;
            }
        }

        string[i] = malloc(sizeof(char) * STRING_LIMIT);
        snprintf(string[i], STRING_LIMIT, " A: %d,\n\t\t G: %d,\n\t\t C: %d,\n\t\t T: %d", numA, numG, numC, numT);
        numA = 0;
        numG = 0;
        numC = 0;
        numT = 0;
    }
    return string;
}

/**
 * @code :
 *           data : [s1- A,C,G,T,A,_,T
 *                   s2- A,_,G,T,A,_,T
 *                   s3- A,C,A,T,A,_,T
 *                   s4- A,C,G,T,A,_,T
 *                   s5- _,C,G,_,A,G,T ];
 *
 *           get_indexes(data, 2, 6);
 *           output: s2
 *
 * for 2nd column, output is s2
 * This function gets data and returns row num
 * which  column index is 'column_index'.
 * @author Oğuz Narlı
 * @param data
 * @param column_index
 * @param row
 * @returns char*
 */
int *get_indexes(char *data[ROW][COLUMN], int column_index)
{
    int *index = malloc(ROW * sizeof(int));
    int k = 0;
    for (int i = 0; i < ROW; ++i)
    {
        if (strcmp(data[i][column_index], "_") == 0)
        {
            index[k] = i;
            k++;
        }
    }
    return index;
}

int get_indexes_length(char *data[ROW][COLUMN], int column_index)
{
    int *index = malloc(ROW * sizeof(int));
    int k = 0;
    for (int i = 0; i < ROW; ++i)
    {
        if (strcmp(data[i][column_index], "_") == 0)
        {
            index[k] = i;
            k++;
        }
    }
    return k;
}

/***
 *
 * @author Kaan Tecik
 */
void print_all_data(char *data[COLUMN][ROW])
{
    char **strings = get_nucleotide(data);

    if (beginning_state == NULL)
    {
        printf("\n Listede Hic dugum yok, lutfen dugum ekleyin");
    }
    else
    {
        int check = 0, unstable_check = 0;
        temp_state = beginning_state;
        printf("{\n \"states\" : {\n");
        printf("\t\"state\" : [");
        while (temp_state != NULL)
        {
            int *indexes = get_indexes(data, check);

            if (temp_state->type == 4)
            {
                printf("\n\t{\n");
                printf("\t\t\"count\" : \"u%d\", \n"
                       "\t\t\"type\" : %d, \n"
                       "\t\t\"match-probability\": null, \n"
                       "\t\t\"deletion-probability\": null, \n "
                       "\t\t\"deletion-indexes\" : null",
                       unstable_check, temp_state->type);

                printf(",\n\t\t\"insertion-probability\": null \n\t },\n");
                unstable_check++;
            }
            else
            {
                if (temp_state->probabilities[1] != 0)
                {
                    printf("\n\t{\n");
                    printf(" \t\t\"count\" : \"m%d\", \n"
                           "\t\t\"type\" : %d, \n"
                           "\t\t\"match-probability\": %.2f, \n"
                           "\t\t\"deletion-probability\": %.2f,\n "
                           "\t\t\"deletion-indexes\" :[",
                           check, temp_state->type, temp_state->probabilities[0], temp_state->probabilities[1]);
                    for (int i = 0; i < get_indexes_length(data, check); ++i)
                    {
                        printf("\"s%d\", ", indexes[i] + 1);
                    }
                    printf("],\n");
                    printf("\n\t\telements: [ {\n\t\t%s", strings[check]);
                    printf("\n\t\t}],");
                    printf("\n\t\t\"insertion-probability\": %.2f \n\t },\n", temp_state->probabilities[2]);
                }

                else
                {
                    printf("\t{\n");
                    printf("\t\t\"count\" : \"m%d\", \n"
                           "\t\t\"type\" : %d, \n"
                           "\t\t\"match-probability\": %.2f, \n"
                           "\t\t\"deletion-probability\": %.2f,\n "
                           "\t\t\"deletion-indexes\" : null",
                           check, temp_state->type, temp_state->probabilities[0], temp_state->probabilities[1]);
                    printf(",\n\t\t\"elements\": [ {\n\t\t%s", strings[check]);
                    printf("\n\t\t}],");
                    printf("\n\t\t\"insertion-probability\": %.2f \n\t },\n", temp_state->probabilities[2]);
                }

                check++;
            }

            temp_state = temp_state->next;
        }

        printf("   ]\n");
        printf("  }\n");
        printf("}\n");
    }
}

/***
 * @author Kaan Tecik
 */
void write_file(char *data[ROW][COLUMN])
{
    FILE *dosya = fopen(OUTPUTFILENAME, "w");
    printf("Tüm stateler output.json dosyasına kaydediliyor..\n");
    int check = 0, unstable_check = 0;
    char **strings = get_nucleotide(data);
    temp_state = beginning_state;

    fprintf(dosya, "{\n \"states\" : {\n");
    fprintf(dosya, "\t\"state\" : [");
    while (temp_state != NULL)
    {
        int *indexes = get_indexes(data, check);

        if (temp_state->type == 4)
        {
            fprintf(dosya, "\n\t{\n");
            fprintf(dosya, "\t\t\"count\" : \"u%d\", \n"
                           "\t\t\"type\" : %d, \n"
                           "\t\t\"match-probability\": null, \n"
                           "\t\t\"deletion-probability\": null, \n "
                           "\t\t\"deletion-indexes\" : null",
                    unstable_check, temp_state->type);

            fprintf(dosya, ",\n\t\t\"insertion-probability\": null \n\t },\n");
            unstable_check++;
        }
        else
        {
            if (temp_state->probabilities[1] != 0)
            {
                fprintf(dosya, "\n\t{\n");
                fprintf(dosya, " \t\t\"count\" : \"m%d\", \n"
                               "\t\t\"type\" : %d, \n"
                               "\t\t\"match-probability\": %.2f, \n"
                               "\t\t\"deletion-probability\": %.2f,\n "
                               "\t\t\"deletion-indexes\" :[",
                        check, temp_state->type, temp_state->probabilities[0], temp_state->probabilities[1]);
                for (int i = 0; i < get_indexes_length(data, check); ++i)
                {
                    fprintf(dosya, "\"s%d\", ", indexes[i] + 1);
                }
                fprintf(dosya, "],\n");
                fprintf(dosya, "\n\t\telements: [ {\n\t\t%s", strings[check]);
                fprintf(dosya, "\n\t\t}],");
                fprintf(dosya, "\n\t\t\"insertion-probability\": %.2f \n\t },\n", temp_state->probabilities[2]);
            }

            else
            {
                fprintf(dosya, "\t{\n");
                fprintf(dosya, "\t\t\"count\" : \"m%d\", \n"
                               "\t\t\"type\" : %d, \n"
                               "\t\t\"match-probability\": %.2f, \n"
                               "\t\t\"deletion-probability\": %.2f,\n "
                               "\t\t\"deletion-indexes\" : null",
                        check, temp_state->type, temp_state->probabilities[0], temp_state->probabilities[1]);
                fprintf(dosya, ",\n\t\t\"elements\": [ {\n\t\t%s", strings[check]);
                fprintf(dosya, "\n\t\t}],");
                fprintf(dosya, "\n\t\t\"insertion-probability\": %.2f \n\t },\n", temp_state->probabilities[2]);
            }

            check++;
        }

        temp_state = temp_state->next;
    }

    fprintf(dosya, "   ]\n");
    fprintf(dosya, "  }\n");
    fprintf(dosya, "}\n");
    fclose(dosya);
}

int get_type(int index)
{
    struct state *pState = beginning_state;
    for (int i = 0; i < index; ++i)
    {
        pState = pState->next;
    }
    return pState->type;
}

int get_sequential_unstable_count()
{
    struct state *pState = beginning_state;
    int _count = 1;
    for (int i = 0; i < COLUMN; ++i)
    {
        if (pState->type == 4 && pState->next->type != 1 && pState->next->type != 5)
        {
            _count++;
        }
        pState = pState->next;
    }
    return _count;
}

bool isNucleo(char *data[ROW][COLUMN], int _row, int hold)
{
    return (strcmp(data[_row][hold], "A") == 0 ||
            strcmp(data[_row][hold], "T") == 0 ||
            strcmp(data[_row][hold], "G") == 0 ||
            strcmp(data[_row][hold], "C") == 0);
}

bool isUnderscore(char *data[ROW][COLUMN], int _row, int hold)
{
    return strcmp(data[_row][hold], "_") == 0;
}

bool isMatch(int hold)
{
    return get_type(hold) == 1;
}

bool isUnstable(int hold)
{
    return get_type(hold) == 4;
}

bool isEnding(int hold)
{
    return get_type(hold) == 5;
}

void calculationManager(char *data[ROW][COLUMN], int *unstable_array, int *unstable_index)
{
    int check = 0;
    //initial
    temp_state = beginning_state;
    while (check <= COLUMN)
    {
        if ((temp_state->next->type == Match && temp_state->type != Unstable) || check == COLUMN)
        {
            temp_state->probabilities[0] = calculate_match_probability(unstable_array[check], ROW - unstable_array[check - 1]);
            if (temp_state->probabilities[0] == 1)
            {
                temp_state->probabilities[1] = 0;
                temp_state->probabilities[2] = 0;
                temp_state->next_states[1] = NULL;
                temp_state->next_states[2] = NULL;
            }
            else
            {
                temp_state->probabilities[1] = calculate_deletion_probability(temp_state->probabilities[0]);
                temp_state->probabilities[2] = 0;
                temp_state->next_states[2] = NULL;
            }
        }
        else if (temp_state->next->type == 4 && temp_state->next->next->type == 4)
        {
            int hold = check;
            float _insertionCount = 0, _deletionCount = 0, _matchCount = 0;
            int _row;
            int _sequentialUnstableCount = get_sequential_unstable_count();
            int _wall = hold + _sequentialUnstableCount + 1;

            while (isUnstable(hold + 1))
            {
                for (_row = 0; _row < ROW; ++_row)
                {
                    for (hold = check; hold < _wall; ++hold)
                    {

                        if (isNucleo(data, _row, hold - 1) && isUnderscore(data, _row, hold) && isMatch(hold + 1))
                        {
                            _deletionCount++;
                            printf("|Deletion index| %s %s %.1f \n", data[_row][hold - 1], data[_row][hold], _deletionCount);
                        }

                        if (isNucleo(data, _row, hold - 1) && isUnderscore(data, _row, hold) && isUnstable(hold + 1))
                        {
                            printf("|_ ignored| %s %s 0\n", data[_row][hold - 1], data[_row][hold]);
                        }

                        if (isNucleo(data, _row, hold - 1) && isNucleo(data, _row, hold) && isUnstable(hold + 1))
                        {
                            _insertionCount++;
                            printf("|Insertion | %s %s %.1f\n", data[_row][hold - 1], data[_row][hold], _insertionCount);
                        }

                        if (isNucleo(data, _row, hold - 1) && isNucleo(data, _row, hold) && (isMatch(hold + 1)))
                        {
                            _matchCount++;
                            printf("|Match | %s %s %.1f\n", data[_row][hold - 1], data[_row][hold], _matchCount);
                        }

                        if (isUnderscore(data, _row, hold - 1) && isNucleo(data, _row, hold) && isUnstable(hold + 1))
                        {
                            _insertionCount++;
                            printf("|Insertion | %s %s %.1f\n", data[_row][hold - 1], data[_row][hold], _insertionCount);
                        }
                        if (isUnderscore(data, _row, hold - 1) && isNucleo(data, _row, hold) && isMatch(hold + 1))
                        {
                            _matchCount++;
                            printf("|Match index| %s %s %.1f\n", data[_row][hold - 1], data[_row][hold], _matchCount);
                        }

                        if (isUnderscore(data, _row, hold - 1) && isUnderscore(data, _row, hold) && isMatch(hold + 1))
                        {
                            _deletionCount++;
                            printf("|Deletion | %s %s %.1f\n", data[_row][hold - 1], data[_row][hold], _deletionCount);
                        }

                        if (isUnderscore(data, _row, hold - 1) && isUnderscore(data, _row, hold) && isUnstable(hold + 1))
                        {
                            printf("|_ ignored| %s %s 0\n", data[_row][hold - 1], data[_row][hold]);
                        }
                    }
                }
            }

            printf("%d match %d insertion %d deletion", (int)_matchCount, (int)_insertionCount, (int)_deletionCount);
            float _sum = _matchCount + _deletionCount + _insertionCount;
            float _unstable_match_prob = (float)(3 / _sum);
            float _unstable_insertion_prob = (float)(3 / _sum);
            float _unstable_deletion_prob = (float)(2 / _sum);
            temp_state->probabilities[0] = _unstable_match_prob;
            temp_state->probabilities[1] = _unstable_deletion_prob;
            temp_state->probabilities[2] = _unstable_insertion_prob;
            printf("\n%.3f match %.3f insertion %.3f deletion\n", _unstable_match_prob, _unstable_insertion_prob, _unstable_deletion_prob);
        }

        else if (temp_state->next->type == 4)
        {
            temp_state->probabilities[0] = calculate_match_probability(ROW - unstable_array[check], ROW - unstable_array[check - 1]);
            temp_state->probabilities[2] = calculate_insertion_probability(temp_state->probabilities[0]);
        }

        temp_state = temp_state->next;
        check++;
    }
}

/***
 * @author Oğuz Narlı
 * get_data()
 * @param fp
 * @return
*/

void get_data(char *filename, char *data[ROW][COLUMN])
{
    FILE *fp;
    fp = fopen(filename, "r");
    char line[150];
    char *nucleotide;
    int _row = 0, _col = 0;

    for (_row = 0; _row < ROW; ++_row)
    {
        _col = 0;
        fgets(line, sizeof(line), fp);
        //printf("\nline : %s", line);

        nucleotide = strtok(line, ",");
        if (strcmp(nucleotide, "\n") != 0)
        {
            data[_row][_col] = strdup(nucleotide);
            //printf("nucleotid : %s | data : %s | col :%d\n", nucleotid, data[_row][_col], _col);
            _col++;
        }
        nucleotide = strtok(NULL, ",");

        while (nucleotide != NULL && strcmp(nucleotide, "\n") != 0)
        {
            data[_row][_col] = strdup(nucleotide);
            //printf("nucleotid : %s | data : %s | col :%d\n", nucleotid, data[_row][_col], _col);
            _col++;
            nucleotide = strtok(NULL, ",");
        }
        //printf("Endline");
    }
    fclose(fp);
}

/***
 * @author Kaan Tecik
 * @param shape : col 10 row 6
 * @param unstable_array : 0 0 3 0 3 3 1 0 1 0
 * @param unstable_index : 3 5 6
 */
void createModel(char *data[ROW][COLUMN], int *unstable_array, int *unstable_index)
{
    int check = 0;
    int unstable_length = get_unstable_column_length(unstable_array, COLUMN);
    printf(" \n%d unstable column\n", unstable_length);
    while (check <= COLUMN + 1)
    {
        struct state *new_state = (struct state *)malloc(sizeof(struct state));
        if (check == 0)
        {
            insert_beginning(new_state);
            new_state->probabilities[0] = calculate_match_probability(unstable_array[check], ROW);
            if (new_state->probabilities[0] == 1)
            {
                new_state->probabilities[1] = 0;
                new_state->probabilities[2] = 0;
                new_state->next_states[1] = NULL;
                new_state->next_states[2] = NULL;
            }
            else
            {
                new_state->probabilities[1] = calculate_deletion_probability(new_state->probabilities[0]);
                new_state->probabilities[2] = 0;
                new_state->next_states[2] = NULL;
            }
        }
        else
        {
            insert_end(new_state);
            for (int i = 0; i < unstable_length; ++i)
            {
                if (check == unstable_index[i])
                {
                    new_state->type = Unstable;
                    new_state->probabilities[0] = 0;
                    new_state->probabilities[1] = 0;
                    new_state->probabilities[2] = 0;
                    i = unstable_length - 1;
                }
            }
        }
        if (check == COLUMN + 1)
        {
            new_state->type = Ending;
            new_state->probabilities[0] = 0;
            new_state->probabilities[1] = 0;
            new_state->probabilities[2] = 0;
            new_state->next_states[0] = NULL;
            new_state->next_states[1] = NULL;
            new_state->next_states[2] = NULL;
        }
        check++;
    }
    calculationManager(data, unstable_array, unstable_index);
    print_all_data(data);
    printf("\n");
    for (int i = 0; i < ROW; ++i)
    {
        for (int j = 0; j < COLUMN; ++j)
        {
            printf("%s\t", data[i][j]);
        }
        printf("\n");
    }
}

/*---------------- PHMM Modelling Ends  ----------------*/

/*---------------- Read File Section ----------------*/

/***
 *
 * This function gets a file pointer as parameter
 * reads it and returns its shape which are column and row.
 * @author Oğuz Narlı
 * @example
 *          int *shape;
 *          shape = get_shape(fp);
 *          shape[0] -> col: 10
 *          shape[1] -> row: 6
 * @param fp
 * @return int*
 */
void get_shape(FILE *fp)
{
    char ch;
    while (!feof(fp))
    {
        ch = fgetc(fp);
        if (ch != ',')
        {
            COLUMN++;
        }
        if (ch == '\n')
        {
            ROW++;
        }
    }
    ROW++;
    COLUMN = (COLUMN - ROW) / ROW;
}

/***
 * @author Oğuz Narlı
 * 3 5 6
 * @param filename
 * @param column
 * @param row
 * @return
 *
 */
int *get_unstable_indexes(char *filename, int column, int row)
{
    FILE *fp;
    char cr;
    int i, j;
    int *unstable_count = (int *)malloc(column * sizeof(int));
    int *unstable_row_indexes = (int *)malloc(column * sizeof(int));
    for (i = 0; i < column; i++)
    {
        unstable_count[i] = 0;
    }
    fp = fopen(filename, "r");
    while (!feof(fp))
    {
        for (i = 0; i < row; i++)
        {
            for (j = 0; j < column; j++)
            {
                cr = getc(fp);
                if (cr == ',' || cr == '\n')
                {
                    j--;
                }
                else if (cr == '_')
                {
                    unstable_count[j] += 1;
                }
            }
        }
    }
    j = 0;
    for (i = 0; i < column; i++)
    {
        if (unstable_count[i] > 2)
        {
            unstable_row_indexes[j] = i + 1;
            j++;
        }
    }
    fclose(fp);

    return unstable_row_indexes;
}

/**
 * This function reads the file, creates new integer
 * array[column] that stores '_' counts.
 *
 * @example:
 *         filename  = data.txt
 *         column = shape[0]
 *         row = shape[1]
 *
 *         get_unstable_array(filename, shape[0], shape[1]);
 *         output : 1 1 0 1 0 4 0
 * @author Oğuz Narlı
 * @param filename
 * @param column
 * @param row
 * @returns int*
 */
int *get_unstable_array(char *filename, int column, int row)
{
    FILE *fp;
    char cr;
    int i, j;
    int *unstable_count = (int *)malloc(column * sizeof(int));
    int *unstable_row_indexes = (int *)malloc(column * sizeof(int));
    for (i = 0; i < column; i++)
    {
        unstable_count[i] = 0;
    }
    fp = fopen(filename, "r");
    while (!feof(fp))
    {
        for (i = 0; i < row; i++)
        {
            for (j = 0; j < column; j++)
            {
                cr = getc(fp);
                if (cr == ',' || cr == '\n')
                {
                    j--;
                }
                else if (cr == '_')
                {
                    unstable_count[j] += 1;
                }
            }
        }
    }
    j = 0;
    for (i = 0; i < column; i++)
    {
        if (unstable_count[i] > 2)
        {
            unstable_row_indexes[j] = i + 1;
            j++;
        }
    }
    fclose(fp);

    return unstable_count;
}

/**
 * @param filename
 */
//char *data[ROW][COLUMN] parametre örnek kullanım
void App(char *filename)
{
    FILE *fp;
    if ((fp = fopen(filename, "r")) == NULL)
    {
        printf("An error occured.");
        exit(1);
    }
    else
    {
        printf("Reading file... \n");
        get_shape(fp);
        printf("row: %d, column: %d ", ROW, COLUMN);
        char *data[ROW][COLUMN];
        get_data(FILENAME, data);
        // returns _ count for each column  0 0 1 4...
        int *unstable_array = get_unstable_array(filename, COLUMN, ROW);
        // returns indexes which _ count is more than 2
        int *unstable_indexes = get_unstable_indexes(filename, COLUMN, ROW);
        createModel(data, unstable_array, unstable_indexes);
        write_file(data);
    }
}

int main(void)
{
    App("data.txt");
    return 0;
}