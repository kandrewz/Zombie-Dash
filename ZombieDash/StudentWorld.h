#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include <string>
#include <list>
#include "Actor.h"
using namespace std;
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    ~StudentWorld();
    void decreaseCitizens()
    {
        m_citizens--;
    }
    int numCitizens() const
    {
        return m_citizens;
    }
    void completeLevel()
    {
        m_level_completed = true;
    }
    int findEuclidean(int x1, int x2, int y1, int y2);
    bool checkBlock(Actor* a, int dir, int move);
    bool checkOverlap(int x1, int y1, int x2, int y2);
    
    void doExit(Exit* ex);
    void doPit(Pit* pi);
    void doFlame(Flame* fl);
    void doGoodie(Goodie* go);
    void doVomit(Vomit* vo);
    void doLandmine(Landmine* la);
    void doSmartZombieDirection(SmartZombie* zo);
    void doCitizen(Citizen* ci);
    
    bool shootFlame(int x, int y);
    void shootVomit(int x, int y);
    
    void placeLandmine(int x, int y);
    void placeFlame(int x, int y);
    void placePit(int x, int y);
    void placeVaccineGoodie(int x, int y);
    void placeDumbZombie(int x, int y);
    void placeSmartZombie(int x, int y);
    
    void increaseVaccine();
    void increaseFlamethrower();
    void increaseLandmine();
    string updateGameStatText();
private:
    Level m_level;
    list<Actor*> m_actors;
    Penelope* m_penelope;
    int m_citizens;
    bool m_level_completed;
};
#endif // STUDENTWORLD_H_
