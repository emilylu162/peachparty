#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class Actor: public GraphObject{
public:
    Actor(StudentWorld* studentworld, int x, int y, int imageID, int startdir, bool removeable, int depth = 0, double size = 1.0, bool impact = false, bool vortex = false): GraphObject(imageID, x, y, startdir, depth, size), m_world(studentworld), remove(removeable), impactable(impact), impactedvortex(vortex){};
    
    virtual void doSomething() = 0;
    bool moveForward(int dir); //given a direction, returns if we can move forward in that direction
    void teleport();
    
    StudentWorld* getWorld(){ return m_world;}
    virtual void set_alive(bool a){ alive = a;}
    bool get_alive() const{ return alive;} //return's the alive status when we check for any dead actors in StudentWorld move()
    bool get_removeable() const{ return remove;}
    
    bool is_impactable() const{ return impactable;}
    bool getimpacted() const{ return impactedvortex;}
    void setimpacted(bool a){ impactedvortex = a;}
    
private:
    const bool impactable;
    bool impactedvortex;
    bool alive = true;
    const bool remove;
    //const bool vortex;
    StudentWorld* m_world;
};

class MovingActor: public Actor{
public:
    MovingActor(StudentWorld* studentworld, int x, int y, int image, int walkdir, bool removeable = false, int spritedir = 0, int depth = 0, double size = 1.0, bool impact = false): Actor(studentworld, x, y, image, spritedir, removeable, depth, size, impact), walk_direction(walkdir){};
    void findNewDirection();
    bool multiDirection();
    int getwalk() const{ return walk_direction;}
    void setwalk(int dir){ walk_direction = dir;}
private:
    int walk_direction;
};

class Vortex;

class Player: public MovingActor{
public:
    Player(StudentWorld* studentworld, int x, int y, int image, int player, int ticks_to_move = 0, int dir = 0, bool wait = true, int depth = 0, double size = 1.0, int walk = right, bool removeable = false):  MovingActor(studentworld, x, y, image, walk, removeable, dir, depth, size), m_wait(wait), m_ticks(ticks_to_move), playernumber(player) { };
    virtual void doSomething();
    
    void addCoins(int coin){ m_coins += coin;}
    void setCoins(int coin){ m_coins = coin;}
    int getCoins() const{ return m_coins;}
    void setRoll(int num){ die_roll = num;}
    int getRoll() const{ return die_roll;}
    int getStars() const{ return m_stars;}
    void addStars(int num){ m_stars += num;}
    void setStars(int num){ m_stars = num;}
    int getTicks() const{ return m_ticks;}; //inline??
    void setTicks(int tick){ m_ticks = tick;};
    bool wait() const{ return m_wait;};
    void setwait(bool wait){ m_wait = wait;};
    int getplayer() const{ return playernumber;};
    bool directional_square() const {return on_directional;}
    void setdirectional_square(bool a){ on_directional = a;}
    bool hasvortex() const{ return has_vortex;}
    void set_hasvortex(bool a){ has_vortex = a;}
    Vortex* getvortex(){ return m_vortex;}
    void setvortex(Vortex* a){ m_vortex = a;}
    
private:
    int die_roll = 0;
    bool m_wait; //if true, player is waiting to roll
    int m_ticks;
    int playernumber;
    int m_coins = 0;
    int m_stars = 0;
    bool on_directional = false;
    Vortex* m_vortex;
    bool has_vortex = false;
};



class Square: public Actor{
public:
    Square(StudentWorld* studentworld, int image, int x, int y, int dir = 0, int depth = 1, double size = 1.0, bool removeable = true): Actor(studentworld, x, y, image, dir, removeable, depth, size){ };
    bool Peachlanded(Player* a);
    bool Yoshilanded(Player* a);
    bool Peachpassed(Player* a);
    bool Yoshipassed(Player* a);
    void Peachreactivate(Player* a);
    void Yoshireactivate(Player* a);
    bool get_pactivate() const{ return p_activate;}
    void set_pactivate(bool a){ p_activate = a;}
    bool get_yactivate() const{ return y_activate;}
    void set_yactivate(bool a){ y_activate = a;}
    
private:
    bool p_activate = true; //activation for peach
    bool y_activate = true; //activation for yoshi
};

class BlueCoinSquare: public Square{
public:
    BlueCoinSquare(StudentWorld* studentworld, int image, int x, int y, int dir = 0, int depth = 1, double size = 1.0, bool removeable = true): Square(studentworld, image, x, y, dir, depth, size, removeable){};
    virtual void doSomething();
};

class RedCoinSquare: public Square{
public:
    RedCoinSquare(StudentWorld* studentworld, int image, int x, int y, int dir = 0, int depth = 1, double size = 1.0, bool removeable = true): Square(studentworld, image, x, y, dir, depth, size, removeable){};
    virtual void doSomething();
};

class StarSquare: public Square{
public:
    StarSquare(StudentWorld* studentworld, int image, int x, int y, int dir = 0, int depth = 1, double size = 1.0, bool removeable = true): Square(studentworld, image, x, y, dir, depth, size, removeable){};
    virtual void doSomething();
};

class DirectionalSquare: public Square{
public:
    DirectionalSquare(StudentWorld* studentworld, int image, int x, int y, int dir, int depth = 1, double size = 1.0, bool removeable = true): Square(studentworld, image, x, y, dir, depth, size, removeable), forcing_direction(dir){}; //check if this still works
    //int getForcing(){ return forcing_direction;}
    virtual void doSomething();
private:
    const int forcing_direction;
};

class BankSquare: public Square{
public:
    BankSquare(StudentWorld* studentworld, int image, int x, int y, int dir = 0, int depth = 1, double size = 1.0, bool removeable = true): Square(studentworld, image, x, y, dir, depth, size, removeable){};
    virtual void doSomething();
};

class EventSquare: public Square{
public:
    EventSquare(StudentWorld* studentworld, int image, int x, int y, int dir = 0, int depth = 1, double size = 1.0, bool removeable = true): Square(studentworld, image, x, y, dir, depth, size, removeable){};
    virtual void doSomething();
    
private:
    void swap(Player* one, Player* two);
};

class DroppingSquare: public Square{
public:
    DroppingSquare(StudentWorld* studentworld, int image, int x, int y, int dir = 0, int depth = 1, double size = 1.0, bool removeable = true): Square(studentworld, image, x, y, dir, depth, size, removeable){};
    virtual void doSomething();
};

class Vortex: public Actor{
public:
    Vortex(StudentWorld* studentworld, int x, int y, int image, int startdir, int dir = 0, int depth = 0, int size = 1.0, bool removeable = false): Actor(studentworld, x, y, image, dir, removeable, depth, size), m_direction(startdir){ };
    virtual void doSomething();
    bool getActive() const{ return m_active;}
    void setActive(bool a){ m_active = a;}
    int getDirection() const{ return m_direction;}
        
private:
    int m_direction;
    bool m_active = true;
};




class Baddie: public MovingActor{
public:
    Baddie(StudentWorld* studentworld, int x, int y, int image, int dir = 0, int depth = 0, double size = 1.0, int pause = 180, int travel = 0, bool walk = false, int walking = right, bool removeable = false, bool impact = true): MovingActor(studentworld, x, y, image, walking, removeable, dir, depth, size, impact), pause_counter(pause), travel_distance(travel), m_walk(walk){ };
    void vortexFunction();
    void impactedbyVortex();
    
    bool walk() const{ return m_walk;}
    void setwalkstate(bool a){ m_walk = a;}
    void addPause(int num){ pause_counter += num;}
    int getPause() const{ return pause_counter;}
    void setPause(int num){ pause_counter = num;}
    void setTravel(int num){ travel_distance = num;}
    int getTravel() const{ return travel_distance;}
    int getTicks() const{ return m_ticks;}
    void setTicks(int num){ m_ticks = num;}
    void addTicks(int num){ m_ticks += num;}
    
private:
    int pause_counter;
    int travel_distance;
    int m_ticks;
    bool m_walk;
};

class Bowser: public Baddie{
public:
    Bowser(StudentWorld* studentworld, int x, int y, int image, int dir = right, int depth = 0, double size = 1.0, int sprite = 0, int pause = 180, int travel = 0, bool walk = false, bool removeable = false): Baddie(studentworld, x, y, image, sprite, depth, size, pause, travel, walk, dir, removeable){};
    virtual void doSomething();
};

class Boo: public Baddie{
public:
    Boo(StudentWorld* studentworld, int x, int y, int image, int dir = right, int depth = 0, double size = 1.0, int sprite = 0, int pause = 180, int travel = 0, bool walk = false, bool removeable = false): Baddie(studentworld, x, y, image, sprite, depth, size, pause, travel, walk, dir, removeable){};
    virtual void doSomething();
    bool getactivated() const{ return activated;}
    void setactivated(bool a){ activated = a;}
    
private:
    bool activated = false;
};


#endif // ACTOR_H_


//Event Squares

//Boo, Bowser, Vortexes
