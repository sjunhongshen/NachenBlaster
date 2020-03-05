#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

//////////////ACTOR///////////////
class StudentWorld;

class Actor : public GraphObject
{
public:
    Actor(int imageID, double startX, double startY, int dir, double size, int depth, StudentWorld* sw);
    virtual void doSomething() = 0;     //let the actor do something
    virtual bool isAlien() const;       //return true if the object is an alien
    bool isAlive() const;               //return true if the object is alive
    StudentWorld* getWorld() const;     //return a pointer the to StudentWorld
    void setDead();                     //set the actor's state to dead
    bool offScreen();                   //set the state to dead if the actor is off-screen
    virtual ~Actor() {};
private:
    bool m_alive;
    StudentWorld* m_world;
};

//////////////STAR/////////////////
class Star : public Actor
{
public:
    Star(double startX, double startY, double size, StudentWorld* sw);
    virtual void doSomething();
};

/////////////////SPACESHIP///////////
class SpaceShip: public Actor
{
public:
    SpaceShip(int imageID, double startX, double startY, int dir, double size, int depth, int hpt, StudentWorld* sw);
    int getHealth() const;              //return health point
    void increaseHealth(int pts);       //increase health by pts
    void decreaseHealth(int pts);       //decrease health by pts
    virtual ~SpaceShip() {};
private:
    int hitPoints;
};

///////////////NACHENBLASTER///////////
class NachenBlaster : public SpaceShip
{
public:
    NachenBlaster(StudentWorld* sw);
    virtual void doSomething();
    int getCabbage() const;             //return number of cabbages
    int getTorpedoe() const;            //return number of torpedoes
    void increaseTorpedoe();            //increase torpedoe by 5
private:
    int cabbagePoints;
    int torpedoePoints;
};

/////////////ALIEN////////////
class Alien : public SpaceShip
{
public:
    Alien(int imageID, double startX, double startY, int hpts, StudentWorld* sw);
    virtual void doSomething();
    virtual bool isAlien() const;
    virtual bool isSmoregon() const;    //return true if the alien is a Smoregon
    virtual bool isSnagglegon() const;  //return true if the alien is a Snagglegon
    virtual int returnScore() const = 0;//return score
    virtual ~Alien() {};
protected:
    double getTravelSpeed() const;      //get travel speed
    int getTravelDirection() const;     //get travel direction
    void setTravelSpeed(double v);      //set travel speed
    void setTravelDirection(int d);     //set travl direction
    virtual void move();
    bool checkCollideWithBlaster(int hpt, int pt);  //decrease Blaster's health by hpt and increase score by pt
    virtual bool collideWithNachenBlaster() = 0;
    virtual bool fireSomething() = 0;
    virtual void changePlan() = 0;
private:
    double travelSpeed;
    int travelDirection;
};

////////////ALIENWITHFLIGHTPLAN/////////////////////
class AlienWithFlightPlan : public Alien
{
public:
    AlienWithFlightPlan(int imageID, double startX, double startY, StudentWorld* sw);
    virtual int returnScore() const;
    virtual ~AlienWithFlightPlan() {};
protected:
    int getFlightPlanLength() const;
    void setFlightPlanLength(int l);
    virtual bool collideWithNachenBlaster();
    virtual void changePlan();
    virtual bool fireSomething();
    virtual void move();
private:
    int flightPlanLength;
};

///////////SMALLGON/////////////
class Smallgon : public AlienWithFlightPlan
{
public:
    Smallgon(double startX, double startY, StudentWorld* sw);
};

///////////SMOREGON/////////////
class Smoregon : public AlienWithFlightPlan
{
public:
    Smoregon(double startX, double startY, StudentWorld* sw);
    virtual bool isSmoregon() const;
private:
    virtual bool fireSomething();
    void changePlanAgain();
};

////////////SNAGGLEGON//////////
class Snagglegon : public Alien
{
public:
    Snagglegon(double startX, double startY, StudentWorld* sw);
    virtual int returnScore() const;
    virtual bool isSnagglegon() const;
private:
    virtual void changePlan();
    virtual bool collideWithNachenBlaster();
    virtual bool fireSomething();
};

///////////PROJECTILE///////////
class Projectile : public Actor
{
public:
    Projectile(int imageID, double startX, double startY, Actor* owner);
    void doSomething();
protected:
    virtual void collideAndMove() = 0;
private:
    Actor* m_owner;
};

/////////CABBAGE////////////
class Cabbage : public Projectile
{
public:
    Cabbage(double startX, double startY, Actor* owner);
private:
    virtual void collideAndMove();
};

////////////TURNIP///////////
class Turnip : public Projectile
{
public:
    Turnip(double startX, double startY, Actor* owner);
private:
    virtual void collideAndMove();
};

///////////TORPEDOE///////////////
class Torpedoe : public Projectile
{
public:
    Torpedoe(double startX, double startY, Actor* owner);
private:
    virtual void collideAndMove();
};

////////////GOODIE//////////////
class Goodie : public Actor
{
public:
    Goodie(int imageID, double startX, double startY, StudentWorld* sw);
    virtual void doSomething();
protected:
    bool collideWithBlaster();
    virtual void giveReward() = 0;
};

////////////REPAIRLIFEGOODIE///////////
class RepairLifeGoodie : public Goodie
{
public:
    RepairLifeGoodie(double startX, double startY, StudentWorld* sw);
private:
    virtual void giveReward();
};

///////////EXTRALIFEGOODIE//////////
class ExtraLifeGoodie : public Goodie
{
public:
    ExtraLifeGoodie(double startX, double startY, StudentWorld* sw);
private:
    virtual void giveReward();
};

///////////TORPEDOEGOODIE///////////
class TorpedoeGoodie : public Goodie
{
public:
    TorpedoeGoodie(double startX, double startY, StudentWorld* sw);
private:
    virtual void giveReward();
};

///////////EXPLOSION//////////
class Explosion : public Actor
{
public:
    Explosion(double startX, double startY, StudentWorld* sw);
    virtual void doSomething();
private:
    int tickCount;
};

#endif // ACTOR_H_
