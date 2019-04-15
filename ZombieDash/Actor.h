#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
class StudentWorld;
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor: public GraphObject
{
public:
    Actor(int imageID, int startX, int startY, Direction startDirection, int depth, StudentWorld* world);
    virtual void doSomething() = 0;
    virtual ~Actor() = default;
    void makeDead()
    {
        m_alive = false;
    }
    bool isAlive() const
    {
        return m_alive;
    }
    virtual bool isBlock() const
    {
        return false;
    }
    virtual bool isHuman() const
    {
        return false;
    }
    virtual bool isZombie() const
    {
        return false;
    }
    virtual bool isFlammable() const
    {
        return false;
    }
    virtual bool isPitable() const
    {
        return false;
    }
    virtual bool isVomitable() const
    {
        return false;
    }
    virtual bool blocksHazard() const
    {
        return false;
    }
    virtual void setInfected(bool status)
    {
        return;
    }
    virtual bool isInfected() const
    {
        return false;
    }
    void damage();
    virtual void doDead()
    {
        return;
    }
protected:
    StudentWorld* getWorld() const
    {
        return m_world;
    }
private:
    virtual int getScore()
    {
        return 0;
    }
    virtual int getKillSound()
    {
        return SOUND_NONE;
    }
    StudentWorld* m_world;
    
    bool m_alive;
    bool m_block;
};

//HUMANS-----------------------------------------------------------------------------------------------------------
class Human: public Actor
{
public:
    Human(int imageID, int startX, int startY, Direction startDirection, int depth, StudentWorld* world);
    virtual bool isBlock() const
    {
        return true;
    }
    virtual bool isFlammable() const
    {
        return true;
    }
    virtual bool isPitable() const
    {
        return true;
    }
    virtual bool isVomitable() const
    {
        return true;
    }
    virtual bool isHuman() const
    {
        return true;
    }
    bool isInfected() const
    {
        return m_infectionStatus;
    }
    void setInfected(bool status)
    {
        m_infectionStatus = status;
    }
    int getInfectionCount() const
    {
        return m_infectionCount;
    }
    void setInfectionCount(int x)
    {
        m_infectionCount = x;
    }
    virtual void doSomething();
    virtual ~Human() = default;
private:
    virtual void doDifferent() = 0;
    virtual void doInfect() = 0;
    bool m_infectionStatus;
    int m_infectionCount;
};
//PENELOPE-----------------------------------------------------------------------------------------------------------
class Penelope: public Human
{
public:
    Penelope(StudentWorld* world, int col, int row);
    virtual ~Penelope() = default;
    int getLandmines() const
    {
        return m_numLandmines;
    }
    int getFlamethrowers() const
    {
        return m_numFlamethrowers;
    }
    int getVaccines() const
    {
        return m_numVaccines;
    }
    void increaseVaccines(int x)
    {
        m_numVaccines+= x;
    }
    void increaseFlamethrowers(int x)
    {
        m_numFlamethrowers += x;
    }
    void increaseLandmines(int x)
    {
        m_numLandmines += x;
    }
private:
    virtual int getKillSound()
    {
        return SOUND_PLAYER_DIE;
    }
    virtual void doDifferent();
    virtual void doInfect();
    int m_numLandmines;
    int m_numFlamethrowers;
    int m_numVaccines;
};
//CITIZENS-----------------------------------------------------------------------------------------------------------
class Citizen: public Human
{
public:
    Citizen(StudentWorld* world, int col, int row);
    virtual ~Citizen() = default;
    virtual void doDead();
    void moveCitizen(Direction dir);
private:
    virtual int getScore()
    {
        return -1000;
    }
    virtual int getKillSound()
    {
        return SOUND_CITIZEN_DIE;
    }
    virtual void doDifferent();
    virtual void doInfect();
    int m_ticks;
};
//ZOMBIES-----------------------------------------------------------------------------------------------------------
class Zombie: public Actor
{
public:
    Zombie(int imageID, int startX, int startY, Direction startDirection, int depth, StudentWorld* world);
    virtual bool isBlock() const
    {
        return true;
    }
    virtual bool isFlammable() const
    {
        return true;
    }
    virtual bool isPitable() const
    {
        return true;
    }
    virtual bool isZombie() const
    {
        return true;
    }
    virtual void doSomething();
    virtual ~Zombie() = default;
    void doRandomDirection();
private:
    virtual void doDifferentDirection() = 0;
    virtual int getScore()
    {
        return 1000;
    }
    virtual int getKillSound()
    {
        return SOUND_ZOMBIE_DIE;
    }
    int m_ticks;
    int m_movementPlanDistance;
};
//DUMB ZOMBIES-----------------------------------------------------------------------------------------------------------
class DumbZombie: public Zombie
{
public:
    DumbZombie(StudentWorld* world,int col, int row);
    virtual ~DumbZombie() = default;
    virtual void doDead();
private:
    virtual void doDifferentDirection();
};
//SMART ZOMBIES-----------------------------------------------------------------------------------------------------------
class SmartZombie: public Zombie
{
public:
    SmartZombie(StudentWorld* world,int col, int row);
    virtual ~SmartZombie() = default;
private:
    virtual int getScore()
    {
        return 2000;
    }
    virtual void doDifferentDirection();
};
//WALLS-----------------------------------------------------------------------------------------------------------
class Wall: public Actor
{
public:
    Wall(StudentWorld* world,int col, int row);
    virtual bool isBlock() const
    {
        return true;
    }
    virtual void doSomething()
    {
        return;
    }
    virtual bool blocksHazard() const
    {
        return true;
    }
    virtual ~Wall() = default;
private:
};

//EXITS-----------------------------------------------------------------------------------------------------------
class Exit: public Actor
{
public:
    Exit(StudentWorld* world,int col, int row);
    virtual void doSomething();
    virtual bool blocksHazard() const
    {
        return true;
    }
    virtual ~Exit() = default;
private:
};
//PITS-----------------------------------------------------------------------------------------------------------
class Pit: public Actor
{
public:
    Pit(StudentWorld* world,int col, int row);
    virtual void doSomething();
    virtual ~Pit() = default;
private:
};
//HAZARDS-----------------------------------------------------------------------------------------------------------
class Hazard: public Actor
{
public:
    Hazard(int imageID, int startX, int startY, Direction startDirection, int depth, StudentWorld* world);
    virtual void doSomething();
    virtual ~Hazard() = default;
private:
    virtual void doDifferent() = 0;
    int m_ticks;
};
//FLAMES-----------------------------------------------------------------------------------------------------------
class Flame: public Hazard
{
public:
    Flame(StudentWorld* world, int col, int row);
    virtual ~Flame() = default;
private:
    virtual void doDifferent();
};
//VOMITS-----------------------------------------------------------------------------------------------------------
class Vomit: public Hazard
{
public:
    Vomit(StudentWorld* world, int col, int row);
    virtual ~Vomit() = default;
private:
    virtual void doDifferent();
};
//GOODIES-----------------------------------------------------------------------------------------------------------
class Goodie: public Actor
{
public:
    Goodie(int imageID, int startX, int startY, Direction startDirection, int depth, StudentWorld* world);
    virtual void doSomething();
    virtual bool isFlammable() const
    {
        return true;
    }
    virtual void doDifferent() = 0;
    virtual ~Goodie() = default;
private:
    
};
//VACCINE GOODIE----------------------------------------------------------------------------------------------------------
class VaccineGoodie: public Goodie
{
public:
    VaccineGoodie(StudentWorld* world, int col, int row);
    virtual ~VaccineGoodie() = default;
    virtual void doDifferent();
private:
};
//GAS CAN GOODIE----------------------------------------------------------------------------------------------------------
class GasCanGoodie: public Goodie
{
public:
    GasCanGoodie(StudentWorld* world, int col, int row);
    virtual ~GasCanGoodie() = default;
    virtual void doDifferent();
private:
};
//LANDMINEGOODIE----------------------------------------------------------------------------------------------------------
class LandmineGoodie: public Goodie
{
public:
    LandmineGoodie(StudentWorld* world, int col, int row);
    virtual ~LandmineGoodie() = default;
    virtual void doDifferent();
private:
};
//LANDMINE----------------------------------------------------------------------------------------------------------
class Landmine: public Actor
{
public:
    Landmine(StudentWorld* world, int col, int row);
    virtual ~Landmine() = default;
    virtual void doDead();
    virtual void doSomething();
    virtual bool isFlammable() const
    {
        return true;
    }
private:
    virtual int getKillSound()
    {
        return SOUND_LANDMINE_EXPLODE;
    }
    bool m_active;
    int m_safetyTicks;
};
#endif // ACTOR_H_
