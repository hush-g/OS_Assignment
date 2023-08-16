#include <stdio.h>

int main() {
    char c;
    int lines = 0, words = 0, characters = 0;
    int in_word = 0; // flag to track if we are inside a word

    while ((c = getchar()) != EOF) {
        characters++;

        if (c == '\n') {
            lines++;
        }

        // Check for word boundaries
        if (c == ' ' || c == '\t' || c == '\n') {
            in_word = 0;
        } else if (!in_word) {
            in_word = 1;
            words++;
        }
    }

    printf("%d %d %d\n", lines, words, characters);
    return 0;
}
