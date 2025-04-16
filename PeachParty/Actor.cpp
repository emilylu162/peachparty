#include "Actor.h"
#include "Board.h"
#include "StudentWorld.h"

void Actor::teleport(){
    int x = -1;
    int y = -1;
    while(getWorld()->getBoard()->getContentsOf(x, y) == Board::empty){
        x = randInt(0, BOARD_WIDTH-1);
        y = randInt(0, BOARD_HEIGHT-1);
    }
    moveTo(x*BOARD_WIDTH, y*BOARD_HEIGHT);
}

bool Actor::moveForward(int dir){
     switch (dir){
         case right:
             if(getWorld()->getBoard()->getContentsOf((getX()/BOARD_WIDTH) + 1, getY()/BOARD_HEIGHT) == Board::empty)
                 return false;
             break;
         case left:
             if(getX() % BOARD_WIDTH == 0){
                 if(getWorld()->getBoard()->getContentsOf((getX()/BOARD_WIDTH) - 1, getY()/BOARD_HEIGHT) == Board::empty)
                     return false;
            }
             else{
                 if(getWorld()->getBoard()->getContentsOf((getX()/BOARD_WIDTH), getY()/BOARD_HEIGHT) == Board::empty)
                     return false;
            }
             break;
         case up:
             if(getWorld()->getBoard()->getContentsOf(getX()/BOARD_WIDTH, (getY()/BOARD_HEIGHT) + 1) == Board::empty)
                 return false;
             break;
         case down:
             if(getY() % BOARD_HEIGHT == 0){
                 if(getWorld()->getBoard()->getContentsOf(getX()/BOARD_WIDTH, (getY()/BOARD_HEIGHT) - 1) == Board::empty)
                     return false;
            }
             else{
                 if(getWorld()->getBoard()->getContentsOf((getX()/BOARD_WIDTH), getY()/BOARD_HEIGHT) == Board::empty)
                     return false;
            }
             break;
     }
     return true;
 }

void MovingActor::findNewDirection(){
    if(getwalk() == right || getwalk() == left){ //if the walking direction is right or left
        if(moveForward(up) && moveForward(down))
            setwalk(up); //check if up and down are available and set walking to up
        else if(moveForward(up))
            setwalk(up); //if only up is available, set walking to up
        else if(moveForward(down))
            setwalk(down); //if only down is available, set walking to down
    }
    else if(getwalk() == up || getwalk() == down){ //if the walking direction is up or down
        if(moveForward(right) && moveForward(left))
            setwalk(right); //check if right and left are available and set walking to right
        else if(moveForward(right))
            setwalk(right); //if only right is available, set walking to right
        else if(moveForward(left))
            setwalk(left); //if only left is available, set walking to left
    }
    
    if(getwalk() == left)
        setDirection(180); //update direction the sprite is facing
    else setDirection(0); //BUG: in board 9, when going left, the avatar is still facing right
}

bool MovingActor::multiDirection(){
    int direction = 0;
    for(int i = 0; i < 4; i++){
        if(moveForward(i*90)) direction++;
    }
    if(direction >= 3) return true;
    return false;
}

void Player::doSomething(){ 
    if(wait()){ //if we're waiting to roll
        //TODO: invalid state due to being teleported
        if(getWorld()->getAction(getplayer()) == ACTION_ROLL){
            int temp = randInt(1, 10);
            setRoll(temp);
            setTicks(temp * 8);
            setwait(false); //walking state
        }
        else if(getWorld()->getAction(getplayer()) == ACTION_FIRE){
            //TODO: Introduce new Vortex in Avatar's current walk direction; still need to add a Vortex from Event Square
            if(hasvortex()){
                Vortex* a = new Vortex(getWorld(), getX(), getY(), IID_VORTEX, getwalk());
                getWorld()->addActor(a);
                getWorld()->playSound(SOUND_PLAYER_FIRE);
                set_hasvortex(false);
            }
        }
        else return;
    }
    
    if(!wait()){ //walking state
        if(multiDirection() && getTicks() % 8 == 0 && !directional_square()){
            switch (getWorld()->getAction(getplayer())) {
                case ACTION_LEFT:
                    setwalk(left);
                    setDirection(180);
                    break;
                case ACTION_RIGHT:
                    setwalk(right);
                    setDirection(0);
                    break;
                case ACTION_UP:
                    setwalk(up);
                    setDirection(0);
                    break;
                case ACTION_DOWN:
                    setwalk(down);
                    setDirection(0);
                    break;
                //TODO: case ACTION_FIRE:
                default:
                    return;
            }
        }
        else if(directional_square()) setdirectional_square(false);
        else if(!moveForward(getwalk())) findNewDirection();
    }
    moveAtAngle(getwalk(), 2);
    if(getTicks() % 8 == 0) setRoll(getRoll()-1); //8 ticks = 1 roll
    setTicks(getTicks()-1);
    if(getTicks() == 0) setwait(true); //if no more ticks left to move, wait to roll
}

bool Square::Peachlanded(Player* a){
    if(getX() == a->getX() && getY() == a->getY() && a->wait() && get_pactivate()) return true;
    return false;
}

bool Square::Yoshilanded(Player* a){
    if(getX() == a->getX() && getY() == a->getY() && a->wait() && get_yactivate()) return true;
    return false;
}

bool Square::Peachpassed(Player* a){
    if(getX() == a->getX() && getY() == a->getY() && !a->wait() && get_pactivate()) return true;
    return false;
}

bool Square::Yoshipassed(Player* a){
    if(getX() == a->getX() && getY() == a->getY() && !a->wait() && get_yactivate()) return true;
    return false;
}

void Square::Peachreactivate(Player* a){
    if(getX() != a->getX() && getY() != a->getY())
        set_pactivate(true);
}

void Square::Yoshireactivate(Player* a){
    if(getX() != a->getX() && getY() != a->getY())
        set_yactivate(true);
}

void BlueCoinSquare::doSomething(){
    if(!get_alive()) return; //if square died, return
    
    //for Peach:
    Peachreactivate(getWorld()->getPeach()); //if there is not a Peach on the square, it reactivates
    if(Peachlanded(getWorld()->getPeach())){ //only activates if Peach is waiting to roll (landed) on the square
            getWorld()->getPeach()->addCoins(3);
            getWorld()->playSound(SOUND_GIVE_COIN);
            set_pactivate(false); //makes sure we don't activate it again unless Peach returns to the same square again
        
    }
    
    //for Yoshi
    Yoshireactivate(getWorld()->getYoshi());
    if(Yoshilanded(getWorld()->getYoshi())){
            getWorld()->getYoshi()->addCoins(3);
            getWorld()->playSound(SOUND_GIVE_COIN);
            set_yactivate(false);
        }
    }


void RedCoinSquare::doSomething(){ //check if Bowsers can destroy red squares
    if(!get_alive()) return;
    
    //for Peach:
    Peachreactivate(getWorld()->getPeach()); //if there is not a Peach on the square, it reactivates
    if(Peachlanded(getWorld()->getPeach())){
            if(getWorld()->getPeach()->getCoins() <= 3) getWorld()->getPeach()->setCoins(0);
            else getWorld()->getPeach()->addCoins(-3);
            getWorld()->playSound(SOUND_TAKE_COIN);
            set_pactivate(false); //makes sure we don't activate it again unless Peach returns to the same square again
    }
    
    //for Yoshi:
    Yoshireactivate(getWorld()->getYoshi());
    if(Yoshilanded(getWorld()->getYoshi())){
            if(getWorld()->getYoshi()->getCoins() <= 3) getWorld()->getYoshi()->setCoins(0);
            else getWorld()->getYoshi()->addCoins(-3);
            getWorld()->playSound(SOUND_TAKE_COIN);
            set_yactivate(false);
        }
    }

void StarSquare::doSomething(){
    //for Peach:
    Peachreactivate(getWorld()->getPeach()); //if Peach isn't on the square, reactive
    if(Peachlanded(getWorld()->getPeach()) || Peachpassed(getWorld()->getPeach())){ //Peach is on the square whether she's passing by or landed
        if(getWorld()->getPeach()->getCoins() > 20){
            getWorld()->getPeach()->addCoins(-20);
            getWorld()->getPeach()->addStars(1);
            getWorld()->playSound(SOUND_GIVE_STAR);
            set_pactivate(false); //makes sure we don't activate it again unless Peach returns to the same square again
        }
    }
    
    //for Yoshi:
    Yoshireactivate(getWorld()->getYoshi());
    if(Yoshilanded(getWorld()->getYoshi()) || Yoshipassed(getWorld()->getYoshi())){
        if(getWorld()->getYoshi()->getCoins() > 20){
            getWorld()->getYoshi()->addCoins(-20);
            getWorld()->getYoshi()->addStars(1);
            getWorld()->playSound(SOUND_GIVE_STAR);
            set_yactivate(false);
        }
    }
}

void DirectionalSquare::doSomething(){
    if(Peachlanded(getWorld()->getPeach()) || Peachpassed(getWorld()->getPeach())){ //if Peach is on the square
        getWorld()->getPeach()->setwalk(forcing_direction); //changes Peach's walking direction
        getWorld()->getPeach()->setdirectional_square(true);
        if(forcing_direction == left)
            getWorld()->getPeach()->setDirection(180);
        else getWorld()->getPeach()->setDirection(0);
    }
    if(Yoshilanded(getWorld()->getYoshi()) || Yoshipassed(getWorld()->getYoshi())){ //if Yoshi is on the square
        getWorld()->getYoshi()->setwalk(forcing_direction); //changes Yoshi's walking direction
        getWorld()->getYoshi()->setdirectional_square(true);
        if(forcing_direction == left)
            getWorld()->getYoshi()->setDirection(180);
        else getWorld()->getYoshi()->setDirection(0);
    }
}

void BankSquare::doSomething(){
    //PEACH
    Peachreactivate(getWorld()->getPeach()); //if there is not a player on the square, it reactivates
    
    //if player has moved and landed
    if(Peachlanded(getWorld()->getPeach())){
        int balance = getWorld()->getBank(); //get the balance
        getWorld()->getPeach()->addCoins(balance); //transfer balance to the player
        getWorld()->setBank(-balance); //subtract the balance from the Bank so it has 0 coins
        getWorld()->playSound(SOUND_WITHDRAW_BANK);
        set_pactivate(false);
    }
    
    //if player has moved and not landed
    if(Peachpassed(getWorld()->getPeach())){
        int balance = 5; //set it to 5 for now
        if(getWorld()->getPeach()->getCoins() < 5) balance = getWorld()->getPeach()->getCoins(); //if the player doesn't have at least 5 coins set the balance to however many coins they have
        getWorld()->getPeach()->addCoins(-balance); //remove the balance
        getWorld()->setBank(balance); //add to Bank
        getWorld()->playSound(SOUND_DEPOSIT_BANK);
        set_pactivate(false);
    }
    
    //YOSHI
    Yoshireactivate(getWorld()->getYoshi());
    
    //if player has moved and landed
    if(Yoshilanded(getWorld()->getYoshi())){
            int balance = getWorld()->getBank();
            getWorld()->getYoshi()->addCoins(balance);
            getWorld()->setBank(-balance);
            getWorld()->playSound(SOUND_WITHDRAW_BANK);
            set_yactivate(false);
    }
    
    //if player has moved and not landed
    if(Yoshipassed(getWorld()->getYoshi())){
            int balance = 5;
            if(getWorld()->getYoshi()->getCoins() < 5) balance = getWorld()->getYoshi()->getCoins();
            getWorld()->getYoshi()->addCoins(-balance);
            getWorld()->setBank(balance);
            getWorld()->playSound(SOUND_DEPOSIT_BANK);
            set_yactivate(false);
    }
}

void EventSquare::swap(Player* one, Player* two){
    int x = one->getX();
    int y = one->getY();
    int other_x = two->getX();
    int other_y = two->getY();
    two->moveTo(x, y);
    one->moveTo(other_x, other_y);
    
    int ticks = one->getTicks();
    int other_ticks = two->getTicks();
    one->setTicks(other_ticks);
    two->setTicks(ticks);
    
    int walk = one->getwalk();
    int other_walk = two->getwalk();
    if(walk == left) two->setDirection(180);
    else two->setDirection(0);
    if(other_walk == left) one->setDirection(180);
    else one->setDirection(0);
    one->setwalk(other_walk);
    two->setwalk(walk);
    
    bool a = one->wait();
    bool other_wait = two->wait();
    one->setwait(other_wait);
    two->setwait(a);
}

void EventSquare::doSomething(){
    //Peach
    Peachreactivate(getWorld()->getPeach());
    
    if(Peachlanded(getWorld()->getPeach())){
            int random = randInt(1, 3);
            switch (random) {
                case 1:
                    getWorld()->getPeach()->teleport();
                    getWorld()->playSound(SOUND_PLAYER_TELEPORT);
                    set_pactivate(false);
                    break;
                case 2:
                    swap(getWorld()->getPeach(), getWorld()->getYoshi());
                    getWorld()->playSound(SOUND_PLAYER_TELEPORT);
                    set_pactivate(false);
                    set_yactivate(false); //make sure the other player will not be activated
                    break;
                case 3:
                    getWorld()->getPeach()->set_hasvortex(true);
                    getWorld()->playSound(SOUND_GIVE_VORTEX);
                    break;
            }
    }
    
    //Yoshi
    Yoshireactivate(getWorld()->getYoshi());
    
    if(Yoshilanded(getWorld()->getYoshi())){
            int random = randInt(1, 3);
            switch (random) {
                case 1:
                   getWorld()->getYoshi()->teleport();
                    getWorld()->playSound(SOUND_PLAYER_TELEPORT);
                    set_yactivate(false);
                    break;
                case 2:
                    swap(getWorld()->getYoshi(), getWorld()->getPeach());
                    getWorld()->playSound(SOUND_PLAYER_TELEPORT);
                    set_yactivate(false);
                    set_pactivate(false); //make sure the other player will not be activated
                    break;
                case 3:
                    getWorld()->getYoshi()->set_hasvortex(true);
                    getWorld()->playSound(SOUND_GIVE_VORTEX);
                    break;
            }
    }
}

void DroppingSquare::doSomething(){
    //Peach
    Peachreactivate(getWorld()->getPeach());
    if(Peachlanded(getWorld()->getPeach())){
            int rand = randInt(1, 2);
            if(rand == 1){
                int balance = 10; //set it to 5 for now
                if(getWorld()->getPeach()->getCoins() < 10) balance = getWorld()->getPeach()->getCoins(); //if the player doesn't have at least 5 coins set the balance to however many coins they have
                getWorld()->getPeach()->addCoins(-balance); //remove the balance
                getWorld()->playSound(SOUND_DROPPING_SQUARE_ACTIVATE);
            }
            else if(rand == 2){
                if(getWorld()->getPeach()->getStars() >= 1) getWorld()->getPeach()->addStars(-1);
                getWorld()->playSound(SOUND_DROPPING_SQUARE_ACTIVATE);
            }
        set_pactivate(false);
    }
    
    //YOSHI
    Yoshireactivate(getWorld()->getYoshi());
    if(Yoshilanded(getWorld()->getYoshi())){
            int rand = randInt(1, 2);
            if(rand == 1){
                int balance = 10; //set it to 5 for now
                if(getWorld()->getYoshi()->getCoins() < 10) balance = getWorld()->getYoshi()->getCoins(); //if the player doesn't have at least 5 coins set the balance to however many coins they have
                getWorld()->getYoshi()->addCoins(-balance); //remove the balance
                getWorld()->playSound(SOUND_DROPPING_SQUARE_ACTIVATE);
            }
            else if(rand == 2){
                if(getWorld()->getYoshi()->getStars() >= 1) getWorld()->getYoshi()->addStars(-1);
                getWorld()->playSound(SOUND_DROPPING_SQUARE_ACTIVATE);
            }
        set_yactivate(false);
    }
}

void Baddie::vortexFunction(){
    teleport();
    setwalk(right);
    setDirection(0);
    setwalkstate(false);
    setPause(180);
}

void Baddie::impactedbyVortex(){
    teleport();
    setwalk(right);
    setDirection(0);
    setwalkstate(false);
    setPause(180);
    setimpacted(false);
}

void Bowser::doSomething(){
    if(getimpacted()) impactedbyVortex();
    if(!walk()){ 
        if(getX() == getWorld()->getPeach()->getX() && getY() == getWorld()->getPeach()->getY() && getWorld()->getPeach()->wait())
            if(randInt(0, 1) == 1){
                getWorld()->getPeach()->setCoins(0);
                getWorld()->playSound(SOUND_BOWSER_ACTIVATE);
            }
        if(getX() == getWorld()->getYoshi()->getX() && getY() == getWorld()->getYoshi()->getY() && getWorld()->getYoshi()->wait())
            if(randInt(0, 1) == 1){
                getWorld()->getYoshi()->setCoins(0);
                getWorld()->playSound(SOUND_BOWSER_ACTIVATE);
            }
        addPause(-1);
        if(getPause() == 0){
            int temp = randInt(1, 10);
            setTravel(temp);
            setTicks(temp * 8);
            while(int rand = (randInt(0, 3))){
                if(moveForward(rand*90)){
                    setwalk(rand*90);
                    break;
                }
            }
            if(getwalk() == left) setDirection(180);
            else setDirection(0);
            setwalkstate(true);
        }
    }
    
    if(walk()){
        if(multiDirection() && getTicks() % 8 == 0){ 
            while(int rand = (randInt(0, 3))){
                if(moveForward(rand*90)){
                    setwalk(rand*90);
                    break;
                }
            }
            if(getwalk() == left) setDirection(180);
            else setDirection(0);
        }
        if(!moveForward(getwalk())) findNewDirection();
        moveAtAngle(getwalk(), 2);
        addTicks(-1);
        if(getTicks() == 0){
            setwalkstate(false);
            setPause(180);
            int rand = randInt(1, 4);
            if(rand == 1){
                getWorld()->setActorDead(getX(), getY());
                DroppingSquare* new_s = new DroppingSquare(getWorld(), IID_DROPPING_SQUARE, getX(), getY());
                getWorld()->addActor(new_s);
                getWorld()->playSound(SOUND_DROPPING_SQUARE_CREATED);
            }
        }
    }
}

void Boo::doSomething(){
    if(getimpacted()) impactedbyVortex();
    if(!walk()){
        if((getX() == getWorld()->getPeach()->getX() && getY() == getWorld()->getPeach()->getY() && getWorld()->getPeach()->wait() && !getactivated()) || (getX() == getWorld()->getYoshi()->getX() && getY() == getWorld()->getYoshi()->getY() && getWorld()->getYoshi()->wait() && !getactivated())){
            if(randInt(0, 1) == 1){
                int peach_coin = getWorld()->getPeach()->getCoins();
                getWorld()->getPeach()->setCoins(getWorld()->getYoshi()->getCoins());
                getWorld()->getYoshi()->setCoins(peach_coin);
            }
            else{
                int peach_star = getWorld()->getPeach()->getStars();
                getWorld()->getPeach()->setStars(getWorld()->getYoshi()->getStars());
                getWorld()->getYoshi()->setStars(peach_star);
            }
            getWorld()->playSound(SOUND_BOO_ACTIVATE);
            setactivated(true);
        }
        else if(getX() != getWorld()->getPeach()->getX() && getY() != getWorld()->getPeach()->getY() && getX() != getWorld()->getYoshi()->getX() && getY() != getWorld()->getYoshi()->getY()) setactivated(false);
        
        addPause(-1);
        if(getPause() == 0){
            setTravel(randInt(1, 3));
            setTicks(getTravel() * 8);
            while(int rand = (randInt(0, 3))){
                if(moveForward(rand*90)){
                    setwalk(rand*90);
                    break;
                }
            }
            if(getwalk() == left) setDirection(180);
            else setDirection(0);
            setwalkstate(true);
        }
    }
    if(walk()){
        if(multiDirection() && getTicks() % 8 == 0){ //TODO: DEBUG WHY IS BOO STILL ON MOLLY
            while(int rand = (randInt(0, 3))){
                if(moveForward(rand*90)){
                    setwalk(rand*90);
                    break;
                }
            }
            if(getwalk() == left) setDirection(180);
            else setDirection(0);
        }
        else if(!moveForward(getwalk())) findNewDirection();
        
        moveAtAngle(getwalk(), 2);
        addTicks(-1);
        if(getTicks() == 0){
            setwalkstate(false);
            setPause(180);
        }
    }
}

void Vortex::doSomething(){
    if(!getActive()) return;
    moveAtAngle(getDirection(), 2);
    if(getX() < 0 || getX() >= VIEW_WIDTH || getY() < 0 || getY() >= VIEW_HEIGHT){
        setActive(false);
        set_alive(false);
    }
    getWorld()->findimpactableActor(getX(), getY());
}
