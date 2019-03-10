#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <utility>
#include <vector>

template <class I, typename V, class Pred>
// I is a forward iterator
// V is a vector of integral type
V find_if_indices(I first, I last, V out_iter, Pred pred) {
  auto ini_first = first;
  if (first == last)
    return out_iter;
  do {
    auto found_elem = std::find_if(first, last, pred);
    std::swap(found_elem, first);
    if (first != last) {
      *out_iter = std::distance(ini_first, first);
      ++out_iter;
      ++first;
    }
  } while (first != last);
  return out_iter;
}

/*bool is_even_permutation(std::array<int, 16> arr);  */
std::vector<int> legal_moves(int zero_pos);
int distance_from_origin(int pos, int val, const std::array<int, 16> &ref_puz);
inline int current_piece(int pos, const std::array<int, 16> &scr_puz);
bool is_valid_move(int x, int y);
inline void make_move(int nxt_pos, int zero_pos, std::array<int, 16> &scr_puz);
int recommended_move(int zero_pos, const std::array<int, 16> &scr_puz,
                     const std::array<int, 16> &ref_puz);

int main() {
  std::array<int, 16> puzzle{1, 2,  3,  4,  5,  6,  7,  8,
                             9, 10, 11, 12, 13, 14, 15, 100};
  std::array<int, 16> scrambled_puzzle{1,  2,  4, 8,   5, 6,  3,  11,
                                       10, 15, 7, 100, 9, 13, 14, 12};
  // std::array<int, 16> scrambled_puzzle_x{1,  2,  4, 8, 5,  6,  3, 11,
  //                                       10, 15, 7, 9, 13, 14, 12, 100};
  std::vector<int> moves_made;

  // auto is_solvable = is_even_permutation(scrambled_puzzle_x);

  // std::cout << is_solvable << " "
  //          << "\n";

  // if (!is_solvable)
  //  return 0;

  int state = 0;

  while (state != -1) {
    auto zero_pos_iter = std::find(std::begin(scrambled_puzzle),
                                   std::end(scrambled_puzzle), 100);
    int zero_pos = std::distance(std::begin(scrambled_puzzle), zero_pos_iter);
    int next_move = recommended_move(zero_pos, scrambled_puzzle, puzzle);

    if (next_move != -1) {
      moves_made.push_back(current_piece(next_move, scrambled_puzzle));
      make_move(next_move, zero_pos, scrambled_puzzle);
    } else
      state = -1;
  }

  std::cout << "\n";

  for (size_t i = 0; i < moves_made.size(); i++) {
    std::cout << moves_made[i] << " ";
  }

  std::cout << "\n";

  for (size_t i = 0; i < scrambled_puzzle.size(); i++) {
    std::cout << scrambled_puzzle[i] << " ";
  }

  std::cout << "\n";

  return 0;
}

/*
bool is_even_permutation(std::array<int, 16> arr) {
  int no_of_swaps{0};
  for (int j = 1; j < arr.size(); j++) {
    int key = arr[j];
    int i = j - 1;

    while ((i >= 0) && (arr[i] > key)) {
      arr[i + 1] = arr[i];
      i = i - 1;
      ++no_of_swaps;
    }
    arr[i + 1] = key;
  }

  return (no_of_swaps % 2) == 0 ? true : false;
}
*/

std::vector<int> legal_moves(int zero_pos) {
  int y = zero_pos / 4;
  int x = zero_pos - (4 * y);

  auto co_moves{std::vector<std::pair<int, int>>{
      (std::make_pair((x - 1), y)), (std::make_pair((x + 1), y)),
      (std::make_pair(x, (y - 1))), (std::make_pair(x, (y + 1)))}};

  std::vector<int> moves;

  std::transform(std::begin(co_moves), std::end(co_moves),
                 std::back_inserter(moves), [](const auto &move) {
                   return is_valid_move(move.first, move.second)
                              ? move.first + (move.second * 4)
                              : -1;
                 });

  auto moves_end = std::remove(std::begin(moves), std::end(moves), -1);

  return std::vector(std::begin(moves), moves_end);
}

bool is_valid_move(int x, int y) {
  if (x > 3 || x < 0 || y > 3 || y < 0)
    return false;

  return true;
}

int distance_from_origin(int pos, int val, const std::array<int, 16> &ref_puz) {
  int y = pos / 4;
  int x = pos - (4 * y);

  auto found_location = std::find(std::begin(ref_puz), std::end(ref_puz), val);
  int pos_ref = std::distance(std::begin(ref_puz), found_location);

  int y_ref = pos_ref / 4;
  int x_ref = pos_ref - (4 * y_ref);

  return abs(y - y_ref) + abs(x - x_ref);
}

inline int current_piece(int pos, const std::array<int, 16> &scr_puz) {
  return scr_puz[pos];
}

inline void make_move(int nxt_pos, int zero_pos, std::array<int, 16> &scr_puz) {
  std::swap(scr_puz[nxt_pos], scr_puz[zero_pos]);
}

int recommended_move(int zero_pos, const std::array<int, 16> &scr_puz,
                     const std::array<int, 16> &ref_puz) {

  auto l_movs = legal_moves(zero_pos);
  std::vector<int> current_pieces;
  std::transform(std::begin(l_movs), std::end(l_movs),
                 std::back_inserter(current_pieces),
                 [&scr_puz](auto pos) { return current_piece(pos, scr_puz); });

  std::vector<int> distances;
  std::transform(std::begin(l_movs), std::end(l_movs),
                 std::begin(current_pieces), std::back_inserter(distances),
                 [&ref_puz](auto pos, auto val) {
                   return distance_from_origin(pos, val, ref_puz);
                 });

  if (std::all_of(std::begin(distances), std::end(distances),
                  [](auto val) { return val == 0; }))
    return -1;

  auto max_pos = std::max_element(std::begin(distances), std::end(distances));
  std::vector<int> max_pos_indices;
  find_if_indices(std::begin(distances), std::end(distances),
                  std::back_inserter(max_pos_indices),
                  [max_pos](auto val) { return *max_pos == val; });

  if (max_pos_indices.size() == std::size_t(1))
    return l_movs[max_pos_indices.back()];

  std::vector<int> predicted_distances;
  std::transform(std::begin(max_pos_indices), std::end(max_pos_indices),
                 std::back_inserter(predicted_distances),
                 [zero_pos, &current_pieces, &ref_puz](auto index) {
                   return distance_from_origin(zero_pos, current_pieces[index],
                                               ref_puz);
                 });

  auto min_pos = std::min_element(std::begin(predicted_distances),
                                  std::end(predicted_distances));

  auto min_index = std::distance(std::begin(predicted_distances), min_pos);

  return l_movs[max_pos_indices[min_index]];
}