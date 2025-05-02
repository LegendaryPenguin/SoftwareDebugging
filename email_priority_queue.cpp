/*
EECS 348 Programming Assignment 8
This program is a CEO email prioritization program that assigns a priority number to each email sender and filters the emails in the order by which they should be read
The input for this program will be any txt file. For the purpose of this assignment, the input will be Assignment1_Test_File.txt
The output for this program will be the emails.
Collaborator: None
Creation Date: 05/02/2025
*/

#include <iostream>   // This line includes the iostream library for input and output operations
#include <vector>     // This line includes the vector library for using dynamic arrays
#include <string>     // This line includes the string library for using the string class
#include <sstream>    // This line includes the sstream library for string stream operations
#include <algorithm>  // This line includes the algorithm library for functions like swap
#include <iomanip>    // This line includes the iomanip library for formatted output
#include <map>        // This line includes the map library for using key-value pairs
#include <ctime>      // This line includes the ctime library for handling date and time
#include <fstream>    // This line includes the fstream library for file input

using namespace std; // This line allows using standard namespace names directly

int arrivalCounter = 0; // This line declares a global counter to assign arrival order to emails

// This function converts a date string in MM-DD-YYYY format to a time_t object for comparison
time_t parseDate(const string& dateStr) {
    struct tm tm{}; // This line declares a tm struct to hold date components
    sscanf(dateStr.c_str(), "%d-%d-%d", &tm.tm_mon, &tm.tm_mday, &tm.tm_year); // This line extracts month, day, year
    tm.tm_mon -= 1;           // This line adjusts month to be zero-based
    tm.tm_year -= 1900;       // This line adjusts year to be relative to 1900
    return mktime(&tm);       // This line converts struct tm to time_t
}

// This map assigns priority values to different sender categories
map<string, int> senderPriority = {
    {"Boss", 5},
    {"Subordinate", 4},
    {"Peer", 3},
    {"ImportantPerson", 2},
    {"OtherPerson", 1}
};

// This class represents an Email with attributes and comparison logic
class Email {
public:
    string senderCategory;  // This line declares a string variable for sender category
    string subject;         // This line declares a string variable for email subject
    string dateStr;         // This line declares a string variable for the date in string format
    time_t date;            // This line declares a time_t variable for date comparison
    int arrivalOrder;       // This line declares an integer for order of arrival

    // This constructor initializes the email attributes and sets arrival order
    Email(string sender, string subj, string date_string) {
        senderCategory = sender;
        subject = subj;
        dateStr = date_string;
        date = parseDate(date_string);
        arrivalOrder = arrivalCounter++;
    }

    // This operator overload defines how to compare email priority
    bool operator>(const Email& other) const {
        if (senderPriority[senderCategory] != senderPriority[other.senderCategory])
            return senderPriority[senderCategory] > senderPriority[other.senderCategory];
        if (date != other.date)
            return date > other.date;
        return arrivalOrder < other.arrivalOrder; // This line prioritizes newer emails if all else is equal
    }

    // This function displays the email details to the console
    void display() const {
        cout << "Sender: " << senderCategory << endl;
        cout << "Subject: " << subject << endl;
        cout << "Date: " << dateStr << endl;
    }
};

// This class implements a MaxHeap to manage emails based on priority
class MaxHeap {
private:
    vector<Email> heap; // This line declares a vector to hold the heap of emails

    // This function restores the heap property by moving the element at idx upward
    void heapifyUp(int idx) {
        while (idx > 0 && heap[idx] > heap[(idx - 1) / 2]) {
            swap(heap[idx], heap[(idx - 1) / 2]);
            idx = (idx - 1) / 2;
        }
    }

    // This function restores the heap property by moving the element at idx downward
    void heapifyDown(int idx) {
        int size = heap.size(); // This line stores the size of the heap
        while (true) {
            int largest = idx;
            int left = 2 * idx + 1;
            int right = 2 * idx + 2;

            if (left < size && heap[left] > heap[largest]) largest = left;
            if (right < size && heap[right] > heap[largest]) largest = right;

            if (largest != idx) {
                swap(heap[idx], heap[largest]);
                idx = largest;
            } else {
                break;
            }
        }
    }

public:
    // This function inserts a new email into the heap
    void push(const Email& email) {
        heap.push_back(email);
        heapifyUp(heap.size() - 1);
    }

    // This function returns the top priority email without removing it
    Email peek() {
        if (!heap.empty()) return heap[0];
        throw runtime_error("No emails to read.");
    }

    // This function removes the top priority email from the heap
    void pop() {
        if (heap.empty()) return;
        heap[0] = heap.back();
        heap.pop_back();
        if (!heap.empty()) heapifyDown(0);
    }

    // This function returns the number of emails in the heap
    int size() const {
        return heap.size();
    }

    // This function checks if the heap is empty
    bool empty() const {
        return heap.empty();
    }
};

// This class manages the flow of email commands and state
class EmailManager {
private:
    MaxHeap heap;                         // This line declares a MaxHeap instance to store emails
    bool hasCurrent = false;              // This line keeps track of whether there is a current email selected
    Email currentEmail{"", "", "01-01-2000"}; // This line initializes a dummy current email

public:
    // This function processes a line of input command
    void processLine(const string& line) {
        if (line.rfind("EMAIL ", 0) == 0) {
            string rest = line.substr(6);
            stringstream ss(rest);
            string sender, subject, date;

            getline(ss, sender, ',');
            getline(ss, subject, ',');
            getline(ss, date, ',');

            Email email(sender, subject, date);
            heap.push(email);
        } else if (line == "COUNT") {
            cout << "There are " << heap.size() << " emails to read.\n" << endl;
        } else if (line == "NEXT") {
            if (!hasCurrent) {
                if (!heap.empty()) {
                    currentEmail = heap.peek();
                    hasCurrent = true;
                }
            }
            if (hasCurrent) {
                cout << "Next email:" << endl;
                currentEmail.display();
                cout << endl;
            } else {
                cout << "No emails to read.\n" << endl;
            }
        } else if (line == "READ") {
            if (hasCurrent) {
                heap.pop();
                hasCurrent = false;
            } else if (!heap.empty()) {
                heap.pop();
            }
        }
    }
};

// This function runs email processing using input from a file
void runFromFile(const string& filename) {
    ifstream infile(filename); // This line opens the input file
    string line;               // This line declares a string for reading lines
    EmailManager manager;      // This line creates an EmailManager instance

    while (getline(infile, line)) {
        if (!line.empty()) {
            manager.processLine(line);
        }
    }
}

int main() {
    runFromFile("test.txt"); // This line calls the file-based email runner with test input
    return 0;                // This line returns 0 to indicate successful execution
}
