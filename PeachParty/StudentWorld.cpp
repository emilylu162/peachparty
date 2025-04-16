#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath){
}

StudentWorld::~StudentWorld(){
    cleanUp();
}

int StudentWorld::init()
{
    m_b = new Board();
    string board = assetPath() + "/board0" + to_string(getBoardNumber()) + ".txt"; //find asset Path and load specific board
    Board::LoadResult result = m_b->loadBoard(board);
    
    if (result == Board::load_fail_file_not_found){
        cerr << "Could not find data file\n";
        return GWSTATUS_BOARD_ERROR;
    } else if (result == Board::load_fail_bad_format){
        cerr << "Your board was improperly formatted\n";
        return GWSTATUS_BOARD_ERROR;
    } else if (result == Board::load_success) cerr << "Successfully loaded board\n";
        
    for(int i = 0; i < BOARD_WIDTH; i++){
        for(int j = 0; j < BOARD_HEIGHT; j++){ //goes through each of the board characters
            Board::GridEntry ge = m_b->getContentsOf(i, j);
            switch(ge){
                case Board::empty:
                    break;
                case Board::player:
                    m_p = new Player(this, i*BOARD_WIDTH, j*BOARD_HEIGHT, IID_PEACH, 1); //initialize private peach pointer
                    m_y = new Player(this, i*BOARD_WIDTH, j*BOARD_HEIGHT, IID_YOSHI, 2); //initialize private yoshi pointer
                    m_actors.push_back(m_p); //add to our  vector
                    m_actors.push_back(m_y);
                    m_actors.push_back(new BlueCoinSquare(this, IID_BLUE_COIN_SQUARE, i*BOARD_WIDTH, j*BOARD_HEIGHT)); //there's a blue square under the players
                    break;
                case Board::blue_coin_square:
                    m_actors.push_back(new BlueCoinSquare(this, IID_BLUE_COIN_SQUARE, i*BOARD_WIDTH, j*BOARD_HEIGHT)); //create blue coin square and add to vector
                    break;
                case Board::red_coin_square:
                    m_actors.push_back(new RedCoinSquare(this, IID_RED_COIN_SQUARE, i*BOARD_WIDTH, j*BOARD_HEIGHT)); //create red coin square and add to vector
                    break;
                case Board::left_dir_square:
                    m_actors.push_back(new DirectionalSquare(this, IID_DIR_SQUARE, i*BOARD_WIDTH, j*BOARD_HEIGHT, 180)); //create left direction square and add to vector
                    break;
                case Board::right_dir_square:
                    m_actors.push_back(new DirectionalSquare(this, IID_DIR_SQUARE, i*BOARD_WIDTH, j*BOARD_HEIGHT, 0)); //create right direction square and add to vector
                    break;
                case Board::up_dir_square:
                    m_actors.push_back(new DirectionalSquare(this, IID_DIR_SQUARE, i*BOARD_WIDTH, j*BOARD_HEIGHT, 90)); //create up direction square and add to vector
                    break;
                case Board::down_dir_square:
                    m_actors.push_back(new DirectionalSquare(this, IID_DIR_SQUARE, i*BOARD_WIDTH, j*BOARD_HEIGHT, 270)); //create down direction square and add to vector
                    break;
                case Board::event_square:
                    m_actors.push_back(new EventSquare(this, IID_EVENT_SQUARE, i*BOARD_WIDTH, j*BOARD_HEIGHT)); //create event square and add to vector
                    break;
                case Board::bank_square:
                    m_actors.push_back(new BankSquare(this, IID_BANK_SQUARE, i*BOARD_WIDTH, j*BOARD_HEIGHT)); //create bank square and add to vector
                    break;
                case Board::star_square:
                    m_actors.push_back(new StarSquare(this, IID_STAR_SQUARE, i*BOARD_WIDTH, j*BOARD_HEIGHT)); //create star square and add to vector
                    break;
                case Board::bowser:
                    m_actors.push_back(new Bowser(this, i*BOARD_WIDTH, j*BOARD_HEIGHT, IID_BOWSER));
                    m_actors.push_back(new BlueCoinSquare(this, IID_BLUE_COIN_SQUARE, i*BOARD_WIDTH, j*BOARD_HEIGHT));
                    break;
                case Board::boo:
                    m_actors.push_back(new Boo(this, i*BOARD_WIDTH, j*BOARD_HEIGHT, IID_BOO));
                    m_actors.push_back(new BlueCoinSquare(this, IID_BLUE_COIN_SQUARE, i*BOARD_WIDTH, j*BOARD_HEIGHT));
                    break;
            }
        }
    }
    
    startCountdownTimer(99);  // timeout after 99 seconds
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    for(Actor* a : m_actors){
        if(a->get_alive()) a->doSomething(); //call each actor to do Something
    }
    
    for(int i = 0; i < m_actors.size(); i++){
        if(!m_actors[i]->get_alive()){ //check if any actors are dead
            Actor* temp = m_actors[i]; //temporary pointer to the dead actor
            m_actors[i] = m_actors[m_actors.size()-1]; //move the last actor to the index of the dead actor
            delete temp; //delete the dead actor
            m_actors.pop_back(); //remove the last actor
        }
    }
    
    setGameStatText("P1 Roll: " + to_string(m_p->getRoll()) + " Stars: " + to_string(m_p->getStars()) + " $$: " + to_string(m_p->getCoins()) + " | Time: " + to_string(timeRemaining()) + " | Bank: " + to_string(getBank()) + " | P2 Roll: " + to_string(m_y->getRoll()) + " Stars: " + to_string(m_y->getStars()) + " $$: " + to_string(m_y->getCoins()) + " VOR"); //BUG: idk about VOR
    
    
    
    if(timeRemaining() == 0){ //no more time left in the game
        playSound(SOUND_GAME_FINISHED);
        if(m_y->getCoins() > m_p->getCoins()){ //yoshi won
            setFinalScore(m_y->getStars(), m_y->getCoins());
            return GWSTATUS_YOSHI_WON;
        }
        else if(m_y->getCoins() == m_p->getCoins() && m_y->getStars() > m_p->getStars()){ //yoshi won
            setFinalScore(m_y->getStars(), m_y->getCoins());
            return GWSTATUS_YOSHI_WON;
        }
        else{ //all other cases, peach won
            setFinalScore(m_p->getStars(), m_p->getCoins());
            return GWSTATUS_PEACH_WON;
        }
    }
                    
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp(){
    for(Actor* a: m_actors) delete a; //delete each actor
    m_actors.clear(); //clear the vector
    m_p = nullptr; //make sure the pointers are null so we don't delete an already deleted pointer
    m_y = nullptr;
    m_b = nullptr;

    delete m_p;
    delete m_y;
    delete m_b;
}

void StudentWorld::setActorDead(int x, int y){
    for(int i = 0; i < m_actors.size(); i++){
        if(m_actors[i]->getX() == x && m_actors[i]->getY() == y && m_actors[i]->get_removeable()){
            m_actors[i]->set_alive(false);
            break;
        }
    }
}

void StudentWorld::findimpactableActor(int x, int y){
    for(int i = 0; i < m_actors.size(); i++){
        if(m_actors[i]->getX() == x && m_actors[i]->getY() == y && m_actors[i]->is_impactable()){
            m_actors[i]->setimpacted(true);
            break;
        }
    }
}
