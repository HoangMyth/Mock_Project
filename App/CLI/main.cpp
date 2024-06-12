#include <iostream>
#include "USB.h"
int main() {
    char choice;
    USB usb;
    while (true) {
        usb.displayMenu();
        while (true) {
            std::cin >> choice;

            // Check if the input was valid and is a single character
            if (std::cin.fail() || std::cin.peek() != '\n') {
                std::cin.clear(); // Clear the error flag
                //std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
                std::cout << "Invalid input. Please enter a single character." << std::endl;
            } else {
                break; // Exit loop if input is valid
            }
        }
        switch (choice) {
            case 'u':
                usb.playFromUSB();
                break;
            case 'f':
                usb.playFromFolder();
                break;
            case 'e':
                std::cout << "Exiting...\n";
                return 0;
            default:
                std::cout << "Invalid choice. Please try again.\n";
                break;
        }
    }

    return 0;
}