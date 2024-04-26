/* COMPSCI 424 Program 3
   Name: Bonnie Sullivan
   
   p3main.cpp: contains the main function for this program.

   This is a template. Feel free to edit any of these files, even
   these pre-written comments or my provided code. You can add any
   classes, methods, and data structures that you need to solve the
   problem and display your solution in the correct format.
*/

// You will probably need to add more #include directives.
// Remember: use <angle brackets> for standard C++ headers/libraries
// and "double quotes" for headers in the same directory as this file.
#include <iostream>
#include <fstream>
#include <string>
#include <cstdio> // for sscanf because Dr. Oster likes the C way to
                  // get data from strings better than the C++ way
using namespace std; // if you want to type out "std::" every time, delete this

/*
   If you want your variables and data structures for the banker's
   algorithm to have global scope, declare them here. This may make
   the rest of your program easier to write. 
   
   Most software engineers say global variables are a Bad Idea (and 
   they're usually correct!), but systems programmers do it all the
   time, so I'm allowing it here.
*/

    int* Available;    // Available units of each resource
    int** Max;         // Max possible claim by each process for each resource
    int** Allocation;  // Num resources currently allocated to each process
    int** Request;     // Request edges in claim graph (????)
    int** Need;

/*
  Arguments:
    argc: the number of command-line arguments, which should be >= 2
    argv[0]: the name of the executable file
    argv[1]: a C-string, either "manual" or "auto"
    argv[2]: a C-string containing the name of the setup file (and the
             path to the file, if it's not in the current directory)
*/
int main (int argc, char *argv[]) {
    // To help you get started, the major steps for the main program
    // are shown here as comments. Feel free to add more comments to
    // help you understand your code, or for any reason. Also feel free
    // to edit this comment to be more helpful.

    // Code to test command-line processing. You can keep, modify, or
    // remove this code. It's not required.
    if (argc < 2) {
        cerr << argc << " command-line arguments provided, 2 expected; exiting." << endl;
        if (argc == 1) cerr << "argv[1] == " << argv[1] << endl;
        return 1; // non-zero return value indicates abnormal termination
    }

    cout << "Selected mode: " << argv[1] << endl;
    cout << "Setup file location: " << argv[2] << endl;

    // 1. Open the setup file using the path in argv[2]
    ifstream setup_file;
    string line;
    int num_resources;
    int num_processes;

    setup_file.open(argv[2], ios::in);
    if (setup_file.is_open()) {
        // 2. Get the number of resources and processes from the setup
        // file, and use this info to create the Banker's Algorithm
        // data structures
        setup_file >> num_resources;
        cout << num_resources << " resources" << endl;

        getline(setup_file, line); // skips the rest of the "resources" line
        
        setup_file >> num_processes;
        cout << num_processes << " processes" << endl;
        
        getline(setup_file, line); // skips the rest of the "processes" line


        // Create the Banker's Algorithm data structures, in any
        // way you like as long as they have the correct size
        // (unfortunately, you might not be able to use sscanf for this...)

        Available = new int[num_resources];    // Available units of each resource
        Max = new int*[num_processes];        // Max possible claim by each process for each resource
        Allocation = new int*[num_processes]; // Num resources currently allocated to each process
        Request = new int* [num_processes];   // Request edges in claim graph (????)
        Need = new int* [num_processes];
        for (int i = 0; i < num_processes; i++) {
            Allocation[i] = new int[num_resources];
            Max[i] = new int[num_resources];
            Request[i] = new int[num_resources];
            Need[i] = new int[num_resources];
        }
        
        // 3. Use the rest of the setup file to initialize the data structures
        
        // Initialize Available[m]
        for (int i = 0; i < num_resources; i++) {
            setup_file >> Available[i];
        }
        cout << "Available" << endl;
        for (int i = 0; i < num_resources; i++) {
            cout << Available[i] << " ";
        }
        getline(setup_file, line);

        // Initialize Max[n][m]
        for (int i = 0; i < num_processes; i++) {
            for (int j = 0; j < num_resources; j++) {
                setup_file >> Max[i][j];
            }
        }
        cout << "Max" << endl;
        for (int i = 0; i < num_processes; i++) {
            for (int j = 0; j < num_resources; j++) {
                cout << Max[i][j] << " ";
            }
            cout << endl;
        }
        getline(setup_file, line);

        // Initialize Allocation[n][m]
        for (int i = 0; i < num_processes; i++) {
            for (int j = 0; j < num_resources; j++) {
                setup_file >> Allocation[i][j];
            }
        }
        cout << "Allocation" << endl;
        for (int i = 0; i < num_processes; i++) {
            for (int j = 0; j < num_resources; j++) {
                cout << Allocation[i][j] << " ";
            }
            cout << endl;
        }
        getline(setup_file, line);  

        // Initialize Need (Potential Requests) = Max - Allocation
        for (int i = 0; i < num_processes; i++) {
            for (int j = 0; j < num_resources; j++) {
                Need[i][j] = Max[i][j] - Allocation[i][j];
            }
        }

        // Done reading the file, so close it
        setup_file.close();
    } // end: if setup_file.is_open()

    // 4. Check initial conditions to ensure that the system is
    // beginning in a safe state: see "Check initial conditions"
    // in the Program 3 instructions
    for (int i = 0; i < num_processes; i++) {
        for (int j = 0; j < num_resources; j++) {
            if (Allocation[i][j] > Max[i][j]) {
                cout << "Request Denied" << endl;
                return 0;
            }
            // Need (Potential Requests) = Max - Allocation
            // The graph is completely reducible if potential requests <= Available units
            if (Need[i][j] > Available[i]) {
                cout << "Request Denied" << endl;
                return 0;
            }
            // Continue with the program if all conditions are met
        }
    }

    // 5. Go into either manual or automatic mode, depending on
    // the value of args[0]; you could implement these two modes
    // as separate methods within this class, as separate classes
    // with their own main methods, or as additional code within
    // this main method.
    
    // Enter manual mode
    if (argv[0] == "manual") {
        string userCommand;
        string junkStr;
        int i;
        int j;
        int k;
        cout << "Entering manual mode. Please input a command: " << endl;
        cout << "(request/release I of J for K, or end to quit)" << endl;
        cin >> userCommand; // request/release
        if (userCommand == "request" || userCommand == "release") {
            cin >> i;           // I
            cin >> junkStr;     // of
            cin >> j;           // J
            cin >> junkStr;     // for 
            cin >> k;           // K
            
            if (userCommand == "request") {
                cout << "Process " << k << " requests " << i << " units of resource " << j;
                // Assume the request is accepted
                Available[j] = Available[j] - i;            // Available decreases by num units req
                Allocation[i][j] = Allocation[i][j] + i;    // Allocated increased by num units req
                Need[i][j] = Max[i][j] - Allocation[i][j];
                // If request is not possible, print it is denied & exit
                if (Allocation[i][j] > Max[i][j] || Need[i][j] > Available[i]) {
                    cout << ":denied" << endl;
                    return 0;
                }
            }
            if (userCommand == "release") {
                cout << "Process " << k << " releases " << i << " units of resource " << j;
                // Assume the request is accepted
                Available[j] = Available[j] + i;            // Available increases by num units req
                Allocation[i][j] = Allocation[i][j] - i;    // Allocated decreases by num units req
                Need[i][j] = Max[i][j] - Allocation[i][j];  // Potential req updated
                // If request is not possible, print it is denied & exit
                if (Allocation[i][j] > Max[i][j] || Need[i][j] > Available[i]) {
                    cout << ":denied" << endl;
                    return 0;
                }
            }

        }
        if (userCommand == "end") {
            return 0;
        }
    }
    
    // Delete dynamically created arrays before the program terminates
    delete[] Available;
    for (int i = 0; i < num_processes; i++) {
        delete[] Max[i];
        delete[] Allocation[i];
        delete[] Request[i];
        delete[] Need[i];
    }
    delete[] Max;
    delete[] Allocation;
    delete[] Request;
    delete[] Need;

    return 0; // terminate normally
}
