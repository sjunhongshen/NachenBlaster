#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <string>
#include <vector>

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    bool targetAtNachenBlaster(std::string user, double x, double y, double r, int pts);
    //check if the position can collide with the NachenBlaster and decrease its health by pts
    bool targetAtAlien(double x, double y, double r, int pts);
    //check if the position can collide with the alien and decrease health by pts
    void decreaseShipHealth(int pts);
    bool closeToBlaster(double x, double y) const;
    //check if the position overlaps with Blaster according to the formula
    void destroyAlien();
    void getRepaired(); //increase Blaster's health
    void getTorpedoe(); //increase torpedoe points
    void createCabbage(double startX, double startY, Actor* owner); //introduce a cabbage at the location
    void createTurnip(double startX, double startY, Actor* owner);  //introduce a turnip at the location
    void createTorpedoe(double startX, double startY, Actor* owner);//introduce a torpedoe at the location
    void createRepairGoodie(double startX, double startY);          //introduce a repair goodie at the location
    void createTorpedoeGoodie(double startX, double startY);        //introduce a torpedoe goodie at the location
    void createExtraLifeGoodie(double startX, double startY);       //introduce a life goodie at the locate
    void createExplosion(double startX, double startY);             //introduce an explosion at the location
    ~StudentWorld();
private:
    void introduceStar();
    void introduceAlien();
    bool overlap(double x1, double y1, double r1, double x2, double y2, double r2);
    bool completeLevel();
    void removeDead();
    void updateText();
    int destroyed;
    int needDestroy;
    int maxShips;
    int curNumShips;
    std::vector<Actor*> m_actors;
    NachenBlaster* m_blaster;
};

#endif // STUDENTWORLD_H_
