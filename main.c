
/*
    Project: DTM - Daily Task Manager
    Name: Amolika Agrawal
    Enrollment No: A26BCAAIU0049
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TASKS 100
#define FILE_NAME "tasks.txt"

typedef struct {
    char title[81];
    int priority;
    int done;
} Task;

Task tasks[MAX_TASKS];
int count = 0;

// Read text input
int readLine(char *text, int size) {
    int ch;

    if (fgets(text, size, stdin) == NULL)
        return 0;

    if (strchr(text, '\n') != NULL)
        text[strcspn(text, "\n")] = '\0';
    else
        while ((ch = getchar()) != '\n' && ch != EOF) {}

    return 1;
}

// Read numerical input
int readNumber(const char *prompt) {
    char line[80], extra;
    int number;

    printf("%s", prompt);

    if (!readLine(line, sizeof(line)))
        return -1;

    if (sscanf(line, "%d %c", &number, &extra) != 1)
        return -2;

    return number;
}

// Return priority name
const char *priorityName(int priority) {
    if (priority == 1)
        return "HIGH";

    if (priority == 2)
        return "MEDIUM";

    return "LOW";
}

// Save tasks into a file
void saveTasks(void) {
    FILE *file = fopen(FILE_NAME, "w");

    if (file == NULL) {
        puts("Warning: could not save tasks.");
        return;
    }

    for (int i = 0; i < count; i++) {
        fprintf(file, "%d %d\n%s\n",
                tasks[i].priority,
                tasks[i].done,
                tasks[i].title);
    }

    if (fclose(file) != 0)
        puts("Warning: could not finish saving tasks.");
}

// Load previously saved tasks
void loadTasks(void) {
    FILE *file = fopen(FILE_NAME, "r");
    char line[100];

    if (file == NULL)
        return;

    while (count < MAX_TASKS &&
           fgets(line, sizeof(line), file)) {

        Task task;

        if (sscanf(line, "%d %d",
                   &task.priority, &task.done) != 2)
            break;

        if (fgets(task.title, sizeof(task.title), file) == NULL)
            break;

        if (strchr(task.title, '\n') == NULL) {
            int ch;
            while ((ch = fgetc(file)) != '\n' && ch != EOF) {}
        }

        task.title[strcspn(task.title, "\r\n")] = '\0';

        if (task.priority >= 1 &&
            task.priority <= 3 &&
            (task.done == 0 || task.done == 1) &&
            task.title[0] != '\0') {

            tasks[count++] = task;
        }
    }

    fclose(file);
}

// Display main dashboard
void dashboard(void) {
    int completed = 0;

    for (int i = 0; i < count; i++)
        completed += tasks[i].done;

    puts("\n+----------------------------------------------------------+");
    puts("| DTM.                         DAILY TASK MANAGER          |");
    puts("| Organize. Focus. Achieve.                                |");
    puts("+----------------------------------------------------------+");

    printf("| TOTAL: %3d     PENDING: %3d     COMPLETED: %3d             |\n",
           count, count - completed, completed);

    printf("| PROGRESS: [");

    int filled = count == 0 ? 0 : completed * 20 / count;

    for (int i = 0; i < 20; i++)
        putchar(i < filled ? '#' : '-');

    printf("] %3d%%                   |\n",
           count == 0 ? 0 : completed * 100 / count);

    puts("+----------------------------------------------------------+");
    puts("  [1] Add task          [2] View tasks");
    puts("  [3] Complete task     [4] Delete task");
    puts("  [5] Edit task         [0] Exit");
    puts("------------------------------------------------------------");
}

// Display all tasks
void viewTasks(void) {
    if (count == 0) {
        puts("\nNo tasks yet. Add your first task!");
        return;
    }

    puts("\nYOUR TASKS");
    puts("------------------------------------------------------------");

    printf("%-4s %-9s %-12s %s\n",
           "ID", "PRIORITY", "STATUS", "TASK");

    puts("------------------------------------------------------------");

    for (int i = 0; i < count; i++) {
        printf("%-4d %-9s %-12s %s\n",
               i + 1,
               priorityName(tasks[i].priority),
               tasks[i].done ? "COMPLETED" : "PENDING",
               tasks[i].title);
    }

    puts("------------------------------------------------------------");
}

// Select priority
int getPriority(void) {
    int priority = readNumber(
        "Priority (1 High / 2 Medium / 3 Low): "
    );

    if (priority < 1 || priority > 3) {
        puts("Invalid priority. No changes made.");
        return 0;
    }

    return priority;
}

// Add a new task
void addTask(void) {
    if (count == MAX_TASKS) {
        puts("Task list is full.");
        return;
    }

    Task task;

    printf("Task title: ");

    if (!readLine(task.title, sizeof(task.title)) ||
        task.title[0] == '\0') {

        puts("Title cannot be empty.");
        return;
    }

    task.priority = getPriority();

    if (!task.priority)
        return;

    task.done = 0;
    tasks[count++] = task;

    saveTasks();

    puts("[OK] Task added.");
}

// Select a task using its ID
int selectTask(void) {
    viewTasks();

    if (count == 0)
        return -1;

    int id = readNumber("Enter task ID: ");

    if (id < 1 || id > count) {
        puts("Invalid task ID.");
        return -1;
    }

    return id - 1;
}

// Mark task as completed
void completeTask(void) {
    int index = selectTask();

    if (index == -1)
        return;

    tasks[index].done = 1;

    saveTasks();

    puts("[OK] Task marked as completed.");
}

// Delete a task
void deleteTask(void) {
    int index = selectTask();

    if (index == -1)
        return;

    for (int i = index; i < count - 1; i++)
        tasks[i] = tasks[i + 1];

    count--;

    saveTasks();

    puts("[OK] Task deleted.");
}

// Edit an existing task
void editTask(void) {
    int index = selectTask();

    if (index == -1)
        return;

    char title[81];

    printf("New task title: ");

    if (!readLine(title, sizeof(title)) ||
        title[0] == '\0') {

        puts("Title cannot be empty.");
        return;
    }

    int priority = getPriority();

    if (!priority)
        return;

    strcpy(tasks[index].title, title);
    tasks[index].priority = priority;

    saveTasks();

    puts("[OK] Task updated.");
}

// Main function
int main(void) {
    loadTasks();

    while (1) {
        dashboard();

        int choice = readNumber("Choose an option: ");

        switch (choice) {
            case 1:
                addTask();
                break;

            case 2:
                viewTasks();
                break;

            case 3:
                completeTask();
                break;

            case 4:
                deleteTask();
                break;

            case 5:
                editTask();
                break;

            case 0:
            case -1:
                puts("Goodbye! Stay productive.");
                return 0;

            default:
                puts("Invalid choice. Try again.");
        }
    }
}
