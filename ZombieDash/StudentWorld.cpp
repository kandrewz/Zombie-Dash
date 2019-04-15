#include "StudentWorld.h"
#include "GameConstants.h"
#include "Level.h"
#include <string>
#include <sstream>  // defines the type std::ostringstream
#include <iomanip>  // defines the manipulator setw
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}
//finds euclidean distance between 2 points squared
int StudentWorld::findEuclidean(int x1, int y1, int x2, int y2)
{
    return (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2);
}
// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), m_level(assetPath)
{
    m_citizens = 0;
    m_penelope = nullptr;
    m_level_completed = false;
}

int StudentWorld::init()
{
    m_citizens = 0; //set # of citizens to false
    m_penelope = nullptr; //set penelope to nullptr
    m_level_completed = false; //set level completed bool to false
    
    Level lev(assetPath()); //load level file using a oss
    ostringstream oss;
    oss.fill('0');
    oss << "level" << setw(2) << getLevel() << ".txt";
    string levelFile = oss.str();
    Level::LoadResult result = lev.loadLevel(levelFile);
    if (result == Level::load_fail_file_not_found)
    {
        cerr << "Cannot find level01.txt data file" << endl;
        return GWSTATUS_PLAYER_WON;
    }
    else if (result == Level::load_fail_bad_format)
    {
        cerr << "Your level was improperly formatted" << endl;
        return GWSTATUS_LEVEL_ERROR;
    }
    else if (result == Level::load_success)
    {
        cerr << "Successfully loaded level" << endl;
        for (int i = 0; i < LEVEL_WIDTH; i++)
        {
            for (int j = 0; j < LEVEL_HEIGHT; j++)
            {
                Level::MazeEntry ge = lev.getContentsOf(i, j); //iterate over the contents of the level and allocate and insert the respective actors
                switch (ge)
                {
                    case Level::empty:
                        break;
                    case Level::smart_zombie:
                        m_actors.push_back(new SmartZombie(this, SPRITE_WIDTH*i, SPRITE_HEIGHT*j));
                        break;
                    case Level::dumb_zombie:
                        m_actors.push_back(new DumbZombie(this, SPRITE_WIDTH*i, SPRITE_HEIGHT*j));
                        break;
                    case Level::player:
                        m_penelope = new Penelope(this, SPRITE_WIDTH*i, SPRITE_HEIGHT*j);
                        break;
                    case Level::exit:
                        m_actors.push_back(new Exit(this, SPRITE_WIDTH*i, SPRITE_HEIGHT*j));
                        break;
                    case Level::wall:
                        m_actors.push_back(new Wall(this, SPRITE_WIDTH*i, SPRITE_HEIGHT*j));
                        break;
                    case Level::pit:
                        m_actors.push_back(new Pit(this, SPRITE_WIDTH*i, SPRITE_HEIGHT*j));
                        break;
                    case Level::citizen:
                        m_actors.push_back(new Citizen(this, SPRITE_WIDTH*i, SPRITE_HEIGHT*j));
                        m_citizens++;
                        break;
                    case Level::vaccine_goodie:
                        m_actors.push_back(new VaccineGoodie(this, SPRITE_WIDTH*i, SPRITE_HEIGHT*j));
                        break;
                    case Level::gas_can_goodie:
                        m_actors.push_back(new GasCanGoodie(this, SPRITE_WIDTH*i, SPRITE_HEIGHT*j));
                        break;
                    case Level::landmine_goodie:
                        m_actors.push_back(new LandmineGoodie(this, SPRITE_WIDTH*i, SPRITE_HEIGHT*j));
                    default:
                        break;
                }
            }
        }
    }
    return GWSTATUS_CONTINUE_GAME; //continue the game
}

int StudentWorld::move()
{
    m_level_completed = false;
    m_penelope->doSomething(); //have penelope do something
    //have each of the actors do something
    list<Actor*>::iterator it = m_actors.begin();
    while(it != m_actors.end())
    {
        if ((*it)->isAlive())
        {
            (*it)->doSomething();
            if (!m_penelope->isAlive())
            {
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }
            if (m_level_completed)
            {
                playSound(SOUND_LEVEL_FINISHED);
                return GWSTATUS_FINISHED_LEVEL;
            }
        }
        it++;
    }
    //clean up the dead actors
    it = m_actors.begin();
    while(it != m_actors.end())
    {
        if (!(*it)->isAlive())
        {
            delete *(it); //IMPORTANT WHEN DELETING OBJECTS WHILE ITERATING THROUGH LIST OF POINTERS
            m_actors.remove(*(it++));
        }
        else
            it++;
    }
    //Update Display Text // update the score/lives/level text at screen top
    setGameStatText(updateGameStatText());

    // the player hasn’t completed the current level and hasn’t died, so // continue playing the current level
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    if (m_penelope != nullptr) //delete penelope
    {
        delete m_penelope;
        m_penelope = nullptr;
    }
    if (!m_actors.empty()) //delete the rest of the actors
    {
        list<Actor*>::iterator it = m_actors.begin();
        while (it != m_actors.end())
        {
            delete *(it); //IMPORTANT WHEN DELETING OBJECTS WHILE ITERATING THROUGH LIST OF POINTERS
            m_actors.remove(*(it++));
        }
    }
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

//CHECK OVERLAP AND CHECKBLOCK----------------------------------------------------------------------------------------------
bool StudentWorld::checkBlock(Actor *a, Direction dir, int move)
{
    list<Actor*>::iterator it = m_actors.begin();
    int x;
    int y;
    
    while (it != m_actors.end()) //loop through all the actors
    {
        if ((*it)->isBlock() && a != *it)
        {
            y = (*it)->getY() - a->getY(); //difference between actor and actor row
            x = (*it)->getX() - a->getX(); //difference between actor and actor col
            switch(dir)
            {
                case Actor::right: //actor facing right
                    if (x-move < SPRITE_WIDTH && x-move > -SPRITE_WIDTH && y < SPRITE_HEIGHT && y > -SPRITE_HEIGHT)
                        return true;
                    break;
                case Actor::up: //actor facing up
                    if (y-move < SPRITE_HEIGHT && y-move > -SPRITE_HEIGHT && x < SPRITE_WIDTH && x > -SPRITE_WIDTH)
                        return true;
                    break;
                case Actor::left: //actor facing left
                    if (x+move > -SPRITE_WIDTH && x+move < SPRITE_WIDTH && y < SPRITE_HEIGHT && y > -SPRITE_HEIGHT)
                        return true;
                    break;
                case Actor::down: //actor facing down
                    if (y+move > -SPRITE_HEIGHT && y+move < SPRITE_HEIGHT && x < SPRITE_WIDTH && x > -SPRITE_WIDTH)
                        return true;
                    break;
            }
        }
        it++;
    }
    //now for penelope
    y = m_penelope->getY() - a->getY(); //difference between actor and actor row
    x = m_penelope->getX() - a->getX(); //difference between actor and actor col

    if (x != 0 || y != 0) //making sure penelope isnt blocking herself
    {
        switch(dir)
        {
            case Actor::right: //actor facing right
                if (x-move < SPRITE_WIDTH && x-move > -SPRITE_WIDTH && y < SPRITE_HEIGHT && y > -SPRITE_HEIGHT)
                    if (x < 30 && x > -30 && y < 30 && y > -30)
                    return true;
                    break;
            case Actor::up: //actor facing up
                if (y-move < SPRITE_HEIGHT && y-move > -SPRITE_HEIGHT && x < SPRITE_WIDTH && x > -SPRITE_WIDTH)
                    if (x < 30 && x > -30 && y < 30 && y > -30)
                    return true;
                break;
            case Actor::left: //actor facing left
                if (x+move > -SPRITE_WIDTH && x+move < SPRITE_WIDTH && y < SPRITE_HEIGHT && y > -SPRITE_HEIGHT)
                    if (x < 30 && x > -30 && y < 30 && y > -30)
                    return true;
                break;
            case Actor::down: //actor facing down
                if (y+move > -SPRITE_HEIGHT && y+move < SPRITE_HEIGHT && x < SPRITE_WIDTH && x > -SPRITE_WIDTH)
                    if (x < 30 && x > -30 && y < 30 && y > -30)
                    return true;
                break;
            default:
                break;
        }
    }
    return false;
}
bool StudentWorld::checkOverlap(int x1, int y1, int x2, int y2)
{
    if (findEuclidean(x1, y1, x2, y2) <= 100) //checks if distance between centers is less than equal to 10 pixels
        return true;
    return false;
}
//UPDATeGAMeSTAtTEXT-----------------------------------------------------------------------------------------------------
string StudentWorld::updateGameStatText() //udpates game stat text
{
    //make a string stream
    ostringstream oss;
    oss.fill('0');
    if (getScore() >= 0)
        oss << "Score: " << setw(6) << getScore(); //score
    else
        oss << "Score: -" << setw(5) << getScore()*-1; //if score is negative, put  minus sign in front
    oss.fill(' ');
    oss << "  Level:" << setw(3) << getLevel(); //level
    oss << "  Lives:" << setw(2) << getLives(); //lives
    oss << "  Vacc:" << setw(3) << m_penelope->getVaccines(); //vaccines
    oss << "  Flames" << setw(3) << m_penelope->getFlamethrowers(); //flames
    oss << "  Mines:" << setw(3) << m_penelope->getLandmines(); //mines
    oss << "  Infected: " << setw(3) << m_penelope->getInfectionCount(); //infected count
    string setGameStat = oss.str();
    return setGameStat;
}
//ALL THE DOS------------------------------------------------------------------------------------------------------
void StudentWorld::doExit(Exit* ex)
{
    list<Actor*>::iterator it = m_actors.begin();
    while (it != m_actors.end())
    {
        if ((*it)->isHuman() &&  checkOverlap(ex->getX(), ex->getY(), (*it)->getX(), (*it)->getY()))
        {
            increaseScore(1000); 
            (*it)->makeDead();
            playSound(SOUND_CITIZEN_SAVED);
            decreaseCitizens();
        }
        it++;
    }
    if (checkOverlap(ex->getX(), ex->getY(), m_penelope->getX(), m_penelope->getY()) && m_citizens == 0)
    {
        completeLevel();
    }
}

void StudentWorld::doPit(Pit* pi)
{
    list<Actor*>::iterator it = m_actors.begin();
    while (it != m_actors.end())
    {
        if ((*it)->isPitable() && checkOverlap(pi->getX(), pi->getY(), (*it)->getX(), (*it)->getY()))
        {
            (*it)->damage();
        }
        it++;
    }
    if (checkOverlap(pi->getX(), pi->getY(), m_penelope->getX(), m_penelope->getY()))
    {
        m_penelope->damage();
    }
}
void StudentWorld::doFlame(Flame* fl)
{
    list<Actor*>::iterator it = m_actors.begin();
    while (it != m_actors.end()) //iterate through actors
    {
        if ((*it)->isFlammable() && checkOverlap(fl->getX(), fl->getY(), (*it)->getX(), (*it)->getY()) && (*it)->isAlive()) //if actor is flammable
        {
            (*it)->damage(); //damage the actor
        }
        it++;
    }
    if (checkOverlap(fl->getX(), fl->getY(), m_penelope->getX(), m_penelope->getY()))
    {
        m_penelope->damage();
    }
}
void StudentWorld::doVomit(Vomit* vo)
{
    list<Actor*>::iterator it = m_actors.begin();
    while (it != m_actors.end())
    {
        if ((*it)->isVomitable() && checkOverlap(vo->getX(), vo->getY(), (*it)->getX(), (*it)->getY()))
        {
            if (!(*it)->isInfected())
                playSound(SOUND_CITIZEN_INFECTED);
            (*it)->setInfected(true);
        }
        it++;
    }
    if (checkOverlap(vo->getX(), vo->getY(), m_penelope->getX(), m_penelope->getY()))
    {
        m_penelope->setInfected(true);
    }
}
void StudentWorld::doGoodie(Goodie* go)
{
    if (checkOverlap(go->getX(), go->getY(), m_penelope->getX(), m_penelope->getY())) //goodie is picked up when it overlaps with penelope
    {
        increaseScore(50); //when peneloep overlaps with a goodie, increase score by 50
        go->makeDead();
        go->doDifferent();
        playSound(SOUND_GOT_GOODIE);
    }
}
void StudentWorld::doLandmine(Landmine* la)
{
    list<Actor*>::iterator it = m_actors.begin(); //iterate through each of the actors
    while (it != m_actors.end())
    {
        if ((*it)->isPitable() && checkOverlap(la->getX(), la->getY(), (*it)->getX(), (*it)->getY())) //if the actor is landminable
        { 
            la->damage(); //damage the landmine
        }
        it++;
    }
    if (checkOverlap(la->getX(), la->getY(), m_penelope->getX(), m_penelope->getY()))
    {
        la->damage(); //damage landmine
    }

}
bool StudentWorld::shootFlame(int x, int y)
{
    list<Actor*>::iterator it = m_actors.begin(); //iterate through each of the actors
    while (it != m_actors.end())
    {
        if ((*it)->blocksHazard() && checkOverlap(x, y, (*it)->getX(), (*it)->getY())) //if the actor is a wall or exit, flame is blocked so return false
        {
            return false;
        }
        it++;
    }
    return true; //not wall or exit in way so can create new flame
}
void StudentWorld::shootVomit(int x, int y)
{
    list<Actor*>::iterator it = m_actors.begin(); //iterate through each of the actors
    while (it != m_actors.end())
    {
        if ((*it)->isHuman() && checkOverlap(x, y, (*it)->getX(), (*it)->getY())) //if the actor is a human, chance to vomit
        {
            int r = randInt(1, 3);
            if (r == 1) //if 1/3 chance, shoot vomit
            {
                m_actors.push_back(new Vomit(this, x, y));
                playSound(SOUND_ZOMBIE_VOMIT);
            }
        }
        it++;
    }
    if (checkOverlap(x, y, m_penelope->getX(), m_penelope->getY())) //same as above but for penelope
    {
        int r = randInt(1, 3);
        if (r == 1) //if 1/3 chance, shoot vomit
        {
            m_actors.push_back(new Vomit(this, x, y));
            playSound(SOUND_ZOMBIE_VOMIT);        }
    }
}
void StudentWorld::doSmartZombieDirection(SmartZombie* zo)
{
    int x = zo->getX(); //zombies col
    int y = zo->getY(); //zombies row
    int closestX = m_penelope->getX(); //closest humans col, initially set it to penelopes
    int closestY = m_penelope->getY(); //closest humans row
    int dist = 1<<15; //make the initial distance rly big
    list<Actor*>::iterator it = m_actors.begin(); //iterate through each of the actors
    while (it != m_actors.end())
    {
        if ((*it)->isHuman()) //if the actor is a human
        {
            if (dist > findEuclidean(x, y, (*it)->getX(), (*it)->getY())) //if the distance between this human and the zombie is lowest than the current min, set the min to this distances
            {
                dist = findEuclidean(x, y, (*it)->getX(), (*it)->getY());
                closestX = (*it)->getX();
                closestY = (*it)->getY();
            }
        }
        it++;
    }
    if (findEuclidean(x, y, m_penelope->getX(), m_penelope->getY()) < dist) //also compute distance for penelope
    {
        dist = findEuclidean(x, y, m_penelope->getX(), m_penelope->getY());
        closestX = m_penelope->getX();
        closestY = m_penelope->getY();
    }
    if (dist > 6400) //if closest distance is greater than 80 pixels, or 80^2 = 6400 euclidean, just do a random direction
    {
        zo->doRandomDirection();
    }
    else //closest distance is within 80 pixels, so move towards that human
    {
        if (closestX-x == x) //if they are on the same col
        {
            if (closestY > y) //human is above zombie
                zo->setDirection(Actor::up); //so move up
            else
                zo->setDirection(Actor::down); //move down bc human below zombie
        }
        else if (closestY == y) //if they are on the same row
        {
            if (closestX > x) //human is to right of zombie
                zo->setDirection(Actor::right); //so move right
            else
                zo->setDirection(Actor::left); //move left bc human to left of zombie
        }
        else
        {
            if (closestX > x && closestY > y) //human is in upper right quadrant to zombie
            {
                int r = randInt(0, 1); //choose randomly from up and right
                if (r == 0) //random said move right
                {
                    if (!checkBlock(zo, Actor::right, 1))
                        zo->setDirection(Actor::right); //if it can move right, it moves right
                    else
                        zo->setDirection(Actor::up); //if its blocked, move up
                }
                else //random said move up
                {
                    if (!checkBlock(zo, Actor::up, 1))
                        zo->setDirection(Actor::up); //if it can move up, it moves up
                    else
                        zo->setDirection(Actor::right);
                }
            }
            else if (closestX < x && closestY > y) //human is in upper left quadrant to zombie
            {
                int r = randInt(0, 1); //choose randomly from up and left
                if (r == 0) //random said move left
                {
                    if (!checkBlock(zo, Actor::left, 1))
                        zo->setDirection(Actor::left); //if it can move left, it moves left
                    else
                        zo->setDirection(Actor::up); //if its blocked, move up
                }
                else //random said move up
                {
                    if (!checkBlock(zo, Actor::up, 1))
                        zo->setDirection(Actor::up); //if it can move up, it moves up
                    else
                        zo->setDirection(Actor::left);
                }

            }
            else if (closestX > x && closestY < y) //human is in lower right quadrant to zombie
            {
                int r = randInt(0, 1); //choose randomly from down and right
                if (r == 0) //random said move right
                {
                    if (!checkBlock(zo, Actor::right, 1))
                        zo->setDirection(Actor::right); //if it can move right, it moves right
                    else
                        zo->setDirection(Actor::down); //if its blocked, move down
                }
                else //random said move down
                {
                    if (!checkBlock(zo, Actor::down, 1))
                        zo->setDirection(Actor::down); //if it can move down, it moves down
                    else
                        zo->setDirection(Actor::right);
                }

            }
            else if (closestX < x && closestY < y) //human is in lower left quadrant to zombie
            {
                int r = randInt(0, 1); //choose randomly from down and left
                if (r == 0) //random said move left
                {
                    if (!checkBlock(zo, Actor::left, 1))
                        zo->setDirection(Actor::left); //if it can move left, it moves left
                    else
                        zo->setDirection(Actor::down); //if its blocked, move down
                }
                else //random said move down
                {
                    if (!checkBlock(zo, Actor::down, 1))
                        zo->setDirection(Actor::down); //if it can move left, it moves left
                    else
                        zo->setDirection(Actor::left);
                }

            }
        }
    }
}
void StudentWorld::doCitizen(Citizen *ci)
{
    int x = ci->getX();
    int y = ci->getY();
    int dist_p = findEuclidean(x, y, m_penelope->getX(), m_penelope->getY()); //distance from penelope squared
    int dist_z = 1<<15; //distance to nearest zombie, initially infinite in case of no zombies
    list<Actor*>::iterator it = m_actors.begin(); //iterate through each of the actors
    while (it != m_actors.end())
    {
        if ((*it)->isZombie()) //if the actor is a zombie
        {
            if (dist_z > findEuclidean(x, y, (*it)->getX(), (*it)->getY())) //if the distance between this human and the zombie is lowest than the current min, set the min to this distances
            {
                dist_z = findEuclidean(x, y, (*it)->getX(), (*it)->getY());
            }
        }
        it++;
    }
    if (dist_p < dist_z && dist_p <= 6400) //if citizen is closer to penelope than to zombie && within 80 pixels
    {
        int closestX = m_penelope->getX();
        int closestY = m_penelope->getY();
        if (closestX == x) //if they are on the same col
        {
            if (closestY > y) //penelope is above citizen
            {
                if (!checkBlock(ci, Actor::up, 2)) //see if can move in that direction
                {
                    ci->moveCitizen(Actor::up); //move the ciitzens
                    return;
                }
            }
            else
            {
                if (!checkBlock(ci, Actor::down, 2)) //see if can move in that direction
                {
                    ci->moveCitizen(Actor::down); //move the ciitzens
                    return;
                }
            }
        }
        else if (closestY == y) //if they are on the same row
        {
            if (closestX > x) //penelope is to right of ciitzen
            {
                if (!checkBlock(ci, Actor::right, 2)) //see if can move in that direction
                {
                    ci->moveCitizen(Actor::right); //move the ciitzens
                    return;
                }
            }
            else
            {
                if (!checkBlock(ci, Actor::left, 2)) //see if can move in that direction
                {
                    ci->moveCitizen(Actor::left); //move the ciitzens
                    return;
                }
            }
        }
        else
        {
            if (closestX > x && closestY > y) //penelope is in upper right quadrant to citizen
            {
                int r = randInt(0, 1); //choose a random directoin
                if (r == 0)
                {
                    if (!checkBlock(ci, Actor::up, 2)) //see if can move in that direction
                    {
                        ci->moveCitizen(Actor::up); //move
                        return;
                    }
                }
                if (!checkBlock(ci, Actor::right, 2)) //else try to move in other direction
                {
                    ci->moveCitizen(Actor::right); //move
                    return;
                }
            }
            else if (closestX < x && closestY > y) //penelope is in upper left quadrant to  citizen
            {
                int r = randInt(0, 1); //choose a random directoin
                if (r == 0)
                {
                    if (!checkBlock(ci, Actor::up, 2)) //see if can move in that direction
                    {
                        ci->moveCitizen(Actor::up); //move
                        return;
                    }
                }
                if (!checkBlock(ci, Actor::left, 2)) //else try to move in other direction
                {
                    ci->moveCitizen(Actor::left); //move
                    return;
                }
            }
            else if (closestX > x && closestY < y) //penelope is in lower right quadrant to citizen
            {
                int r = randInt(0, 1); //choose a random directoin
                if (r == 0)
                {
                    if (!checkBlock(ci, Actor::down, 2)) //see if can move in that direction
                    {
                        ci->moveCitizen(Actor::down); //move
                        return;
                    }
                }
                if (!checkBlock(ci, Actor::right, 2)) //else try to move in other direction
                {
                    ci->moveCitizen(Actor::right); //move
                    return;
                }
            }
            else if (closestX < x && closestY < y) //penelope is in lower left quadrant to citizen
            {
                int r = randInt(0, 1); //choose a random directoin
                if (r == 0)
                {
                    if (!checkBlock(ci, Actor::down, 2)) //see if can move in that direction
                    {
                        ci->moveCitizen(Actor::down); //move
                        return;
                    }
                }
                if (!checkBlock(ci, Actor::left, 2)) //else try to move in other direction
                {
                    ci->moveCitizen(Actor::left); //move
                    return;
                }
            }
        }
    }
    if (dist_z <= 6400)
    {
        int near_up = 1<<15; //closest distance moving up, initially set to big number
        int near_down = 1<<15; //closest distance moving down
        int near_right = 1<<15; //closest distance moving right
        int near_left = 1<<15; //closest distance moving left
        if (!checkBlock(ci, Actor::up, 2)) //see if the citizen can move up away from the zombie
        {
            list<Actor*>::iterator it = m_actors.begin(); //iterate through each of the actors
            while (it != m_actors.end())
            {
                if ((*it)->isZombie()) //if the actor is a zombie
                {
                    if (near_up > findEuclidean(x, y+2, (*it)->getX(), (*it)->getY())) //if the distance between this human and the zombie is lowest than the current min, set the min to this distances
                    {
                        near_up = findEuclidean(x, y+2, (*it)->getX(), (*it)->getY());
                    }
                }
                it++;
            }
        }
        if (!checkBlock(ci, Actor::right, 2)) //see if the citizen can move right away from the zombie
        {
            list<Actor*>::iterator it = m_actors.begin(); //iterate through each of the actors
            while (it != m_actors.end())
            {
                if ((*it)->isZombie()) //if the actor is a zombie
                {
                    if (near_right > findEuclidean(x+2, y, (*it)->getX(), (*it)->getY())) //if the distance between this human and the zombie is lowest than the current min, set the min to this distances
                    {
                        near_right = findEuclidean(x+2, y, (*it)->getX(), (*it)->getY());
                    }
                }
                it++;
            }
        }
        if (!checkBlock(ci, Actor::down, 2)) //see if the citizen can move down away from the zombie
        {
            list<Actor*>::iterator it = m_actors.begin(); //iterate through each of the actors
            while (it != m_actors.end())
            {
                if ((*it)->isZombie()) //if the actor is a zombie
                {
                    if (near_down > findEuclidean(x, y-2, (*it)->getX(), (*it)->getY())) //if the distance between this human and the zombie is lowest than the current min, set the min to this distances
                    {
                        near_down = findEuclidean(x, y-2, (*it)->getX(), (*it)->getY());
                    }
                }
                it++;
            }
        }
        if (!checkBlock(ci, Actor::left, 2)) //see if the citizen can move left away from the zombie
        {
            list<Actor*>::iterator it = m_actors.begin(); //iterate through each of the actors
            while (it != m_actors.end())
            {
                if ((*it)->isZombie()) //if the actor is a zombie
                {
                    if (near_left > findEuclidean(x-2, y, (*it)->getX(), (*it)->getY())) //if the distance between this human and the zombie is lowest than the current min, set the min to this distances
                    {
                        near_left = findEuclidean(x-2, y, (*it)->getX(), (*it)->getY());
                    }
                }
                it++;
            }
        }
        //if cant move in a direction, set nearest distance as closest zombie
        if (near_up == 1<<15) near_up = dist_z;
        if (near_right == 1<<15) near_right = dist_z;
        if (near_down == 1<<15) near_down = dist_z;
        if (near_left == 1<<15) near_left = dist_z;
        
        if (near_up <= dist_z && near_down <= dist_z && near_left <= dist_z && near_right <= dist_z)
        {
            return; //if moving in any direction isn't good, return
        }
        
        if (near_up > near_right && near_up > near_left && near_up > near_down) //if up moves furthest away
        {
            ci->moveCitizen(Actor::up); //then move the citizen up
            return;
        }
        else if (near_right > near_up && near_right > near_left && near_right > near_down) //if right moves furthest away
        {
            ci->moveCitizen(Actor::right); //then move the citizen right
            return;
        }
        else if (near_down > near_right && near_down > near_left && near_down > near_up) //if down moves furthest away
        {
            ci->moveCitizen(Actor::down); //then move the citizen down
            return;
        }
        else if (near_left > near_right && near_left > near_up && near_left > near_down) //if left moves furthest away
        {
            ci->moveCitizen(Actor::left); //then move the citizen left
            return;
        }
    }
    return; //nothing to do
}
//ALL THE PLACES------------------------------------------------------------------------------------------------------
void StudentWorld::placeFlame(int x, int y)
{
    m_actors.push_back(new Flame(this, x, y));
}
void StudentWorld::placePit(int x, int y)
{
    m_actors.push_back(new Pit(this, x, y));
}
void StudentWorld::placeVaccineGoodie(int x, int y)
{
    m_actors.push_back(new VaccineGoodie(this, x, y));
}
void StudentWorld::placeLandmine(int x, int y)
{
    m_actors.push_back(new Landmine(this, x, y));
}
void StudentWorld::placeDumbZombie(int x, int y)
{
    m_actors.push_back(new DumbZombie(this, x, y));
}
void StudentWorld::placeSmartZombie(int x, int y)
{
    m_actors.push_back(new SmartZombie(this, x, y));
}
//ALL THE INCREASES------------------------------------------------------------------------------------------------------
void StudentWorld::increaseVaccine()
{
    m_penelope->increaseVaccines(1);
}
void StudentWorld::increaseFlamethrower()
{
    m_penelope->increaseFlamethrowers(5);
}
void StudentWorld::increaseLandmine()
{
    m_penelope->increaseLandmines(2);
}
