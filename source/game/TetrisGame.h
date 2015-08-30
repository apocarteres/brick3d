#pragma once 

#include <map>
#include "glm/glm.hpp"
#include "figures/AbstractFigure.h"
#include "RandomNumberProvider.h"

#define GAME_FIELD_COLS 12
#define GAME_FIELD_ROWS 22

using glm::vec4;

typedef std::pair<int, int> int_pair;
typedef void (*game_over_cb)();

enum ProcessResult {
    MOVE,
    DESTROY,
    DROP,
    GAME_OVER
};

class TetrisGame {
public:
    TetrisGame(RandomNumberProvider* provider);
    virtual ~TetrisGame();

    virtual ProcessResult process();
    virtual ProcessResult drop();
    virtual bool rotate();
    virtual bool move_left();
    virtual bool move_right();
    virtual bool is_free(int, int);
    virtual vec4 get_color(int, int);
    virtual bool is_clean();
    virtual bool is_border(int, int);    
    virtual void debug();
    virtual void addGameOverCb(game_over_cb cb);
    bool contains_pair(std::vector<int_pair>&, int, int);    
private:
    RandomNumberProvider* rnd_provider;
    std::map<int, AbstractFigure*> figures;
    std::vector<game_over_cb> game_over_callbacks;
    AbstractFigure* current_f;
    int current_i;
    int current_j;
    vec4 current_c;
    char current_t;
    vec4 field[GAME_FIELD_ROWS][GAME_FIELD_COLS];
    
    virtual int destroy();
    virtual int calc_height(geometry & geo);
    virtual void init_field();
    virtual bool move(int, int, bool dt = false, bool spawned = true);
    virtual bool spawn();
};

