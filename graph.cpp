#include <iostream>
#include <map>
#include "constants.h"
// #include "matrixes.cpp"
#include "new_matrixes.cpp"


class Graph {
private:
    int size;
    int vertexes[300] = {-1};

public:
    explicit Graph(const int &size = 20) : size(size) {
        for (int i = 0; i < size; i++) {
            vertexes[i] = -1;
        }
    }

    Graph(const Graph &copy) : size(copy.size) {
        for (int i = 0; i < size; i++) {
            vertexes[i] = copy.vertexes[i];
        }
    }

    Graph &operator=(const Graph &graph) {
        if (this == &graph) {
            return *this;
        }

        size = graph.size;
        for (int i = 0; i < size; i++) {
            vertexes[i] = graph.vertexes[i];
        }

        return *this;
    }

    friend bool operator==(const Graph &leftGraph, const Graph &rightGraph);

    friend bool operator!=(const Graph &lh, const Graph &rh);

    [[nodiscard]] int get_size() const {
        return size;
    }

    void color_vertex(const int &number, const int &color) {
        vertexes[number] = color;
    }

    void get_neighbors(const int &number, int (&neighbors)[NA]) const {
        if (number % 2 == 0) {
            neighbors[0] = (number + size - 2) % size;
            neighbors[1] = (number + 1) % size;
            neighbors[2] = (number + 2) % size;
        } else {
            neighbors[0] = (number + size - 4) % size;
            neighbors[1] = (number + size - 1) % size;
            neighbors[2] = (number + 4) % size;
        }
    }

    [[nodiscard]] int get_color(const int &number) const {
        return vertexes[number];
    }

    void get_n_colors(const int &number, int (&n_colors)[CA]) const {
        int neighbors[NA];
        get_neighbors(number, neighbors);
        for (int neighbor: neighbors) {
            if (vertexes[neighbor] != -1) {
                n_colors[vertexes[neighbor]]++;
            }
        }
    }

    int colors_amount(int full_period) {
        int n_colors[CA] = {0};
        for (int i = 0; i < full_period; ++i) {
            n_colors[vertexes[i]] += 1;
        }
        int ca = 0;
        for (int n_color : n_colors) {
            if (n_color > 0) {
                ca++;
            }
        }
        return ca;
    }

    [[nodiscard]] bool all_colors_used(int full_period) const {
        std::bitset<CA> used_colors;
        for (int i = 0; i < full_period; ++i) {
            used_colors.set(vertexes[i]);
        }
        return used_colors.all();
    }

    void print() {
        for (int i = 1; i <= 3; i++) {
            if (i == 3) {
                std::cout << "\t\t";
            }
            for (int j = 0; j < size; j++) {
                if ((j % 4 == i) | (j % 2 == 0 & i == 2)) {
                    std::cout << j << "(" << (vertexes[j] + 1) << ")";
                    if (j % 2 == 0) {
                        std::cout << "\t";
                    } else {
                        std::cout << "\t\t\t";
                    }
                }
            }
            std::cout << std::endl << std::endl;
        }
    }

    void print(int n) {
        for (int i = 1; i <= 3; i++) {
            if (i == 3) {
                for (int j = 0; j < n; j++) {
                    if ((j % 4 == i) | (j % 2 == 0 & i == 2)) {
                        std::cout << "   |  ";
                    }
                }
                std::cout << std::endl;
            }
            if (i == 3) {
                std::cout << "---";
            }
            for (int j = 0; j < n; j++) {
                if ((j % 4 == i) | (j % 2 == 0 & i == 2)) {
                    std::cout << (vertexes[j] + 1);
                    if (j % 2 == 0) {
                        std::cout << "--";
                    } else {
                        std::cout << "-----";
                    }
                }
            }
            std::cout << std::endl;
            if (i == 1) {
                for (int j = 0; j < n; j++) {
                    if ((j % 4 == i) | (j % 2 == 0 & i == 2)) {
                        std::cout << "|     ";
                    }
                }
                std::cout << std::endl;
            }
        }
    }

    void print_latex(const int T, const int cnt) const {
        int n = T*2;
        std::string dist = "dist";
//        fout << "\t\\bigskip\n";
        fout << "\t\\begin{tikzpicture}[node distance=\\" << dist << " cm,thin,inner sep=\\rad]\n"
                "\t\\def\\n{" << n << "}\n"
                                       "\t\t\\pgfmathtruncatemacro{\\nmm}{\\n-2}\n"
                                       "\t\t\\pgfmathtruncatemacro{\\np}{\\n+1}\n"
                                       "\t\t\\pgfmathtruncatemacro{\\npp}{\\n+2}\n"
                                       "\t\t\\pgfmathtruncatemacro{\\nppp}{\\n+3}\n\n"
                                       "\t\t\\node (0) {};\n"
                                       "\t\t\\foreach \\v in {2,4,...,\\npp}\n"
                                       "\t\t{\n"
                                       "\t\t\t\\pgfmathtruncatemacro{\\u}{\\v-2}\n"
                                       "\t\t\t\\node (\\v) [right of=\\u] {};\n"
                                       "\t\t\t\\path (\\u) edge (\\v);%путь между вершинами на нижней линии\n"
                                       "\t\t}\n"
                                       "\t\t\\foreach \\v in {0,2,...,\\npp}\n"
                                       "\t\t{\n"
                                       "\t\t\t\\pgfmathtruncatemacro{\\w}{\\v+1}\n"
                                       "\t\t\t\\node (\\w) [above of=\\v] {};\n"
                                       "\t\t\t\\path (\\v) edge (\\w);%путь между рядами\n"
                                       "\t\t}\n"
                                       "\t\t\\foreach \\v in {5,7,...,\\nppp}\n"
                                       "\t\t{\n"
                                       "\t\t\t\\pgfmathtruncatemacro{\\u}{\\v-4}\n"
                                       "\t\t\t\\path (\\u) edge [bend left] (\\v);%путь между вершинами на верхней линии\n"
                                       "\t\t}\n\n"
                                       "\t\t\\node (a0) [left of=0] {};\n"
                                       "\t\t\\node (a1) [left of=1] {};\n"
                                       "\t\t\\node (a2) at (-\\" << dist << ",\\" << dist << "*4/3) {};\n"
                                       "\t\t\\node (b0) [right of=\\npp] {};\n"
                                       "\t\t\\node (b1) [right of=\\nppp] {};\n"
                                       "\t\t\\node (b2) at (\\" << dist << "*" << n/2 + 2 << ",\\" << dist << "*4/3) {};\n"
                                       "\t\t\\path (a0) edge (0);\n"
                                       "\t\t\\path (a1) edge [bend left] (3);\n"
                                       "\t\t\\path (a2) edge [bend left=10] (1);\n"
                                       "\t\t\\path (\\npp) edge (b0);\n"
                                       "\t\t\\path (\\np) edge [bend left] (b1);\n"
                                       "\t\t\\path (\\nppp) edge [bend left=10] (b2);\n\n";

        for (int i = 0; i < CA; ++i) {
            fout << "\t\t\\foreach \\v in {";
            bool is_first = true;
            for (int j = 0; j < size; ++j) {
                if (vertexes[j] == i) {
                    for (int k = 0; k <= (n + 3 - j) / 2 / T; ++k) {
                        if (!is_first) {
                            fout << ", ";
                        }
                        is_first = false;
                        fout << j + 2 * T * k ;
                    }
                }
            }
            fout << "} { \\node at (\\v) [f" << i << "] {}; }\n";
        }

        if (T > 1) {
            for (int i = 0; i <= (n / 2 + 1) / T; ++i) {
                fout << "\t\t\\path (\\" << dist << "*" << 0.5 + T * i << ",\\" << dist << "*4/3) " <<
                     "edge [dashed] (\\" << dist << "*" << 0.5 + T * i << ",-\\" << dist << "/3);\n";
            }
        }

        fout << "\t\t\\node () [right of=\\npp,xshift=0.35cm] {$C_{" << cnt << "}$};\n";

        fout << "\t\\end{tikzpicture}";
        if (cnt % 2 == 0) {
            fout << "\n\n";
        } else {
            fout << " \\hspace{1cm}\n";
        }
    }
};

bool operator==(const Graph &leftGraph, const Graph &rightGraph) {
    if (leftGraph.get_size() != rightGraph.get_size()) {
        return false;
    }

    int size = leftGraph.get_size();
    bool is_equal = false;

    for (int shift = 0; shift < size && not is_equal; shift += 2) {
        is_equal = true;
        for (int i = 0; i < size; ++i) {
            if (leftGraph.get_color(i) != rightGraph.get_color((i + shift) % size)) {
                is_equal = false;
                break;
            }
        }
    }

    for (int shift = 0; shift < size && not is_equal; shift += 2) {
        is_equal = true;
        for (int i = 0; i < size; i += 2) {
            if (leftGraph.get_color(i) != rightGraph.get_color((2 * size - (i + shift)) % size) ||
                leftGraph.get_color(i + 1) != rightGraph.get_color((2 * size - (i + shift) + 1) % size)) {
                is_equal = false;
                break;
            }
        }

    }

//    int colors[CA];
//    // псевдо-симметричен ли граф относительно периода
//    for (int shift = 0; shift < size && not is_equal; shift += 2) {
//        std::fill(colors, colors + CA, -1);
//        is_equal = true;
//        for (int i = 0; i < size; i += 2) {
//            int color_leftGraph_even = leftGraph.get_color(i),
//                color_leftGraph_odd = leftGraph.get_color(i + 1);
//            int color_rightGraph_even = leftGraph.get_color((i + shift) % size),
//                color_rightGraph_odd = leftGraph.get_color((i + shift + 1) % size);
//            if (colors[color_leftGraph_even] == -1) {
//                colors[color_leftGraph_even] = color_rightGraph_even;
//            }
////            if (colors[color_rightGraph_even] == -1) {
////                colors[color_rightGraph_even] = color_leftGraph_even;
////            }
//            if (colors[color_leftGraph_odd] == -1) {
//                colors[color_leftGraph_odd] = color_rightGraph_odd;
//            }
////            if (colors[color_rightGraph_odd] == -1) {
////                colors[color_rightGraph_odd] = color_leftGraph_odd;
////            }
//
//            if (colors[color_leftGraph_even] != color_rightGraph_even ||
//                colors[color_leftGraph_odd] != color_rightGraph_odd) {
//                is_equal = false;
//                break;
//            }
//        }
//    }


//    int colors[CA];
//    for (int shift = 0; shift < size && not is_equal; shift += 2) {
//        std::fill(colors, colors + CA, -1);
//        is_equal = true;
//        for (int i = 0; i < size && is_equal; ++i) {
//            const int color_leftGraph = leftGraph.get_color(i);
//            const int color_rightGraph = leftGraph.get_color((i + shift) % size);
//
//            if (colors[color_leftGraph] == -1) {
//                colors[color_leftGraph] = color_rightGraph;
//            }
////            if (colors[color_rightGraph] == -1) {
////                colors[color_rightGraph] = color_leftGraph;
////            }
//
//            if (colors[color_leftGraph] != color_rightGraph) {
//                is_equal = false;
////                break;
//            }
//        }
//        if (is_equal) {
//            std::cout << "shift = " << shift << std::endl;
//            for (auto &c : colors) {
//                std::cout << c << " ";
//            }
//            std::cout << std::endl;
//        }
//    }

    return is_equal;
}

bool operator!=(const Graph &lh, const Graph &rh) {
    return !(lh == rh);
}
