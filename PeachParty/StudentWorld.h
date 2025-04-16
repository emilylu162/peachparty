#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Board.h"
#include <string>
#include <vector>

class Actor;
class Player;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    Player* getPeach(){ return m_p;}
    Player* getYoshi(){ return m_y;}
    Board* getBoard(){ return m_b; }
    void setActorDead(int x, int y);
    void setBank(int num){ central_bank += num;}
    int getBank(){ return central_bank;}
    void addActor(Actor* a){ m_actors.push_back(a);}
    void findimpactableActor(int x, int y);
    

private:
    Board* m_b;
    int central_bank = 0; //creates a bank in the World
    Player* m_p;
    Player* m_y;
    std::vector<Actor*> m_actors;
};

#endif // STUDENTWORLD_H_
