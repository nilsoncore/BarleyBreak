#include <iostream>
#include <string>
#include <vector>
#include <ctime>

#define uint unsigned int // unsigned int - 4 bytes
#define u2 unsigned short int // unsigned short int - 2 bytes

struct Vertex {
    Vertex *previous_vertex = nullptr; // Предыдущая вершина
    u2 G = 0;  // G - Расстояние от начальной до текущей позиции.
    u2 H = 0;  // H - Эвристическая оценка:
           // "Суммарный путь, который нужно пройти каждой костяшке до терминальной позиции".
    char last_action = '\0'; // Предыдущее действие, которое привело к этой вершине.
    u2 slots[3][3] = { 0, 0, 0,
                       0, 0, 0,
                       0, 0, 0 };
};

bool check_solvability_manual_method(u2 (&slots)[3][3]);
bool check_solvability_array_method(u2 (&slots)[3][3]);
std::string solve_a_star(u2 (&slots)[3][3]);
u2 get_H(u2 (&slots)[3][3], const u2 (&terminal_positions)[3][3]);
u2 get_H(u2 (&slots)[3][3]);
std::vector<Vertex> parse_child_vertices(Vertex *vertex);
u2 parse_empty_slot(u2 (&slots)[3][3]);
inline bool is_similar_vertices(Vertex *vertex1, Vertex *vertex2);
Vertex parse_vertex_with_min_F(std::vector<Vertex> *list);
inline u2 get_F(Vertex *vertex);
inline void copy_vertex_slots(Vertex *vertex_copy_from, Vertex *vertex_copy_to);
inline void copy_slots(u2 (&slots_copy_from)[3][3], u2 (&slots_copy_to)[3][3]);
inline void move_empty_slot(Vertex *vertex, u2 empty_slot_row, u2 empty_slot_column, int move_row, int move_column);
Vertex new_child_vertex(Vertex *parent_vertex, u2 empty_slot_row, u2 empty_slot_column, int move_row, int move_column);
void print_slots(u2 (&slots)[3][3], const char *title);
bool is_similar_slots(u2 (&first_slots)[3][3], const u2 (&second_slots)[3][3]);

int main() {
    setlocale(LC_ALL, "rus");
    u2 slots[3][3];

    printf("Enter starting position: ");
    scanf_s("%hu %hu %hu %hu %hu %hu %hu %hu %hu", &slots[0][0], &slots[0][1], &slots[0][2],
        &slots[1][0], &slots[1][1], &slots[1][2],
        &slots[2][0], &slots[2][1], &slots[2][2]);

    print_slots(slots, "Starting position");

    bool is_solvable = false;

    printf("\n\"Manual method\":");
    is_solvable = check_solvability_manual_method(slots);
    if (is_solvable)  printf("\nSolvable: yes\n");
    else              printf("\nSolvable: no\n");

    printf("\n\"Array method\":");
    is_solvable = check_solvability_array_method(slots);
    if (is_solvable)  printf("\nSolvable: yes\n");
    else              printf("\nSolvable: no\n");

    if (is_solvable) {
        std::string result = solve_a_star(slots);
        std::cout << std::endl << "Steps to solve: " << result << "\n";
    }
    return 0;
}

bool check_solvability_manual_method(u2 (&slots)[3][3]) {
    u2 result = 0;
    u2 empty_slot_row = 0;
    u2 count_slot = 0;
    u2 compare_slot = 0;

    for (int i = 0; i != 3; i++) {
        for (int j = 0; j != 3; j++) {
            count_slot = slots[i][j];
            printf("\n(%d) [%d][%d]: ", count_slot, i, j);

            for (int k = i; k != 3; k++) {
                for (int l = 0; l != 3; l++) {
                    compare_slot = slots[k][l];
                    if (k == i)
                        if (l <= j) continue;
                    if (compare_slot == 0) empty_slot_row = k + 1;
                    else if (count_slot > compare_slot) printf("\n ... >  (%d) [%d][%d], result: %d", compare_slot, k, l, ++result);
                    else                                printf("\n ... <= (%d) [%d][%d]", compare_slot, k, l);
                }
            }

        }
    }

    printf("\n\nEmpty slot row: %d", empty_slot_row);
    result += empty_slot_row;
    printf("\nResult: %d", result);
    return (result % 2 == 0);
}

bool check_solvability_array_method(u2 (&slots)[3][3]) {
    u2 result = 0;
    u2 empty_slot_row = 0;
    u2 count_slot = 0;
    u2 compare_slot = 0;
    u2 compare_exit_slot = 0;

    u2 compared_slots[9] { 9, 9, 9, 9, 9, 9, 9, 9, 9 };

    for (int i = 0; i != 3; i++) {
        for (int j = 0; j != 3; j++) {
            count_slot = slots[i][j];
            if (count_slot == 0) {
                empty_slot_row = i + 1;
                continue;
            }
            result += count_slot - 1;
            printf("\n(%d) [%d][%d]: > %d, result: %d", count_slot, i, j, count_slot - 1, result);
            for (int k = 0; k != 9; k++) {
                compare_slot = compared_slots[k];
                if (compare_slot == 9) {
                    compare_exit_slot = k;
                    break;
                }
                if (count_slot > compare_slot)
                    printf("\n ... - (%d), result: %d", compare_slot, --result);
                compare_exit_slot = k;
            }
            compared_slots[compare_exit_slot] = count_slot;
        }
    }

    printf("\n\nEmpty slot row: %d", empty_slot_row);
    result += empty_slot_row;
    printf("\nResult: %d", result);
    return (result % 2 == 0);
}

std::string solve_a_star(u2 (&starting_positions)[3][3]) {
    // Открытый список вершин графа.
    // Здесь находятся вершины, еще не проверенные алгоритмом.
    std::vector<Vertex> open_list;
    // Закрытый список вершин графа.
    // Здесь находятся вершины, которые уже встречались в ходе поиска решения.
    std::vector<Vertex> closed_list;
    // Список действий для достижения терминальной позиции.
    std::string result;

    // На каждом новом шаге из списка открытых вершин выбирается вершина с наименьшим весом.

    // Терминальная позиция (позиция, при которой вершина считается собраной).
    const u2 terminal_positions[3][3] = { 1, 2, 3,
                                          4, 5, 6, 
                                          7, 8, 0 };

    Vertex start_vertex;
    copy_slots(starting_positions, start_vertex.slots);
    start_vertex.G = 0;
    start_vertex.H = get_H(starting_positions, terminal_positions);
    open_list.push_back(start_vertex);

    uint iterations = 0;
    while (!open_list.empty()) {
        iterations++;

        Vertex vertex = parse_vertex_with_min_F(&open_list);
        result.push_back(vertex.last_action);

        if (is_similar_slots(vertex.slots, terminal_positions)) {
            printf("\nterminal vertex found: \n G: %u\n H: %u\n F: %u\n", vertex.G, vertex.H, get_F(&vertex));
            print_slots(vertex.slots, "terminal position");
            printf("\ntotal iterations: %u", iterations);
            return result;
        }

        for (int i = 0; i != open_list.size() - 1; i++) {
            if (is_similar_vertices(&open_list[i], &vertex)) {
                open_list.erase(open_list.begin() + i);
                break;
            }
        }

        closed_list.push_back(vertex);
        std::vector<Vertex> child_vertices = parse_child_vertices(&vertex);

        for (Vertex child_vertex : child_vertices) {

            for (Vertex closed_vertex : closed_list)
                if (is_similar_vertices(&closed_vertex, &child_vertex))
                    continue;

            u2 F = vertex.G + vertex.H;
            bool contains = false;
            bool is_better = false;

            for (Vertex open_vertex : open_list) { // if open_list contains child_vertex
                if (!is_similar_vertices(&open_vertex, &child_vertex)) {
                    child_vertex.H = get_H(child_vertex.slots);
                    open_list.push_back(child_vertex);
                    is_better = true;
                    contains = true;
                    break;
                }
            }

            if (!contains)
                is_better = F < get_F(&child_vertex); // else

            if (is_better)
                child_vertex.previous_vertex = &vertex;

        }
    }
}

inline bool is_similar_vertices(Vertex *vertex1, Vertex *vertex2) {
    if (vertex1->G == vertex2->G && vertex1->H == vertex2->H && vertex1->last_action == vertex2->last_action)
        return true;
    else return false;
}

bool is_similar_slots(u2 (&slots)[3][3], const u2 (&terminal_slots)[3][3]) {
    u2 similar_count = 0;
    for (int i = 0; i != 3; i++) {
        for (int j = 0; j != 3; j++) {
            if (slots[i][j] == terminal_slots[i][j]) similar_count++;
        }
    }
    if (similar_count == 9) return true;
    else return false;
}

Vertex parse_vertex_with_min_F(std::vector<Vertex> *list) {
    Vertex result;
    u2 min = UINT16_MAX; // 2^16 - 65,536
    for (Vertex vertex : *list) {
        u2 F = get_F(&vertex);
        if (F < min) {
            min = F;
            result = vertex;
        }
    }
    return result;
}

std::vector<Vertex> parse_child_vertices(Vertex *vertex) {
    std::vector<Vertex> result;
    u2 empty_slot = parse_empty_slot(vertex->slots);
    u2 empty_slot_row = empty_slot / 10;
    u2 empty_slot_column = empty_slot % 10;

    if (empty_slot_column < 2) { // Left-to-right
        Vertex new_vertex = new_child_vertex(vertex, empty_slot_row, empty_slot_column, 0, 1);
        result.push_back(new_vertex);
    }
    if (empty_slot_column > 0) { // Right-to-left
        Vertex new_vertex = new_child_vertex(vertex, empty_slot_row, empty_slot_column, 0, -1);
        result.push_back(new_vertex);
    }
    if (empty_slot_row < 2) { // Top-to-down
        Vertex new_vertex = new_child_vertex(vertex, empty_slot_row, empty_slot_column, 1, 0);
        result.push_back(new_vertex);
    }
    if (empty_slot_row > 0) { // Down-to-top
        Vertex new_vertex = new_child_vertex(vertex, empty_slot_row, empty_slot_column, -1, 0);
        result.push_back(new_vertex);
    }

    return result;
}

Vertex new_child_vertex(Vertex *parent_vertex, u2 empty_slot_row, u2 empty_slot_column, int move_row, int move_column) {
    Vertex new_vertex;
    new_vertex.previous_vertex = parent_vertex;
    new_vertex.G = parent_vertex->G + 1;
    copy_vertex_slots(parent_vertex, &new_vertex);
    move_empty_slot(&new_vertex, empty_slot_row, empty_slot_column, move_row, move_column);

    char action = '\0';
    if (move_column > 0) action = 'R';
    else if (move_column < 0) action = 'L';
    else if (move_row > 0) action = 'D';
    else if (move_row < 0) action = 'U';
    new_vertex.last_action = action;

    new_vertex.H = get_H(new_vertex.slots);

    return new_vertex;
}

inline void copy_vertex_slots(Vertex *vertex_copy_from, Vertex *vertex_copy_to) {
    for (int i = 0; i != 3; i++)
        for (int j = 0; j != 3; j++)
            vertex_copy_to->slots[i][j] = vertex_copy_from->slots[i][j];
}

inline void copy_slots(u2 (&slots_copy_from)[3][3], u2 (&slots_copy_to)[3][3]) {
    for (int i = 0; i != 3; i++)
        for (int j = 0; j != 3; j++)
            slots_copy_to[i][j] = slots_copy_from[i][j];
}

inline void move_empty_slot(Vertex *vertex, u2 empty_slot_row, u2 empty_slot_column, int move_row, int move_column) {
    u2 temp = 0;
    temp = vertex->slots[empty_slot_row + move_row][empty_slot_column + move_column];
    vertex->slots[empty_slot_row + move_row][empty_slot_column + move_column] = 0;
    vertex->slots[empty_slot_row][empty_slot_column] = temp;
}

u2 parse_empty_slot(u2 (&slots)[3][3]) {
    u2 result = 0;
    for (int i = 0; i != 3; i++) {
        for (int j = 0; j != 3; j++) {
            if (slots[i][j] == 0) {
                result = i * 10 + j;
                return result;
            }
        }
    }
    return 0;
}

u2 get_H(u2 (&slots)[3][3], const u2 (&terminal_positions)[3][3]) {
    bool found = false;
    u2 H = 0;
    int k = 0;

    for (int i = 0; i != 3; i++) {
        for (int j = 0; j != 3; j++) {
            found = false;
            k = 0;

            while (!found) {
                for (int l = 0; l != 3; l++) {
                    if (slots[i][j] == terminal_positions[k][l]) {
                        found = true;
                        H += abs(i - k) + abs(j - l);
                        break;
                    }
                }
                k++;
            }

        }
    }

    return H;
}

u2 get_H(u2 (&slots)[3][3]) {
    const u2 terminal_positions[3][3] = { 1, 2, 3,
                                          4, 5, 6,
                                          7, 8, 0 };
    bool found = false;
    u2 H = 0;
    int k = 0;

    for (int i = 0; i != 3; i++) {
        for (int j = 0; j != 3; j++) {
            found = false;
            k = 0;

            while (!found) {
                for (int l = 0; l != 3; l++) {
                    if (k > 3) __debugbreak();
                    if (slots[i][j] == terminal_positions[k][l]) {
                        found = true;
                        H += abs(i - k) + abs(j - l);
                        break;
                    }
                }
                k++;
            }

        }
    }

    return H;
}

void print_slots(u2 (&slots)[3][3], const char *title) {
    printf("\n%s:\n\n", title);
    printf("     %u %u %u\n", slots[0][0], slots[0][1], slots[0][2]);
    printf("     %u %u %u\n", slots[1][0], slots[1][1], slots[1][2]);
    printf("     %u %u %u\n", slots[2][0], slots[2][1], slots[2][2]);
}

inline u2 get_F(Vertex *vertex) {
    return vertex->G + vertex->H;
}