

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TASKS 100
#define MAX_LEN 256
#define DATA_FILE "tasks.txt"

typedef struct {
    char text[MAX_LEN];
    int done;
} Task;

static Task tasks[MAX_TASKS];
static int task_count = 0;

/* Remove trailing newline from a string. */
static void trim_newline(char *s) {
    size_t n = strlen(s);
    if (n > 0 && s[n - 1] == '\n') {
        s[n - 1] = '\0';
    }
}

/* Read a line from stdin safely. Returns 0 on EOF/error. */
static int read_line(char *buf, size_t size) {
    if (fgets(buf, (int)size, stdin) == NULL) {
        return 0;
    }
    if (strchr(buf, '\n') == NULL) {
        /* Line was longer than the buffer: discard the rest. */
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {
        }
    }
    trim_newline(buf);
    return 1;
}

/* Read an integer from stdin. Returns -1 on invalid input. */
static int read_int(void) {
    char buf[32];
    char *end;
    long value;

    if (!read_line(buf, sizeof buf)) {
        return -1;
    }
    value = strtol(buf, &end, 10);
    if (end == buf || *end != '\0') {
        return -1;
    }
    return (int)value;
}

static void load_tasks(void) {
    FILE *fp = fopen(DATA_FILE, "r");
    char line[MAX_LEN + 8];

    if (fp == NULL) {
        return; /* No saved file yet. */
    }
    while (task_count < MAX_TASKS && fgets(line, sizeof line, fp) != NULL) {
        trim_newline(line);
        /* Format: <0|1>|<text> */
        if (line[0] != '0' && line[0] != '1') {
            continue;
        }
        if (line[1] != '|') {
            continue;
        }
        tasks[task_count].done = (line[0] == '1');
        strncpy(tasks[task_count].text, line + 2, MAX_LEN - 1);
        tasks[task_count].text[MAX_LEN - 1] = '\0';
        task_count++;
    }
    fclose(fp);
}

static void save_tasks(void) {
    FILE *fp = fopen(DATA_FILE, "w");
    int i;

    if (fp == NULL) {
        perror("Could not save tasks");
        return;
    }
    for (i = 0; i < task_count; i++) {
        fprintf(fp, "%d|%s\n", tasks[i].done, tasks[i].text);
    }
    fclose(fp);
}

static void add_task(void) {
    char buf[MAX_LEN];

    if (task_count >= MAX_TASKS) {
        printf("Task list is full.\n");
        return;
    }
    printf("Enter task: ");
    if (!read_line(buf, sizeof buf) || buf[0] == '\0') {
        printf("Task cannot be empty.\n");
        return;
    }
    strncpy(tasks[task_count].text, buf, MAX_LEN - 1);
    tasks[task_count].text[MAX_LEN - 1] = '\0';
    tasks[task_count].done = 0;
    task_count++;
    save_tasks();
    printf("Task added.\n");
}

static void list_tasks(void) {
    int i;

    if (task_count == 0) {
        printf("No tasks yet.\n");
        return;
    }
    printf("\n--- Your Tasks ---\n");
    for (i = 0; i < task_count; i++) {
        printf("%2d. [%c] %s\n", i + 1, tasks[i].done ? 'x' : ' ', tasks[i].text);
    }
}

static void complete_task(void) {
    int n;

    list_tasks();
    if (task_count == 0) {
        return;
    }
    printf("Task number to mark as done: ");
    n = read_int();
    if (n < 1 || n > task_count) {
        printf("Invalid task number.\n");
        return;
    }
    tasks[n - 1].done = 1;
    save_tasks();
    printf("Task marked as done.\n");
}

static void delete_task(void) {
    int n, i;

    list_tasks();
    if (task_count == 0) {
        return;
    }
    printf("Task number to delete: ");
    n = read_int();
    if (n < 1 || n > task_count) {
        printf("Invalid task number.\n");
        return;
    }
    for (i = n - 1; i < task_count - 1; i++) {
        tasks[i] = tasks[i + 1];
    }
    task_count--;
    save_tasks();
    printf("Task deleted.\n");
}

static void print_menu(void) {
    printf("\n=== To-Do List Manager ===\n");
    printf("1. Add task\n");
    printf("2. List tasks\n");
    printf("3. Mark task as done\n");
    printf("4. Delete task\n");
    printf("5. Exit\n");
    printf("Choose an option: ");
}

int main(void) {
    int choice;

    load_tasks();

    for (;;) {
        print_menu();
        choice = read_int();
        switch (choice) {
        case 1:
            add_task();
            break;
        case 2:
            list_tasks();
            break;
        case 3:
            complete_task();
            break;
        case 4:
            delete_task();
            break;
        case 5:
            printf("Goodbye!\n");
            return 0;
        default:
            printf("Invalid choice, try again.\n");
        }
    }
}