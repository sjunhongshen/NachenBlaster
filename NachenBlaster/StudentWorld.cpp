#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cmath>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

StudentWorld::StudentWorld(string assetDir)
: GameWorld(assetDir)
{
    m_blaster = nullptr;
    destroyed = 0;
    needDestroy = 0;
    maxShips = 0;
    curNumShips = 0;
}

int StudentWorld::init()
{
    m_blaster = new NachenBlaster(this);        //initialize a NachenBlaster
    for(int i = 0; i < 30; i++)                 //initialize 30 random stars
    {
        double s_x = randInt(0, VIEW_WIDTH - 1);
        double s_y = randInt(0, VIEW_HEIGHT - 1);
        double s_size = static_cast<double>(randInt(5, 50)) / 100;
        Star* s = new Star(s_x, s_y, s_size, this);
        m_actors.push_back(s);
    }
    curNumShips = 0;
    destroyed = 0;
    needDestroy = 6 + 4 * getLevel() - destroyed;
    maxShips = 4 + 0.5 * getLevel();
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    introduceStar();        //introduce stars
    introduceAlien();       //introduce aliens
    if(m_blaster->isAlive())        //let NachenBlaster do something if it is alive
        m_blaster->doSomething();
    if(!m_blaster->isAlive())
        return GWSTATUS_PLAYER_DIED;
    if(completeLevel())
        return GWSTATUS_FINISHED_LEVEL;
    for(int i = 0; i < m_actors.size(); i++)    //let each actor do something if it is alive
    {
        if(m_actors[i]->isAlive())
        {
            m_actors[i]->doSomething();
            if(!m_blaster->isAlive())
                return GWSTATUS_PLAYER_DIED;
            if(completeLevel())
                return GWSTATUS_FINISHED_LEVEL;
        }
    }
    removeDead();       //remove the dead actors from the screen
    updateText();       //update text on the screen
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()    //delete all actors
{
    delete m_blaster;
    m_blaster = nullptr;
    if(!m_actors.empty())
    {
        std::vector<Actor*>::iterator p = m_actors.begin();
        while(p != m_actors.end())
        {
            delete *p;
            p = m_actors.erase(p);
        }
        m_actors.clear();
    }
}

bool StudentWorld::targetAtNachenBlaster(string user, double x, double y, double r, int pts)
{
    if(overlap(x, y, r, m_blaster->getX(), m_blaster->getY(), m_blaster->getRadius()))
    //if the specified position is close enough to NachenBlaster, a collision happens
    {
        m_blaster->decreaseHealth(pts);     //decrease health point as specified
        if(m_blaster->getHealth() <= 0)     //check NachenBlaster's state
        {
            m_blaster->setDead();
            decLives();
        }
        else if(user == "PROJECTILE")       //if the colliding object is projectile, play this sound effect
            playSound(SOUND_BLAST);
        else if(user == "ALIEN")            //if the colliding object is alien, play this sound effect
            playSound(SOUND_DEATH);
        else if(user == "GOODIE")           //if the colliding object is goodie, play this sound effect
            playSound(SOUND_GOODIE);
        return true;
    }
    return false;
}

bool StudentWorld::targetAtAlien(double x, double y, double r, int pts)
{
    for(int i = 0; i < m_actors.size(); i++)        //check each actor
    {
        if(m_actors[i]->isAlien())                  //make sure the actor is an alien
        {
            if(overlap(x, y, r, m_actors[i]->getX(), m_actors[i]->getY(), m_actors[i]->getRadius()))
            //if the specified position is close enough to the alien, a collision happens
            {
                static_cast<Alien*>(m_actors[i])->decreaseHealth(pts);      //decrease health as specified
                if(static_cast<Alien*>(m_actors[i])->getHealth() <= 0)
                //if the alien is health drops below 0 because of the collision, play this sound effect, set its state to dead, inform the StudentWorld, introduce an explosion and increase score
                {
                    playSound(SOUND_DEATH);
                    m_actors[i]->setDead();
                    needDestroy--;
                    destroyed++;
                    createExplosion(m_actors[i]->getX(), m_actors[i]->getY());
                    increaseScore(static_cast<Alien*>(m_actors[i])->returnScore());
                    if(static_cast<Alien*>(m_actors[i])->isSmoregon())
                    //if the alien is Smoregon, there is a chance it will drop certain goodie
                    {
                        int r1 = randInt(1, 3);
                        if(r1 == 1)
                        {
                            int r2 = randInt(1, 2);
                            switch(r2)
                            {
                                case 1: createRepairGoodie(m_actors[i]->getX(), m_actors[i]->getY()); break;
                                case 2: createTorpedoeGoodie(m_actors[i]->getX(), m_actors[i]->getY()); break;
                            }
                        }
                    }
                    else if(static_cast<Alien*>(m_actors[i])->isSnagglegon())
                    //if the alien is Snagglegon, there is a chance it will drop certain goodie
                    {
                        int r = randInt(1, 6);
                        if(r == 1)
                            createExtraLifeGoodie(m_actors[i]->getX(), m_actors[i]->getY());
                    }
                }
                else playSound(SOUND_BLAST);
                return true;
            }
        }
    }
    return false;
}

bool StudentWorld::closeToBlaster(double x, double y) const   //check overlap according to the eucilide formula
{
    return (m_blaster->getX() < x && m_blaster->getY() - y <= 4 && m_blaster->getY() - y >= -4);
}

void StudentWorld::createCabbage(double startX, double startY, Actor* owner)
//introduce a cabbage with the sound effect
{
    m_actors.push_back(new Cabbage(startX, startY, owner));
    playSound(SOUND_PLAYER_SHOOT);
}

void StudentWorld::createTorpedoe(double startX, double startY, Actor* owner)
//introduce a tropedoe with the sound effect
{
    m_actors.push_back(new Torpedoe(startX, startY, owner));
    playSound(SOUND_TORPEDO);
}

void StudentWorld::createTurnip(double startX, double startY, Actor* owner)
//introduce a turnip with the sound effect
{
    m_actors.push_back(new Turnip(startX, startY, owner));
    playSound(SOUND_ALIEN_SHOOT);
}

void StudentWorld::createRepairGoodie(double startX, double startY)
//introduce a repair goodie with the sound effect
{
    m_actors.push_back(new RepairLifeGoodie(startX, startY, this));
}

void StudentWorld::createExtraLifeGoodie(double startX, double startY)
//introduce an extra life goodie with the sound effect
{
    m_actors.push_back(new ExtraLifeGoodie(startX, startY, this));
}

void StudentWorld::createTorpedoeGoodie(double startX, double startY)
//introduce a torpedoe goodie with the sound effect
{
    m_actors.push_back(new TorpedoeGoodie(startX, startY, this));
}

void StudentWorld::createExplosion(double startX, double startY)
//introduce an explosion with the sound effect
{
    m_actors.push_back(new Explosion(startX, startY, this));
}

void StudentWorld::destroyAlien()
{
    destroyed++;
    needDestroy--;
}

void StudentWorld::getRepaired()
{
    if(m_blaster->getHealth() <= 40)
        m_blaster->increaseHealth(10);
    else m_blaster->increaseHealth(50 - m_blaster->getHealth());
}

void StudentWorld::getTorpedoe()
{
    m_blaster->increaseTorpedoe();
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

/////////////////////////////////
void StudentWorld::introduceStar()
{
    int r = randInt(1, 15);
    if(r == 1)
    {
        double s_y = randInt(0, VIEW_HEIGHT - 1);
        double s_size = static_cast<double>(randInt(5, 50)) / 100;
        Star* s = new Star(VIEW_WIDTH - 1, s_y, s_size, this);
        m_actors.push_back(s);
    }
}

void StudentWorld::introduceAlien()
{
    int minOfNeedDestroyAndMaxShips;
    if(needDestroy <= maxShips)
        minOfNeedDestroyAndMaxShips = needDestroy;
    else minOfNeedDestroyAndMaxShips = maxShips;
    if(curNumShips < minOfNeedDestroyAndMaxShips)
    {
        int s1 = 60;
        int s2 = 20 + 5 * getLevel();
        int s3 = 5 + 10 * getLevel();
        int s = s1 + s2 + s3;
        int r = randInt(1, s);
        if(r >= 1 && r <= s1)
        {
            int a_y = randInt(0, VIEW_HEIGHT - 1);
            m_actors.push_back(new Smallgon(VIEW_WIDTH - 1, a_y, this));
        }
        else if(r > s1 && r <= s1 + s2)
        {
            int a_y = randInt(0, VIEW_HEIGHT - 1);
            m_actors.push_back(new Smoregon(VIEW_WIDTH - 1, a_y, this));
        }
        else
        {
            int a_y = randInt(0, VIEW_HEIGHT - 1);
            m_actors.push_back(new Snagglegon(VIEW_WIDTH - 1, a_y, this));
        }
        curNumShips++;
    }
}

bool StudentWorld::overlap(double x1, double y1, double r1, double x2, double y2, double r2)
{
    double dis = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
    return dis < (0.75 * (r1 + r2));
}

bool StudentWorld::completeLevel()
{
    if(needDestroy == 0)
    {
        playSound(SOUND_FINISHED_LEVEL);
        return true;
    }
    else return false;
}

void StudentWorld::removeDead()
{
    std::vector<Actor*>::iterator p;
    for(p = m_actors.begin(); p != m_actors.end(); p++)
    {
        if(!((*p)->isAlive()))
        {
            if((*p)->isAlien())
                curNumShips--;
            delete *p;
            p = m_actors.erase(p);
            p--;
        }
    }
}

void StudentWorld::updateText()
{
    ostringstream text;
    text.setf(ios::fixed);
    text.precision(0);
    text.fill(' ');
    text << "Lives: " << getLives() << setw(10) << "Health: " << m_blaster->getHealth() * 2 << "%"
    << setw(9) << "Score: " << getScore() << setw(9) << "Level: " << getLevel() << setw(12)
    << "Cabbages: " << m_blaster->getCabbage() * 100 / 30 << "%" << setw(13) << "Torpedoes: "
    << m_blaster->getTorpedoe();
    setGameStatText(text.str());
}

