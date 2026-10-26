// ==========================================================
//  Student Grade Management System
//  Algorithms and Object-Oriented Programming & Lab - Week 7
//  Task: compile it, run it, and find every bug.
// ==========================================================
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
using namespace std;

const int PASS_MARK = 60;
const int NUM_BINS = 5;

struct Student {
    int id;
    string name;
    int quiz;
    int midterm;
    int finalExam;
    int total;
    char grade;
}

// ----------------------------------------------------------
//  Section 1: scoring
// ----------------------------------------------------------

// Weighted total: quiz 20%, midterm 30%, final exam 50%.
// Integer math with +50 so the result is rounded to the nearest whole number.
int computeTotal(const Student& s) {
    return (s.quiz * 20 + s.midterm * 30 + s.finalExam * 50 + 50) / 100;
}

char letterGrade(int score) {
    if (score >= 85) return 'A';
    if (score >= 70) return 'B';
    if (score >= 60) return 'C';
    if (score >= 50) return 'D';
    return 'E';
}

void fillResults(vector<Student>& list) {
    for (int i = 0; i < (int)list.size(); i++) {
        list[i].total = computeTotal(list[i]);
        list[i].grade = letterGrade(list[i].total);
    }
}

// Adds bonus points to every student, but never above 100.
void applyBonus(vector<Student>& list, int bonus) {
    for (int i = 0; i < (int)list.size(); i++) {
        list[i].total += bonus;
        if (list[i].total > 100) {
            list[i].total = 100;
        }
        list[i].grade = letterGrade(list[i].total);
    }
}

// ----------------------------------------------------------
//  Section 2: printing helpers
// ----------------------------------------------------------

void printLine(char symbol, int length) {
    for (int i = 0; i < length; i++) {
        cout << symbol;
    }
    cout << endl;
}

void printHeader(const string& title) {
    cout << endl;
    printLine('=', 52);
    cout << "  " << title << endl;
    printLine('=', 52);
}

void printTable(const vector<Student>& list) {
    cout << left << setw(5) << "ID" << setw(10) << "Name"
         << right << setw(7) << "Quiz" << setw(9) << "Midterm"
         << setw(7) << "Final" << setw(8) << "Total"
         << setw(7) << "Grade" << endl;
    printLine('-', 52);
    for (int i = 0; i < (int)list.size(); i++) {
        const Student& s = list[i];
        cout << left << setw(5) << s.id << setw(10) << s.name
             << right << setw(7) << s.quiz << setw(9) << s.midterm
             << setw(7) << s.finalExam << setw(8) << s.total
             << setw(7) << s.grade << endl;
    }
}

// ----------------------------------------------------------
//  Section 3: class statistics
// ----------------------------------------------------------

double classAverage(const vector<Student>& list) {
    int sum = 0;
    int n = (int)list.size();
    for (int i = 0; i < n - 1; i++) {
        sum += list[i].total;
    }
    return static_cast<double>(sum) / n;
}

int highestScore(const vector<Student>& list) {
    int best = list[0].total;
    for (int i = 1; i < (int)list.size(); i++) {
        if (list[i].total > best) {
            best = list[i].total;
        }
    }
    return best;
}

int lowestScore(const vector<Student>& list) {
    int worst = 0;
    for (int i = 0; i < (int)list.size(); i++) {
        if (list[i].total < worst) {
            worst = list[i].total;
        }
    }
    return worst;
}

int countPassing(const vector<Student>& list) {
    int count = 0;
    for (int i = 0; i < (int)list.size(); i++) {
        if (list[i].total > PASS_MARK) {
            count++;
        }
    }
    return count;
}

// part out of whole, as a whole-number percentage
int percentage(int part, int whole) {
    return part * 100 / whole;
}

void printStatistics(const vector<Student>& list) {
    cout << fixed << setprecision(2);
    cout << "Class average : " << classAverage(list) << endl;
    cout << "Highest score : " << highestScore(list) << endl;
    cout << "Lowest score  : " << lowestScore(list) << endl;
    cout << "Passing (>= " << PASS_MARK << ") : " << countPassing(list)
         << " of " << list.size() << endl;
}

// ----------------------------------------------------------
//  Section 4: searching and sorting
// ----------------------------------------------------------

// Returns the position of the student with this id, or -1 if not found.
int findStudentIndex(const vector<Student>& list, int id) {
    int index = 0;
    while (index < (int)list.size()) {
        if (list[index].id == id) {
            return index;
        }
    }
    return -1;
}

void searchAndShow(const vector<Student>& list, int id) {
    int pos = findStudentIndex(list, id);
    if (pos == -1) {
        cout << "Student " << id << " not found." << endl;
    } else {
        cout << "Student " << id << " is " << list[pos].name
             << " (total " << list[pos].total
             << ", grade " << list[pos].grade << ")" << endl;
    }
}

// Bubble sort, highest total first.
void sortByTotalDescending(vector<Student>& list) {
    int n = (int)list.size();
    for (int pass = 0; pass < n - 1; pass++) {
        for (int i = 0; i < n - 1 - pass; i++) {
            if (list[i].total < list[i + 1].total) {
                Student temp = list[i];
                list[i] = list[i + 1];
                list[i + 1] = temp;
            }
        }
    }
}

void printRanking(const vector<Student>& list) {
    for (int i = 0; i < (int)list.size(); i++) {
        cout << setw(2) << (i + 1) << ". " << left << setw(10) << list[i].name
             << right << setw(4) << list[i].total
             << "  (" << list[i].grade << ")" << endl;
    }
}

// ----------------------------------------------------------
//  Section 5: grade distribution histogram
// ----------------------------------------------------------

void buildHistogram(const vector<Student>& list, int bins[]) {
    for (int b = 0; b < NUM_BINS; b++) {
        bins[b] = 0;
    }
    for (int i = 0; i < (int)list.size(); i++) {
        int bin = list[i].total / 20;
        if (bin > NUM_BINS - 1) {
            bin = NUM_BINS - 1;
        }
        bins[bin]++;
    }
}

void printHistogram(const int bins[]) {
    const string labels[NUM_BINS] = {"  0-19", " 20-39", " 40-59", " 60-79", "80-100"};
    for (int b = 0; b < NUM_BINS; b++) {
        cout << labels[b] << " | ";
        for (int k = 0; k < bins[b]; k++) {
            cout << '#';
        }
        cout << " (" << bins[b] << ")" << endl;
    }
}

// ----------------------------------------------------------
//  Main program
// ----------------------------------------------------------

int main() {
    vector<Student> students;
    students.push_back({1, "Andi",   80, 75, 90, 0, ' '});
    students.push_back({2, "Budi",   60, 55, 65, 0, ' '});
    students.push_back({3, "Citra",  95, 90, 98, 0, ' '});
    students.push_back({4, "Dewi",   70, 65, 60, 0, ' '});
    students.push_back({5, "Eko",    45, 50, 38, 0, ' '});
    students.push_back({6, "Fitri",  88, 92, 85, 0, ' '});
    students.push_back({7, "Gita",   60, 60, 60, 0, ' '});
    students.push_back({8, "Hadi",   72, 80, 77, 0, ' '});

    fillResults(students);

    printHeader("1. RESULTS TABLE");
    printTable(students);

    printHeader("2. CLASS STATISTICS");
    printStatistics(students);

    printHeader("3. STUDENT SEARCH");
    searchAndShow(students, 1);
    searchAndShow(students, 5);
    searchAndShow(students, 99);

    printHeader("4. RANKING");
    vector<Student> ranked = students;
    sortByTotalDescending(ranked);
    printRanking(ranked);

    printHeader("5. GRADE DISTRIBUTION");
    int bins[NUM_BINS];
    buildHistogram(students, bins);
    printHistogram(bins);

    printHeader("6. RE-SIT REPORT");
    int resitTotal = 0;
    int resitPassed = 0;
    for (int i = 0; i < (int)students.size(); i++) {
        if (students[i].total < 40) {
            resitTotal++;
            // a re-sit result would be recorded here
        }
    }
    cout << "Students needing a re-sit : " << resitTotal << endl;
    cout << "Re-sit pass rate          : "
         << percentage(resitPassed, resitTotal) << "%" << endl;

    printHeader("7. AFTER 5 BONUS POINTS");
    vector<Student> curved = students;
    applyBonus(curved, 5);
    printTable(curved);
    cout << "New class average : " << classAverage(curved) << endl;

    return 0;
}
