#include "httplib.h"
#include "database.h"
#include <iostream>
#include <string>
#include <memory>

// Flat JSON parsing helper to extract a field's value as a string
std::string getJSONValue(const std::string& json, const std::string& key) {
    std::string searchKey = "\"" + key + "\"";
    size_t pos = json.find(searchKey);
    if (pos == std::string::npos) return "";
    
    pos = json.find(":", pos + searchKey.length());
    if (pos == std::string::npos) return "";
    
    pos++; // Move past colon
    // Trim leading whitespace
    while (pos < json.length() && (json[pos] == ' ' || json[pos] == '\t' || json[pos] == '\r' || json[pos] == '\n')) {
        pos++;
    }
    
    if (pos >= json.length()) return "";
    
    if (json[pos] == '"') {
        pos++; // Move past opening quote
        size_t endPos = json.find("\"", pos);
        if (endPos == std::string::npos) return "";
        return json.substr(pos, endPos - pos);
    } else {
        // Number, boolean, or null
        size_t endPos = json.find_first_of(",}", pos);
        if (endPos == std::string::npos) return json.substr(pos);
        std::string val = json.substr(pos, endPos - pos);
        // Trim trailing whitespace
        while (!val.empty() && (val.back() == ' ' || val.back() == '\t' || val.back() == '\r' || val.back() == '\n')) {
            val.pop_back();
        }
        return val;
    }
}

int main() {
    // Instantiate database manager pointing to '../data' folder
    DatabaseManager db("./data");

    // Initialize HTTP server
    httplib::Server svr;

    // Set server mount point to serve static files from './frontend'
    // If files are in a sibling directory frontend
    svr.set_mount_point("/", "./frontend");

    // Root redirect to index.html
    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_redirect("/index.html");
    });

    // --- Statistics Endpoint ---
    svr.Get("/api/stats", [&](const httplib::Request&, httplib::Response& res) {
        res.set_content(db.getStatsJSON(), "application/json");
    });

    // --- Search Endpoint ---
    svr.Get("/api/search", [&](const httplib::Request& req, httplib::Response& res) {
        std::string query = req.get_param_value("q");
        res.set_content(db.searchPeopleJSON(query), "application/json");
    });

    // --- STUDENTS API ---
    svr.Get("/api/students", [&](const httplib::Request&, httplib::Response& res) {
        res.set_content(db.getStudentsJSON(), "application/json");
    });

    svr.Post("/api/students", [&](const httplib::Request& req, httplib::Response& res) {
        std::string body = req.body;
        std::string id = getJSONValue(body, "id");
        std::string name = getJSONValue(body, "name");
        std::string email = getJSONValue(body, "email");
        std::string dept = getJSONValue(body, "department");
        std::string gpaStr = getJSONValue(body, "gpa");
        std::string major = getJSONValue(body, "major");
        std::string coursesStr = getJSONValue(body, "courses");

        if (id.empty() || name.empty() || email.empty()) {
            res.status = 400;
            res.set_content("{\"error\":\"ID, Name and Email are required\"}", "application/json");
            return;
        }

        double gpa = gpaStr.empty() ? 0.0 : std::stod(gpaStr);
        std::vector<std::string> courses = splitString(coursesStr, ';');

        Student student(id, name, email, dept, gpa, major, courses);
        if (db.addStudent(student)) {
            res.status = 201;
            res.set_content(student.toJSON(), "application/json");
        } else {
            res.status = 409;
            res.set_content("{\"error\":\"Student ID already exists\"}", "application/json");
        }
    });

    svr.Put(R"(/api/students/([^/]+))", [&](const httplib::Request& req, httplib::Response& res) {
        std::string id = req.matches[1];
        std::string body = req.body;
        
        std::string name = getJSONValue(body, "name");
        std::string email = getJSONValue(body, "email");
        std::string dept = getJSONValue(body, "department");
        std::string gpaStr = getJSONValue(body, "gpa");
        std::string major = getJSONValue(body, "major");
        std::string coursesStr = getJSONValue(body, "courses");

        double gpa = gpaStr.empty() ? 0.0 : std::stod(gpaStr);
        std::vector<std::string> courses = splitString(coursesStr, ';');

        Student updated(id, name, email, dept, gpa, major, courses);
        if (db.updateStudent(id, updated)) {
            res.set_content(updated.toJSON(), "application/json");
        } else {
            res.status = 404;
            res.set_content("{\"error\":\"Student not found\"}", "application/json");
        }
    });

    svr.Delete(R"(/api/students/([^/]+))", [&](const httplib::Request& req, httplib::Response& res) {
        std::string id = req.matches[1];
        if (db.deleteStudent(id)) {
            res.set_content("{\"message\":\"Student deleted successfully\"}", "application/json");
        } else {
            res.status = 404;
            res.set_content("{\"error\":\"Student not found\"}", "application/json");
        }
    });

    // --- PROFESSORS API ---
    svr.Get("/api/professors", [&](const httplib::Request&, httplib::Response& res) {
        res.set_content(db.getProfessorsJSON(), "application/json");
    });

    svr.Post("/api/professors", [&](const httplib::Request& req, httplib::Response& res) {
        std::string body = req.body;
        std::string id = getJSONValue(body, "id");
        std::string name = getJSONValue(body, "name");
        std::string email = getJSONValue(body, "email");
        std::string dept = getJSONValue(body, "department");
        std::string salaryStr = getJSONValue(body, "salary");
        std::string room = getJSONValue(body, "room");
        std::string coursesStr = getJSONValue(body, "courses");

        if (id.empty() || name.empty() || email.empty()) {
            res.status = 400;
            res.set_content("{\"error\":\"ID, Name and Email are required\"}", "application/json");
            return;
        }

        double salary = salaryStr.empty() ? 0.0 : std::stod(salaryStr);
        std::vector<std::string> courses = splitString(coursesStr, ';');

        Professor professor(id, name, email, dept, salary, room, courses);
        if (db.addProfessor(professor)) {
            res.status = 201;
            res.set_content(professor.toJSON(), "application/json");
        } else {
            res.status = 409;
            res.set_content("{\"error\":\"Professor ID already exists\"}", "application/json");
        }
    });

    svr.Put(R"(/api/professors/([^/]+))", [&](const httplib::Request& req, httplib::Response& res) {
        std::string id = req.matches[1];
        std::string body = req.body;

        std::string name = getJSONValue(body, "name");
        std::string email = getJSONValue(body, "email");
        std::string dept = getJSONValue(body, "department");
        std::string salaryStr = getJSONValue(body, "salary");
        std::string room = getJSONValue(body, "room");
        std::string coursesStr = getJSONValue(body, "courses");

        double salary = salaryStr.empty() ? 0.0 : std::stod(salaryStr);
        std::vector<std::string> courses = splitString(coursesStr, ';');

        Professor updated(id, name, email, dept, salary, room, courses);
        if (db.updateProfessor(id, updated)) {
            res.set_content(updated.toJSON(), "application/json");
        } else {
            res.status = 404;
            res.set_content("{\"error\":\"Professor not found\"}", "application/json");
        }
    });

    svr.Delete(R"(/api/professors/([^/]+))", [&](const httplib::Request& req, httplib::Response& res) {
        std::string id = req.matches[1];
        if (db.deleteProfessor(id)) {
            res.set_content("{\"message\":\"Professor deleted successfully\"}", "application/json");
        } else {
            res.status = 404;
            res.set_content("{\"error\":\"Professor not found\"}", "application/json");
        }
    });

    // --- ADMINS API ---
    svr.Get("/api/admins", [&](const httplib::Request&, httplib::Response& res) {
        res.set_content(db.getAdminsJSON(), "application/json");
    });

    svr.Post("/api/admins", [&](const httplib::Request& req, httplib::Response& res) {
        std::string body = req.body;
        std::string id = getJSONValue(body, "id");
        std::string name = getJSONValue(body, "name");
        std::string email = getJSONValue(body, "email");
        std::string dept = getJSONValue(body, "department");
        std::string title = getJSONValue(body, "title");
        std::string accessLevel = getJSONValue(body, "accessLevel");

        if (id.empty() || name.empty() || email.empty()) {
            res.status = 400;
            res.set_content("{\"error\":\"ID, Name and Email are required\"}", "application/json");
            return;
        }

        Admin admin(id, name, email, dept, title, accessLevel);
        if (db.addAdmin(admin)) {
            res.status = 201;
            res.set_content(admin.toJSON(), "application/json");
        } else {
            res.status = 409;
            res.set_content("{\"error\":\"Admin ID already exists\"}", "application/json");
        }
    });

    svr.Put(R"(/api/admins/([^/]+))", [&](const httplib::Request& req, httplib::Response& res) {
        std::string id = req.matches[1];
        std::string body = req.body;

        std::string name = getJSONValue(body, "name");
        std::string email = getJSONValue(body, "email");
        std::string dept = getJSONValue(body, "department");
        std::string title = getJSONValue(body, "title");
        std::string accessLevel = getJSONValue(body, "accessLevel");

        Admin updated(id, name, email, dept, title, accessLevel);
        if (db.updateAdmin(id, updated)) {
            res.set_content(updated.toJSON(), "application/json");
        } else {
            res.status = 404;
            res.set_content("{\"error\":\"Admin not found\"}", "application/json");
        }
    });

    svr.Delete(R"(/api/admins/([^/]+))", [&](const httplib::Request& req, httplib::Response& res) {
        std::string id = req.matches[1];
        if (db.deleteAdmin(id)) {
            res.set_content("{\"message\":\"Admin deleted successfully\"}", "application/json");
        } else {
            res.status = 404;
            res.set_content("{\"error\":\"Admin not found\"}", "application/json");
        }
    });

    std::cout << "University Database Server running at http://localhost:8080" << std::endl;
    svr.listen("localhost", 8080);
    return 0;
}
