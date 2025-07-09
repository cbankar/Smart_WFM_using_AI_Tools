#include<stdio.h>
int main() {
    printf("Hello, World!\n");
    // Define employee structure
    struct Employee {
        char name[50];
        char skills[100];
        int experience;
    };

    // Create a collection of employees
    struct Employee employees[] = {
        {"Alice Johnson", "C, C++, Python", 5},
        {"Bob Smith", "Java, SQL, HTML", 3},
        {"Carol White", "Go, Rust, Docker", 7},
        {"David Brown", "JavaScript, React, Node.js", 4},
        {"Eve Black", "C#, .NET, Azure", 6}
    };
    int num_employees = sizeof(employees) / sizeof(employees[0]);

    // Write employee data to file
    FILE *fout = fopen("employees.txt", "w");
    if (fout == NULL) {
        printf("Error opening file for writing.\n");
        return 1;
    }
    for (int i = 0; i < num_employees; i++) {
        fprintf(fout, "%s;%s;%d\n", employees[i].name, employees[i].skills, employees[i].experience);
    }
    fclose(fout);

    // Read employee data from file
    FILE *fin = fopen("employees.txt", "r");
    if (fin == NULL) {
        printf("Error opening file for reading.\n");
        return 1;
    }
    printf("Employee Data from File:\n");
    char line[200];
    while (fgets(line, sizeof(line), fin)) {
        char name[50], skills[100];
        int experience;
        if (sscanf(line, "%49[^;];%99[^;];%d", name, skills, &experience) == 3) {
            printf("Name: %s, Skills: %s, Experience: %d years\n", name, skills, experience);
        }
    }
    fclose(fin);
    return 0;
}
    
