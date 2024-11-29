#include "Player.hh"
#include <list>

/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Novice

struct PLAYER_NAME : public Player {
    /**
     * Factory: returns a new instance of this class.
     * Do not modify this function.
     */
    static Player *factory() { return new PLAYER_NAME; }

    /**
     * Types and attributes for your player can be defined here.
     */

    /**
     * Play method, invoked once per each round.
     */
    virtual void play() {}
    list<Pos> search_targets(Pos pos, int depth, int min_targets) {
        return {{0, 0}};
    }
};

/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
