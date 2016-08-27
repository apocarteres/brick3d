#include <iostream>
#include <assert.h>
#include <algorithm>

#include "TetrisGame.h"
#include "figures/Figure1.h"
#include "figures/Figure2.h"
#include "figures/Figure3.h"
#include "figures/Figure4.h"
#include "figures/Figure5.h"
#include "test.h"

TetrisGame::TetrisGame(RandomNumberProvider* provider) {
    this->rnd_provider = provider;
    figures[1] = new Figure1();
    figures[2] = new Figure2();
    figures[3] = new Figure3();
    figures[4] = new Figure4();
    figures[5] = new Figure5();
    init_field();
}

TetrisGame::~TetrisGame() {
    for (auto & f : figures) {
        delete f.second;
    }
}

bool TetrisGame::move(int di, int dj, bool dt, bool spawned) {
    if (!current_f) {
        return false;
    }
    assert(di == 0 || di == 1);
    assert(dj == 0 || std::abs(dj) == 1);
    geometry geo = current_f->find_geometry(current_t);
    //when figure on the left bound
    if ((dj < 0 || dt) && current_j == 0) {
        return false;
    }
    //when figure already on the floor
    if (di > 0 || dt) {
        int h = calc_height(geo);
        if (current_i + h >= GAME_FIELD_ROWS - 1) {
            return false;
        }
    }
    //collect occupied cells
    std::vector<int_pair> ocp_cells;
    if (spawned) {
        for (int i = 0; i < geo.size(); ++i) {
            for (int j = 0; j < GEOMETRY_SIZE; ++j) {
                if (geo[i][j]) {
                    //check figure doesn't exceed right bound
                    if (current_j + j + dj >= GAME_FIELD_COLS) {
                        return false;
                    }
                    ocp_cells.push_back({current_i + i, current_j + j});
                }
            }
        }
    }
    char translate = current_t;
    if (dt) {
        translate++;
        if (translate > MAX_ROTATION_INDEX) {
            translate = 0;
        }
        geo = current_f->find_geometry(translate);
    }
    for (int i = 0; i < geo.size(); ++i) {
        for (int j = 0; j < GEOMETRY_SIZE; ++j) {
            int offset_i = current_i + i + di;
            int offset_j = current_j + j + dj;
            if (geo[i][j] && !is_free(offset_i, offset_j) && !contains_pair(ocp_cells, offset_i, offset_j)) {
                return false;
            }
        }
    }
    for (auto & p : ocp_cells) {
        field[p.first][p.second] = vec4(0.0f, 0.0f, 0.0f, 0.0f);
    }
    for (int i = 0; i < geo.size(); ++i) {
        for (int j = 0; j < GEOMETRY_SIZE; ++j) {
            if (geo[i][j]) {
                field[current_i + i + di][current_j + j + dj] = current_c;
            }
        }
    }
    current_i += di;
    current_j += dj;
    current_t = translate;
    return true;
}

bool TetrisGame::rotate() {
    return move(0, 0, true);
}

void TetrisGame::init_field() {
    for (int i = 0; i < GAME_FIELD_ROWS; ++i) {
        for (int j = 0; j < GAME_FIELD_COLS; ++j) {
            if (is_border(i, j)) {
                field[i][j] = vec4(1.0f, 0.0f, 0.0f, 6.0f);
            } else {
                field[i][j] = vec4(0.0f, 0.0f, 0.0f, 0.0f);
            }

        }
    }
    spawn();
}

vec4 TetrisGame::get_color(int i, int j) {
    return field[i][j];
}

bool TetrisGame::is_border(int i, int j) {
    return (j == 0 || j == GAME_FIELD_COLS - 1);
}

int TetrisGame::calc_height(geometry& geo) {
    int h = 0;
    for (int i = 0; i < geo.size(); ++i) {
        if (geo[i].any()) {
            h = i;
        }
    }
    return h;
}

ProcessResult TetrisGame::drop() {
    ProcessResult result;
    while ((result = process()) == MOVE);
    return result;
}

void TetrisGame::debug() {
    for (int i = 0; i < GAME_FIELD_ROWS; ++i) {
        std::cout << "x" << " | ";
        for (int j = 0; j < GAME_FIELD_COLS; ++j) {
            std::cout << (!is_free(i, j)) << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

bool TetrisGame::spawn() {
    int i = rnd_provider->next_int(figures.size());
    assert(figures.count(i) == 1);
    current_f = figures[i];
    int gsize_2 = GEOMETRY_SIZE / 2;
    current_t = 0;
    current_i = 0;
    current_j = 1 + GAME_FIELD_COLS / 2 - gsize_2;
    current_c = vec4(
            rnd_provider->next_float(0.0f),
            rnd_provider->next_float(0.0f),
            rnd_provider->next_float(0.0f),
            rnd_provider->next_float(9.0f)
            );
    geometry g = current_f->find_geometry(current_t);
    return move(0, 0, false, false);
}

bool TetrisGame::move_left() {
    return move(0, -1);
}

bool TetrisGame::move_right() {
    return move(0, 1);
}

int TetrisGame::destroy() {
    int count = 0;
    for (int i = GAME_FIELD_ROWS - 1; i >= 0; --i) {
        bool ready = 1;
        for (int j = 0; j < GAME_FIELD_COLS; ++j) {
            ready &= !is_free(i, j);
            if (!ready) {
                break;
            }
        }
        if (ready) {
            for (int j = i; j > 0; --j) {
                for (int f = 0; f < GAME_FIELD_COLS; ++f) {
                    field[j][f] = field[j - 1][f];
                }
            }
            count++;
        }

    }
    if (count > 0) {
        count += destroy();
    }
    return count;
}

ProcessResult TetrisGame::process() {
    ProcessResult result = MOVE;
    if (!move(1, 0)) {
        result = DROP;
        int destroyed = destroy();
        if (destroyed) {
            result = DESTROY;
        }
        if (!spawn()) {
            result = GAME_OVER;
            current_f = 0;
        }
    }
    return result;
}

bool TetrisGame::is_free(int i, int j) {
    return field[i][j] == glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
}

bool TetrisGame::is_clean() {
    for (int i = 0; i < GAME_FIELD_ROWS; ++i) {
        for (int j = 0; j < GAME_FIELD_COLS; ++j) {
            if (!is_free(i, j)) {
                return false;
            }
        }
    }
    return true;
}

void TetrisGame::addGameOverCb(game_over_cb cb) {
    game_over_callbacks.push_back(cb);
}

bool TetrisGame::contains_pair(std::vector<int_pair>& pairs, int i, int j) {
    return 1 == count_if(pairs.begin(), pairs.end(), [i, j](int_pair & p) {
        return p.first == i && p.second == j;
    });
}


