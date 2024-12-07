#include "Player.hh"
#include "Structs.hh"
#include <cmath>
#include <iostream>
#include <list>
#include <queue>

/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME SirNicolas

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

    struct Move {
        int score;
        int id;
        Dir dir;
    };

    friend bool operator<(Move mov1, Move mov2) {
        if (mov1.score < mov2.score)
            return true;
        return false;
    }

    // global vairables
    vector<Dir> wizard_dirs = {Up, Down, Right, Left};
    vector<Dir> ghost_dirs = {Down, DR, Right, RU, Up, UL, Left, LD};
    set<Pos> taken_objectives;
    map<int, list<Pos>> wizard_enemies;
    list<int> enemy_players;
    int afaid_radius = 10;
    int victim = 0;
    vector<vector<int>> enemy_distances;

    /**
     * Play method, invoked once per each round.
     */
    virtual void play() {
        int max_path_len = 500;
        enemy_players.clear();
        wizard_enemies.clear();
        taken_objectives.clear();
        priority_queue<Move> movesq;
        enemy_distances.resize(board_rows(), vector<int>(board_cols(), 0));
        fillMat(enemy_distances, 0);

        // cast spell
        Unit ughost = unit(ghost(me()));
        if (ughost.resting_rounds() == 0) {
            enchantment();
        }

        // chose enemy players
        for (int i = 0; i < 4; i++) {
            if (i != me() and probab_win(i) < 0.40) {
                enemy_players.push_back(i);
            }
        }

        // TODO: change objective only on demand
        // TODO: compute enemyes with bfs

        // voldemort objective
        int distvic = 1000;
        for (int i = 0; i < num_ini_wizards_per_clan() * 4; i++) {
            Unit wiz = unit(i);
            if (pitagoras_dist(wiz.pos, pos_voldemort()) < distvic) {
                victim = i;
            }
        }

        for (int wid : wizards(me())) {
            Unit wiz = unit(wid);
            for (int enpl : enemy_players) {
                for (int en : wizards(enpl)) {
                    if (distance(unit(en).pos, wiz.pos) > afaid_radius) {
                        wizard_enemies[wid].push_back(unit(en).pos);
                    }
                }
            }
        }

        for (int id : wizards(me())) {
            Unit wiz = unit(id); // per cada mag meu
            movesq.push(search_targets(wiz.pos, max_path_len, id, wizard_dirs));
        }

        movesq.push(
            search_targets(ughost.pos, max_path_len, ughost.id, ghost_dirs));

        while (not movesq.empty()) {
            move(movesq.top().id, movesq.top().dir);
            movesq.pop();
        }
    }

    void fillMat(vector<vector<int>> mat, int val) {
        for (vector<int> &v : mat) {
            for (int &el : v) {
                el = val;
            }
        }
    }

    inline int distance(Pos pos1, Pos pos2) {
        return abs(pos2.i - pos1.i) + abs(pos2.j - pos1.j);
    }

    inline int pitagoras_dist(Pos pos1, Pos pos2) {
        Pos pdiff = {pos1.i - pos2.i, pos1.j - pos2.j};
        return sqrt(pdiff.i * pdiff.i + pdiff.j * pdiff.j);
    }

    inline double probab_win(int player) {
        if (magic_strength(me()) == 0 and magic_strength(player) == 0)
            return 0.5;

        return (double)magic_strength(me()) /
               (magic_strength(player) + magic_strength(me()));
    }

    inline int abs(int n) {
        if (n < 0)
            return -n;
        return n;
    }

    bool bdf_pos(Pos resercher, Pos target, int dist,
                 vector<vector<int>> &dists) {
        queue<Pos> Q;
        Q.push(resercher);
        while (not Q.empty()) {
            Pos pos = Q.front();

            if (dists[pos.i][pos.j] == -1)
                for (auto dir : wizard_dirs) {
                    Pos npos = pos + dir;
                }
        }
        return false;
    }

    bool safe_pos(Pos pos, int radius, int wiz) {

        Cell c = cell(pos);
        if (c.type == Wall)
            return false;

        if (unit(wiz).type == Wizard) {
            for (Pos en : wizard_enemies[wiz]) {
                if (distance(en, pos) < radius + 1)
                    return false;
            }
        } else if (unit(wiz).type == Ghost) {
            for (int i = 0; i < 4; i++) {
                if (i != me()) {
                    for (int en : wizards(i)) {
                        int endist = distance(unit(en).pos, pos);
                        if (endist < radius + 1 and endist < afaid_radius)
                            return false;
                    }
                }
            }
        }

        if (c.id != -1 and unit(c.id).player == me() and radius == 1)
            return false;

        if (wiz == victim) {
            Pos vdist = {abs(pos_voldemort().i - pos.i),
                         abs(pos_voldemort().j - pos.j)};

            if ((vdist.i < 2 + radius and vdist.j < 1) or
                (vdist.j < 2 + radius and vdist.i < 1))
                return false;
        } else {
            Pos vdist = {abs(pos_voldemort().i - pos.i),
                         abs(pos_voldemort().j - pos.j)};

            if ((vdist.i < 3 and vdist.j < 2) or (vdist.j < 3 and vdist.i < 2))
                return false;
        }

        return true;
    }

    void enchantment() {
        auto ingred = spell_ingredients();
        vector<int> sol(ingred.size(), -1);

        int ngrps = ingred.size() / 3;
        int total_sum = 0;
        for (int ing : ingred) {
            total_sum += ing;
        }
        int sum = total_sum / ngrps;
        bool res = enchantment(ingred, sol, ngrps - 1, sum);
        spell(ghost(me()), sol);
    }

    bool enchantment(const vector<int> &ingred, vector<int> &sol, int grpidx,
                     int sum) {
        if (grpidx < 0)
            return true;
        for (size_t i = 0; i < sol.size(); i++) {
            int local_sum = 0;
            if (sol[i] == -1) {
                sol[i] = grpidx;
                local_sum += ingred[i];
                for (size_t j = i + 1; j < sol.size(); j++) {
                    if (sol[j] == -1) {
                        sol[j] = grpidx;
                        local_sum += ingred[j];
                        for (size_t k = j + 1; k < sol.size(); k++) {
                            if (sol[k] == -1) {
                                local_sum += ingred[k];
                                sol[k] = grpidx;
                                if (local_sum == sum) {
                                    bool res = enchantment(ingred, sol,
                                                           grpidx - 1, sum);
                                    if (res)
                                        return res;
                                }
                                sol[k] = -1;
                                local_sum -= ingred[k];
                            }
                        }
                        sol[j] = -1;
                        local_sum -= ingred[j];
                    }
                }
                sol[i] = -1;
                local_sum -= ingred[i];
            }
        }
        return false;
    }

    pair<bool, int> ghost_cell_eval(Pos p, int dist) {
        Cell c = cell(p);
        if (c.is_empty())
            return {false, 0};

        set<Pos>::iterator it = taken_objectives.find(p);

        if (it != taken_objectives.end())
            return {false, 0};

        if (c.book) {
            taken_objectives.insert(it, p);
            return {true, 0};
        }

        return {false, 0};
    }

    pair<bool, int> wizard_cell_eval(Pos p, int dist) {
        Cell c = cell(p);
        if (c.is_empty())
            return {false, 0};

        set<Pos>::iterator it = taken_objectives.find(p);

        if (it != taken_objectives.end())
            return {false, 0};

        if (c.book) {
            taken_objectives.insert(it, p);
            return {true, 0};
        }

        Unit u = unit(c.id);
        if (u.type == Ghost)
            return {false, 0};

        if (u.player == me() and u.is_in_conversion_process() and
            u.rounds_for_converting() <= dist) {
            taken_objectives.insert(it, p);
            return {true, 0};
        }

        if (u.player != me() and not u.is_in_conversion_process()) {
            taken_objectives.insert(it, p);
            if (dist == 2)
                return {true, -1};
            else if (dist == 1) {
                return {true, 2};
            } else {
                return {true, 0};
            }
        }

        if (u.player != me() and u.is_in_conversion_process() and
            u.player_to_be_converted_to() != me()) {
            taken_objectives.insert(it, p);
            if (dist == 2)
                return {true, -1};
            else if (dist == 1) {
                return {true, 2};
            } else {
                return {true, 0};
            }
        }

        return {false, 0};
    }

    // busca l'objectiu apetitoso més proxim evitant parets i enemics
    // (mantenint el camí a un radi de l'enemic per que no et pugui
    // interceptar) i retorna el camí a seguir si torna un camí buit és que
    // no n'ha trobat cap
    Move search_targets(Pos pos, int depth, int uid,
                        const vector<Dir> &allowedDirections) {
        Dir res;
        map<Pos, Dir> parents;
        vector<vector<int>> distances(board_rows(),
                                      vector<int>(board_cols(), -1));
        queue<pair<Pos, Dir>> Q;
        Q.push({pos, Up});
        distances[pos.i][pos.j] = 0;

        // by default go to non wall option and rotate between wizards
        auto dir_distrib = random_permutation(allowedDirections.size());
        Decision default_desc = {pos, Up};
        int best_dist;
        bool danger = false;

        // check the surroundings
        // set default position and if in danger
        for (int dirindx : dir_distrib) {
            Pos new_pos = pos + allowedDirections[dirindx];
            Cell c = cell(new_pos);
            if (c.is_empty()) {
                default_desc.second = allowedDirections[dirindx];
            } else if (c.id != -1) {
                for (int en : enemy_players) {
                    if (unit(c.id).player == en)
                        danger = true;
                }
            }
        }

        bool is_first = true;
        while (not Q.empty()) {
            Decision nod = Q.front();
            Q.pop();
            parents.emplace(nod);
            pair<bool, int> score;
            if (unit(uid).type == Wizard)
                score = wizard_cell_eval(nod.first,
                                         distances[nod.first.i][nod.first.j]);
            else if (unit(uid).type == Ghost) {
                score = ghost_cell_eval(nod.first,
                                        distances[nod.first.i][nod.first.j]);
            }
            if (score.first) {
                // cerr << "Interesting path for witcher at: " << pos <<
                // endl; cerr << "Interesting element at pos: " << nod.first
                // << endl; cerr << "Direction to follow: " << nod.second <<
                // endl;

                // if has a enemy touching high priority
                if (danger)
                    score.second = 1;

                return {score.second, uid, nod.second};
            }

            int dist = distances[nod.first.i][nod.first.j]; // distancia fins al
                                                            // parent analitzat
            if (dist > best_dist) {
                default_desc = nod;
                best_dist = dist;
            }

            if (dist < depth) {
                for (int dirindx : dir_distrib) {
                    Dir dir = allowedDirections[dirindx];
                    Pos npos = nod.first + dir;
                    if (pos_ok(npos) and distances[npos.i][npos.j] == -1) {
                        if (safe_pos(npos, dist + 1, uid)) {
                            distances[npos.i][npos.j] = dist + 1;
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

        cerr << "Not safe path found unit: " << uid << " at pos: " << pos
             << endl;
        return {1, uid, default_desc.second};
    }
};

/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
