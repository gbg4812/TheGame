#include "Player.hh"
#include "Structs.hh"
#include <array>
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

    struct TargetWizard {
        double probab;
        int dist;
        Pos path;
        int id;
        bool operator<(const TargetWizard &other) {
            if (this->probab < other.probab) {
                return true;
            } else if (this->probab > other.probab) {
                return false;
            } else {
                if (this->dist > other.dist) {
                    return true;
                } else {
                    return false;
                }
            }
        }
    };

    friend bool operator>(const TargetWizard w1, const TargetWizard w2) {
        if (w1.probab > w2.probab) {
            return true;
        } else if (w1.probab < w2.probab) {
            return false;
        } else {
            return w1.dist < w2.dist;
        }
    }

    vector<Dir> wizard_dirs = {Up, Down, Right, Left};
    vector<list<Pos>> wizard_enemies;

    /**
     * Play method, invoked once per each round.
     */
    virtual void play() {
        int radi_efecte = 30;
        list<int> enemy_players;
        wizard_enemies.resize(wizards(me()).size());

        for (int i = 0; i < 4; i++) {
            if (i != me()) {
                enemy_players.push_back(i);
            }
        }

        // construeixo les llisted d'enemics
        for (int id : wizards(me())) {
            Unit wiz = unit(id); // per cada mag meu

            for (int epl : enemy_players) { // per cada player enemic

                for (int eid : wizards(epl)) { // per cada mag de l'enemic
                    Unit ewiz = unit(eid);

                    Pos
                }
            }
        }

        // calcular zones on hi ha wizards meus per buscar només en aquelles
        // els llibres per cada wizard meu
        // "desactivar" celles al voltant enemigs abans de buscar camins cap
        // objectius!

        for (int id : wizards(me())) {
            Unit wizard = unit(id);

            for (int i = 0; i < 4; i++) {
                if (i == me())
                    continue;

                double probab = probab_win(i);

                for (int oid : wizards(i)) {
                    Unit owizard = unit(oid);
                    Pos opath = path(wizard.pos, owizard.pos);
                    int dist = abs(opath.i) + abs(opath.j);
                    if (dist > radi_efecte) {
                        continue;
                    }

                    if (probab >= 0.4) {
                        weakerUnits.push({probab, dist, opath, oid});
                    } else {
                        strongerUnits.push_back({probab, dist, opath, oid});
                    }
                }
            }

            // we search for a weak target with no intersection

            Pos target = {-1, -1};
            while (not weakerUnits.empty() and not pos_ok(target)) {
                TargetWizard wunit = weakerUnits.top();
                weakerUnits.pop();

                for (TargetWizard sunit : strongerUnits) {
                    if (safe_path(wizard.pos, wunit.path, sunit.path)) {
                        target = wunit.path;
                        break;
                    }
                }
            }

            if (pos_ok(target)) {
                if (target.i > target.j) {
                    if (target.i > 0) {
                        move(wizard.id, Down);
                    } else {
                        move(wizard.id, Up);
                    }
                } else {

                    if (target.j > 0) {
                        move(wizard.id, Right);
                    } else {
                        move(wizard.id, Left);
                    }
                }
            }
        }
    }

    bool safe_path(Pos mypos, Pos safep, Pos dangerp) {
        dangerp = {dangerp.i - safep.i, dangerp.j - safep.j};
        if (path_length(dangerp) >= path_length(safep)) {
            return true;
        } else {
            return false;
        }

        int i = 0;
        int j = 0;
        while (i < safep.i and j < safep.j) {
            if (i < j) {
                mypos.i += 1;
                i++;
            } else {
                mypos.j += 1;
                j++;
            }

            if (not pos_ok(mypos))
                return false;
            if (cell(mypos).type == Wall)
                return false;
        }

        while (i < safep.i) {
            if (not pos_ok(mypos))
                return false;
            if (cell(mypos).type == Wall)
                return false;
            mypos.i += 1;
            i++;
        }

        while (j < safep.j) {
            if (not pos_ok(mypos))
                return false;
            if (cell(mypos).type == Wall)
                return false;
            mypos.j += 1;
            j++;
        }

        return true;
    }

    inline int path_length(Pos path) { return path.i + path.j; }

    inline double probab_win(int player) {
        return (double)magic_strength(me()) /
               (magic_strength(player) + magic_strength(me()));
    }

    inline int abs(int n) {
        if (n < 0)
            return -n;
        return n;
    }

    // moviments en i o j a fer per anar de pos1 a pos2
    Pos path(Pos pos1, Pos pos2) { return {pos2.i - pos1.i, pos2.j - pos1.j}; }

    // implementació bfs de moment no té utilitat
    vector<list<Dir>> search_targets(Pos pos, int depth) {
        vector<vector<list<Dir>>> paths;
        vector<vector<int>> visited(depth, vector<int>(depth, 0));
        queue<Pos> Q;
        Q.push(pos);
        int i = 0;
        while (not Q.empty() and i < depth) {
            Pos v = Q.front();
            Q.pop();

            for (auto dir : wizard_dirs) {
                Pos npos = v + dir;
                if (not pos_ok(npos) or visited[npos.i][npos.j]) {
                    continue;
                }

                Cell c = cell(v);

                if (c.type == Wall) {
                    continue;
                }

                visited[npos.i][npos.j] = 1;
                Q.push(v + dir);
            }
        }
        return {};
    }
};

/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
