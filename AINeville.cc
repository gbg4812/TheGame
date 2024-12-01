#include "Player.hh"
#include "Structs.hh"
#include <iostream>
#include <list>
#include <math.h>
#include <queue>

/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Neville

struct PLAYER_NAME : public Player {
    /**
     * Factory: returns a new instance of this class.
     * Do not modify this function.
     */
    static Player *factory() { return new PLAYER_NAME; }

    /**
     * Types and attributes for your player can be defined here.
     */

    vector<Dir> wizard_dirs = {Up, Down, Right, Left};
    map<int, list<Pos>> wizard_enemies;

    /**
     * Play method, invoked once per each round.
     */
    virtual void play() {
        int radi_efecte = 30;
        list<int> enemy_players;

        for (int i = 0; i < 4; i++) {
            if (i != me()) {
                enemy_players.push_back(i);
            }
        }

        // construeixo les llistes d'enemics
        for (int id : wizards(me())) {
            Unit wiz = unit(id); // per cada mag meu

            for (int epl : enemy_players) { // per cada player enemic

                for (int eid : wizards(epl)) { // per cada mag de l'enemic
                    Unit ewiz = unit(eid);

                    if (distance(ewiz.pos, wiz.pos) <= radi_efecte) {
                        wizard_enemies[wiz.id].push_back(ewiz.pos);
                    };
                }
            }
        }

        // "desactivar" celles al voltant enemigs abans de buscar camins cap
        // objectius!

        for (int id : wizards(me())) {
            Unit wiz = unit(id); // per cada mag meu
            auto paths = search_targets(wiz.pos, radi_efecte, id);
            if (not paths.empty() and paths.top().first > 0)
                move(id, paths.top().second);
        }
    }

    inline Dir pos_to_dir(Pos in, Pos fi) {
        if (in.i > fi.i and in.j == fi.j) {
            return Up;
        } else if (in.i < fi.i and in.j == fi.j) {
            return Down;
        } else if (in.j < fi.j and in.i == fi.i) {
            return Right;
        } else if (in.j > fi.j and in.i == fi.i) {
            return Left;
        } else {
            std::cerr << "Invalid pos to dir conversion!" << endl;
            return Up;
        }
    }

    inline int distance(Pos pos1, Pos pos2) {
        return abs(pos2.i - pos1.i) + abs(pos2.j - pos1.j);
    }

    inline double probab_win(int player) {
        return (double)magic_strength(me()) /
               (magic_strength(player) + magic_strength(me()));
    }

    inline int abs(int n) {
        if (n < 0)
            return -n;
        return n;
    }

    bool safe_pos(Pos pos, int radius, int wiz) {

        if (cell(pos).type == Wall)
            return false;

        for (Pos en : wizard_enemies[wiz]) {
            if (distance(en, pos) < radius)
                return false;
        }

        return true;
    }

    bool interesting_cell(Cell c, int dist) {
        if (c.is_empty())
            return false;

        if (c.book)
            return true;

        Unit u = unit(c.id);
        if (u.player == me() and u.is_in_conversion_process() and
            u.rounds_for_converting() < dist) {
            return true;
        }

        if (u.player != me() and not u.is_in_conversion_process()) {
            return true;
        }

        if (u.player != me() and u.is_in_conversion_process() and
            u.player_to_be_converted_to() != me()) {
            return true;
        }

        return false;
    }

    // busca l'objectiu apetitoso més proxim evitant parets i enemics (mantenint
    // el camí a un radi de l'enemic per que no et pugui interceptar) i retorna
    // el camí a seguir si torna un camí buit és que no n'ha trobat cap
    priority_queue<pair<int, Dir>> search_targets(Pos pos, int depth, int wiz) {
        priority_queue<pair<int, Dir>> paths;
        list<Pos> path;
        vector<vector<int>> visited(2 * depth, vector<int>(2 * depth, 0));
        stack<Pos> S;
        S.push(pos);

        while (not S.empty()) {
            Pos nod = S.top();
            path.push_back(nod);
            Cell c = cell(nod);
            if (interesting_cell(c, path.size() - 1)) {
                cerr << "Interesting path for witcher at: " << pos << endl;
                for (auto el : path) {
                    cerr << el << ",";
                }
                cerr << endl;
                if(path.size()>1) {
                    Dir dir = pos_to_dir(pos, *(++path.begin()));
                    paths.push({path.size() - 1, dir});
                }
            }

            bool isparent = false;

            if (path.size() < depth) {
                for (Dir dir : wizard_dirs) {
                    Pos npos = nod + dir;
                    if (pos_ok(npos) and not visited[npos.i - pos.i + depth]
                                                    [npos.j - pos.j + depth]) {
                        if (safe_pos(nod + dir, 0, wiz)) {
                            isparent = true;
                            visited[npos.i - pos.i + depth]
                                   [npos.j - pos.j + depth] = 1;
                            S.push(npos);
                        }
                    }
                }
            }

            if (not isparent) {
                path.pop_back();
                S.pop();
            }
        }

        return paths;
    }
};

/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
