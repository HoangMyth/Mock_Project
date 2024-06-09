#include <iostream>
#include "USB.h"

int main() {
    int choice = 0;
    USB usb;
    while (true) {
        usb.displayMenu();
        std::cin >> choice;

        switch (choice) {
            case 1:
                usb.playFromUSB();
                break;
            case 2:
                usb.playFromFolder();
                break;
            case 3:
                std::cout << "Exiting...\n";
                return 0;
            default:
                std::cout << "Invalid choice. Please try again.\n";
                break;
        }
    }

    return 0;
}