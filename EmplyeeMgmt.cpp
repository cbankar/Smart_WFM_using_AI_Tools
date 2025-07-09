#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <curl/curl.h>

using namespace std;
/// @brief 
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


// This function reads employees from a binary file and returns a vector of Employee objects.       



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

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int main() {
    std::vector<Employee> employees(100);
    // Fill with sample data
    // Generate 400 different employees with realistic data
    employees.clear();
    std::vector<std::string> names = {
        "Alice Johnson", "Bob Smith", "Charlie Lee", "Diana Evans", "Ethan Brown",
        "Fiona Clark", "George Harris", "Hannah Lewis", "Ian Walker", "Julia Young",
        "Kevin Hall", "Laura Allen", "Michael King", "Nina Wright", "Oscar Scott",
        "Paula Green", "Quentin Adams", "Rachel Baker", "Sam Carter", "Tina Davis"
        // Add more names as needed or repeat with suffixes
    };
    std::vector<std::string> skills = {
        "C", "C++", "Java", "Python", "JavaScript"
    };

    for (int i = 0; i < 400; ++i) {
        Employee emp;
        emp.name = names[i % names.size()] + " #" + std::to_string(i+1);
        std::string primarySkill = skills[i % skills.size()];
        int score = 60 + (i * 7) % 41; // Score between 60 and 100
        emp.skills = "Primary: " + primarySkill + " (" + std::to_string(score) + ")";
        emp.experience = 1 + (i % 30); // 1 to 30 years
        emp.talentScore = score+ (i % 10) * 0.5; // Add some variation

        //regenerate talent score between 50 to 80 not more than 80
        emp.talentScore = 50 + (i % 31) * 0.5; // Score between 50 and 80
        employees.push_back(emp);
    }
    
   /* for (int i = 0; i < 100; ++i) {
        employees[i].name = "Employee" + std::to_string(i+1);
        employees[i].skills = "Skill" + std::to_string(i%5+1);
        employees[i].experience = i % 20 + 1;
        employees[i].talentScore = 50.0 + (i % 50);
    }*/

    std::string filename = "employees.dat";
    writeEmployeesToFile(employees, filename);

    auto loadedEmployees = readEmployeesFromFile(filename);

    // Print loaded employees
    for (const auto& emp : loadedEmployees) {
        std::cout << emp.name << ", " << emp.skills << ", "
                  << emp.experience << ", " << emp.talentScore << std::endl;
    }

    std::string searchSkill;
    std::cout << "Enter the skills to hunt for employee: ";
    std::getline(std::cin, searchSkill);

    // Find employees with matching primary skill
    std::vector<Employee> matched;
    for (const auto& emp : loadedEmployees) {
        // Extract primary skill from emp.skills string
        size_t pos1 = emp.skills.find("Primary: ");
        size_t pos2 = emp.skills.find(" (");
        if (pos1 != std::string::npos && pos2 != std::string::npos) {
            std::string primarySkill = emp.skills.substr(pos1 + 9, pos2 - (pos1 + 9));
            if (primarySkill == searchSkill) {
                matched.push_back(emp);
            }
        }
    }

    // Sort by talentScore descending
    std::sort(matched.begin(), matched.end(), [](const Employee& a, const Employee& b) {
        return a.talentScore > b.talentScore;
    });

    std::cout << "Top 8 employees with primary skill '" << searchSkill << "':\n";
    for (size_t i = 0; i < matched.size() && i < 8; ++i) {
        const auto& emp = matched[i];
        std::cout << emp.name << ", " << emp.skills << ", "
                  << emp.experience << ", " << emp.talentScore << std::endl;
    }
   
    //write this top 8 matched employees to text file
    std::ofstream outFile("matched_employees.txt");
    for (size_t i = 0; i < matched.size() && i < 8; ++i) {
        const auto& emp = matched[i];
        outFile << emp.name << ", " << emp.skills << ", "
                 << emp.experience << ", " << emp.talentScore << std::endl;
    }
    outFile.close();

    //prompts
for (size_t i = 0; i < matched.size() && i < 8; ++i) 
{
  const auto& emp = matched[i];
   CURL* curl;
    CURLcode res;

    std::string readBuffer;
    std::string api_key = "<API_KEY>";
    std::string url = "https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash-latest:generateContent?key=" + api_key;
    std::string primarySkill;
    size_t pos1 = emp.skills.find("Primary: ");
    size_t pos2 = emp.skills.find(" (");
    if (pos1 != std::string::npos && pos2 != std::string::npos) {
        primarySkill = emp.skills.substr(pos1 + 9, pos2 - (pos1 + 9));
    } else {
        primarySkill = "Unknown";
    }
    std::string prompt = "training required to candidate having talent score " + std::to_string(emp.talentScore) + " out of 100 on " + primarySkill + " for Networking domain, give high level 3 KT titles.";
   std::string payload = R"({
        "contents": [{
            "parts": [{
                "text": ")" + prompt + R"("
            }]
        }]
    })";
   

    curl = curl_easy_init();
    if (curl) {
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_CAINFO, "C:\\MinGW\\curl-8.14.1_2-win64-mingw\\bin\\curl-ca-bundle.crt");

        res = curl_easy_perform(curl);
        std::string textContent="";
        if (res != CURLE_OK)
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << "\n";
        else
        {
            //std::cout << "Response: " << readBuffer << "\n\n\n\n";
            // Parse the response and print only the "text" field content
            size_t textPos = readBuffer.find("\"text\":");
            if (textPos != std::string::npos) {
                size_t startQuote = readBuffer.find("\"", textPos + 7);
                size_t endQuote = readBuffer.find("\"", startQuote + 1);
                if (startQuote != std::string::npos && endQuote != std::string::npos) {
                    textContent = readBuffer.substr(startQuote + 1, endQuote - startQuote - 1);
                    std::cout << "Text: " << textContent << std::endl;
                } else {
                    std::cout << "Could not find text content in response." << std::endl;
                }
            } else {
                std::cout << "No text field found in response." << std::endl;
            }
           cout<<"=============================================================="<<endl;
            textContent += " => youtube KT table for this content ";
            // Prepare new payload for table content prompt
            std::string tablePayload = R"({
                "contents": [{
                    "parts": [{
                        "text": ")" + textContent + R"("
                    }]
                }]
            })";

            std::string tableBuffer;
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, tablePayload.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &tableBuffer);

            res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                std::cerr << "curl_easy_perform() failed (table prompt): " << curl_easy_strerror(res) << "\n";
            } else {
                //std::cout << "Table Content Response: " << tableBuffer << std::endl;
                // Optionally extract and print only the "text" field
                size_t textPos = tableBuffer.find("\"text\":");
                if (textPos != std::string::npos) {
                    size_t startQuote = tableBuffer.find("\"", textPos + 7);
                    size_t endQuote = tableBuffer.find("\"", startQuote + 1);
                    if (startQuote != std::string::npos && endQuote != std::string::npos) {

                        
                        std::string tableText = tableBuffer.substr(startQuote + 1, endQuote - startQuote - 1);
                    // Replace escaped newlines with actual newlines
                    std::string formattedText = tableText;
                    size_t pos = 0;
                    // Replace \n with actual newlines
                    while ((pos = formattedText.find("\\n", pos)) != std::string::npos) {
                        formattedText.replace(pos, 2, "\n");
                        pos += 1;
                    }
                    // Add newlines before each numbered/bullet point if not already present
                    pos = 0;
                    while ((pos = formattedText.find_first_of("1234567890-", pos)) != std::string::npos) {
                        if (pos == 0 || formattedText[pos-1] == '\n') {
                            ++pos;
                            continue;
                        }
                        // Check for pattern like "1. ", "2. ", "- "
                        if ((isdigit(formattedText[pos]) && formattedText[pos+1] == '.' && formattedText[pos+2] == ' ') ||
                            (formattedText[pos] == '-' && formattedText[pos+1] == ' ')) {
                            formattedText.insert(pos, "\n");
                            pos += 2;
                        } else {
                            ++pos;
                        }
                    }
                    std::cout << "\nFormatted Table Text:\n" << formattedText << std::endl;

                     std::string empFileName = "KT_Table_" + emp.name+"_"+searchSkill;
                    std::replace(empFileName.begin(), empFileName.end(), ' ', '_');
                    std::replace(empFileName.begin(), empFileName.end(), '#', '_');
                    empFileName += ".txt";
                    std::ofstream outFile(empFileName);
                    
                    // Write to file in readable format proper table
                    if (outFile.is_open()) {
                    outFile << "\n";
                    outFile << "--------------------------------------------------\n";
                    outFile << "|   KT Title                                      |\n";
                    outFile << "--------------------------------------------------\n";
                    // Print each line of formattedText as a table row
                    istringstream iss(formattedText);
                    string line;
                    while (getline(iss, line)) {
                        if (!line.empty()) {
                            // Remove leading/trailing whitespace
                            line.erase(0, line.find_first_not_of(" \t\r\n"));
                            line.erase(line.find_last_not_of(" \t\r\n") + 1);
                            outFile << "| " << line;
                            // Pad to 48 chars for alignment
                            if (line.length() < 48)
                                outFile << std::string(48 - line.length(), ' ');
                            outFile << "|\n";
                        }
                    }
                    outFile << "--------------------------------------------------\n";
                }

                  /*  if (outFile.is_open()) {
                        outFile << "Employee Name: " << emp.name << "\n";
                        outFile << "Primary Skill: " << searchSkill << "\n";
                        outFile << "Experience: " << emp.experience << " years\n";
                        outFile << "Talent Score: " << emp.talentScore << "\n";
                        outFile << "--------------------------------------------------\n";
                        outFile << "KT Table:\n";
                        outFile << formattedText << "\n";
                        outFile << "--------------------------------------------------\n";
                    }*/
                                              
                   /*if (outFile.is_open()) {
                        outFile << formattedText;
                        outFile.close();
                        std::cout << "Formatted table text written to " << empFileName << "\n";
                    }*/

                     else {
                        std::cerr << "Failed to open " << empFileName << " for writing.\n";
                    }
                    
                    /*std::ofstream outFile("KT_Table.txt");
                    if (outFile.is_open()) {
                        outFile << formattedText;
                        outFile.close();
                        std::cout << "Formatted table text written to KT_Table.txt\n";
                    } else {
                        std::cerr << "Failed to open KT_Table.txt for writing.\n";
                    }
                    */
                        std::cout << std::endl;
                        std::cout << "Table Text: " << tableText << std::endl;
                    } else {
                        std::cout << "Could not find table text content in response." << std::endl;
                    }
                } else {
                    std::cout << "No text field found in table response." << std::endl;
                }
            }

        }

     

        cout<<" activity is done for employee "<<emp.name<<endl;
        
        curl_easy_cleanup(curl);
    }
   }
        int val;
        cin>>val;
    return 0;
}
     
