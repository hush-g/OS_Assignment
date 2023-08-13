#define MAX_HISTORY_SIZE 100
char history[MAX_HISTORY_SIZE][1024];
int history_count = 0;

void history_command() {
    for (int i = 0; i < history_count; i++) {
        printf("%d: %s\n", i + 1, history[i]);
    }
}

void add_to_history(char *command) {
    if (history_count < MAX_HISTORY_SIZE) {
        strcpy(history[history_count], command);
        history_count++;
    } else {
        // Shift history to make space for the new command
        for (int i = 1; i < history_count; i++) {
            strcpy(history[i - 1], history[i]);
        }
        strcpy(history[history_count - 1], command);
    }
}
