# University Database System (C++ & Web Interface)

A clean, modern **University Database System** designed as a mini-project for an Object-Oriented Programming (OOP) course in C++. It features a C++ backend web server that implements OOP structures and communicates via a REST API with a responsive, glassmorphic HTML/CSS/JS frontend dashboard.

---

## 🚀 Key Features

* **Polymorphic Database**: Manages Students, Professors, and Administrators under a unified, abstract `Person` class hierarchy.
* **Persistent CSV Storage**: Auto-saves and loads all directory entries to/from local CSV databases using standard C++ file streams (`std::ifstream` / `std::ofstream`).
* **Polymorphic Search**: Real-time unified database search (by Name, ID, or Department) dynamically bound at runtime.
* **Interactive Frontend**: Rich, glassmorphic dark-mode Single Page Application (SPA) dashboard showcasing data counts, average student GPAs, salary rolls, and complete CRUD forms.
* **Portable Dev Environment**: Includes automatic toolchain compilation setup for systems without preinstalled compilers.

---

## 🛠️ OOP Architecture (C++)

This project demonstrates several core object-oriented paradigms in C++:

1. **Inheritance & Abstract Base Class**:
   * Base class `Person` declares common attributes (`id`, `name`, `email`, `department`) and pure virtual methods like `getRole()`, `toJSON()`, and `toCSV()`.
   * Derived classes `Student`, `Professor`, and `Admin` extend `Person` with role-specific attributes (`gpa`, `major`, `salary`, `room`, `title`, `accessLevel`).
2. **Polymorphism**:
   * Uses dynamic binding on virtual functions (such as `toJSON()`) allowing the server to handle records polymorphically as `std::shared_ptr<Person>` during searches and database operations.
3. **Encapsulation**:
   * Class fields are protected or private, accessed and modified strictly via public getters/setters.
4. **Data Deserialization/Serialization**:
   * Uses raw string splitting, stringstreams, and stream buffers to manage custom JSON serialization without heavy external dependencies.

---

## 📂 File Structure

```text
university_database/
├── compile.bat          # Batch script to compile and run the server
├── setup_toolchain.py   # Python helper script to set up compiler & httplib.h
├── .gitignore           # Excludes compiler and binaries from Git
├── README.md            # Project documentation
├── data/                # CSV database folder
│   ├── students.csv
│   ├── professors.csv
│   └── admins.csv
├── backend/             # C++ source code
│   ├── httplib.h        # Header-only C++ HTTP Library
│   ├── models.h         # OOP data structures
│   ├── database.h       # CSV file operations & CRUD manager
│   └── main.cpp         # Server setup & REST API controllers
└── frontend/            # Client-side web interface
    ├── index.html       # UI Dashboard structure
    ├── style.css        # Glassmorphic layout styles
    └── app.js           # AJAX request router & DOM controllers
```

---

## 💻 How to Run Locally

### Prerequisites
* Windows OS
* Python 3.x (used only for the initial setup script)

### Setup & Run
1. Clone this repository or download the files.
2. In the project directory, run the Python setup script to download the portable C++ compiler and HTTP header library:
   ```cmd
   python setup_toolchain.py
   ```
3. Compile the backend and start the server:
   ```cmd
   compile.bat
   ```
4. Once compiled successfully, open your browser and navigate to:
   **http://localhost:8080**
