#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <iomanip>

using namespace std;

// --------------------- Data Structures ---------------------

struct Student {
    string id;
    string name;
    string password;
};

struct SubjectProgress {
    string subject;
    int finished;   // number of lessons finished (0..15)
    int total;      // total lessons up to current level (5,10,15)
    double percent() const {
        if (total == 0) return 0.0;
        return (finished * 100.0) / total;
    }
};

struct StudentRecord {
    Student s;
    vector<SubjectProgress> subjects;
    // derived fields for printing can be computed on the fly
};

// --------------------- Helpers ---------------------

static inline string trim(const string &str) {
    size_t a = str.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = str.find_last_not_of(" \t\r\n");
    return str.substr(a, b - a + 1);
}

// --------------------- Load students list ---------------------

vector<Student> loadStudents(const string &filename = "students.txt") {
    vector<Student> students;
    ifstream in(filename);
    if (!in) {
        cerr << "[WARNING] Could not open " << filename << ". No students loaded.\n";
        return students;
    }

    // try skipping header if present
    string firstLine;
    getline(in, firstLine);

    // If firstLine doesn't look like a header (starts with an ID token), we might need to process it.
    // We will attempt to parse lines in the form: ID Name Password
    // We'll reprocess the first line token-by-token.
    auto processLine = [&](const string &line) {
        if (trim(line).empty()) return;
        istringstream iss(line);
        string id;
        if (!(iss >> id)) return;
        string rest;
        getline(iss, rest);
        rest = trim(rest);
        string name, pw;
        size_t pos = rest.rfind(' ');
        if (pos != string::npos) {
            name = trim(rest.substr(0, pos));
            pw = trim(rest.substr(pos + 1));
        } else {
            name = rest;
            pw = "";
        }
        students.push_back({id, name, pw});
    };

    // process the first line (it may have been header or actual student)
    // Heuristic: if first token contains letters like "Student" treat as header
    bool firstIsHeader = false;
    {
        istringstream iss(firstLine);
        string tok;
        iss >> tok;
        string lowered = tok;
        for (auto &c : lowered) c = char(tolower(c));
        if (lowered == "studentid" || lowered == "student" || lowered == "id" || lowered == "studentid," )
            firstIsHeader = true;
    }
    if (!firstIsHeader) processLine(firstLine);

    // process the rest
    string line;
    while (getline(in, line)) {
        processLine(line);
    }
    in.close();
    return students;
}

// --------------------- Load one student's subject data (from <ID>.txt) ---------------------
// Expected format per line (same as your menu.cpp save):
// SubjectName currentLevel s0 s1 s2
// Example:
// Math 2 5 4 5

struct LoadedSubjectLine {
    string name;
    int level;
    int s0, s1, s2;
};

vector<LoadedSubjectLine> loadStudentFile(const string &studentFilename) {
    vector<LoadedSubjectLine> out;
    ifstream in(studentFilename);
    if (!in) return out;

    string name;
    while (in >> name) {
        int lvl = 0, a = 0, b = 0, c = 0;
        if (!(in >> lvl >> a >> b >> c)) break;
        out.push_back({name, lvl, a, b, c});
    }
    in.close();
    return out;
}

// Convert LoadedSubjectLine -> SubjectProgress (finished / total)
SubjectProgress convertLoadedToProgress(const LoadedSubjectLine &ls) {
    int finished = 0;
    // In your menu logic: scores[] stores correct answers each level (0..5).
    // currentLevel indicates the last level reached (0..2). When currentLevel == 2, all three levels done.
    if (ls.level <= 0) finished = ls.s0;
    else if (ls.level == 1) finished = ls.s0 + ls.s1;
    else /* ls.level == 2 */ finished = ls.s0 + ls.s1 + ls.s2;

    int total = (ls.level + 1) * 5; // level 0 -> 5, level1 ->10, level2 ->15
    return {ls.name, finished, total};
}

// Build a StudentRecord by reading student's file (<id>.txt)
// If file missing, subjects vector will be empty
StudentRecord buildStudentRecordFromFile(const Student &s) {
    StudentRecord rec;
    rec.s = s;
    string filename = s.id + ".txt";
    vector<LoadedSubjectLine> raw = loadStudentFile(filename);
    for (auto &ls : raw) {
        rec.subjects.push_back(convertLoadedToProgress(ls));
    }
    return rec;
}

// --------------------- Printing: 2x2 matrix & report card ---------------------

void print2x2Matrix(const SubjectProgress &sp) {
    int finished = sp.finished;
    int remaining = sp.total - sp.finished;
    if (remaining < 0) remaining = 0;
    string rec;
    double pct = sp.percent();
    if (pct >= 90.0) rec = "Excellent — encourage mastery & enrichment.";
    else if (pct >= 70.0) rec = "Good — reinforce with practice.";
    else if (pct >= 40.0) rec = "Needs improvement — remediation recommended.";
    else rec = "At risk — recommend tutoring & school attendance.";

    cout << "+----------------------+----------------------+\n";
    cout << "| Lessons Finished: " << setw(3) << finished << "       | Lessons Remaining: " << setw(3) << remaining << "     |\n";
    cout << "+----------------------+----------------------+\n";
    cout << "| Recommendation: " << left << setw(44) << rec << "|\n";
    cout << "+-----------------------------------------------+\n";
}

void printStudentReportCardExact(const StudentRecord &rec) {
    cout << "\n============================================\n";
    cout << "StudentID: " << rec.s.id << "    Name: " << rec.s.name << "\n";
    cout << "--------------------------------------------\n";
    if (rec.subjects.empty()) {
        cout << "No report card data available for this student (student file missing or empty).\n";
        return;
    }

    double overallTotalScore = 0.0;
    int subjectsMasteredCount = 0;

    cout << setw(20) << left << "SUBJECT" << setw(15) << "SCORE (%)" << setw(15) << "STATUS" << "\n";
    cout << string(60, '-') << "\n";

    for (auto &sp : rec.subjects) {
        // Determine mastery status from total: if total == 15 => mastered all levels
        string status;
        if (sp.total >= 15) {
            status = "MASTERED";
            subjectsMasteredCount++;
            overallTotalScore += sp.percent();
        } else {
            status = "In Progress";
        }
        cout << setw(20) << left << sp.subject
             << setw(15);
        if (sp.total > 0)
            cout << fixed << setprecision(2) << sp.percent();
        else
            cout << "N/A";
        cout << setw(15) << status << "\n";

        // print 2x2 matrix for subject
        print2x2Matrix(sp);
    }

    cout << string(60, '-') << "\n";
    if (subjectsMasteredCount > 0) {
        double overallAvg = overallTotalScore / subjectsMasteredCount;
        cout << setw(20) << left << "OVERALL AVERAGE:" << fixed << setprecision(2) << overallAvg << "%\n";
    } else {
        cout << "OVERALL AVERAGE: N/A (Complete at least one subject to calculate.)\n";
    }

    cout << "============================================\n";
}

// --------------------- Sorting (merge sort: divide & conquer) ---------------------

double getPercentForSubject(const StudentRecord &rec, const string &subject) {
    string s = subject;
    for (auto &c : s) c = char(tolower(c));
    for (auto &sp : rec.subjects) {
        string sub = sp.subject;
        for (auto &c : sub) c = char(tolower(c));
        if (sub == s) return sp.percent();
    }
    return 0.0;
}

void mergeSortRecords(vector<StudentRecord> &arr, const string &subject) {
    if (arr.size() <= 1) return;
    size_t mid = arr.size() / 2;
    vector<StudentRecord> L(arr.begin(), arr.begin() + mid);
    vector<StudentRecord> R(arr.begin() + mid, arr.end());
    mergeSortRecords(L, subject);
    mergeSortRecords(R, subject);
    arr.clear();
    size_t i = 0, j = 0;
    while (i < L.size() && j < R.size()) {
        if (getPercentForSubject(L[i], subject) >= getPercentForSubject(R[j], subject)) {
            arr.push_back(L[i++]);
        } else arr.push_back(R[j++]);
    }
    while (i < L.size()) arr.push_back(L[i++]);
    while (j < R.size()) arr.push_back(R[j++]);
}

// --------------------- Binary Search (by student ID) ---------------------

int binarySearchByID(const vector<Student> &sortedByID, const string &id) {
    int l = 0, r = (int)sortedByID.size() - 1;
    while (l <= r) {
        int m = l + (r - l) / 2;
        if (sortedByID[m].id == id) return m;
        if (sortedByID[m].id < id) l = m + 1;
        else r = m - 1;
    }
    return -1;
}

// --------------------- Admin Menu ---------------------

void adminMenu(vector<Student> &students) {
    // Build initial lightweight records (only student info). We'll read each student's file on demand.
    vector<StudentRecord> baseRecords;
    for (auto &st : students) {
        StudentRecord r; r.s = st;
        baseRecords.push_back(r);
    }

    while (true) {
        cout << "\n============ ADMIN MENU ============\n";
        cout << "1. Sort students by subject completion (Divide & Conquer)\n";
        cout << "2. Display all students (option to print report card)\n";
        cout << "3. Search student by ID (Binary Search)\n";
        cout << "0. Exit\n";
        cout << "Choice: ";
        int choice;
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input.\n";
            continue;
        }
        cin.ignore(10000, '\n');

        if (choice == 0) {
            cout << "Exiting admin.\n";
            break;
        }

        if (choice == 1) {
            string subject;
            cout << "Enter subject name to sort by (e.g. Math): ";
            getline(cin, subject);
            if (trim(subject).empty()) {
                cout << "Subject cannot be empty.\n";
                continue;
            }

            // Build full records by reading each student's file (so percent is accurate)
            vector<StudentRecord> records;
            for (auto &s : students) records.push_back(buildStudentRecordFromFile(s));

            mergeSortRecords(records, subject);
            cout << "\nStudents sorted by percent-complete in {" << subject << "} (highest -> lowest):\n";
            cout << left << setw(10) << "ID" << setw(30) << "Name" << "Percent\n";
            cout << "-------------------------------------------------\n";
            for (const auto &r : records) {
                cout << setw(10) << r.s.id << setw(30) << r.s.name
                     << fixed << setprecision(2) << getPercentForSubject(r, subject) << "%\n";
            }
        }
        else if (choice == 2) {
            cout << "\nAll students:\n";
            for (size_t i = 0; i < students.size(); ++i) {
                cout << i + 1 << ". " << students[i].id << " - " << students[i].name << "\n";
            }

            cout << "\nDo you want to print report card for a student or all students?\n";
            cout << "1. Print one student's report card\n";
            cout << "2. Print ALL report cards\n";
            cout << "0. Back\n";
            cout << "Choice: ";
            int subchoice;
            if (!(cin >> subchoice)) { cin.clear(); cin.ignore(10000, '\n'); continue; }
            cin.ignore(10000, '\n');

            if (subchoice == 0) continue;
            if (subchoice == 1) {
                string id;
                cout << "Enter Student ID: ";
                getline(cin, id);
                id = trim(id);
                bool found = false;
                for (auto &st : students) {
                    if (st.id == id) {
                        StudentRecord rec = buildStudentRecordFromFile(st);
                        printStudentReportCardExact(rec);
                        found = true;
                        break;
                    }
                }
                if (!found) cout << "Student not found.\n";
            } else if (subchoice == 2) {
                for (auto &st : students) {
                    StudentRecord rec = buildStudentRecordFromFile(st);
                    printStudentReportCardExact(rec);
                }
            } else {
                cout << "Invalid choice.\n";
            }
        }
        else if (choice == 3) {
            // search by student ID using binary search (needs sorted student list)
            vector<Student> sorted = students;
            sort(sorted.begin(), sorted.end(), [](const Student &a, const Student &b){ return a.id < b.id; });

            string id;
            cout << "Enter Student ID to search: ";
            getline(cin, id);
            id = trim(id);

            int idx = binarySearchByID(sorted, id);
            if (idx == -1) {
                cout << "StudentID not found.\n";
            } else {
                Student found = sorted[idx];
                StudentRecord rec = buildStudentRecordFromFile(found);
                printStudentReportCardExact(rec);
            }
        }
        else {
            cout << "Invalid choice.\n";
        }
    }
}

// --------------------- Main ---------------------

int main() {
    auto students = loadStudents("students.txt");
    if (students.empty()) {
        cout << "No students found. Ensure students.txt exists.\n";
    }
    adminMenu(students);
    return 0;
}
