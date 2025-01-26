#include <iostream>
#include <cctype>
#include <iomanip>
#include <fstream>
using namespace std;

struct Node {
    string licensePlate;
    int row, col;
    Node* next;
};

class ParkingLot {
private:
    int rows, columns;
    string** parkingSpots;
    Node* parkedVehicles;
    const int maxQueueSize = 15;
    string parkingQueue[15];
    int front, rear;
    bool isSetUp;

public:
    ParkingLot() : rows(0), columns(0), parkingSpots(nullptr), parkedVehicles(nullptr), front(0), rear(0), isSetUp(false) {}

    void setParkingLot() {
        cout << "Enter the number of rows of the parking lot: ";
        cin >> rows;
        cout << "Enter the number of columns of the parking lot: ";
        cin >> columns;
        cout << "\nParking lot is made with the dimension of: " << rows << " x " << columns << endl;

        parkingSpots = new string*[rows];
        for (int i = 0; i < rows; i++) {
            parkingSpots[i] = new string[columns];
        }

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < columns; j++) {
                parkingSpots[i][j] = "Available";
            }
        }

        isSetUp = true;
        saveParkingLot();
        cout << "Parking lot set up successfully.\n";
    }

    void displayParkingLot() {

        int width = 12;
        cout << setw(width) << "\nAVAILABLE SLOTS\n\n";
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < columns; j++) {
                cout << setw(width) << (parkingSpots[i][j] == "Available" ? "Available" : parkingSpots[i][j]) << " ";
            }
            cout << "\n\n";
        }
    }

    void parkCar(int row, int col, const string& licensePlate) {
        Node* newCar = new Node;
        newCar->licensePlate = licensePlate;
        newCar->row = row;
        newCar->col = col;
        newCar->next = parkedVehicles;
        parkedVehicles = newCar;

        logActivity("Parked", licensePlate, row, col);
    }
    
    void logActivity(const string& action, const string& licensePlate, int row, int col) {
        ofstream log("parking_log.txt", ios::app);
        log << action << ": License Plate " << licensePlate << " at (" << row << ", " << col << ")\n";
        log.close();
    }

    bool isLicensePlateValid(const string& licensePlate) {
        if (licensePlate.length() != 5 && licensePlate.length() != 6 && licensePlate.length() != 7) {
            return false;
        }

        for (int i = 0; i < 3; i++) {
            if (!isalpha(licensePlate[i])) {
                return false;
            }
        }

        for (int i = 3; i < licensePlate.length(); i++) {
            if (!isdigit(licensePlate[i])) {
                return false;
            }
        }

        return true;
    }

	void retrieveCar(const string& licensePlate) {
    if (parkedVehicles == nullptr) {
        cout << "\nThe parking lot is empty. There are no cars to retrieve.\n";
        return;
    }
 

    Node* prev = nullptr;
    Node* current = parkedVehicles;

    while (current != nullptr) {
        if (current->licensePlate == licensePlate) {
            cout << "Car with license plate " << licensePlate << " has been retrieved\n";
            parkingSpots[current->row][current->col] = "Available"; 
            

            if (prev != nullptr) {
                prev->next = current->next;
            } else {
                parkedVehicles = current->next;
            }
            
           
            logActivity("Retrieved", licensePlate, current->row, current->col);
            
            delete current;


            if (front != rear) { 
                string waitingCar = dequeue(); 
                bool parked = false;

                for (int i = 0; i < rows && !parked; i++) {
                    for (int j = 0; j < columns && !parked; j++) {
                        if (parkingSpots[i][j] == "Available") {
                            parkingSpots[i][j] = waitingCar; 
                            parkCar(i, j, waitingCar); 
                            parked = true;
                            cout << "Car " << waitingCar << " parked successfully\n";
                        }
                    }
                }

                if (!parked) {
                    cout << "Parking lot is full, car is still in waiting queue.\n";
                }
            }
            
            return;
        }
        prev = current;
        current = current->next;
    }

    cout << "Car " << licensePlate << " not found.\n";
}

    void enqueue(const string& licensePlate) {
        if (rear >= maxQueueSize) {
            cout << "Waiting queue is full.\n";
        } else {
            parkingQueue[rear++] = licensePlate;
        }
    }

    string dequeue() {
        if (front == rear) {
            return "Available";
        } else {
            return parkingQueue[front++];
        }
    }

	void parkVehicle() {
        if (!isSetUp) {
        	cout << "\nSet up the parking lot first.\n";
        	return;
    	}

    string licensePlate;
    cout << "\nLicense plate of car you want to park (AAA000/AAA00/AAA0000) [A-Z, 0-9]: ";
    cin >> licensePlate;

    if (!isLicensePlateValid(licensePlate)) {
        cout << "Invalid license plate format.\n";
        return;
    }

    bool parked = false;
    for (int i = 0; i < rows && !parked; i++) {
        for (int j = 0; j < columns && !parked; j++) {
            if (parkingSpots[i][j] == "Available") {
                parkingSpots[i][j] = licensePlate;
                parkCar(i, j, licensePlate);
                parked = true;
                cout << "\nCar " << licensePlate << " parked successfully\n";
            }
        }
    }

    if (!parked) {
        cout << "Parking lot is full. Adding to the waiting queue.\n";
        enqueue(licensePlate);
    }
}

    bool getIsSetUp() {
        return isSetUp;
    }

    void saveParkingLot() {
        ofstream outFile("parking_lot.txt");

        outFile << rows << " " << columns << endl;  
        Node* current = parkedVehicles;
        while (current != nullptr) {
            outFile << current->licensePlate << " " << current->row << " " << current->col << endl;
            current = current->next;
        }

        outFile.close();
    }

    void loadParkingLot() {
        ifstream inFile("parking_lot.txt");

        if (inFile.is_open()) {
            inFile >> rows >> columns; 

            parkingSpots = new string*[rows];
            for (int i = 0; i < rows; i++) {
                parkingSpots[i] = new string[columns];
            }

            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < columns; j++) {
                    parkingSpots[i][j] = "Available";
                }
            }

            string licensePlate;
            int row, col;
            while (inFile >> licensePlate >> row >> col) {
                parkingSpots[row][col] = licensePlate;
                parkCar(row, col, licensePlate);
            }

            inFile.close();
            isSetUp = true;
            cout << "Parking lot loaded successfully from file.\n";
        } else {
            cout << "No saved parking lot data found. Please set up the parking lot first.\n";
        }
    }

    void deleteParkingLot() {
    	if (!isSetUp) {
        	cout << "\nCan't delete a parking lot that doesn't exist. Set up a parking lot first.\n";
        	return;
    	}
    	
    	
        if (parkedVehicles != nullptr) {
            Node* current = parkedVehicles;
            while (current != nullptr) {
                Node* toDelete = current;
                current = current->next;
                delete toDelete;
            }
            parkedVehicles = nullptr;
        }

        if (parkingSpots != nullptr) {
            for (int i = 0; i < rows; i++) {
                delete[] parkingSpots[i];
            }
            delete[] parkingSpots;
            parkingSpots = nullptr;
        }

        rows = 0;
        columns = 0;
        isSetUp = false;
        remove("parking_lot.txt"); 
        cout << "Parking lot has been deleted and reset.\n";
    }

};

	

int main() {
    int option;
    bool programRunning = true;
    ParkingLot parkingLot;
    
	parkingLot.loadParkingLot();

    while (programRunning) {
    	cout << "\n----- Parking Lot Management System -----\n";
        cout << "1. Set up parking lot\n";
        cout << "2. Park a vehicle\n";
        cout << "3. Retrieve a vehicle by license plate\n";
        cout << "4. Display available slots\n";
        cout << "5. Delete parking lot\n";
        cout << "6. Exit program";
        cout << "\n-----------------------------------------\n";
        cout << "Choice: ";
        cin >> option;
     	system("cls");

        switch (option) {
            case 1: {
                parkingLot.setParkingLot();
                break;
            }
			case 2: {
                if (!parkingLot.getIsSetUp()) {
        			cout << "\nSet up the parking lot first.\n";
        			break;
    			}
    			parkingLot.displayParkingLot();
    			parkingLot.parkVehicle();
    			break;
    		}
            case 3: {
                if (!parkingLot.getIsSetUp()) {
        			cout << "\nSet up the parking lot first.\n";
        			break;
    			}
    			parkingLot.displayParkingLot();
                string licensePlate;
                cout << "\nEnter the license plate of the car to retrieve: ";
                cin >> licensePlate;
                parkingLot.retrieveCar(licensePlate);
                break;
            }
            case 4:{
                if (!parkingLot.getIsSetUp()) {
        			cout << "\nSet up the parking lot first.\n";
        			break;
    			}
                parkingLot.displayParkingLot();
                break;
            }
            case 5:{
                parkingLot.deleteParkingLot();
                break;
        	}
            case 6:{
                cout << "\n\n\nThank you for using the Parking Lot Management System.\n\n\n";
                parkingLot.saveParkingLot();
                programRunning = false;
                break;
            }
            default:
                cout << "\nInvalid choice, try again.\n";
                break;
        }
    }

    return 0;
}

