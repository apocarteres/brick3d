#include <iostream>
#include <algorithm>
#include <assert.h>
#include <vector>

#include "TetrisGame.h"
#include "MockRandomNumberProvider.h"
#include "test.h"
#include "figures/Figure1.h"
#include "figures/Figure2.h"

RandomNumberProvider* rnd_provider = new MockRandomNumberProvider();

void ensure_closed_only(TetrisGame& game, std::vector<int_pair> & pairs) {
    for (int i = 0; i < GAME_FIELD_ROWS; ++i) {
        for (int j = 0; j < GAME_FIELD_COLS; ++j) {
            if (game.is_border(i, j)) {
                assert(!game.is_free(i, j));
                continue;
            }
            bool f = game.is_free(i, j);
            bool c = game.contains_pair(pairs, i, j);
            assert((f && !c) || (!f && c));
        }
    }
}

void test_spawns_well_on_game_created() {
    TetrisGame game(rnd_provider);
    std::vector<int_pair> v;
    v.push_back({1, 5});
    v.push_back({1, 6});
    v.push_back({1, 7});
    v.push_back({1, 8});
    ensure_closed_only(game, v);
}

void test_moves_left_well() {
    TetrisGame game(rnd_provider);
    game.move_left();
    std::vector<int_pair> v;
    v.push_back({1, 4});
    v.push_back({1, 5});
    v.push_back({1, 6});
    v.push_back({1, 7});
    ensure_closed_only(game, v);
}

void test_moves_right_well() {
    TetrisGame game(rnd_provider);
    game.move_right();
    std::vector<int_pair> v;
    v.push_back({1, 6});
    v.push_back({1, 7});
    v.push_back({1, 8});
    v.push_back({1, 9});
    ensure_closed_only(game, v);
}

void test_moves_bottom_well() {
    TetrisGame game(rnd_provider);
    std::vector<int_pair> v;
    game.process();
    v.push_back({2, 5});
    v.push_back({2, 6});
    v.push_back({2, 7});
    v.push_back({2, 8});
    ensure_closed_only(game, v);
}

void test_drops_and_spawns_well() {
    MockRandomNumberProvider p(2);
    TetrisGame game(&p);
    game.drop();
    game.move_right();
    std::vector<int_pair> v;
    v.push_back({0, 6});
    v.push_back({0, 7});
    v.push_back({1, 6});
    v.push_back({1, 7});

    v.push_back({GAME_FIELD_ROWS - 2, 5});
    v.push_back({GAME_FIELD_ROWS - 2, 6});
    v.push_back({GAME_FIELD_ROWS - 1, 5});
    v.push_back({GAME_FIELD_ROWS - 1, 6});
    ensure_closed_only(game, v);
}

void test_moves_left_well_when_down_on_the_floor() {
    TetrisGame game(rnd_provider);
    for (int i = 1; i < GAME_FIELD_ROWS - 1; ++i) {
        assert(MOVE == game.process());
    }

    assert(game.move_left());
}

void test_moves_right_well_when_down_on_the_floor() {
    TetrisGame game(rnd_provider);
    for (int i = 1; i < GAME_FIELD_ROWS - 1; ++i) {
        assert(MOVE == game.process());
    }

    assert(game.move_right());
}

void test_can_not_move_left_when_left_bound_exceed() {
    TetrisGame game(rnd_provider);
    for (int i = 0; i < 4; ++i) {
        assert(game.move_left());
    }
    assert(!game.move_left());
}

void test_can_not_move_right_when_right_bound_exceed() {
    TetrisGame game(rnd_provider);
    for (int i = 0; i < 2; ++i) {
        assert(game.move_right());
    }
    assert(!game.move_right());
}

void test_can_not_rotate_when_right_bound_exceed() {
    TetrisGame game(rnd_provider);
    game.rotate();
    for (int i = 0; i < 3; ++i) {
        assert(game.move_right());
    }
    assert(!game.rotate());
    std::vector<int_pair> v;
    v.push_back({0, 9});
    v.push_back({1, 9});
    v.push_back({2, 9});
    v.push_back({3, 9});
    ensure_closed_only(game, v);
}

void test_can_not_rotate_when_right_obstacle() {
    TetrisGame game(rnd_provider);
    for (int i = 0; i < 3; ++i) {
        game.rotate();
        game.move_right();
        game.drop();
    }
    game.rotate();
    for (int i = 1; i < GAME_FIELD_ROWS - 4; ++i) {
        assert(MOVE == game.process());
    }
    assert(!game.rotate());
}

void test_can_not_rotate_when_bottom_bound_exceed() {
    TetrisGame game(rnd_provider);
    for (int i = 0; i < GAME_FIELD_ROWS - 2; ++i) {
        assert(MOVE == game.process());
    }
    assert(!game.rotate());
    std::vector<int_pair> v;
    v.push_back({GAME_FIELD_ROWS - 1, 5});
    v.push_back({GAME_FIELD_ROWS - 1, 6});
    v.push_back({GAME_FIELD_ROWS - 1, 7});
    v.push_back({GAME_FIELD_ROWS - 1, 8});

    ensure_closed_only(game, v);
}

void test_can_not_drop_on_floor_in_case_of_obstacle() {
    MockRandomNumberProvider rnd_p(2);
    TetrisGame game(&rnd_p);
    game.drop();
    game.move_left();
    game.drop();
    std::vector<int_pair> v;
    v.push_back({0, 5});
    v.push_back({0, 6});
    v.push_back({1, 5});
    v.push_back({1, 6});

    v.push_back({GAME_FIELD_ROWS - 4, 4});
    v.push_back({GAME_FIELD_ROWS - 4, 5});
    v.push_back({GAME_FIELD_ROWS - 3, 4});
    v.push_back({GAME_FIELD_ROWS - 3, 5});

    v.push_back({GAME_FIELD_ROWS - 2, 5});
    v.push_back({GAME_FIELD_ROWS - 2, 6});
    v.push_back({GAME_FIELD_ROWS - 1, 5});
    v.push_back({GAME_FIELD_ROWS - 1, 6});
    ensure_closed_only(game, v);
}

void test_move_bottom_well_if_on_left_bound() {
    TetrisGame game(rnd_provider);
    game.rotate();
    while (game.move_left());
    game.process();
    std::vector<int_pair> v;
    v.push_back({1, 1});
    v.push_back({2, 1});
    v.push_back({3, 1});
    v.push_back({4, 1});
    ensure_closed_only(game, v);
}

void test_can_not_move_right_in_case_of_obstacle() {
    TetrisGame game(rnd_provider);
    for (int i = 0; i < 3; i++) {
        game.rotate();
        game.move_right();
        game.move_right();
        game.drop();
    }
    game.rotate();
    game.move_right();
    for (int i = 0; i < 8; i++) {
        assert(MOVE == game.process());
    }
    assert(!game.move_right());
}

void test_figure1_rotates_well() {
    TetrisGame game(rnd_provider);
    game.process();
    std::vector<int_pair> v;
    for (int i = 0; i < 4; ++i) {
        v.push_back({2, 5 + i});
    }
    ensure_closed_only(game, v);
    game.rotate();
    v.clear();
    for (int i = 0; i < 4; ++i) {
        v.push_back({1 + i, 6});
    }
    ensure_closed_only(game, v);
    game.rotate();
    v.clear();
    for (int i = 0; i < 4; ++i) {
        v.push_back({2, 5 + i});
    }
    ensure_closed_only(game, v);
    game.rotate();
    v.clear();
    for (int i = 0; i < 4; ++i) {
        v.push_back({1 + i, 6});
    }
    ensure_closed_only(game, v);
}

void test_figure2_rotates_well() {
    MockRandomNumberProvider rnd_p(2);
    TetrisGame game(&rnd_p);
    std::vector<int_pair> v;
    for (int i = 0; i < 4; ++i) {
        v.clear();
        v.push_back({0, 5});
        v.push_back({0, 6});
        v.push_back({1, 5});
        v.push_back({1, 6});
        ensure_closed_only(game, v);
        game.rotate();
    }
}

void test_figure3_rotates_well() {
    MockRandomNumberProvider rnd_p(3);
    TetrisGame game(&rnd_p);
    std::vector<int_pair> v;
    v.push_back({0, 6});
    v.push_back({1, 6});
    v.push_back({1, 7});
    v.push_back({2, 6});
    ensure_closed_only(game, v);
    game.rotate();
    v.clear();
    v.push_back({1, 5});
    v.push_back({1, 6});
    v.push_back({1, 7});
    v.push_back({2, 6});
    ensure_closed_only(game, v);
    game.rotate();
    v.clear();
    v.push_back({0, 6});
    v.push_back({1, 5});
    v.push_back({1, 6});
    v.push_back({2, 6});
    ensure_closed_only(game, v);
    game.rotate();
    v.clear();
    v.push_back({0, 6});
    v.push_back({1, 5});
    v.push_back({1, 6});
    v.push_back({1, 7});
    ensure_closed_only(game, v);
}

void test_figure4_rotates_well() {
    MockRandomNumberProvider rnd_p(4);
    TetrisGame game(&rnd_p);
    std::vector<int_pair> v;
    v.push_back({0, 6});
    v.push_back({0, 7});
    v.push_back({1, 5});
    v.push_back({1, 6});
    ensure_closed_only(game, v);
    game.rotate();
    v.clear();
    v.push_back({0, 5});
    v.push_back({1, 5});
    v.push_back({1, 6});
    v.push_back({2, 6});
    ensure_closed_only(game, v);
    game.rotate();
    v.clear();
    v.push_back({0, 5});
    v.push_back({0, 6});
    v.push_back({1, 6});
    v.push_back({1, 7});
    ensure_closed_only(game, v);
    game.rotate();
    v.clear();
    v.push_back({0, 6});
    v.push_back({1, 5});
    v.push_back({1, 6});
    v.push_back({2, 5});
    ensure_closed_only(game, v);
}

void test_figure5_rotates_well() {
    MockRandomNumberProvider rnd_p(5);
    TetrisGame game(&rnd_p);
    std::vector<int_pair> v;
    v.push_back({0, 5});
    v.push_back({0, 6});
    v.push_back({0, 7});
    v.push_back({1, 7});
    ensure_closed_only(game, v);
    game.rotate();
    v.clear();
    v.push_back({0, 6});
    v.push_back({1, 6});
    v.push_back({2, 6});
    v.push_back({2, 5});
    ensure_closed_only(game, v);
    game.rotate();
    v.clear();
    v.push_back({0, 5});
    v.push_back({1, 5});
    v.push_back({1, 6});
    v.push_back({1, 7});
    ensure_closed_only(game, v);
    game.rotate();
    v.clear();
    v.push_back({0, 5});
    v.push_back({0, 6});
    v.push_back({1, 5});
    v.push_back({2, 5});
    ensure_closed_only(game, v);
}

void test_game_overs_when_spawn_failed() {
    TetrisGame game(rnd_provider);
    for (int i = 0; i < GAME_FIELD_ROWS - 2; ++i) {
        game.drop();
    }
    assert(GAME_OVER == game.drop());
}

void test_full_row_get_destroyed() {
    TetrisGame game(rnd_provider);
    while (game.move_left());
    game.drop();
    while (game.move_right());
    game.drop();
    game.rotate();
    game.drop();
    game.rotate();
    game.move_left();
    game.drop();
    std::vector<int_pair> v;
    v.push_back({GAME_FIELD_ROWS - 3, 5});
    v.push_back({GAME_FIELD_ROWS - 2, 5});
    v.push_back({GAME_FIELD_ROWS - 1, 5});
    v.push_back({GAME_FIELD_ROWS - 3, 6});
    v.push_back({GAME_FIELD_ROWS - 2, 6});
    v.push_back({GAME_FIELD_ROWS - 1, 6});

    v.push_back({1, 5});
    v.push_back({1, 6});
    v.push_back({1, 7});
    v.push_back({1, 8});
    ensure_closed_only(game, v);
}

void memTest() {
    TetrisGame game(rnd_provider);
    for (int i = 0; i < 10000; ++i) {
        game.process();
        game.rotate();
        game.move_left();
        game.move_left();
        game.drop();

        game.process();
        game.rotate();
        game.move_left();
        game.drop();

        game.process();
        game.rotate();
        game.drop();

        game.process();
        game.rotate();
        game.move_right();
        game.drop();

        game.process();
        game.rotate();
        game.move_right();
        game.move_right();
        game.drop();

        game.process();
        game.rotate();
        game.move_right();
        game.move_right();
        game.move_right();
        game.drop();
    }
}

void runTests() {
    test_figure1_rotates_well();
    test_figure2_rotates_well();
    test_figure3_rotates_well();
    test_figure4_rotates_well();
    test_figure5_rotates_well();
    test_spawns_well_on_game_created();
    test_moves_left_well();
    test_moves_right_well();
    test_moves_bottom_well();
    test_drops_and_spawns_well();
    test_moves_left_well_when_down_on_the_floor();
    test_moves_right_well_when_down_on_the_floor();
    test_can_not_move_left_when_left_bound_exceed();
    test_can_not_move_right_when_right_bound_exceed();
    test_can_not_rotate_when_right_bound_exceed();
    test_can_not_rotate_when_right_obstacle();
    test_can_not_move_right_in_case_of_obstacle();
    test_can_not_drop_on_floor_in_case_of_obstacle();
    test_can_not_rotate_when_bottom_bound_exceed();
    test_move_bottom_well_if_on_left_bound();
    test_game_overs_when_spawn_failed();
    test_full_row_get_destroyed();
    //    memTest();
}
