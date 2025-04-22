#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "spell.h"

#define MAX_WORD_SIZE 101  // 100 chars + '\0'
#define MAX_DICT_SIZE 20000

// Θ(T * D * MAX_LEN^2)
// T = number of test words
// D = number of dictionary words
// MAX_LEN = maximum word length

int min(int a, int b, int c) {
    int m = a < b ? a : b;
    return m < c ? m : c;
}

void print_table(char *first, char *second, int **table, int m, int n) {
    printf("    ");
    for (int j = 0; j < n; j++)
        printf("  %c", second[j]);
    printf("\n");

    printf("   ");
    for (int j = 0; j <= n; j++)
        printf("----");
    printf("\n");

    for (int i = 0; i <= m; i++) {
        if (i == 0) printf("  ");
        else printf("%c ", first[i - 1]);

        for (int j = 0; j <= n; j++) {
            printf("|%2d", table[i][j]);
        }
        printf("|\n");

        printf("   ");
        for (int j = 0; j <= n; j++)
            printf("----");
        printf("\n");
    }

    printf("Edit Distance: %d\n", table[m][n]);
}

int edit_distance(char *first_str, char *second_str, int print_on) {
    int m = strlen(first_str);
    int n = strlen(second_str);

    int **table = (int **)malloc((m + 1) * sizeof(int *));
    for (int i = 0; i <= m; i++)
        table[i] = (int *)calloc(n + 1, sizeof(int));

    for (int i = 0; i <= m; i++)
        table[i][0] = i;
    for (int j = 0; j <= n; j++)
        table[0][j] = j;

    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (first_str[i - 1] == second_str[j - 1])
                table[i][j] = table[i - 1][j - 1];
            else
                table[i][j] = 1 + min(table[i - 1][j], table[i][j - 1], table[i - 1][j - 1]);
        }
    }

    if (print_on == 1)
        print_table(first_str, second_str, table, m, n);

    int result = table[m][n];

    for (int i = 0; i <= m; i++)
        free(table[i]);
    free(table);

    return result;
}

char **load_words(FILE *fp, int *count) {
    fscanf(fp, "%d", count);
    char **words = (char **)malloc((*count) * sizeof(char *));
    for (int i = 0; i < *count; i++) {
        words[i] = (char *)malloc(MAX_WORD_SIZE);
        fscanf(fp, "%s", words[i]);
    }
    return words;
}

void spell_check(char *dictname, char *testname) {
    FILE *f_dict = fopen(dictname, "r");
    if (!f_dict) {
        printf("Could not open dictionary file.\n");
        return;
    }

    FILE *f_test = fopen(testname, "r");
    if (!f_test) {
        printf("Could not open test file.\n");
        fclose(f_dict);
        return;
    }

    int D, T;
    char **dictionary = load_words(f_dict, &D);
    char **test_words = load_words(f_test, &T);

    for (int i = 0; i < T; i++) {
        printf("---> |%s|\n", test_words[i]);

        int *distances = (int *)malloc(D * sizeof(int));
        int min_dist = MAX_WORD_SIZE;

        for (int j = 0; j < D; j++) {
            distances[j] = edit_distance(test_words[i], dictionary[j], 0);
            if (distances[j] < min_dist)
                min_dist = distances[j];
        }

        printf("-1 - type correction\n");
        printf(" 0 - leave word as is (do not fix spelling)\n");
        printf("     Minimum distance: %d\n", min_dist);
        printf("     Words that give minimum distance:\n");

        int index_map[D];
        int count = 0;

        for (int j = 0; j < D; j++) {
            if (distances[j] == min_dist) {
                index_map[count] = j;
                printf(" %d - %s\n", count + 1, dictionary[j]);
                count++;
            }
        }

        printf("Enter your choice (from the above options): ");
        int choice;
        scanf("%d", &choice);

        char corrected[MAX_WORD_SIZE];
        if (choice == -1) {
            printf("Type the correct word: ");
            scanf("%s", corrected);
        } else if (choice == 0) {
            strcpy(corrected, test_words[i]);
        } else if (choice > 0 && choice <= count) {
            strcpy(corrected, dictionary[index_map[choice - 1]]);
        } else {
            printf("Invalid option. Using original word.\n");
            strcpy(corrected, test_words[i]);
        }

        printf("The corrected word is: %s\n", corrected);
        free(distances);
    }

    for (int i = 0; i < D; i++) free(dictionary[i]);
    free(dictionary);
    for (int i = 0; i < T; i++) free(test_words[i]);
    free(test_words);
    fclose(f_dict);
    fclose(f_test);
}
