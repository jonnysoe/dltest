#include <iostream>
#include <vector>
#include "common.h"

std::vector<int> numbers;

void DisplayNumbers() {
    std::cout << "Numbers:" << std::endl;
    for (auto& i: numbers) {
        std::cout << i << std::endl;
    }
}

void AddNumbers(int num) {
    numbers.push_back(num);
    DisplayNumbers();
}