#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp
//ACTOR--------------------------------------------------------------------------------------------------------
Actor::Actor(int imageID, int startX, int startY, Direction startDirection, int depth, StudentWorld* world)
:GraphObject(imageID, startX, startY, startDirection, depth)
{
    m_world = world;
    m_alive = true;
}
void Actor::damage()
{
    getWorld()->increaseScore(getScore()); //score is initially 0
    makeDead();
    doDead();
    if (getKillSound() != SOUND_NONE)
        getWorld()->playSound(getKillSound()); //plays death sound if actor has one
}
//HUMAN--------------------------------------------------------------------------------------------------------
Human::Human(int imageID, int startX, int startY, Direction startDirection, int depth, StudentWorld* world)
:Actor(imageID, startX, startY, startDirection, depth, world)
{
    m_infectionCount = 0; //humans start off uninfected
    m_infectionStatus = false;}
void Human::doSomething()
{
    if (!isAlive()) //check if human is alive
        return;
    if (isInfected()) //check if human is infected
    {
        m_infectionCount++; //if human infected, increase infection
        if (m_infectionCount == 500) //if infection count is 500 ticks, human dies
        {
            makeDead(); //human dies
            doInfect();
            return;
        }
    }
    doDifferent();
}
//PENELOPE--------------------------------------------------------------------------------------------------------
Penelope::Penelope(StudentWorld* world, int col, int row)
:Human(IID_PLAYER, col, row, right, 0, world)
{
    m_numLandmines = 0;
    m_numFlamethrowers = 0;
    m_numVaccines = 0;
}

void Penelope::doDifferent()
{
    int ch;
    if (getWorld()->getKey(ch)) //get input from user
    {
        switch(ch)
        {
            case KEY_PRESS_UP:
                setDirection(up);
                if (!getWorld()->checkBlock(this, up, 4))
                    moveTo(getX(), getY() + 4);
                break;
            case KEY_PRESS_LEFT:
                setDirection(left);
                if (!getWorld()->checkBlock(this, left, 4))
                    moveTo(getX() - 4, getY());
                break;
            case KEY_PRESS_DOWN:
                setDirection(down);
                if (!getWorld()->checkBlock(this, down, 4))
                    moveTo(getX(), getY() - 4);
                break;
            case KEY_PRESS_RIGHT:
                setDirection(right);
                if (!getWorld()->checkBlock(this, right, 4))
                    moveTo(getX() + 4, getY());
                break;
            case KEY_PRESS_SPACE:
                if (getFlamethrowers() > 0)
                {
                    increaseFlamethrowers(-1);
                    getWorld()->playSound(SOUND_PLAYER_FIRE);
                    int dir = getDirection();
                    for (int i = 1; i < 4; i++)
                    {
                        int newposx = getX();
                        int newposy = getY();
                        switch(dir)
                        {
                            case up:
                                newposy = getY() + i*SPRITE_HEIGHT;
                                break;
                            case left:
                                newposx = getX() - i*SPRITE_WIDTH;
                                break;
                            case down:
                                newposy = getY() - i*SPRITE_HEIGHT;
                                break;
                            case right:
                                newposx = getX() + i*SPRITE_WIDTH;
                                break;
                        }
                        if (!getWorld()->shootFlame(newposx, newposy))
                        {
                            break;
                        }
                        else
                        {
                            getWorld()->placeFlame(newposx, newposy);
                        }
                    }
                }
                break;
            case KEY_PRESS_TAB:
                if (getLandmines() > 0)
                {
                    increaseLandmines(-1);
                    getWorld()->placeLandmine(getX(), getY());
                }
                break;
            case KEY_PRESS_ENTER:
                if (getVaccines() > 0)
                {
                    increaseVaccines(-1);
                    setInfected(false);
                    setInfectionCount(0);
                }
                break;
            default:
                ;
        }
     }
    return;
}
void Penelope::doInfect()
{
    getWorld()->playSound(SOUND_PLAYER_DIE);
}

//CITIZEN--------------------------------------------------------------------------------------------------------
Citizen::Citizen(StudentWorld* world, int col, int row)
:Human(IID_CITIZEN, col, row, right, 0, world)
{
}
void Citizen::doDifferent()
{
    m_ticks++;
    if (m_ticks % 2 == 0)
        return;
    getWorld()->doCitizen(this);
}
void Citizen::moveCitizen(Direction dir) //function to make moving citizen easier
{
    if (!getWorld()->checkBlock(this, dir, 2)) //if citizen isnt blocked and can move in the direction 2 pixels
    {
        setDirection(dir); //set direction to the direction
        switch(dir)
        {
            case up:
                moveTo(getX(), getY()+2);
                break;
            case left:
                moveTo(getX()-2, getY());
                break;
            case right:
                moveTo(getX()+2, getY());
                break;
            case down:
                moveTo(getX(), getY()-2);
                break;
        }
        return;
    }
}
void Citizen::doInfect()
{
    getWorld()->increaseScore(-1000); //decrease score by 1000
    getWorld()->decreaseCitizens();
    getWorld()->playSound(SOUND_ZOMBIE_BORN);
    int r = randInt(1, 10);
    if (r > 3)
        getWorld()->placeDumbZombie(getX(), getY());
    else
        getWorld()->placeSmartZombie(getX(), getY());
}

void Citizen::doDead()
{
    getWorld()->decreaseCitizens();
}
//WALL--------------------------------------------------------------------------------------------------------
Wall::Wall(StudentWorld* world, int col, int row)
:Actor(IID_WALL, col, row, right, 0, world)
{
}
//EXIT--------------------------------------------------------------------------------------------------------
Exit::Exit(StudentWorld* world, int col, int row)
:Actor(IID_EXIT, col, row, right, 1, world)
{
    
}
void Exit::doSomething()
{
    getWorld()->doExit(this);
}
//PIT--------------------------------------------------------------------------------------------------------
Pit::Pit(StudentWorld* world, int col, int row)
:Actor(IID_PIT, col, row, right, 0, world)
{
    
}

void Pit::doSomething()
{
    getWorld()->doPit(this);
}
//HAZARDS--------------------------------------------------------------------------------------------------------
Hazard::Hazard(int imageID, int startX, int startY, Direction startDirection, int depth, StudentWorld* world)
:Actor(imageID, startX, startY, startDirection, depth, world)
{
    m_ticks = 0;
}
void Hazard::doSomething()
{
    if (!isAlive())
        return;
    if (m_ticks == 1)
    {
        makeDead();
        return;
    }
    doDifferent();
    m_ticks++;
}
//FLAMES--------------------------------------------------------------------------------------------------------
Flame::Flame(StudentWorld* world, int col, int row)
:Hazard(IID_FLAME, col, row, up, 0, world)
{
    
}

void Flame::doDifferent()
{
    getWorld()->doFlame(this);
}
//VOMITS--------------------------------------------------------------------------------------------------------
Vomit::Vomit(StudentWorld* world, int col, int row)
:Hazard(IID_VOMIT, col, row, right, 0, world)
{
    
}

void Vomit::doDifferent()
{
    getWorld()->doVomit(this);
}
//GOODIES--------------------------------------------------------------------------------------------------------
Goodie::Goodie(int imageID, int startX, int startY, Direction startDirection, int depth, StudentWorld* world)
:Actor(imageID, startX, startY, startDirection, depth, world)
{
}

void Goodie::doSomething()
{
    if (!isAlive())
        return;
    getWorld()->doGoodie(this);
}
//VACCINE GOODIE--------------------------------------------------------------------------------------------------------
VaccineGoodie::VaccineGoodie(StudentWorld* world, int col, int row)
:Goodie(IID_VACCINE_GOODIE, col, row, right, 1, world)
{

}

void VaccineGoodie::doDifferent()
{
    getWorld()->increaseVaccine();
}
//Gas Can GOODIE--------------------------------------------------------------------------------------------------------
GasCanGoodie::GasCanGoodie(StudentWorld* world, int col, int row)
:Goodie(IID_GAS_CAN_GOODIE, col, row, right, 1, world)
{
    
}

void GasCanGoodie::doDifferent()
{
    getWorld()->increaseFlamethrower();
}
//Landmine GOODIE--------------------------------------------------------------------------------------------------------
LandmineGoodie::LandmineGoodie(StudentWorld* world, int col, int row)
:Goodie(IID_LANDMINE_GOODIE, col, row, right, 1, world)
{
    
}

void LandmineGoodie::doDifferent()
{
    getWorld()->increaseLandmine();
}
//LANDMINE----------------------------------------------------------------------------------------------------------
Landmine::Landmine(StudentWorld* world, int col, int row)
:Actor(IID_LANDMINE, col, row, right, 1, world)
{
    m_active = false;
    m_safetyTicks = 30;
}

void Landmine::doSomething()
{
    if (!isAlive())
        return;
    if (!m_active)
    {
        m_safetyTicks--;
        if (m_safetyTicks == 0)
        {
            m_active = true;
        }
        return;
    }
    getWorld()->doLandmine(this);
     
}
void Landmine::doDead()
{
    int x = getX();
    int y = getY();
    getWorld()->placeFlame(x, y);//make flames in square around landmine, center
    if (getWorld()->shootFlame(x+SPRITE_WIDTH, y))
        getWorld()->placeFlame(x+SPRITE_WIDTH, y);//right
    
    if (getWorld()->shootFlame(x, y+SPRITE_HEIGHT))
        getWorld()->placeFlame(x, y+SPRITE_HEIGHT); //up
    
    if (getWorld()->shootFlame(x-SPRITE_WIDTH, y))
        getWorld()->placeFlame(x-SPRITE_WIDTH, y);//left
    
    if (getWorld()->shootFlame(x, y-SPRITE_WIDTH))
        getWorld()->placeFlame(x, y-SPRITE_HEIGHT); //down
    
    if (getWorld()->shootFlame(x+SPRITE_WIDTH, y+SPRITE_HEIGHT))
        getWorld()->placeFlame(x+SPRITE_WIDTH, y+SPRITE_HEIGHT);//up right
    
    if (getWorld()->shootFlame(x+SPRITE_WIDTH, y-SPRITE_HEIGHT))
        getWorld()->placeFlame(x+SPRITE_WIDTH, y-SPRITE_HEIGHT); //down right
    
    if (getWorld()->shootFlame(x-SPRITE_WIDTH, y+SPRITE_HEIGHT))
        getWorld()->placeFlame(x-SPRITE_WIDTH, y+SPRITE_HEIGHT); //up left
    
    if (getWorld()->shootFlame(x-SPRITE_WIDTH, y-SPRITE_HEIGHT))
        getWorld()->placeFlame(x-SPRITE_WIDTH, y-SPRITE_HEIGHT);//down left
    
    getWorld()->placePit(x, y); //make pit at landmine location
}
//ZOMBIES--------------------------------------------------------------------------------------------------------
Zombie::Zombie(int imageID, int startX, int startY, Direction startDirection, int depth, StudentWorld* world)
:Actor(imageID, startX, startY, startDirection, depth, world)
{
    m_ticks = 0;
    m_movementPlanDistance = 0;
}
void Zombie::doSomething()
{
    m_ticks++;
    if (!isAlive())
        return;
    if (m_ticks % 2 == 0)
        return;
    int dir = getDirection(); //compute hypothetical vomit
    int x = getX();
    int y = getY();
    switch(dir)
    {
        case up:
            getWorld()->shootVomit(x, y+SPRITE_HEIGHT);
            break;
        case left:
            getWorld()->shootVomit(x-SPRITE_WIDTH, y);
            break;
        case down:
            getWorld()->shootVomit(x, y-SPRITE_HEIGHT);
            break;
        case right:
            getWorld()->shootVomit(x+SPRITE_WIDTH, y);
            break;
    }
    if (m_movementPlanDistance == 0) //if zombie has run out of movement plan distance
    {
        m_movementPlanDistance = randInt(3, 10); //make a new movement plan distanxe
        doDifferentDirection(); //different amongst both zombies
    }
    if (!getWorld()->checkBlock(this, getDirection(), 1)) //if the zombie can move in that direction, movethere
    {
        switch(getDirection())
        {
            case up:
                moveTo(getX(), getY()+1);
                break;
            case left:
                moveTo(getX()-1, getY());
                break;
            case down:
                moveTo(getX(), getY()-1);
                break;
            case right:
                moveTo(getX()+1, getY());
                break;
        }
        m_movementPlanDistance--;
    }
    else m_movementPlanDistance = 0; //else ur stuck and set ur movement plan distanceto 0
}
void Zombie::doRandomDirection()
{
    int r = randInt(1, 4); //make a random new direction
    switch(r)
    {
        case 1:
            setDirection(up);
            break;
        case 2:
            setDirection(left);
            break;
        case 3:
            setDirection(down);
            break;
        case 4:
            setDirection(right);
            break;
    }
}
//DUMB ZOMBIES--------------------------------------------------------------------------------------------------------
DumbZombie::DumbZombie(StudentWorld* world, int col, int row)
:Zombie(IID_ZOMBIE, col, row, right, 0, world)
{
    
}
void DumbZombie::doDead()
{
    int x = randInt(1, 10);
    if (x == 1) //1/10 chance for a dumbzombie to drop a goodie
    {
        int r = randInt(1, 4);
        switch(r)
        {
            case 1: //fling vaccine goodie SPRITE_HEIGHT pixels up if not blocked
                if (!getWorld()->checkBlock(this, up, SPRITE_HEIGHT))
                {
                    getWorld()->placeVaccineGoodie(getX(), getY() + SPRITE_HEIGHT) ;
                }
                break;
            case 2: //fling vaccine goodie SPRITE_HEIGHT pixels down if not blocked
                if (!getWorld()->checkBlock(this, down, SPRITE_HEIGHT))
                {
                    getWorld()->placeVaccineGoodie(getX(), getY() - SPRITE_HEIGHT) ;
                }
                break;
            case 3: //fling vaccine goodie SPRITE_WIDTH pixels right if not blocked
                if (!getWorld()->checkBlock(this, right, SPRITE_WIDTH))
                {
                    getWorld()->placeVaccineGoodie(getX() + SPRITE_WIDTH, getY()) ;
                }
                break;
            case 4: //fling vaccine goodie SPRITE_HEIGHT pixels left if not blocked
                if (!getWorld()->checkBlock(this, left, SPRITE_WIDTH))
                {
                    getWorld()->placeVaccineGoodie(getX() - SPRITE_WIDTH, getY()) ;
                }
                break;
            
        }
    }

}
void DumbZombie::doDifferentDirection()
{
    doRandomDirection(); //a dumb zombie is dumb adn just does a random direction
}
//SMART ZOMBIES--------------------------------------------------------------------------------------------------------
SmartZombie::SmartZombie(StudentWorld* world, int col, int row)
:Zombie(IID_ZOMBIE, col, row, right, 0, world)
{
    
}
void SmartZombie::doDifferentDirection()
{
    getWorld()->doSmartZombieDirection(this);
}
