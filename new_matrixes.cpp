#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "constants.h"

std::ofstream fout;
pthread_mutex_t mutex2[2];
std::vector<int> rows;
std::vector<std::vector<std::vector<char>>> list_lock_lm;
std::vector<std::vector<std::vector<char>>> list_lock_nlm;
std::vector<std::vector<bool>> list_bool_lm;
std::vector<std::vector<char>> list_matrix;
long long int glob_tmp_mt_num = 0, glob_count_to_eqv = 0;

int list_cnt = 1;
long long matrix_cnt = 0;
bool finish = false;
int cnt_fs = 0;


void print_matrix_latex(const std::vector<char> &matrix) {
    fout << "M =\n"
            "\\begin{tikzpicture}[baseline=-0.3\\baselineskip]\n"
            "\\matrix\n"
            "{";
    for (int i = 0; i < CA; ++i) {
        for (int j = 0; j < CA; ++j) {
            fout << int(matrix[i * CA + j]);
            if (j != CA - 1) {
                fout << " & ";
            }
        }
        fout << " \\\\ ";
    }
    fout << "};\n"
            "\\end{tikzpicture}";
}


void print_matrix(const std::vector<char> &matrix) {
    for (int i = 0; i < CA; ++i) {
        for (int j = 0; j < CA; ++j) {
            std::cout << int(matrix[i * CA + j]) << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}


bool on_i_is_bad(const int *rows_number, const int &i) {
    if (rows[rows_number[i] * CA + i] == NA) {
        return true;
    }
    if (i > 0 && rows[rows_number[i] * CA + i] < rows[rows_number[i - 1] * CA + i - 1]) {
        return true;
    } else if (i > 0 && rows[rows_number[i] * CA + i] > rows[rows_number[i - 1] * CA + i - 1]) {
        return false;
    } else {
        for (int j = 0; j < i - 1; ++j) {
            if (rows[rows_number[j] * CA + i] < rows[rows_number[j] * CA + i - 1]) {
                return true;
            } else if (rows[rows_number[j] * CA + i] > rows[rows_number[j] * CA + i - 1]) {
                return false;
            }
        }
    }
    for (int j = 0; j < i - 1; ++j) {
        if (rows[rows_number[i] * CA + j] < rows[rows_number[i - 1] * CA + j]) {
            return true;
        } else if (rows[rows_number[i] * CA + j] > rows[rows_number[i - 1] * CA + j]) {
            return false;
        }
    }
    if (i > 0 && rows[rows_number[i] * CA + i - 1] < rows[rows_number[i - 1] * CA + i]) {
        return true;
    } else if (i > 0 && rows[rows_number[i] * CA + i - 1] > rows[rows_number[i - 1] * CA + i]) {
        return false;
    }
    return false;
}


bool on_i_is_bad(const std::vector<char> &m, const std::vector<int> &per, const int &i, const int &s) {
    for (int j = 0; j < i - 1; ++j) {
        if (j < s) {
            if (m[j * CA + per[i - s]] < m[j * CA + per[i - s - 1]]) {
                return true;
            } else if (m[j * CA + per[i - s]] > m[j * CA + per[i - s - 1]]) {
                return false;
            }
        } else {
            if (m[per[j - s] * CA + per[i - s]] < m[per[j - s] * CA + per[i - s - 1]]) {
                return true;
            } else if (m[per[j - s] * CA + per[i - s]] > m[per[j - s] * CA + per[i - s - 1]]) {
                return false;
            }
        }
    }
    for (int j = 0; j < i - 1; ++j) {
        if (j < s) {
            if (m[per[i - s] * CA + j] < m[per[i - s - 1] * CA + j]) {
                return true;
            } else if (m[per[i - s] * CA + j] > m[per[i - s - 1] * CA + j]) {
                return false;
            }
        } else {
            if (m[per[i - s] * CA + per[j - s]] < m[per[i - s - 1] * CA + per[j - s]]) {
                return true;
            } else if (m[per[i - s] * CA + per[j - s]] > m[per[i - s - 1] * CA + per[j - s]]) {
                return false;
            }
        }
    }
//    if (i - s > 0 && m[per[i - s] * CA + per[i - s - 1]] < m[per[i - s - 1] * CA + per[i - s]]) {
//        return true;
//    }
//    return false;
    return i - s > 0 && m[per[i - s] * CA + per[i - s - 1]] < m[per[i - s - 1] * CA + per[i - s]];
}


void make_per(std::vector<char> &m,
              const std::vector<char> &matrix,
              const std::vector<int> &per,
              const int &s, const int &f) {
    for (int i = 0; i < CA; ++i) {
        for (int j = 0; j < CA; ++j) {
            if ((s <= i && i < f) && (s <= j && j < f)) {
                m[i * CA + j] = matrix[per[i - s] * CA + per[j - s]];
            } else if (!(s <= i && i < f) && (s <= j && j < f)) {
                m[i * CA + j] = matrix[i * CA + per[j - s]];
            } else if ((s <= i && i < f) && !(s <= j && j < f)) {
                m[i * CA + j] = matrix[per[i - s] * CA + j];
            } else {
                m[i * CA + j] = matrix[i * CA + j];
            }
        }
    }
}


void create_list_per_matrix(const std::vector<char> &matrix, std::vector<std::vector<char>> &list_per_matrix) {
    int s1 = CA, s2 = CA;
    for (int i = 0; i < CA && s2 == CA; ++i) {
        if (matrix[i * CA + i] == 1 && s1 == CA) {
            s1 = i;
        } else if (matrix[i * CA + i] == 2) {
            s2 = i;
            if (s1 == CA) {
                s1 = s2;
            }
        }
    }

    std::vector<int> per0(s1);
    for (int i = 0; i < per0.size(); ++i) { per0[i] = i; }
    std::vector<int> per1(s2 - s1), cper1(s2 - s1);
    for (int i = 0; i < per1.size(); ++i) { per1[i] = i + s1; }
    for (int i = 0; i < cper1.size(); ++i) { cper1[i] = i + s1; }
    std::vector<int> per2(CA - s2), cper2(CA - s2);
    for (int i = 0; i < per2.size(); ++i) { per2[i] = i + s2; }
    for (int i = 0; i < cper2.size(); ++i) { cper2[i] = i + s2; }

    std::vector<char> m_cmp0(CA * CA), m_cmp1(CA * CA), m_cmp2(CA * CA);

    do {
        bool is_good = true;
        for (int j = 1; j < s1; ++j) {
            if (on_i_is_bad(matrix, per0, j, 0)) {
                is_good = false;
                break;
            }
        }
        if (is_good) {
            make_per(m_cmp0, matrix, per0, 0, s1);
            per1 = cper1;
            do {
                is_good = true;
                for (int j = s1 + 1; j < s2; ++j) {
                    if (on_i_is_bad(m_cmp0, per1, j, s1)) {
                        is_good = false;
                        break;
                    }
                }
                if (is_good) {
                    make_per(m_cmp1, m_cmp0, per1, s1, s2);
                    per2 = cper2;
                    do {
                        is_good = true;
                        for (int j = s2 + 1; j < CA; ++j) {
                            if (on_i_is_bad(m_cmp1, per2, j, s2)) {
                                is_good = false;
                                break;
                            }
                        }
                        if (is_good) {
                            make_per(m_cmp2, m_cmp1, per2, s2, CA);
                            list_per_matrix.push_back(m_cmp2);
                        }
                    } while (std::next_permutation(per2.begin(), per2.end()));
                }
            } while (std::next_permutation(per1.begin(), per1.end()));
        }
    } while (std::next_permutation(per0.begin(), per0.end()));
}


void calc_number_count(const std::vector<char> &matrix, std::vector<char> &number_count) {
    for (int i = 0; i < 30; ++i) {
        number_count[i] = 0;
    }
    int a = -1, b = -1;
    for (int i = 0; i < CA; ++i) {
        ++number_count[16 + matrix[i * CA + i]];
        for (int j = 0; j < CA; ++j) {
            ++number_count[matrix[i * CA + j]];
        }
        if (a == -1 && matrix[i * CA + i] != 0) {
            a = i;
        } else if (b == -1 && matrix[i * CA + i] == 2) {
            b = i;
        }
    }
    if (a == -1) {
        a = CA;
    }
    if (b == -1) {
        b = CA;
    }
    for (int i = 0; i < a; ++i) {
        for (int j = 0; j < a; ++j) {
            ++number_count[4 + matrix[i * CA + j]];
        }
    }
    for (int i = a; i < b; ++i) {
        for (int j = a; j < b; ++j) {
            ++number_count[8 + matrix[i * CA + j]];
        }
    }
    for (int i = b; i < CA; ++i) {
        for (int j = b; j < CA; ++j) {
            ++number_count[12 + matrix[i * CA + j]];
        }
    }
    for (int i = 0; i < CA; ++i) {
        for (int j = i + 1; j < CA; ++j) {
            ++number_count[20 + matrix[i * CA + j] * matrix[j * CA + i]];
        }
    }
}


bool local_consist(const std::vector<char> &matrix,
                   const std::vector<std::vector<char>> &lock_lm,
                   const std::vector<std::vector<char>> &lock_nlm,
                   const std::vector<char> &number_count) {
    std::vector<std::vector<char>> list_per_matrix;
//    list_per_matrix.reserve(CA * CA * CA);
    size_t idx = 0;
    bool was_create = false;
    for (const auto &m_tmp: lock_lm) {
        if (number_count == lock_nlm[idx++]) {
            if (not was_create) {
                create_list_per_matrix(matrix, list_per_matrix);
//                if (list_per_matrix.size() > 54)
//                std::cout << list_per_matrix.size() << " ";
                was_create = true;
            }
            for (const auto &per_matrix: list_per_matrix) {
                if (per_matrix == m_tmp) {
                    return true;
                }
            }
        }
    }
    return false;
}


bool next_first_strings(int *row_number, const int &strings_amount) {
    pthread_mutex_lock(&mutex2[1]);
    if (cnt_fs == std::pow(strings_amount, SC)) {
        std::cout << "Поток " << pthread_self() << " завершил свою работу" << std::endl;
        pthread_mutex_unlock(&mutex2[1]);
        return true;
    }

    if (SC == 1) {
//        std::cout << "Поток " << pthread_self() << " берёт " << cnt_fs << "ю строку" << std::endl;
        row_number[0] = cnt_fs++;
    } else if (SC == 2) {
//        std::cout << "Поток " << pthread_self() << " берёт строки [" << int(cnt_fs / strings_amount) << ", "
//                  << cnt_fs % strings_amount << "]" << std::endl;
        row_number[0] = cnt_fs / strings_amount;
        row_number[1] = cnt_fs++ % strings_amount;
    }
    pthread_mutex_unlock(&mutex2[1]);
    return false;
}


bool next_strings(int *rows_number, const int &strings_amount, int index, int &start_index) {
    do {
        --index;
        if (index < SC) {
            for (int i = SC; i < CA; ++i) {
                rows_number[i] = 0;
            }
            start_index = SC - 1;
            return next_first_strings(rows_number, strings_amount);
        }
        ++rows_number[index];
        for (int i = index + 1; i < CA; ++i) {
            rows_number[i] = 0;
        }
    } while (rows_number[index] == strings_amount);
    start_index = index;
    return false;
}


void *make_lock_lm(void *) {
    long long int tmp_mt_num = 0;
    std::vector<std::vector<char>> lock_lm;
    std::vector<std::vector<char>> lock_nlm;
    int number_of_rows = int(rows.size() / CA);
    std::vector<char> matrix(CA * CA);
    std::vector<char> number_count(30, 0);

    int rows_number[CA] = {0};
    long long int count_to_eqv = 0, count2 = 0, count4 = 0;
    long long int colors_count[CA] = {0};

    bool is_finish, need_next;
    int start_index = 0, count_zero, count_trash[10] = {0};

    is_finish = next_first_strings(rows_number, number_of_rows);

    do {
        need_next = false;

        // В столбце больше трёх ненулевых:
        for (int i = 0; i < CA && !need_next && !is_finish; ++i) {
            if (rows[rows_number[start_index] * CA + i] != 0) {
                count_zero = 0;
                for (int j = 0; j <= start_index && !need_next && !is_finish; ++j) {
                    if (rows[rows_number[j] * CA + i] != 0) {
                        ++count_zero;
                    }
                    if (count_zero > 3) {
                        need_next = true;
                        is_finish = next_strings(rows_number, number_of_rows, j + 1, start_index);
                        ++count4;
                    }
                }
            }
        }


        // Моя проверка на канон
        for (int i = start_index; i < CA && !need_next && !is_finish; ++i) {
            if (on_i_is_bad(rows_number, i)) {
                need_next = true;
                is_finish = next_strings(rows_number, number_of_rows, i + 1, start_index);
                ++count2;
            }
            for (int j = 0; j <= start_index && !need_next && !is_finish; ++j) {
                if ((rows[rows_number[i] * CA + j] == 0) != (rows[rows_number[j] * CA + i] == 0)) {
                    need_next = true;
                    is_finish = next_strings(rows_number, number_of_rows, i + 1, start_index);
                }
            }
        }


        // Проверка на замкнутость 2 цветов
        if (CA > 3) {
            for (int i = 0; i <= start_index && !need_next && !is_finish; ++i) {
                for (int j = i + 1; j <= start_index && !need_next && !is_finish; ++j) {
                    if (rows[rows_number[i] * CA + i] + rows[rows_number[i] * CA + j] +
                        rows[rows_number[j] * CA + i] + rows[rows_number[j] * CA + j] == 6) {
                        need_next = true;
                        is_finish = next_strings(rows_number, number_of_rows, j + 1, start_index);
                        ++colors_count[2];
                    }
                }
            }
        }


        // Проверка на замкнутость 3 цветов
        if (CA > 5) {
            for (int i = 0; i <= start_index && !need_next && !is_finish; ++i) {
                for (int j = i + 1; j <= start_index && !need_next && !is_finish; ++j) {
                    for (int k = j + 1; k <= start_index && !need_next && !is_finish; ++k) {
                        if (rows[rows_number[i] * CA + i] + rows[rows_number[i] * CA + j] +
                            rows[rows_number[j] * CA + i] +
                            rows[rows_number[j] * CA + j] + rows[rows_number[k] * CA + k] +
                            rows[rows_number[i] * CA + k] +
                            rows[rows_number[k] * CA + i] + rows[rows_number[k] * CA + j] +
                            rows[rows_number[j] * CA + k] == 9) {
                            need_next = true;
                            is_finish = next_strings(rows_number, number_of_rows, k + 1, start_index);
                            ++colors_count[3];
                        }
                    }
                }
            }
        }


        // Проверка на замкнутость 4 цветов
        if (CA > 7) {
            for (int i = 0; i <= start_index && !need_next && !is_finish; ++i) {
                for (int j = i + 1; j <= start_index && !need_next && !is_finish; ++j) {
                    for (int k = j + 1; k <= start_index && !need_next && !is_finish; ++k) {
                        for (int l = k + 1; l <= start_index && !need_next && !is_finish; ++l) {
                            if (rows[rows_number[i] * CA + i] + rows[rows_number[i] * CA + j] +
                                rows[rows_number[j] * CA + i] +
                                rows[rows_number[j] * CA + j] + rows[rows_number[k] * CA + k] +
                                rows[rows_number[i] * CA + k] +
                                rows[rows_number[k] * CA + i] + rows[rows_number[k] * CA + j] +
                                rows[rows_number[j] * CA + k] +
                                rows[rows_number[l] * CA + i] + rows[rows_number[l] * CA + j] +
                                rows[rows_number[l] * CA + k] +
                                rows[rows_number[i] * CA + l] + rows[rows_number[j] * CA + l] +
                                rows[rows_number[k] * CA + l] +
                                rows[rows_number[l] * CA + l] == 12) {
                                need_next = true;
                                is_finish = next_strings(rows_number, number_of_rows, l + 1, start_index);
                                ++colors_count[4];
                            }
                        }
                    }
                }
            }
        }


        // Проверка на замкнутость 5 цветов
        if (CA > 9) {
            for (int i = 0; i <= start_index && !need_next && !is_finish; ++i) {
                for (int j = i + 1; j <= start_index && !need_next && !is_finish; ++j) {
                    for (int k = j + 1; k <= start_index && !need_next && !is_finish; ++k) {
                        for (int l = k + 1; l <= start_index && !need_next && !is_finish; ++l) {
                            for (int n = l + 1; n <= start_index && !need_next && !is_finish; ++n) {
                                if (rows[rows_number[i] * CA + i] + rows[rows_number[i] * CA + j] +
                                    rows[rows_number[j] * CA + i] +
                                    rows[rows_number[j] * CA + j] + rows[rows_number[k] * CA + k] +
                                    rows[rows_number[i] * CA + k] +
                                    rows[rows_number[k] * CA + i] + rows[rows_number[k] * CA + j] +
                                    rows[rows_number[j] * CA + k] +
                                    rows[rows_number[l] * CA + i] + rows[rows_number[l] * CA + j] +
                                    rows[rows_number[l] * CA + k] +
                                    rows[rows_number[i] * CA + l] + rows[rows_number[j] * CA + l] +
                                    rows[rows_number[k] * CA + l] +
                                    rows[rows_number[l] * CA + l] + rows[rows_number[n] * CA + n] +
                                    rows[rows_number[n] * CA + i] + rows[rows_number[n] * CA + j] +
                                    rows[rows_number[n] * CA + k] + rows[rows_number[n] * CA + l] +
                                    rows[rows_number[i] * CA + n] + rows[rows_number[j] * CA + n] +
                                    rows[rows_number[k] * CA + n] + rows[rows_number[l] * CA + n] == 15) {
                                    need_next = true;
                                    is_finish = next_strings(rows_number, number_of_rows, n + 1, start_index);
                                    ++colors_count[5];
                                }
                            }
                        }
                    }
                }
            }
        }

        // Проерка, если строка 3
        for (int i = 0; i < CA && !need_next && !is_finish; ++i) {
            for (int j = 0; j < CA && !need_next && !is_finish; ++j) {
                if (rows[rows_number[i] * CA + j] == 3) {
                    int second_k = -1;
                    for (int k = 0; k < CA && !need_next && !is_finish; ++k) {
                        if ((i != k && rows[rows_number[j] * CA + k] == 2 && rows[rows_number[k] * CA + k] == 0) ||
                            (i == k && rows[rows_number[j] * CA + k] == 2 && rows[rows_number[j] * CA + j] == 0)) {
                            need_next = true;
                            is_finish = next_strings(rows_number, number_of_rows, std::max({i, j, k}) + 1, start_index);
                            ++count_trash[0];
                        }
                        if (i != k && rows[rows_number[j] * CA + k] == 1) {
                            if (second_k != -1) {
                                if (not(rows[rows_number[k] * CA + k] != 0 ||
                                        rows[rows_number[second_k] * CA + second_k] != 0 ||
                                        rows[rows_number[second_k] * CA + k] != 0)) {
                                    need_next = true;
                                    is_finish = next_strings(rows_number, number_of_rows, std::max({i, j, k}) + 1,
                                                             start_index);
                                    ++count_trash[1];
                                }
                            } else {
                                second_k = k;
                            }
                        }
                    }
                }
            }
        }

        // Проверка, если строка 1 2
        for (int i = 0; i < CA && !need_next && !is_finish; ++i) {
            int idx_1 = -1, idx_2 = -1;
            for (int j = 0; j < CA; ++j) {
                if (rows[rows_number[i] * CA + j] == 2) {
                    idx_2 = j;
                } else if (rows[rows_number[i] * CA + j] == 1) {
                    idx_1 = j;
                }
            }
            if (idx_1 != -1 && idx_2 != -1) {
                bool is_g = false;
                int max_k = -1;
                for (int j = 0; j < CA && !is_g; ++j) {
                    for (int k = 0; k < CA && !is_g; ++k) {
                        if (rows[rows_number[idx_1] * CA + j] != 0 &&
                            rows[rows_number[idx_2] * CA + k] != 0) {
                            if (rows[rows_number[j] * CA + k] != 0) {
                                is_g = true;
                            } else {
                                max_k = k;
                            }
                        }
                    }
                }
                if (not is_g) {
                    need_next = true;
                    is_finish = next_strings(rows_number, number_of_rows, std::max({i, idx_1, idx_2, max_k}) + 1,
                                             start_index);
                    ++count_trash[2];
                }
            }
        }

        // Проверка, если строка 1 1 1
//        for (int i = 0; i < CA && !need_next && !is_finish; ++i) {
//            int idx_1 = -1, idx_2 = -1, idx_3 = -1;
//            for (int j = 0; j < CA; ++j) {
//                if (rows[rows_number[i] * CA + j] == 1) {
//                    if (idx_1 == -1) {
//                        idx_1 = j;
//                    } else if (idx_2 == -1) {
//                        idx_2 = j;
//                    } else {
//                        idx_3 = j;
//                    }
//                }
//            }
//            if (idx_1 != -1 && idx_2 != -1 && idx_3 != -1) {
//                int is_g_1 = 0;
//                int max_k = -1;
//                for (int j = 0; j < CA; ++j) {
//                    for (int k = 0; k < CA; ++k) {
//                        if ((rows[rows_number[idx_1] * CA + i] > 1 ||
//                             rows[rows_number[idx_2] * CA + i] > 1 ||
//                             rows[rows_number[idx_3] * CA + i] > 1) ||
//                                (i != j && i != k &&
//                           ((rows[rows_number[idx_1] * CA + j] != 0 &&
//                            rows[rows_number[idx_2] * CA + k] != 0) ||
//                            (rows[rows_number[idx_1] * CA + j] != 0 &&
//                            rows[rows_number[idx_3] * CA + k] != 0) ||
//                            (rows[rows_number[idx_2] * CA + j] != 0 &&
//                            rows[rows_number[idx_3] * CA + k] != 0)))) {
//                            if (rows[rows_number[j] * CA + k] != 0) {
//                                ++is_g_1;
//                            } else {
//                                max_k = k;
//                            }
//                        }
//                    }
//                }
//                if (is_g_1 < 1) {
//                    need_next = true;
//                    is_finish = next_strings(rows_number, number_of_rows, std::max({i, idx_1, idx_2, idx_3, max_k}) + 1,
//                                             start_index);
//                    ++count_trash[3];
//                }
//            }
//        }


        if (!need_next && !is_finish) {
            ++count_to_eqv;
            for (int i = 0; i < CA; ++i) {
                copy(&rows[rows_number[i] * CA], &rows[rows_number[i] * CA + CA], matrix.begin() + i * CA);
            }
            calc_number_count(matrix, number_count);
            if (!local_consist(matrix, lock_lm, lock_nlm, number_count)) {
                lock_nlm.push_back(number_count);
                lock_lm.push_back(matrix);
            }
            is_finish = next_strings(rows_number, number_of_rows, CA, start_index);
        }
        ++tmp_mt_num;
    } while (!is_finish);

    std::cout << tmp_mt_num << std::endl;

    if (!lock_lm.empty()) {
        pthread_mutex_lock(&mutex2[0]);

        std::cout << count_trash[0] << std::endl;
        std::cout << count_trash[1] << std::endl;
        std::cout << count_trash[2] << std::endl;
        std::cout << count_trash[3] << std::endl;
        std::cout << std::endl;

        list_lock_nlm.push_back(lock_nlm);
        list_lock_lm.push_back(lock_lm);
        glob_tmp_mt_num += tmp_mt_num;
        glob_count_to_eqv += count_to_eqv;
        pthread_mutex_unlock(&mutex2[0]);
    }

    return nullptr;
}


bool take_matrix(std::vector<char> &matrix, std::vector<char> &number_count, int &lock_list_cnt,
                 long long &lock_matrix_cnt) {
    pthread_mutex_lock(&mutex2[1]);
    if (!finish) {
        if (matrix_cnt == list_lock_lm[list_cnt].size()) {
            if (++list_cnt >= list_lock_lm.size()) {
                finish = true;
                pthread_mutex_unlock(&mutex2[1]);
                return false;
            }
            matrix_cnt = 0;
        }
        matrix = list_lock_lm[list_cnt][matrix_cnt];
        number_count = list_lock_nlm[list_cnt][matrix_cnt];
        lock_list_cnt = list_cnt;
        lock_matrix_cnt = matrix_cnt++;

        pthread_mutex_unlock(&mutex2[1]);
        return true;
    }

    pthread_mutex_unlock(&mutex2[1]);
    return false;
}


bool global_consist(const std::vector<char> &matrix, const std::vector<char> &number_count, const int &lock_list_cnt) {
    std::vector<std::vector<char>> list_per_matrix;
    create_list_per_matrix(matrix, list_per_matrix);

    for (int i = 0; i < lock_list_cnt; ++i) {
        int idx = 0;
        for (const auto &m_tmp: list_lock_lm[i]) {
            if (list_bool_lm[i][idx]) {
                if (number_count != list_lock_nlm[i][idx]) {
                    ++idx;
                    continue;
                }
                for (const auto &per_matrix: list_per_matrix) {
                    if (per_matrix == m_tmp) {
                        return true;
                    }
                }
            }
            ++idx;
        }
    }
    return false;
}


void *compare_lock_lm(void *) {
    std::vector<char> matrix;
    std::vector<char> number_count;
    int lock_list_cnt = 0;
    long long lock_matrix_cnt = 0;

    if (list_lock_lm.size() > 1) {
        while (take_matrix(matrix, number_count, lock_list_cnt, lock_matrix_cnt)) {
            if (!global_consist(matrix, number_count, lock_list_cnt)) {
                pthread_mutex_lock(&mutex2[0]);
                list_matrix.push_back(matrix);
                pthread_mutex_unlock(&mutex2[0]);
            } else {
                list_bool_lm[lock_list_cnt][lock_matrix_cnt] = false;
            }
        }
    }

    return nullptr;
}