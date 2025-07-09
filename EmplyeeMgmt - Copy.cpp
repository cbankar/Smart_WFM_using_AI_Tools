#include <iostream>
#include <fstream>
#include <vector>
#include <string>

struct Employee {
    std::string name;
    std::string skills;
    int experience;
    double talentScore;
};

void writeEmployeesToFile(const std::vector<Employee>& employees, const std::string& filename) {
    std::ofstream ofs(filename, std::ios::binary);
    for (const auto& emp : employees) {
        size_t nameLen = emp.name.size();
        size_t skillsLen = emp.skills.size();
        ofs.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
        ofs.write(emp.name.c_str(), nameLen);
        ofs.write(reinterpret_cast<const char*>(&skillsLen), sizeof(skillsLen));
        ofs.write(emp.skills.c_str(), skillsLen);
        ofs.write(reinterpret_cast<const char*>(&emp.experience), sizeof(emp.experience));
        ofs.write(reinterpret_cast<const char*>(&emp.talentScore), sizeof(emp.talentScore));
    }
    ofs.close();
}

std::vector<Employee> readEmployeesFromFile(const std::string& filename) {
    std::vector<Employee> employees;
    std::ifstream ifs(filename, std::ios::binary);
    while (ifs.peek() != EOF) {
        Employee emp;
        size_t nameLen, skillsLen;
        ifs.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        emp.name.resize(nameLen);
        ifs.read(&emp.name[0], nameLen);
        ifs.read(reinterpret_cast<char*>(&skillsLen), sizeof(skillsLen));
        emp.skills.resize(skillsLen);
        ifs.read(&emp.skills[0], skillsLen);
        ifs.read(reinterpret_cast<char*>(&emp.experience), sizeof(emp.experience));
        ifs.read(reinterpret_cast<char*>(&emp.talentScore), sizeof(emp.talentScore));
        if (ifs) employees.push_back(emp);
    }
    ifs.close();
    return employees;
}

int main() {
    std::vector<Employee> employees(100);
    // Fill with sample data
    for (int i = 0; i < 100; ++i) {
        employees[i].name = "Employee" + std::to_string(i+1);
        employees[i].skills = "Skill" + std::to_string(i%5+1);
        employees[i].experience = i % 20 + 1;
        employees[i].talentScore = 50.0 + (i % 50);
    }

    std::string filename = "employees.dat";
    writeEmployeesToFile(employees, filename);

    auto loadedEmployees = readEmployeesFromFile(filename);

    // Print loaded employees
    for (const auto& emp : loadedEmployees) {
        std::cout << emp.name << ", " << emp.skills << ", "
                  << emp.experience << ", " << emp.talentScore << std::endl;
    }

    return 0;
}
     
