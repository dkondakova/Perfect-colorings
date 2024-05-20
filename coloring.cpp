#include <iostream>
#include <vector>
#include <cmath>
#include "graph.cpp"

long long int m_a = 0;
pthread_mutex_t mutex[2];


bool new_check(const int &number, const Graph &G, const std::vector<char> &M) {
    int n_colors[CA] = {0};
    G.get_n_colors(number, n_colors);
    for (int k = 0; k < CA; k++) {
        if (M[G.get_color(number) * CA + k] - n_colors[k] < 0) {
            return false;
        }
    }
    return true;
}

int new_get_unused_color(const int &vertex_number, const Graph &G, const std::vector<char> &M) {
    int n_colors[CA] = {0};
    G.get_n_colors(vertex_number, n_colors);

    for (int k = 0; k < CA; k++) {
        if (M[G.get_color(vertex_number) * CA + k] - n_colors[k] > 0) {
            return k;
        }
    }
    std::cout << "Ошибка: не удалось найти неиспользуемый цвет соседа вершины " << vertex_number << std::endl;
    return -1;
}


bool new_step1(const int (&seq)[6], Graph &G, const std::vector<char> &M, const int (&v)[6], const int (&v2)[4]) {
    for (int i = 0; i < 6; ++i) {
        G.color_vertex(v[i], seq[i]);
    }
    G.color_vertex(3, new_get_unused_color(2, G, M));
    G.color_vertex(5, new_get_unused_color(4, G, M));
    for (const int i: v2) {
        if (!new_check(i, G, M)) {
            return false;
        }
    }
    return true;
}

void new_get_seq(int (&seq)[6], int c) {
    for (int &i: seq) {
        i = c % CA;
        c /= CA;
    }
}

bool new_seq_valid(const int (&seq)[6], const std::vector<char> &M, char (&ms)[CA]) {
    for (int i = 1; i < 5; ++i) {
        for (int j = 0; j < CA; ++j) {
            ms[j] = M[seq[i] * CA + j];
        }
        if (--ms[seq[i - 1]] < 0 || --ms[seq[i + 1]] < 0) {
            return false;
        }
    }
    return true;
}

int period_start(Graph &G, int T) {
    bool is_symmetrical = true;

    // симметричен ли граф относительно периода
    for (int m = 0; m < T * 2; m = m + 2) {
        is_symmetrical = true;
        for (int k = m, j = T * 2 - 2 + m; k < m + T && j > T + m && is_symmetrical; k = k + 2, j = j - 2) {
            if (G.get_color(k) != G.get_color(j % (T * 2)) ||
                G.get_color(k + 1) != G.get_color((j + 1) % (T * 2))) {
                is_symmetrical = false;
            }
        }
        if (is_symmetrical) {
            return m;
        }
    }

    // симметричен ли граф относительно половины периода
    if (!is_symmetrical && (T % 2 == 0)) {
        for (int m = 0; m < T; m = m + 2) {
            is_symmetrical = true;
            for (int k = m, j = T - 2 + m;
                 k < m + T / 2 && j > T / 2 + m && is_symmetrical; k = k + 2, j = j - 2) {
                if (G.get_color(k) != G.get_color(j % (T * 2)) ||
                    G.get_color(k + 1) != G.get_color((j + 1) % (T * 2))) {
                    is_symmetrical = false;
                }
            }
            if (is_symmetrical) {
                return m;
            }
        }
    }

    int colors[CA];
    // псевдо-симметричен ли граф относительно периода
    for (int m = 0; m < T * 2; m = m + 2) {
        for (int &color : colors) {
            color = -1;
        }
        is_symmetrical = true;
//        std::cout << "_____________" << std::endl;
        for (int k = m, j = T * 2 - 2 + m; k < m + T && j > T + m && is_symmetrical; k = k + 2, j = j - 2) {
            int color_k = G.get_color(k % (T * 2)),
                    color_k1 = G.get_color((k + 1) % (T * 2));
            int color_j = G.get_color(j % (T * 2)),
                    color_j1 = G.get_color((j + 1) % (T * 2));
            if (colors[color_k] == -1) {
                colors[color_k] = color_j;
            }
            if (colors[color_j] == -1) {
                colors[color_j] = color_k;
            }
            if (colors[color_k1] == -1) {
                colors[color_k1] = color_j1;
            }
            if (colors[color_j1] == -1) {
                colors[color_j1] = color_k1;
            }
//            std::cout << "m = " << m << ", k = " << k << ", color - " << color_k << ", " << colors[color_k] << " - "
//                      << color_j << std::endl;
//            std::cout << "k1 = " << k + 1 << ", color - " << color_k1 << ", " << colors[color_k1] << " - " << color_j1
//                      << std::endl;
            if (colors[color_k] != color_j || colors[color_j] != color_k ||
                colors[color_k1] != color_j1 || colors[color_j1] != color_k1) {
                is_symmetrical = false;
            }
        }
        if (is_symmetrical) {
            return m;
        }
    }

    return 0;
}

void *new_try_to_color(void *) {
    std::vector<Graph> answers;

    std::vector<char> M;
    const int count_all_comb = int(std::pow(CA, 6));
    const int board = 8;
    const int vertex[6] = {1, 0, 2, 4, 6, 7};
    const int vertex2[4] = {3, 5, 1, 7};

    int seq[6];
    char ms[CA];
    while (!list_matrix.empty()) {
        pthread_mutex_lock(&mutex[0]);
        if (!list_matrix.empty()) {
            M = list_matrix[0];
            list_matrix.erase(list_matrix.begin());
        } else {
            pthread_mutex_unlock(&mutex[0]);
            continue;
        }
        pthread_mutex_unlock(&mutex[0]);

        bool is_counted = false;

        for (int c = 0; c < count_all_comb; ++c) {
            new_get_seq(seq, c);
            if (!new_seq_valid(seq, M, ms)) {
                continue;
            }

            Graph graph_temp(12);
            if (!new_step1(seq, graph_temp, M, vertex, vertex2)) {
                continue;
            }

            Graph G(300);

            for (int i = 0; i < board; ++i) {
                G.color_vertex(i, graph_temp.get_color(i));
            }

            bool is_period = false;
            for (int i = board; i < 68 && not is_period; i += 2) {
                is_period = true;

                G.color_vertex(i, new_get_unused_color(i - 2, G, M));
                G.color_vertex(i + 1, new_get_unused_color(i - 3, G, M));

                if (!new_check(i, G, M) || !new_check(i + 1, G, M)) {
                    break;
                }

                int start_period = i - 6;

                for (int j = 0, k = start_period; j < board && k < start_period + board; ++j, ++k) {
                    if (G.get_color(k) != graph_temp.get_color(j)) {
                        is_period = false;
                        break;
                    }
                }

                if (is_period) {
                    int coef = (i % 2 == 0) ? 1 : 2;
                    int T = coef * (start_period) / 2;

//                    if (G.colors_amount(T * 2) != CA) {
//                        break;
//                    }

                    if (!G.all_colors_used(T * 2)) {
                        break;
                    }

                    int start_ind_to_color = period_start(G, T);

                    Graph graph(coef * (start_period));
                    for (int j = 0; j < graph.get_size(); j++) {
                        graph.color_vertex(j, G.get_color((j + start_ind_to_color - 2 + T * 2) % (T * 2)));
                    }

                    bool is_equal = false;
                    for (const Graph &other_graph: answers) {
                        if (graph == other_graph) {
                            is_equal = true;
                            break;
                        }
                    }

                    if (!is_equal) {
                        if (!is_counted) {
                            pthread_mutex_lock(&mutex[1]);
                            ++m_a;
                            pthread_mutex_unlock(&mutex[1]);
                            is_counted = true;
                        }
                        answers.push_back(graph);
                    }
                }
                if (i >= 58) {
                    std::cout << "!!!дошли до конца!!!" << std::endl;
                }
            }
        }

        if (!answers.empty()) {
            pthread_mutex_lock(&mutex[1]);
            print_matrix_latex(M);
            fout << "\n\n\t{\\centering\n";
            int cnt = 1;
            for (const Graph &G : answers) {
                G.print_latex(G.get_size() / 2, cnt);
                ++cnt;
            }
            fout << "\\par}\n~\\\\\n\n";
            pthread_mutex_unlock(&mutex[1]);
        }
        answers.clear();

    }
    return nullptr;
}