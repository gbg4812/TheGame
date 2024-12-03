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
#define PLAYER_NAME Ron

struct PLAYER_NAME : public Player {
    /**
     * Factory: returns a new instance of this class.
     * Do not modify this function.
     */
    static Player *factory() { return new PLAYER_NAME; }

    /**
     * Types and attributes for your player can be defined here.
     */

    typedef pair<Pos, Dir> Decision;

    vector<Dir> wizard_dirs = {Up, Down, Right, Left};
    map<int, list<Pos>> wizard_enemies;
    set<Pos> taken_objectives;

    /**
     * Play method, invoked once per each round.
     */
    virtual void play() {
        int radi_efecte = 180;
        list<int> enemy_players;
        wizard_enemies.clear();
        taken_objectives.clear();

        for (int i = 0; i < 4; i++) {
            if (i != me() and probab_win(i) < 0.33) {
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

        // TODO: Compute Voldemort objective
        // TODO: Compute Ghoust recipe
        // TODO: Order in moves
        // TODO: ONLY CHANGE OBJECTIVE IF NO LONGER OK

        for (int id : wizards(me())) {
            Unit wiz = unit(id); // per cada mag meu
            move(id, search_targets(wiz.pos, radi_efecte, id));
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

        Cell c = cell(pos);
        if (c.type == Wall)
            return false;

        for (Pos en : wizard_enemies[wiz]) {
            if (distance(en, pos) < radius)
                return false;
        }

        if (c.id != -1 and unit(c.id).player == me())
            return false;
        if (distance(pos_voldemort(), pos) < 10)
            return false;

        return true;
    }

    bool interesting_cell(Pos p, int dist) {
        Cell c = cell(p);
        if (c.is_empty())
            return false;

        set<Pos>::iterator it = taken_objectives.find(p);
        if (c.book) {
            taken_objectives.insert(it, p);
            return true;
        }

        Unit u = unit(c.id);
        if (u.type == Ghost)
            return false;

        if (it != taken_objectives.end())
            return false;

        if (u.player == me() and u.is_in_conversion_process() and
            u.rounds_for_converting() < dist) {
            taken_objectives.insert(it, p);
            return true;
        }

        if (u.player != me() and not u.is_in_conversion_process()) {
            taken_objectives.insert(it, p);
            return true;
        }

        if (u.player != me() and u.is_in_conversion_process() and
            u.player_to_be_converted_to() != me()) {
            taken_objectives.insert(it, p);
            return true;
        }

        return false;
    }

    // busca l'objectiu apetitoso més proxim evitant parets i enemics (mantenint
    // el camí a un radi de l'enemic per que no et pugui interceptar) i retorna
    // el camí a seguir si torna un camí buit és que no n'ha trobat cap
    Dir search_targets(Pos pos, int depth, int wiz) {
        Dir res;
        map<Pos, Dir> parents;
        vector<vector<int>> distances(2 * depth + 1,
                                      vector<int>(2 * depth + 1, -1));
        queue<pair<Pos, Dir>> Q;
        Q.push({pos, Up});
        distances[depth][depth] = 0;

        // by default go to non wall option and rotate between wizards
        auto dir_distrib = random_permutation(4);
        Decision current_best = {pos, Up};
        for (int dirindx : dir_distrib) {
            if (cell(pos).type != Wall)
                current_best = {pos, wizard_dirs[dirindx]};
        }

        int current_best_dist = 0;

        bool is_first = true;
        while (not Q.empty()) {
            Decision nod = Q.front();
            Q.pop();
            parents.emplace(nod);
            if (interesting_cell(nod.first,
                                 distances[nod.first.i - pos.i + depth]
                                          [nod.first.j - pos.j + depth])) {
                // cerr << "Interesting path for witcher at: " << pos << endl;
                // cerr << "Interesting element at pos: " << nod.first << endl;
                // cerr << "Direction to follow: " << nod.second << endl;
                return nod.second;
            }

            int dist = distances[nod.first.i - pos.i + depth]
                                [nod.first.j - pos.j +
                                 depth]; // distancia fins al parent analitzat

            if (dist > current_best_dist)
                current_best = nod;

            if (dist < depth) {
                for (Dir dir : wizard_dirs) {
                    Pos npos = nod.first + dir;
                    if (pos_ok(npos) and distances[npos.i - pos.i + depth]
                                                  [npos.j - pos.j + depth] ==
                                             -1) {
                        if (safe_pos(npos, dist, wiz)) {
                            distances[npos.i - pos.i + depth]
                                     [npos.j - pos.j + depth] = dist + 1;
                            if (is_first) {

                                Q.push({npos, dir});
                            }
                            Q.push({npos, nod.second});
                        }
                    }
                }
                is_first = false;
            }
        }

        return current_best.second;
    }
};

/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
