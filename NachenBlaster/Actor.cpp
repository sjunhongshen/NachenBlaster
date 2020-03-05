#include "Actor.h"
#include "StudentWorld.h"

//////////////ACTOR//////////////////
Actor::Actor(int imageID, double startX, double startY, int dir, double size, int depth, StudentWorld* sw)
:GraphObject(imageID, startX, startY, dir, size, depth)
{
    m_alive = true;
    m_world = sw;
}

bool Actor::isAlien() const     //return true if the Actor is an alien
{
    return false;
}

bool Actor::isAlive() const     //return true if the Actor is alive
{
    return m_alive;
}

void Actor::setDead()       //set the Actor's state to dead
{
    m_alive = false;
}

StudentWorld* Actor::getWorld() const       //return a pointer to the StudentWorld object
{
    return m_world;
}

bool Actor::offScreen()     //check whether the Actor is off-screen and if so, set its state to dead
{
    if(getX() < 0 || getX() >= VIEW_WIDTH || getY() < 0 || getY() >= VIEW_HEIGHT)
    {
        setDead();
        return true;
    }
    return false;
}

///////////STAR///////////
Star::Star(double startX, double startY, double size, StudentWorld* sw)
: Actor(IID_STAR, startX, startY, 0, size, 3, sw)
{
}

void Star::doSomething()
{
    if(!isAlive())      //check alive
        return;
    if(getX() <= 0)     //check off-screen
    {
        setDead();
        return;
    }
    moveTo(getX() - 1, getY());     //move the star to the left
}

//////////////////SPACESHIP///////////
SpaceShip::SpaceShip(int imageID, double startX, double startY, int dir, double size, int depth, int hpt, StudentWorld* sw)
: Actor(imageID, startX, startY, dir, size, depth, sw)
{
    hitPoints = hpt;
}

int SpaceShip::getHealth() const        //return the health point of the SpaceShip
{
    return hitPoints;
}

void SpaceShip::increaseHealth(int pts) //increase health point as indicated
{
    hitPoints += pts;
}

void SpaceShip::decreaseHealth(int pts) //decrease health point as indicated
{
    hitPoints -= pts;
}

////////////NACHENBLASTER////////////
NachenBlaster::NachenBlaster(StudentWorld* sw)
: SpaceShip(IID_NACHENBLASTER, 0, 128, 0, 1.0, 0, 50, sw)
{
    cabbagePoints = 30;
    torpedoePoints = 0;
}

void NachenBlaster::doSomething()
{
    if(!isAlive())      //check alive
        return;
    int ch;
    if(getWorld()->getKey(ch))  //read user input
    {
        switch(ch)
        {
            case KEY_PRESS_SPACE:   //fire cabbage if press the key space
                if(cabbagePoints >= 5)
                {
                    getWorld()->createCabbage(getX() + 12, getY(), this);
                    cabbagePoints -= 5;
                }
                break;
            case KEY_PRESS_TAB:     //fire torpedoe if allowed
                if(torpedoePoints > 0)
                {
                    getWorld()->createTorpedoe(getX() + 12, getY(), this);
                    torpedoePoints--;
                }
                break;
            case KEY_PRESS_UP:      //move up/down/left/right
                if(getY() + 6 < VIEW_HEIGHT)
                    moveTo(getX(), getY() + 6);
                break;
            case KEY_PRESS_DOWN:
                if(getY() - 6 >= 0)
                    moveTo(getX(), getY() - 6);
                break;
            case KEY_PRESS_LEFT:
                if(getX() - 6 >= 0)
                    moveTo(getX() - 6, getY());
                break;
            case KEY_PRESS_RIGHT:
                if(getX() + 6 < VIEW_WIDTH)
                    moveTo(getX() + 6, getY());
                break;
        }
    }
    if(getHealth() <= 0)    //set NachenBlaster's state to dead if its health drops below 0
    {
        setDead();
        getWorld()->decLives();
        return;
    }
    if(cabbagePoints < 30)  //increase cabbages
        cabbagePoints++;
}

int NachenBlaster::getCabbage() const   //return number of cabbages
{
    return cabbagePoints;
}

int NachenBlaster::getTorpedoe() const      //return number of torpedoes
{
    return torpedoePoints;
}

void NachenBlaster::increaseTorpedoe()      //increase the number of tropedoes by 5
{
    torpedoePoints += 5;
}

///////////////EXPLOSION//////////////
Explosion::Explosion(double startX, double startY, StudentWorld* sw)
: Actor(IID_EXPLOSION, startX, startY, 0, 1, 0, sw)
{
    tickCount = 4;
}

void Explosion::doSomething()
{
    if(!isAlive())      //check alive
        return;
    if(tickCount == 0)  //set the state to dead after 4 ticks
    {
        setDead();
        return;
    }
    setSize(1.5 * getSize());   //within 4 ticks, grow as idicated
    tickCount--;
}

/////////////////PROJECTILE///////////
Projectile::Projectile(int imageID, double startX, double startY, Actor* owner)
: Actor(imageID, startX, startY, 0, 0.5, 1, owner->getWorld())
{
    m_owner = owner;
}

void Projectile::doSomething()
{
    if(!isAlive())      //check alive
        return;
    if(offScreen())     //check off-screen
        return;
    collideAndMove();   //let the projectile collide with something and make a movement
}

///////////////CABBAGE//////////
Cabbage::Cabbage(double startX, double startY, Actor* owner)
: Projectile(IID_CABBAGE, startX, startY, owner)
{
}

void Cabbage::collideAndMove()
{
    if(getWorld()->targetAtAlien(getX(), getY(), getRadius(), 2))
    //if the cabbage collides with an alien, set its state to dead and return
    {
        setDead();
        return;
    }
    moveTo(getX() + 8, getY());             //move as requried
    setDirection(getDirection() + 20);      //change direction as required
    if(getWorld()->targetAtAlien(getX(), getY(), getRadius(), 2))
    //if the cabbage collides with an alien, set its state to dead and return
    {
        setDead();
        return;
    }
}

/////////////////TURNIP////////////
Turnip::Turnip(double startX, double startY, Actor* owner)
: Projectile(IID_TURNIP, startX, startY, owner)
{
}

void Turnip::collideAndMove()
{
    if(getWorld()->targetAtNachenBlaster("PROJECTILE", getX(), getY(), getRadius(), 2))
    //if the turnip collides with the NachenBlaster, set its state to dead and return
    {
        setDead();
        return;
    }
    moveTo(getX() - 6, getY());             //move as required
    setDirection(getDirection() + 20);      //change direction as required
    if(getWorld()->targetAtNachenBlaster("PROJECTILE", getX(), getY(), getRadius(), 2))
    //if the turnip collides with the NachenBlaster, set its state to dead and return
    {
        setDead();
        return;
    }
}

////////////////TORPEDOE//////////////
Torpedoe::Torpedoe(double startX, double startY, Actor* owner)
: Projectile(IID_TORPEDO, startX, startY, owner)
{
    if(owner->isAlien())
        setDirection(180);
}

void Torpedoe::collideAndMove()
{
    if(getDirection() == 180)   //if the torpedoe is fired by an alien
    {
        if(getWorld()->targetAtNachenBlaster("PROJECTILE", getX(), getY(), getRadius(), 8))
        //if the torpedoe collides with the NachenBlaster, set its state to dead and return
        {
            setDead();
            return;
        }
        moveTo(getX() - 8, getY());     //move as required
        if(getWorld()->targetAtNachenBlaster("PROJECTILE", getX(), getY(), getRadius(), 8))
        //if the torpedoe collides with the NachenBlaster, set its state to dead and return
        {
            setDead();
            return;
        }
    }
    else        //if the torpedoe is fired by NachenBlaster
    {
        if(getWorld()->targetAtAlien(getX(), getY(), getRadius(), 8))
        //if the torpedoe collides with an alien, set its state to dead and return
        {
            setDead();
            return;
        }
        moveTo(getX() + 8, getY());     //move as required
        if(getWorld()->targetAtAlien(getX(), getY(), getRadius(), 8))
        //if the torpedoe collides with an alien, set its state to dead and return
        {
            setDead();
            return;
        }
    }
}

//////////////GOODIE///////////
Goodie::Goodie(int imageID, double startX, double startY, StudentWorld* sw)
: Actor(imageID, startX, startY, 0, 0.5, 1, sw)
{
}

bool Goodie::collideWithBlaster()
{
    if(offScreen())     //check off-screen
        return false;
    if(getWorld()->targetAtNachenBlaster("GOODIE", getX(), getY(), getRadius(), 0))
    //if the goodie collides with the NachenBlaster, increase score by 100, set its state to dead and return true
    {
        getWorld()->increaseScore(100);
        setDead();
        return true;
    }
    return false;
}

void Goodie::doSomething()
{
    if(!isAlive())  //check alive
        return;
    if(offScreen()) //check off-screen
        return;
    if(collideWithBlaster())    //check collision
    {
        giveReward();           //give specific reward
        return;
    }
    moveTo(getX() - 0.75, getY() - 0.75);   //move as required
    if(collideWithBlaster())    //check collision again
    {
        giveReward();
        return;
    }
}

////////////////EXTRALIFEGOOIE/////////////////
ExtraLifeGoodie::ExtraLifeGoodie(double startX, double startY, StudentWorld* sw)
: Goodie(IID_LIFE_GOODIE, startX, startY, sw)
{
}

void ExtraLifeGoodie::giveReward()  //increase life by 1
{
    getWorld()->incLives();
}

///////////////REPAIRLIFEGOODIE////////////
RepairLifeGoodie::RepairLifeGoodie(double startX, double startY, StudentWorld* sw)
: Goodie(IID_REPAIR_GOODIE, startX, startY, sw)
{
}

void RepairLifeGoodie::giveReward()     //increase health point
{
    getWorld()->getRepaired();
}

/////////////TORPEDOEGOODIE//////
TorpedoeGoodie::TorpedoeGoodie(double startX, double startY, StudentWorld* sw)
: Goodie(IID_TORPEDO_GOODIE, startX, startY, sw)
{
}

void TorpedoeGoodie::giveReward()   //increase torpedoe point
{
    getWorld()->getTorpedoe();
}

////////////////ALIEN//////////////
Alien::Alien(int imageID, double startX, double startY, int hpts, StudentWorld* sw)
: SpaceShip(imageID, startX, startY, 0, 1.5, 1, hpts, sw)
{
}

double Alien::getTravelSpeed() const        //return travel speed
{
    return travelSpeed;
}

int Alien::getTravelDirection() const       //return travel direction
{
    return travelDirection;
}

void Alien::setTravelSpeed(double v)        //set travel speed as indicated
{
    travelSpeed = v;
}

void Alien::setTravelDirection(int d)       //set travel direction as indicated
{
    travelDirection = d;
}

bool Alien::checkCollideWithBlaster(int hpt, int pt)
{
    if(getWorld()->targetAtNachenBlaster("ALIEN", getX(), getY(), getRadius(), hpt))
    //if the alien collides with the NachenBlaster, set its state to dead, inform the StudentWorld, increase score as indicated and introduce an explosion
    {
        setDead();
        getWorld()->destroyAlien();
        getWorld()->increaseScore(pt);
        getWorld()->createExplosion(getX(), getY());
        return true;
    }
    return false;
}

void Alien::move()      //move as required
{
    if(getTravelDirection() == 135)
        moveTo(getX() - getTravelSpeed(), getY() + getTravelSpeed());
    if(getTravelDirection() == 180)
        moveTo(getX() - getTravelSpeed(), getY());
    if(getTravelDirection() == 225)
        moveTo(getX() - getTravelSpeed(), getY() - getTravelSpeed());
}

void Alien::doSomething()
{
    if(!isAlive())      //check alive
        return;
    if(getX() < 0)      //check off-screen
    {
        setDead();
        return;
    }
    if(collideWithNachenBlaster())  //check if collide with the NachenBlaster
        return;
    changePlan();
    if(fireSomething())             //fire something
        return;
    move();
    if(collideWithNachenBlaster())  //check if collide with the NachenBlaster agiain
        return;
}

bool Alien::isAlien() const         //return true because it is an alien
{
    return true;
}

bool Alien::isSmoregon() const      //return false if it is not a smoregon
{
    return false;
}

bool Alien::isSnagglegon() const    //return false if it is not a snagglegon
{
    return false;
}

//////////////ALIENWITHFLIGHTPLAN///////////////
AlienWithFlightPlan::AlienWithFlightPlan(int imageID, double startX, double startY, StudentWorld* sw)
: Alien(imageID, startX, startY, 5 * (1 + (sw->getLevel() - 1) * 0.1), sw)
{
    flightPlanLength = 0;
    setTravelSpeed(2.0);
}

int AlienWithFlightPlan::getFlightPlanLength() const        //return flight plan length
{
    return flightPlanLength;
}

void AlienWithFlightPlan::setFlightPlanLength(int l)        //set flight plan length as indicated
{
    flightPlanLength = l;
}

bool AlienWithFlightPlan::fireSomething()
{
    if(getWorld()->closeToBlaster(getX(), getY()))
    //if the position satisfies the requirement, there is a chance that the alien will fire a turnip
    {
        int chance = randInt(1, (20 / getWorld()->getLevel()) + 5);
        if(chance == 1)
        {
            getWorld()->createTurnip(getX() - 14, getY(), this);
            return true;
        }
    }
    return false;
}

void AlienWithFlightPlan::changePlan()
{
    if(getFlightPlanLength() == 0 || getY() <= 0 || getY() >= VIEW_HEIGHT - 1)
    // if the flight plan or the current position satisfies requirement, change its direction and flight plan accordingly
    {
        if(getY() >= VIEW_HEIGHT - 1)
            setTravelDirection(225);
        else if(getY() <= 0)
            setTravelDirection(135);
        else if(getFlightPlanLength() == 0)
        {
            int r = randInt(1, 3);
            switch(r)
            {
                case 1: setTravelDirection(180); break;
                case 2: setTravelDirection(135); break;
                case 3: setTravelDirection(225); break;
            }
        }
        setFlightPlanLength(randInt(1, 32));
    }
}

void AlienWithFlightPlan::move()
{
    Alien::move();
    flightPlanLength--;     //in addition to making a movement, decrease flight plan length by 1
}

int AlienWithFlightPlan::returnScore() const        //Smoregon and Smallgon increase score by 250 when destroyed
{
    return 250;
}

bool AlienWithFlightPlan::collideWithNachenBlaster()
{
    if(checkCollideWithBlaster(5, 250))
    //if collision happens, decrease the health point by 5 and increase score by 250
        return true;
    return false;
}

///////////SMALLGON///////////
Smallgon::Smallgon(double startX, double startY, StudentWorld* sw)
: AlienWithFlightPlan(IID_SMALLGON, startX, startY, sw)
{
}

/////////////////SMOREGON//////////////
Smoregon::Smoregon(double startX, double startY, StudentWorld* sw)
: AlienWithFlightPlan(IID_SMOREGON, startX, startY, sw)
{
}

void Smoregon::changePlanAgain()
{
    if(getWorld()->closeToBlaster(getX(), getY()))
    //if position satisfies requirement, there is a certain chance that Smoregon will change flight plan again
    {
        int chance = randInt(1, (20 / getWorld()->getLevel()) + 5);
        if(chance == 1)
        {
            setTravelDirection(180);
            setFlightPlanLength(VIEW_WIDTH);
            setTravelSpeed(5);
        }
    }
}

bool Smoregon::fireSomething()
{
    if(AlienWithFlightPlan::fireSomething())
        return true;
    changePlanAgain();
    return false;
}

bool Smoregon::isSmoregon() const
{
    return true;
}

///////////////SNAGGLEGON////////////////
Snagglegon::Snagglegon(double startX, double startY, StudentWorld* sw)
: Alien(IID_SNAGGLEGON, startX, startY, 10 * (1 + (sw->getLevel() - 1) * 0.1), sw)
{
    setTravelDirection(225);
    setTravelSpeed(1.75);
}

bool Snagglegon::fireSomething()
{
    if(getWorld()->closeToBlaster(getX(), getY()))
    //if the position satisfies the requirement, there is a chance that the Snagglegon will fire a torpedoe
    {
        int chance = randInt(1, (15 / getWorld()->getLevel()) + 10);
        if(chance == 1)
        {
            getWorld()->createTorpedoe(getX() - 14, getY(), this);
            return true;
        }
    }
    return false;
}

bool Snagglegon::collideWithNachenBlaster()
{
    if(checkCollideWithBlaster(15, 1000))
    //if collision happens, decrease health by 15 and increase score by 1000, there is 1/6 chance that it will drop an extra life goodie
    {
        int r = randInt(1, 6);
        if(r == 1)
            getWorld()->createExtraLifeGoodie(getX(), getY());
        return true;
    }
    return false;
}

void Snagglegon::changePlan()
{
    if(getY() <= 0 || getY() >= VIEW_HEIGHT - 1)   //if position satisfies requirement, change plan accordingly
    {
        if(getY() >= VIEW_HEIGHT - 1)
            setTravelDirection(225);
        else if(getY() <= 0)
            setTravelDirection(135);
    }
}

int Snagglegon::returnScore() const                 //Snagglegon increases score by 1000 when destroyed
{
    return 1000;
}

bool Snagglegon::isSnagglegon() const
{
    return true;
}

