#include <vector>
#include <iostream>
#include "constants.h"


std::vector<int> generate_rows() {
    std::vector<int> strings;
    int plus_number[CA - 1] = {0};
    int row[CA];

    bool is_finish = false;
    do {
        for (int i = 0; i < CA; ++i) {
            if (i == 0) {
                row[i] = plus_number[i];
            } else if (i == CA - 1) {
                row[i] = NA - plus_number[i - 1];
            } else {
                row[i] = plus_number[i] - plus_number[i - 1];
            }
        }

        strings.insert(strings.end(), row, row + CA);

        int index = CA - 1;
        do {
            --index;
            if (index < 0) {
                is_finish = true;
            }
            ++plus_number[index];
            for (int i = index + 1; i < CA - 1; ++i) {
                plus_number[i] = plus_number[index];
            }
        } while (plus_number[index] == NA + 1);
    } while (!is_finish);

    return strings;
}


void print_rows(const std::vector<int> &rows) {
    for (int i = 0; i < rows.size() / CA; ++i) {
        std::cout << i << ": ";
        for (int j = 0; j < CA; ++j) {
            std::cout << rows[i * CA + j];
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}