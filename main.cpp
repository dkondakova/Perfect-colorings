#include <iostream>
#include <vector>
#include <pthread.h>
#include "rows.cpp"
#include "coloring.cpp"
#include <iomanip>
#include <ctime>
#include <sstream>
#include <chrono>

void mutex_init(pthread_mutex_t m[2]) {
    pthread_mutexattr_t mutex_attr;
    pthread_mutexattr_init(&mutex_attr);
    pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&m[0], &mutex_attr);
    pthread_mutex_init(&m[1], &mutex_attr);
    pthread_mutexattr_destroy(&mutex_attr);
}

void print_info(const std::string &message, const auto &object) {
    std::cout << message << " " << object << std::endl;
    fout << "% " << message << " " << object << std::endl;
}

void print_work_time(const clock_t &clock_start, const auto &chrono_start, const std::string &message) {
    print_info("Время всех процессов:", (double) (clock() - clock_start) / CLOCKS_PER_SEC);

    auto chrono_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> duration = chrono_end - chrono_start;
    print_info("Время программы:", duration.count());

    std::time_t t = std::time(nullptr);
    std::tm f_tm = *std::localtime(&t);
    print_info(message, std::put_time(&f_tm, "%T"));

    std::cout << "==============================" << std::endl;
}

void generate_filestream() {
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    std::stringstream ss;
    ss << std::put_time(&tm, "%d-%m-%y_%H-%M-%S") << std::endl;
    fout = std::ofstream(std::to_string(char(CA)) + "_" + ss.str() + ".txt");
}

void do_work_on_threads(auto func, pthread_t (&threads)[TA_M]) {
    for (auto &thread: threads) {
        pthread_create(&thread, nullptr, func, nullptr);
    }
    for (auto thread: threads) {
        pthread_join(thread, nullptr);
    }
}

int main() {
    generate_filestream();
    clock_t clock_start = clock();
    auto chrono_start = std::chrono::high_resolution_clock::now();
    print_work_time(clock_start, chrono_start, "Начало программы:");

    rows = generate_rows();

//    // 7 цветов
//    if (CA == 7) {
//        list_matrix = {{0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 2, 1, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1},
//                       {0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 2, 0, 0, 0, 1, 0, 2, 0, 0, 1, 1, 0, 1, 0, 0, 2, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0}};
//    }
//
//    // 8 цветов
//    if (CA == 8) {
//        list_matrix = {{0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0},
//                       {0, 0, 0, 0, 0, 0, 1, 2, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 2, 0, 1, 0, 0, 0, 0, 2, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 2, 1, 0, 0, 0, 0, 0, 0},
//                       {0, 0, 0, 0, 0, 1, 0, 2, 0, 0, 0, 0, 1, 0, 2, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 2, 0, 0, 0, 0, 1, 0, 2, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1},
//                       {0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 2, 0, 0, 0, 0, 1, 0, 2, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1},
//                       {0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1}};
//    }
//
//    // 10 цветов
//    if (CA == 10) {
//    list_matrix = {{0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0},
//                   {0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1},
//                   {0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 0, 2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1},
//                   {0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1}};
//    }

//%===================================%
// Однопоточный поиск возможных матриц
//%===================================%

//    list_matrix = make_list_matrix(rows); //глобальная переменная, объявлена в coloring.cpp
//    new_make_list_matrix(rows);


//%====================================%
// Многопоточный поиск возможных матриц
//%===================================%

    pthread_t threads[TA_M];
    mutex_init(mutex2);
    do_work_on_threads(make_lock_lm, threads);
    print_work_time(clock_start, chrono_start, "Перебор матриц потоками окончен:");

    for (auto &i : list_lock_lm) {
        std::vector<bool> bool_lm(i.size(), true);
        list_bool_lm.push_back(bool_lm);
    }
    for (const auto &i: list_lock_lm[0]) {
        list_matrix.push_back(i);
    }

    do_work_on_threads(compare_lock_lm, threads);
    print_info("Матриц перебрали все потоки:", glob_tmp_mt_num);
    print_info("Дошли до проверки на эквивалентность:", glob_count_to_eqv);
    print_info("Всего возможных матриц:", list_matrix.size());
    print_work_time(clock_start, chrono_start, "Получены возможные матрицы:");


//%=======================%
// Многопоточная раскраска
//%=======================%

    mutex_init(mutex);
    do_work_on_threads(new_try_to_color, threads);
    print_info("Всего допустимых матриц:", m_a);
    print_work_time(clock_start, chrono_start, "Получены раскраски:");


    return 0;
}