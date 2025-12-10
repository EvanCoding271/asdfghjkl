#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

// ====================== STUDENT STRUCT ======================
struct Student {
    string studentNumber;
    string name;
    string password;
};

vector<Student> students;

<<<<<<< HEAD
// ====================== ADMIN STRUCT ========================
struct Admin {
    string adminID;
    string name;
    string password;
};

vector<Admin> admins;

// ====================== LOAD ADMINS =========================
void loadAdmins() {
    ifstream infile("admins.txt");
    if (!infile) {
        cout << "[ERROR] Cannot open admins.txt\n";
        return;
    }

    string header;
    getline(infile, header); // skip header: AdminID Name Password

    while (infile) {
        string id;
        if (!(infile >> id)) break;

        string rest;
        getline(infile, rest);

        // trim leading space
        while (!rest.empty() && rest[0] == ' ')
            rest.erase(rest.begin());

        size_t pos = rest.rfind(' ');
        string name = rest.substr(0, pos);
        string pw = rest.substr(pos + 1);

        admins.push_back({id, name, pw});
    }
}

// ====================== CHECK ADMIN LOGIN ====================
bool loginAdmin(const string& id, const string& pw) {
    for (auto& a : admins) {
        if (a.adminID == id && a.password == pw) {
            cout << "\n=============================\n";
            cout << "  ADMIN LOGIN SUCCESSFUL\n";
            cout << "  Welcome, " << a.name << "!\n";
            cout << "=============================\n";

            // Open admin.exe
            #ifdef _WIN32
                system("admin.exe");
            #else
                system("./admin");
            #endif

            return true;
        }
    }
    return false;
}

// ====================== LOAD STUDENTS ========================
=======
// ---------------------- CLEAR CONSOLE --------------------------
void clearConsole() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// ---------------------- LOAD & SAVE --------------------------
>>>>>>> b69d17e9bac5bca6eeb8e803a6839335307977ad
void loadStudents() {
    ifstream infile("loginrecord.txt");
    string sn, nm, pw;
    students.clear();

    string header;
    getline(infile, header);

    while (infile >> sn) {
        infile >> ws;
        getline(infile, nm, '\n');

        size_t pos = nm.rfind(' ');
        if (pos != string::npos) {
            pw = nm.substr(pos + 1);
            nm = nm.substr(0, pos);
        }

        students.push_back({sn, nm, pw});
    }
    infile.close();
}

// ====================== SAVE STUDENTS ========================
void saveStudents() {
    ofstream outfile("loginrecord.txt");
    outfile << "StudentID Name Password\n";
    for (auto &s : students) {
        outfile << s.studentNumber << " " << s.name << " " << s.password << endl;
    }
    outfile.close();
}

<<<<<<< HEAD
// ====================== REGISTER STUDENT =====================
=======
// ---------------------- REGISTER STUDENT --------------------------
>>>>>>> b69d17e9bac5bca6eeb8e803a6839335307977ad
bool registerStudent() {
    clearConsole();
    Student s;
    cout << "\n--- Student Registration ---\n";
    cout << "Enter Student Number: ";
    cin >> s.studentNumber;

    for (char &c : s.studentNumber) c = toupper(c);

    cin.ignore();
    cout << "Enter Full Name: ";
    getline(cin, s.name);

    cout << "Enter Password: ";
    while (true) {
        getline(cin, s.password);

        if (s.password.length() < 4) {
            cout << "❌ Password must be at least 4 characters. Try again: ";
        } else if (s.password.find(" ") != string::npos) {
            cout << "❌ Password cannot contain spaces. Try again: ";
        } else break;
    }

    for (auto &st : students) {
        if (st.studentNumber == s.studentNumber) {
            cout << "❌ Student already exists!\n";
            return false;
        }
    }

    students.push_back(s);
    saveStudents();
    cout << "✅ Registration Successful!\n";
    return true;
}

<<<<<<< HEAD
// ========================= LOGIN ==============================
bool loginStudent() {
    string idInput, pwInput;

    cout << "\n--- Login ---\n";
    cout << "Enter ID: ";
    cin >> idInput;
=======
// ---------------------- ADMIN LOGIN --------------------------
bool loginAdmin() {
    clearConsole();
    string adminID, pwInput;

    while (true) {
        cout << "\n--- Admin Login ---\n";
        cout << "Enter Admin ID (or C to cancel): ";
        cin >> adminID;

        // Cancel option
        if (adminID == "C" || adminID == "c") {
            cout << "❌ Admin login cancelled.\n";
            return false;
        }

        cout << "Enter Password: ";
        cin >> pwInput;

        if ((adminID == "101" || adminID == "102" || adminID == "103") && pwInput == "admin") {
            cout << "✅ Admin access granted!\n";
            #ifdef _WIN32
                system("admin.exe"); // Launch admin program on Windows
            #else
                system("./admin");   // Launch admin program on Linux/Mac
            #endif
            return true;
        } else {
            cout << "❌ Wrong admin ID or password. Try again, or type C to cancel.\n";
        }
    }
}

// ---------------------- STUDENT LOGIN --------------------------
bool loginStudent() {
    clearConsole();
    string snInput, pwInput;

    while (true) {
        cout << "\n--- Student Login ---\n";
        cout << "Enter Student Number (or C to cancel): ";
        cin >> snInput;

        // Cancel option
        if (snInput == "C" || snInput == "c") {
            cout << "❌ Student login cancelled.\n";
            return false;
        }

        for (char &c : snInput) c = toupper(c);
>>>>>>> b69d17e9bac5bca6eeb8e803a6839335307977ad

    // Uppercase ID
    for (char &c : idInput) c = toupper(c);

<<<<<<< HEAD
    cout << "Enter Password: ";
    cin >> pwInput;

    // ========== FIRST CHECK IF ADMIN ==========
    if (loginAdmin(idInput, pwInput)) {
        return true;
    }

    // ========== ELSE CHECK STUDENT ============
    Student* found = nullptr;
    for (auto &s : students) {
        if (s.studentNumber == idInput) {
            found = &s;
            break;
=======
        if (!found) {
            cout << "❌ Student number not found. Try again, or type C to cancel.\n";
            continue;
        }

        cout << "Enter Password: ";
        cin >> pwInput;

        if (pwInput == found->password) {
            cout << "✅ Welcome " << found->name << "! Login successful.\n";
            #ifdef _WIN32
                system("menu.exe");
            #else
                system("./menu");
            #endif
            return true;
        } else {
            cout << "❌ Wrong ID or password. Try again, or type C to cancel.\n";
>>>>>>> b69d17e9bac5bca6eeb8e803a6839335307977ad
        }
    }

    if (!found) {
        cout << "❌ ID not found.\n";
        return false;
    }

    if (pwInput != found->password) {
        cout << "❌ Incorrect password.\n";
        return false;
    }

    cout << "✅ Welcome " << found->name << "! Login successful.\n";

    // open menu.exe
    #ifdef _WIN32
        system("menu.exe");
    #else
        system("./menu");
    #endif

    return true;
}

// =========================== MAIN =============================
int main() {
    int choice;

    loadAdmins();   // load admin accounts
    loadStudents(); // load students

    while (true) {
<<<<<<< HEAD
        cout << "\n=== System Login ===\n";
        cout << "1. Login\n";
        cout << "2. Register (Student Only)\n";
        cout << "3. Exit\n";
=======
        clearConsole();
        cout << "\n=== Student System ===\n";
        cout << "1. Admin Login\n";
        cout << "2. Student Login\n";
        cout << "3. Register Student\n";
        cout << "4. Exit\n";
>>>>>>> b69d17e9bac5bca6eeb8e803a6839335307977ad
        cout << "Choose an option: ";
        cin >> choice;

        switch (choice) {
            case 1: loginAdmin(); break;
            case 2: loginStudent(); break;
            case 3: registerStudent(); break;
            case 4: 
                cout << "Goodbye!\n"; 
                return 0;
            default: cout << "Invalid choice!\n"; break;
        }
<<<<<<< HEAD
    }
    return 0;
=======

        cout << "\nPress Enter to continue...";
        cin.ignore();
        cin.get();
    }
>>>>>>> b69d17e9bac5bca6eeb8e803a6839335307977ad
}
