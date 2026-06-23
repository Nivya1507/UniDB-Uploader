#ifndef DATABASE_H
#define DATABASE_H

#include "models.h"
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

// Utility to split a string by delimiter
inline std::vector<std::string> splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        // Trim whitespace from token
        while (!token.empty() && (token.front() == ' ' || token.front() == '\t')) {
            token.erase(token.begin());
        }
        while (!token.empty() && (token.back() == ' ' || token.back() == '\t' || token.back() == '\r' || token.back() == '\n')) {
            token.pop_back();
        }
        tokens.push_back(token);
    }
    return tokens;
}

class DatabaseManager {
private:
    std::string studentsFile;
    std::string professorsFile;
    std::string adminsFile;

    std::vector<std::shared_ptr<Student>> students;
    std::vector<std::shared_ptr<Professor>> professors;
    std::vector<std::shared_ptr<Admin>> admins;

public:
    DatabaseManager(std::string dataDir) {
        studentsFile = dataDir + "/students.csv";
        professorsFile = dataDir + "/professors.csv";
        adminsFile = dataDir + "/admins.csv";
        loadAll();
    }

    // --- Loading Data ---
    void loadAll() {
        loadStudents();
        loadProfessors();
        loadAdmins();
    }

    void loadStudents() {
        students.clear();
        std::ifstream file(studentsFile);
        if (!file.is_open()) {
            std::cerr << "Could not open " << studentsFile << std::endl;
            return;
        }
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            std::vector<std::string> fields = splitString(line, ',');
            if (fields.size() < 6) continue;
            
            std::string id = fields[0];
            std::string name = fields[1];
            std::string email = fields[2];
            std::string dept = fields[3];
            double gpa = std::stod(fields[4]);
            std::string major = fields[5];
            
            std::vector<std::string> courses;
            if (fields.size() > 6) {
                courses = splitString(fields[6], ';');
            }
            
            students.push_back(std::make_shared<Student>(id, name, email, dept, gpa, major, courses));
        }
        file.close();
    }

    void loadProfessors() {
        professors.clear();
        std::ifstream file(professorsFile);
        if (!file.is_open()) {
            std::cerr << "Could not open " << professorsFile << std::endl;
            return;
        }
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            std::vector<std::string> fields = splitString(line, ',');
            if (fields.size() < 6) continue;
            
            std::string id = fields[0];
            std::string name = fields[1];
            std::string email = fields[2];
            std::string dept = fields[3];
            double salary = std::stod(fields[4]);
            std::string room = fields[5];
            
            std::vector<std::string> courses;
            if (fields.size() > 6) {
                courses = splitString(fields[6], ';');
            }
            
            professors.push_back(std::make_shared<Professor>(id, name, email, dept, salary, room, courses));
        }
        file.close();
    }

    void loadAdmins() {
        admins.clear();
        std::ifstream file(adminsFile);
        if (!file.is_open()) {
            std::cerr << "Could not open " << adminsFile << std::endl;
            return;
        }
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            std::vector<std::string> fields = splitString(line, ',');
            if (fields.size() < 6) continue;
            
            std::string id = fields[0];
            std::string name = fields[1];
            std::string email = fields[2];
            std::string dept = fields[3];
            std::string title = fields[4];
            std::string accessLevel = fields[5];
            
            admins.push_back(std::make_shared<Admin>(id, name, email, dept, title, accessLevel));
        }
        file.close();
    }

    // --- Saving Data ---
    void saveAll() {
        saveStudents();
        saveProfessors();
        saveAdmins();
    }

    void saveStudents() {
        std::ofstream file(studentsFile);
        if (!file.is_open()) return;
        for (const auto& s : students) {
            file << s->toCSV() << "\n";
        }
        file.close();
    }

    void saveProfessors() {
        std::ofstream file(professorsFile);
        if (!file.is_open()) return;
        for (const auto& p : professors) {
            file << p->toCSV() << "\n";
        }
        file.close();
    }

    void saveAdmins() {
        std::ofstream file(adminsFile);
        if (!file.is_open()) return;
        for (const auto& a : admins) {
            file << a->toCSV() << "\n";
        }
        file.close();
    }

    // --- Retrieve Collections as JSON ---
    std::string getStudentsJSON() const {
        std::string json = "[";
        for (size_t i = 0; i < students.size(); ++i) {
            json += students[i]->toJSON();
            if (i + 1 < students.size()) json += ",";
        }
        json += "]";
        return json;
    }

    std::string getProfessorsJSON() const {
        std::string json = "[";
        for (size_t i = 0; i < professors.size(); ++i) {
            json += professors[i]->toJSON();
            if (i + 1 < professors.size()) json += ",";
        }
        json += "]";
        return json;
    }

    std::string getAdminsJSON() const {
        std::string json = "[";
        for (size_t i = 0; i < admins.size(); ++i) {
            json += admins[i]->toJSON();
            if (i + 1 < admins.size()) json += ",";
        }
        json += "]";
        return json;
    }

    // --- Polymorphic User Retrieval and Search ---
    std::vector<std::shared_ptr<Person>> getAllPeople() const {
        std::vector<std::shared_ptr<Person>> all;
        for (const auto& s : students) all.push_back(s);
        for (const auto& p : professors) all.push_back(p);
        for (const auto& a : admins) all.push_back(a);
        return all;
    }

    std::string searchPeopleJSON(const std::string& query) const {
        std::string lowerQuery = query;
        std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
        
        std::vector<std::shared_ptr<Person>> all = getAllPeople();
        std::string json = "[";
        bool first = true;
        
        for (const auto& person : all) {
            std::string name = person->getName();
            std::string dept = person->getDepartment();
            std::string id = person->getId();
            std::transform(name.begin(), name.end(), name.begin(), ::tolower);
            std::transform(dept.begin(), dept.end(), dept.begin(), ::tolower);
            std::transform(id.begin(), id.end(), id.begin(), ::tolower);
            
            if (name.find(lowerQuery) != std::string::npos || 
                dept.find(lowerQuery) != std::string::npos ||
                id.find(lowerQuery) != std::string::npos) {
                if (!first) json += ",";
                json += person->toJSON();
                first = false;
            }
        }
        json += "]";
        return json;
    }

    // --- Statistics ---
    std::string getStatsJSON() const {
        double avgGpa = 0.0;
        if (!students.empty()) {
            double totalGpa = 0.0;
            for (const auto& s : students) {
                totalGpa += s->getGpa();
            }
            avgGpa = totalGpa / students.size();
        }

        double totalSalary = 0.0;
        for (const auto& p : professors) {
            totalSalary += p->getSalary();
        }

        std::ostringstream gpa_stream, sal_stream;
        gpa_stream << std::fixed << std::setprecision(2) << avgGpa;
        sal_stream << std::fixed << std::setprecision(2) << totalSalary;

        std::string json = "{";
        json += "\"studentCount\":" + std::to_string(students.size()) + ",";
        json += "\"professorCount\":" + std::to_string(professors.size()) + ",";
        json += "\"adminCount\":" + std::to_string(admins.size()) + ",";
        json += "\"averageGpa\":" + gpa_stream.str() + ",";
        json += "\"totalSalary\":" + sal_stream.str();
        json += "}";
        return json;
    }

    // --- CRUD: Students ---
    bool addStudent(const Student& s) {
        // Check duplicate ID
        for (const auto& stu : students) {
            if (stu->getId() == s.getId()) return false;
        }
        students.push_back(std::make_shared<Student>(s));
        saveStudents();
        return true;
    }

    bool updateStudent(const std::string& id, const Student& updated) {
        for (auto& s : students) {
            if (s->getId() == id) {
                s->setName(updated.getName());
                s->setEmail(updated.getEmail());
                s->setDepartment(updated.getDepartment());
                s->setGpa(updated.getGpa());
                s->setMajor(updated.getMajor());
                s->setCourses(updated.getCourses());
                saveStudents();
                return true;
            }
        }
        return false;
    }

    bool deleteStudent(const std::string& id) {
        auto it = std::remove_if(students.begin(), students.end(),
            [&](const std::shared_ptr<Student>& s) { return s->getId() == id; });
        if (it != students.end()) {
            students.erase(it, students.end());
            saveStudents();
            return true;
        }
        return false;
    }

    // --- CRUD: Professors ---
    bool addProfessor(const Professor& p) {
        for (const auto& prof : professors) {
            if (prof->getId() == p.getId()) return false;
        }
        professors.push_back(std::make_shared<Professor>(p));
        saveProfessors();
        return true;
    }

    bool updateProfessor(const std::string& id, const Professor& updated) {
        for (auto& p : professors) {
            if (p->getId() == id) {
                p->setName(updated.getName());
                p->setEmail(updated.getEmail());
                p->setDepartment(updated.getDepartment());
                p->setSalary(updated.getSalary());
                p->setRoom(updated.getRoom());
                p->setCourses(updated.getCourses());
                saveProfessors();
                return true;
            }
        }
        return false;
    }

    bool deleteProfessor(const std::string& id) {
        auto it = std::remove_if(professors.begin(), professors.end(),
            [&](const std::shared_ptr<Professor>& p) { return p->getId() == id; });
        if (it != professors.end()) {
            professors.erase(it, professors.end());
            saveProfessors();
            return true;
        }
        return false;
    }

    // --- CRUD: Admins ---
    bool addAdmin(const Admin& a) {
        for (const auto& adm : admins) {
            if (adm->getId() == a.getId()) return false;
        }
        admins.push_back(std::make_shared<Admin>(a));
        saveAdmins();
        return true;
    }

    bool updateAdmin(const std::string& id, const Admin& updated) {
        for (auto& a : admins) {
            if (a->getId() == id) {
                a->setName(updated.getName());
                a->setEmail(updated.getEmail());
                a->setDepartment(updated.getDepartment());
                a->setTitle(updated.getTitle());
                a->setAccessLevel(updated.getAccessLevel());
                saveAdmins();
                return true;
            }
        }
        return false;
    }

    bool deleteAdmin(const std::string& id) {
        auto it = std::remove_if(admins.begin(), admins.end(),
            [&](const std::shared_ptr<Admin>& a) { return a->getId() == id; });
        if (it != admins.end()) {
            admins.erase(it, admins.end());
            saveAdmins();
            return true;
        }
        return false;
    }
};

#endif // DATABASE_H
