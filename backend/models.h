#ifndef MODELS_H
#define MODELS_H

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

// Helper to escape special characters for JSON output
inline std::string escapeJSON(const std::string& val) {
    std::ostringstream o;
    for (auto c = val.cbegin(); c != val.cend(); c++) {
        switch (*c) {
        case '"': o << "\\\""; break;
        case '\\': o << "\\\\"; break;
        case '\b': o << "\\b"; break;
        case '\f': o << "\\f"; break;
        case '\n': o << "\\n"; break;
        case '\r': o << "\\r"; break;
        case '\t': o << "\\t"; break;
        default:
            if ('\x00' <= *c && *c <= '\x1f') {
                o << "\\u"
                  << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(*c);
            } else {
                o << *c;
            }
        }
    }
    return o.str();
}

// Abstract Base Class
class Person {
protected:
    std::string id;
    std::string name;
    std::string email;
    std::string department;

public:
    Person(std::string id, std::string name, std::string email, std::string dept)
        : id(id), name(name), email(email), department(dept) {}

    virtual ~Person() {}

    // Encapsulation: Getters and Setters
    std::string getId() const { return id; }
    std::string getName() const { return name; }
    std::string getEmail() const { return email; }
    std::string getDepartment() const { return department; }

    void setName(const std::string& n) { name = n; }
    void setEmail(const std::string& e) { email = e; }
    void setDepartment(const std::string& d) { department = d; }

    // Polymorphism: Pure virtual methods
    virtual std::string getRole() const = 0;
    virtual std::string toJSON() const = 0;
    virtual std::string toCSV() const = 0;
};

// Derived Class: Student
class Student : public Person {
private:
    double gpa;
    std::string major;
    std::vector<std::string> courses;

public:
    Student(std::string id, std::string name, std::string email, std::string dept, double gpa, std::string major, std::vector<std::string> courses)
        : Person(id, name, email, dept), gpa(gpa), major(major), courses(courses) {}

    double getGpa() const { return gpa; }
    std::string getMajor() const { return major; }
    std::vector<std::string> getCourses() const { return courses; }

    void setGpa(double g) { gpa = g; }
    void setMajor(const std::string& m) { major = m; }
    void setCourses(const std::vector<std::string>& c) { courses = c; }

    std::string getRole() const override { return "Student"; }

    std::string toJSON() const override {
        std::string json = "{";
        json += "\"id\":\"" + escapeJSON(id) + "\",";
        json += "\"name\":\"" + escapeJSON(name) + "\",";
        json += "\"email\":\"" + escapeJSON(email) + "\",";
        json += "\"department\":\"" + escapeJSON(department) + "\",";
        json += "\"role\":\"student\",";
        
        std::ostringstream gpa_stream;
        gpa_stream << std::fixed << std::setprecision(2) << gpa;
        json += "\"gpa\":" + gpa_stream.str() + ",";
        
        json += "\"major\":\"" + escapeJSON(major) + "\",";
        json += "\"courses\":[";
        for (size_t i = 0; i < courses.size(); ++i) {
            json += "\"" + escapeJSON(courses[i]) + "\"";
            if (i + 1 < courses.size()) json += ",";
        }
        json += "]}";
        return json;
    }

    std::string toCSV() const override {
        std::string csv = id + "," + name + "," + email + "," + department + ",";
        std::ostringstream gpa_stream;
        gpa_stream << std::fixed << std::setprecision(2) << gpa;
        csv += gpa_stream.str() + "," + major + ",";
        for (size_t i = 0; i < courses.size(); ++i) {
            csv += courses[i];
            if (i + 1 < courses.size()) csv += ";";
        }
        return csv;
    }
};

// Derived Class: Professor
class Professor : public Person {
private:
    double salary;
    std::string room;
    std::vector<std::string> courses;

public:
    Professor(std::string id, std::string name, std::string email, std::string dept, double salary, std::string room, std::vector<std::string> courses)
        : Person(id, name, email, dept), salary(salary), room(room), courses(courses) {}

    double getSalary() const { return salary; }
    std::string getRoom() const { return room; }
    std::vector<std::string> getCourses() const { return courses; }

    void setSalary(double s) { salary = s; }
    void setRoom(const std::string& r) { room = r; }
    void setCourses(const std::vector<std::string>& c) { courses = c; }

    std::string getRole() const override { return "Professor"; }

    std::string toJSON() const override {
        std::string json = "{";
        json += "\"id\":\"" + escapeJSON(id) + "\",";
        json += "\"name\":\"" + escapeJSON(name) + "\",";
        json += "\"email\":\"" + escapeJSON(email) + "\",";
        json += "\"department\":\"" + escapeJSON(department) + "\",";
        json += "\"role\":\"professor\",";
        
        std::ostringstream sal_stream;
        sal_stream << std::fixed << std::setprecision(2) << salary;
        json += "\"salary\":" + sal_stream.str() + ",";
        
        json += "\"room\":\"" + escapeJSON(room) + "\",";
        json += "\"courses\":[";
        for (size_t i = 0; i < courses.size(); ++i) {
            json += "\"" + escapeJSON(courses[i]) + "\"";
            if (i + 1 < courses.size()) json += ",";
        }
        json += "]}";
        return json;
    }

    std::string toCSV() const override {
        std::string csv = id + "," + name + "," + email + "," + department + ",";
        std::ostringstream sal_stream;
        sal_stream << std::fixed << std::setprecision(2) << salary;
        csv += sal_stream.str() + "," + room + ",";
        for (size_t i = 0; i < courses.size(); ++i) {
            csv += courses[i];
            if (i + 1 < courses.size()) csv += ";";
        }
        return csv;
    }
};

// Derived Class: Admin
class Admin : public Person {
private:
    std::string title;
    std::string accessLevel;

public:
    Admin(std::string id, std::string name, std::string email, std::string dept, std::string title, std::string accessLevel)
        : Person(id, name, email, dept), title(title), accessLevel(accessLevel) {}

    std::string getTitle() const { return title; }
    std::string getAccessLevel() const { return accessLevel; }

    void setTitle(const std::string& t) { title = t; }
    void setAccessLevel(const std::string& a) { accessLevel = a; }

    std::string getRole() const override { return "Admin"; }

    std::string toJSON() const override {
        std::string json = "{";
        json += "\"id\":\"" + escapeJSON(id) + "\",";
        json += "\"name\":\"" + escapeJSON(name) + "\",";
        json += "\"email\":\"" + escapeJSON(email) + "\",";
        json += "\"department\":\"" + escapeJSON(department) + "\",";
        json += "\"role\":\"admin\",";
        json += "\"title\":\"" + escapeJSON(title) + "\",";
        json += "\"accessLevel\":\"" + escapeJSON(accessLevel) + "\"";
        json += "}";
        return json;
    }

    std::string toCSV() const override {
        return id + "," + name + "," + email + "," + department + "," + title + "," + accessLevel;
    }
};

#endif // MODELS_H
