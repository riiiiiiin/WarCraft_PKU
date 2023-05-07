////////////////////////////////////////////
/*你说得对，但是《魔兽世界》是由北京大学自主研发
的一款开放一维世界冒险游戏。游戏发生在一个被称作
[IDE]的电子世界，在这里，被程序设计实习选中的人
将被授予[编译器]，导引cpp之力。你将扮演一位名为
[绩点掉光人]的神秘角色在自由的旅行中邂逅性格各异
能力独特的bug们，和他们一起击败case，找回失散的
    马——同时，逐步发觉[魔兽世界]的真相*/
////////////////////////////////////////////

////////////////////////////////////////////
/*要求比上一个还恶心，全tmd写成一点也不优雅的补
       丁了放弃补完注释和重构代码了*/
////////////////////////////////////////////




#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <queue>


////////////////////////////////
////       game data        ////
////////////////////////////////


enum WeaponType{sword=0,bomb=1,arrow=2};
enum WarriorType{dragon=0,ninja=1,iceman=2,lion=3,wolf=4};
enum HeadquarterStategy{red=0,blue=1};
enum BattleResults{bothDead=0,redWon=1,blueWon=2,bothAlive=3};
const std::string WeaponName[3]{"sword", "bomb","arrow"};
const std::string WarriorName[5]{"dragon","ninja","iceman","lion","wolf"};
const std::string HeadquarterName[2]{"red","blue"};
int WarriorHp[5]{};
int WarriorATK[5]{};
// 红方顺序:3,4,5,2,1;蓝方顺序:4,1,2,3,5
const WarriorType statgies[2][5]{{iceman,lion,wolf,ninja,dragon},{lion,dragon,ninja,iceman,wolf}};
int LoyaltyDeclineRate{};
int HQPosition[2]{};
int MaxTime{};
int HQHealth{};
int CitySum{};
int ArrowAtk{};



////////////////////////////////
////        weapons         ////
////////////////////////////////

class Sword{
private:
    int _swordAtk_;
public:
    Sword(int _atk):_swordAtk_(_atk/5){}
    ~Sword() = default;
    int attackDmg(int _atk,bool _if_fightback){
        if(_if_fightback){
            return _atk/2 + _swordAtk_;
        }
        else{
            return _atk + _swordAtk_;
        }
    }
    friend std::ostream& operator<<(std::ostream& os,Sword const& dhis){
        os<<"sword("<<dhis._swordAtk_<<')';
        return os;
    }
    //false == unreuseable
    //true == reuseable
    bool beUsed(){
        _swordAtk_ = _swordAtk_ * 4 / 5;
        if(_swordAtk_ <= 0){
            return false;
        }
        else{
            return true;
        }
    }
};

class Bomb{
public:
    Bomb(){}
    ~Bomb() = default;
    int attackDmg(int atk){
        return 0;
    }
    friend std::ostream& operator<<(std::ostream& os,Bomb const& dhis){
        os<<"bomb";
        return os;
    }
    //false == unreuseable
    //true == reuseable
    bool beUsed(){
        return false;
    }
};

class Arrow{
private:
    int _arrowAtk_;
    int _duralbility_;
public:
    Arrow(int _arrowAtk):_arrowAtk_(_arrowAtk),_duralbility_(3){}
    ~Arrow() = default;
    int attackDmg(int atk){
        return _arrowAtk_;
    }
    friend std::ostream& operator<<(std::ostream& os,Arrow const& dhis){
        os<<"arrow("<<dhis._duralbility_<<')';
        return os;
    }
    //false == unreuseable
    //true == reuseable
    bool beUsed(){
        --_duralbility_;
        if(_duralbility_<=0){
            return false;
        }
        else{
            return true;
        }
    }
};



/////////////////////////////////
////        warriors         ////
/////////////////////////////////


class Warrior{
protected:
    int _warrior_id_;
    int _hp_;
    int _atk_;
    int _position_;
    int _weapon_roll_;
    int _hp_bak_;
    WarriorType _warriortype_;
    Sword* _slot_sword_;
    Bomb* _slot_bomb_;
    Arrow* _slot_arrow_;
    HeadquarterStategy _HQ;
    //确定初始化将哪种武器装进背包
    virtual void _initial_weapon(int roll){
        switch(roll){
            case sword:{
                if(_atk_/5>0){
                    _slot_sword_ = new Sword(_atk_);
                }
                break;
            }               
            case bomb:{
                _slot_bomb_ = new Bomb;
                break;
            }                
            case arrow:{
                _slot_arrow_ = new Arrow(ArrowAtk);
                break;
            }               
        }
    }
    //true : should use bomb
    //false : dont use bomb
    virtual bool _try_bomb_(Warrior* enemy,bool if_attacked){
        bool _use_bomb{false};
        if(enemy->Is_Alive()&&Is_Alive()){
            //be attacked
            if(if_attacked){
                int _sim_atk = (enemy->_slot_sword_==nullptr)?(enemy->_atk_):(enemy->_slot_sword_->attackDmg(enemy->_atk_,false));
                if(_sim_atk >= _hp_){
                    _use_bomb = true;
                }
            }
            else{
                int _sim_atk = (_slot_sword_==nullptr)?_atk_:_slot_sword_->attackDmg(_atk_,false);
                if(_sim_atk >= enemy->hp()){
                    _use_bomb = false;
                }
                else{
                    _sim_atk = (enemy->_slot_sword_==nullptr)?(enemy->_atk_/2):(enemy->_slot_sword_->attackDmg(enemy->_atk_,true));
                    if(enemy->warriorType()==ninja){
                        _sim_atk = 0;
                    }
                    if(_sim_atk >= _hp_){
                        _use_bomb = true;
                    }
                }
            }
        }
        if(_slot_bomb_==nullptr){
            _use_bomb = false;
        }
        return _use_bomb;
    }
public:
    Warrior(int wid,int hp,int atk,int pos,WarriorType wtype,HeadquarterStategy HQ_):_warrior_id_(wid),_hp_(hp),_atk_(atk),
                                                              _position_(pos),_warriortype_(wtype),_HQ(HQ_),
                                                              _slot_sword_(nullptr), _slot_bomb_(nullptr),_slot_arrow_(nullptr){}
    virtual ~Warrior(){
        if(_slot_sword_!=nullptr){
            delete _slot_sword_;
        }
        if(_slot_bomb_!=nullptr){
            delete _slot_bomb_;
        }
        if(_slot_arrow_!=nullptr){
            delete _slot_arrow_; 
        }
        _slot_sword_ = nullptr;
        _slot_bomb_ = nullptr;
        _slot_arrow_ = nullptr;
    }
    void backup_hp(){_hp_bak_=_hp_;}
    int hp_bak(){return _hp_bak_;}
    HeadquarterStategy HQ() const& {return _HQ;}
    bool Is_Alive() const {return _hp_>0;}
    int hp() const& {return _hp_;}
    void hp(int hp){_hp_+=hp;}
    int const& position(){return _position_;}
    Arrow* & slot_arrow_(){return _slot_arrow_;}
    Bomb* & slot_bomb_(){return _slot_bomb_;}
    Sword* & slot_sword_(){return _slot_sword_;}
    WarriorType const& warriorType(){return _warriortype_;}
    //iceman should override this
    virtual void March(int const& time_){
        _position_+=(_HQ==red?1:-1);
        std::cout<<std::setw(3)<<std::setfill('0')<<time_/60<<std::setfill(' ')<<":10 "
                 <<(*this)
                 <<" marched to city "<<_position_<<" with "
                 <<_hp_<<" elements and force "<<_atk_<<std::endl;
        return;
    }
    virtual void Reach_HQ(int const& time_){
        _position_+=(_HQ==red?1:-1);
        std::cout<<std::setw(3)<<std::setfill('0')<<time_/60<<std::setfill(' ')<<":10 "
                 <<(*this)
                 <<" reached "<<HeadquarterName[1-_HQ]<<" headquarter with "
                 <<_hp_<<" elements and force "<<_atk_<<std::endl;
    }
    //for lion
    virtual bool Runaway(int const& time_){return false;}
    virtual bool Shoot_Arrow(Warrior* enemy,int const& time_){
        bool if_shooted{false};
        if(_slot_arrow_!=nullptr){
            enemy->hp(-_slot_arrow_->attackDmg(_atk_));
            if_shooted = true;
            std::cout<<std::setw(3)<<std::setfill('0')<<time_/60<<std::setfill(' ')<<":35 "
                     <<(*this)
                     <<" shot";
            if(enemy->Is_Alive()){
                std::cout<<std::endl;
            }
            else{
                std::cout<<" and killed "<<(*enemy)<<std::endl;
            }
            if(not _slot_arrow_->beUsed()){
                delete _slot_arrow_;
                _slot_arrow_ = nullptr;
            }
        }
        return if_shooted;
    }
    //caller must kill both warriors
    virtual bool Use_Bomb(Warrior* enemy,int const& time_,bool if_attacked){
        bool if_exploded{false};
        if(_try_bomb_(enemy,if_attacked)==true){
            if_exploded = true;
            std::cout<<std::setw(3)<<std::setfill('0')<<time_/60<<std::setfill(' ')<<":38 "
                     <<(*this)<<" used a bomb and killed "<<(*enemy)<<std::endl;
            delete _slot_bomb_;
            _slot_bomb_ = nullptr;
        }
        return if_exploded;
    }
    virtual bool Sword_Attack(Warrior* enemy,int const& time_){
        if(_slot_sword_==nullptr){
            enemy->hp(-_atk_);
        }
        else{
            enemy->hp(-_slot_sword_->attackDmg(_atk_,false));
            if(not _slot_sword_->beUsed()){
                delete _slot_sword_;
                _slot_sword_ = nullptr;
            }
        }
        std::cout<<std::setw(3)<<std::setfill('0')<<time_/60<<std::setfill(' ')<<":40 "
                 <<(*this)<<" attacked "<<(*enemy)<<" in city "<<_position_<<" with "
                 <<_hp_<<" elements and force "<<_atk_<<std::endl;
        return true;
    } 
    virtual bool Sword_FightBack(Warrior* enemy,int const& time_){
        if(_slot_sword_==nullptr){
            enemy->hp(-_atk_/2);
        }
        else{
            enemy->hp(-_slot_sword_->attackDmg(_atk_,true));
            if(not _slot_sword_->beUsed()){
                delete _slot_sword_;
                _slot_sword_ = nullptr;
            }
        }
        std::cout<<std::setw(3)<<std::setfill('0')<<time_/60<<std::setfill(' ')<<":40 "
                 <<(*this)<<" fought back against "<<(*enemy)<<" in city "<<_position_<<std::endl;
        return true;
    }
    //抢夺对方背包，wolf用
    virtual bool Loot(Warrior& enemy,bool loot_once=false){
        bool has_looted{false};
        return has_looted;
    }
    virtual bool Suck_Blood(Warrior& enemy){
        bool has_sucked;
        if(enemy.warriorType()==lion){
            _hp_ += enemy.hp_bak();
            has_sucked = true;
        }
        return has_sucked;
    }
    //for dragon
    virtual void Yell(int const& time_){}
    virtual void Report(int const& time_){
        int weapon_cnt{0};
        if(_slot_arrow_!=nullptr){
            ++weapon_cnt;
        }
        if(_slot_bomb_!=nullptr){
            ++weapon_cnt;
        }
        if(_slot_sword_!=nullptr){
            ++weapon_cnt;
        }
        std::cout<<std::setw(3)<<std::setfill('0')<<time_/60<<std::setfill(' ')<<":55 "<<(*this)<<" has ";
        if(weapon_cnt==0){
            std::cout<<"no weapon"<<std::endl;
        }
        else{
            if(_slot_arrow_!=nullptr){
                std::cout<<(*_slot_arrow_);
                std::cout<<(--weapon_cnt?',':'\n');
            }
            if(_slot_bomb_!=nullptr){
                std::cout<<(*_slot_bomb_);
                std::cout<<(--weapon_cnt?',':'\n');
            }
            if(_slot_sword_!=nullptr){
                std::cout<<(*_slot_sword_);
                std::cout<<(--weapon_cnt?',':'\n');
            }
        }
    }
    friend std::ostream& operator<<(std::ostream& os,Warrior const& dhis){
        os<<HeadquarterName[dhis._HQ]<<' '<<WarriorName[dhis._warriortype_]<<' '
            <<dhis._warrior_id_;
        return os;
    }
    //for dragon and lion
    virtual void BornBonusWords(){}
    // for dragon
    virtual void Morale_Change(bool add){}
    //for lion
    virtual void Loyalty_Decline(){}
};

template<HeadquarterStategy HQ>
class warrior_cmp{
public:
    bool operator()(Warrior* lhs,Warrior* rhs) const{
        if(HQ==red){
            return lhs->position()>rhs->position();
        }
        else{
            return lhs->position()<rhs->position();
        }
    }
};


class Dragon:public Warrior{
private:
    double _morale_;
public:
    Dragon(int pos,int wid,HeadquarterStategy HQ_,int HQHealth):Warrior(wid,WarriorHp[dragon],WarriorATK[dragon],pos,dragon,HQ_),_morale_(1.0*HQHealth/WarriorHp[dragon]){
        _initial_weapon(wid%3);
    }
    ~Dragon() = default;
    void Yell(int const& time_){
        if(_morale_>0.8){
            std::cout<<std::setw(3)<<std::setfill('0')<<time_/60<<std::setfill(' ')<<":40 "
                     <<(*this)
                     <<" yelled in city "<<_position_<<std::endl;
        }
    }
    void Morale_Change(bool add){
        if(add){
            _morale_+=0.2;
        }
        else{
            _morale_-=0.2;
        }
    }
    void BornBonusWords(){
        std::cout<<"Its morale is "<<std::fixed<<std::setprecision(2)<<_morale_<<std::endl;
    }
};

class Ninja:public Warrior{
public:
    Ninja(int pos,int wid,HeadquarterStategy HQ_,int HQHealth):Warrior(wid,WarriorHp[ninja],WarriorATK[ninja],pos,ninja,HQ_){
        _initial_weapon(wid%3);
        _initial_weapon((wid+1)%3);
    }
    ~Ninja() = default;
    bool Sword_FightBack(Warrior* enemy,int const& time_){
        return false;
    }
};

class Iceman:public Warrior{
private:
    int _move_cnt_;
public:
    Iceman(int pos,int wid,HeadquarterStategy HQ_,int HQHealth):Warrior(wid,WarriorHp[iceman],WarriorATK[iceman],pos,iceman,HQ_),_move_cnt_(0){
        _initial_weapon(wid%3);
    }
    ~Iceman() = default;
    void March(int const& time_){
        ++_move_cnt_;
        _position_+=(_HQ==red?1:-1);
        if(_move_cnt_%2 == 0){
            _hp_-=9;
            _hp_ = _hp_ > 1 ? _hp_ : 1 ;
            _atk_+=20;
        }
        std::cout<<std::setw(3)<<std::setfill('0')<<time_/60<<std::setfill(' ')<<":10 "
                 <<(*this)
                 <<" marched to city "<<_position_<<" with "
                 <<_hp_<<" elements and force "<<_atk_<<std::endl;
        return;
    }
    virtual void Reach_HQ(int const& time_){
        ++_move_cnt_;
        _position_+=(_HQ==red?1:-1);
        if(_move_cnt_%2 == 0){
            _hp_-=9;
            _hp_ = _hp_>1?_hp_:1;
            _atk_+=20;
        }
        std::cout<<std::setw(3)<<std::setfill('0')<<time_/60<<std::setfill(' ')<<":10 "
                 <<HeadquarterName[_HQ]<<' '<<WarriorName[_warriortype_]<<' '
                 <<_warrior_id_<<" reached "<<HeadquarterName[1-_HQ]<<" headquarter with "
                 <<_hp_<<" elements and force "<<_atk_<<std::endl;
    }
};

class Lion:public Warrior{
private:
    int _loyalty;
public:
    Lion(int pos,int wid,HeadquarterStategy HQ_,int HQhealth):Warrior(wid,WarriorHp[lion],WarriorATK[lion],pos,lion,HQ_),
                                                            _loyalty(HQhealth){}
    ~Lion() = default;
    void Loyalty_Decline(){
        _loyalty-=LoyaltyDeclineRate;
    }
    bool Runaway(int const& time_){
        if((_position_!=HQPosition[1-_HQ])&&(_loyalty<=0)){
            std::cout<<std::setw(3)<<std::setfill('0')<<time_/60<<std::setfill(' ')<<":05 "
             <<(*this)
             <<" ran away"<<std::endl;
            return true;
        }
        return false;
    }
    void BornBonusWords(){
        std::cout<<"Its loyalty is "<<_loyalty<<std::endl;
    }
};

class Wolf:public Warrior{
public:
    Wolf(int pos,int wid,HeadquarterStategy HQ_,int hq_hp):Warrior(wid,WarriorHp[wolf],WarriorATK[wolf],pos,wolf,HQ_){}
    ~Wolf()=default;
    virtual bool Loot(Warrior& enemy,bool loot_once=false){
        bool has_looted{false};
        if(enemy.slot_arrow_() != nullptr && _slot_arrow_ == nullptr){
            _slot_arrow_ = enemy.slot_arrow_();
            enemy.slot_arrow_() = nullptr;
            has_looted = true;
        }
        if(enemy.slot_bomb_() != nullptr && _slot_bomb_ == nullptr){
            _slot_bomb_ = enemy.slot_bomb_();
            enemy.slot_bomb_() = nullptr;
            has_looted = true;
        }
        if(enemy.slot_sword_() != nullptr && _slot_sword_ == nullptr){
            _slot_sword_ = enemy.slot_sword_();
            enemy.slot_sword_() = nullptr;
            has_looted = true;
        }
        return has_looted;
    }
};



//////////////////////////////
////        cities        ////
//////////////////////////////


class City{
protected:
    int _city_pos;
    int _health;
    std::vector<Warrior*> _warriors_here;

public:
    City(int city_pos,int health):_city_pos(city_pos),_health(health){
        _warriors_here.resize(2);
        _warriors_here[red]=nullptr;
        _warriors_here[blue]=nullptr;
    }
    virtual ~City(){
        _warriors_here.clear();
    }
    int& health(){return _health;}
    std::vector<Warrior*>& warriors_here(){return _warriors_here;}
    virtual int city_pos(){return _city_pos;}
    virtual void Warrior_March(City* other,HeadquarterStategy direction){
        //恶心死了：输出和事件应当不同步
        other->warriors_here()[direction]=_warriors_here[direction];
        _warriors_here[direction]=nullptr;
    }
    virtual void Warrior_Arrive(int const& time_){
        if(_warriors_here[red]!=nullptr)
            _warriors_here[red]->March(time_);
        if(_warriors_here[blue]!=nullptr)
            _warriors_here[blue]->March(time_);
    }
    virtual bool Give_Birth(int const& time_){return false;}
    virtual BattleResults Battle(int const& time_){return bothAlive;}
    virtual void Report(int const& time_){}
    virtual bool Is_Overtaken(){return false;}
    virtual bool Reward(Warrior* _winner,int time_){return false;}
    virtual void GetElements(City* other,int time_){}
    virtual void Try_Bomb(int const& time_){}
    virtual void Generate_Element(){_health+=10;}
};


class Headquarter:public City{
private:
    int _warrior_roll;
    int _warrior_sum;
    int _arrived_enemy_cnt_;
    bool _is_overtaken;
    HeadquarterStategy _stategy;
    std::vector<Warrior*> _own_warriors;
    Warrior* _storage_enemy_;
public:
    Headquarter(int health,HeadquarterStategy stategy):_warrior_roll(0),_warrior_sum(0),
                                                      _stategy(stategy),City(HQPosition[stategy],health),_is_overtaken(false),
                                                      _arrived_enemy_cnt_(0)
                                                      {
                                                        _own_warriors.clear();
                                                        _storage_enemy_ = nullptr;
                                                      }
    ~Headquarter(){
        
        for(auto iter = _own_warriors.begin();iter!=_own_warriors.end();++iter){
            
            delete *iter;
        }
        
        _own_warriors.clear();  
    }
    bool Give_Birth(int const& time_);
    bool Is_Overtaken(){return _is_overtaken;}
    void Warrior_March(City* other,HeadquarterStategy direction){
        
        other->warriors_here()[direction]=_warriors_here[direction];
        _warriors_here[direction]=nullptr;
        
    }
    void Warrior_Arrive(int const& time_){
        if(_warriors_here[1-_stategy]!=nullptr){
            _warriors_here[1-_stategy]->Reach_HQ(time_);
            _storage_enemy_ = _warriors_here[1-_stategy];
            _warriors_here[1-_stategy] = nullptr;
            ++_arrived_enemy_cnt_;
            if(_arrived_enemy_cnt_>=2){
                _is_overtaken=true;
                std::cout<<std::setw(3)<<std::setfill('0')<<time_/60<<std::setfill(' ')<<":10 "
                        <<HeadquarterName[_stategy]<<" headquarter was taken"<<std::endl;
            }
        }
    }
    Warrior* storage_enemy(){return _storage_enemy_;}
    bool Reward(Warrior* _winner,int time_){
        bool if_rewarded{false};
        if(_health>=8){
            _winner->hp(8);
            _health-=8;
            if_rewarded = true;
        }
        return if_rewarded;
    }
    void GetElements(City* other,int time_){
        _health+=other->health();
        other->health()=0;
    }
    void Report(int const& time_){
        std::cout<<std::setw(3)<<std::setfill('0')<<time_/60<<std::setfill(' ')<<":50 "
                 <<_health<<" elements in "<<HeadquarterName[_stategy]<<" headquarter"<<std::endl;
    }
};

bool Headquarter::Give_Birth(int const& time_){
    if(_health<WarriorHp[statgies[_stategy][_warrior_roll]]){
        return false;//failed
    }
    _health-=WarriorHp[statgies[_stategy][_warrior_roll]];
    ++_warrior_sum;
    Warrior* newWarrior;
    switch(statgies[_stategy][_warrior_roll]){
        /*makes new warriors by type*/
        case dragon:{
            newWarrior = new Dragon(HQPosition[_stategy],_warrior_sum,_stategy,_health);
            break;
        }
        case ninja:{
            newWarrior = new Ninja(HQPosition[_stategy],_warrior_sum,_stategy,_health);
            break;
        }
        case iceman:{
            newWarrior = new Iceman(HQPosition[_stategy],_warrior_sum,_stategy,_health);
            break;
        }
        case lion:{
            newWarrior = new Lion(HQPosition[_stategy],_warrior_sum,_stategy,_health);
            break;
        }
        case wolf:{
            newWarrior = new Wolf(HQPosition[_stategy],_warrior_sum,_stategy,_health);            
            break;
        }
    }
    _own_warriors.push_back(newWarrior);
    _warriors_here[_stategy]=newWarrior;
    _warrior_roll=(_warrior_roll+1)%5;
    //output
    std::cout<<std::setw(3)<<std::setfill('0')<<time_/60<<std::setfill(' ')<<":00 "
             <<(*newWarrior)<<" born"<<std::endl;
    //far from elegant lol
    newWarrior->BornBonusWords();
    return true;//success
}


class BattleCity:public City{
private:
    //-1: no flag 0: red 1: blue
    int _city_flag{-1};
    int _latest_won{-1};
    BattleResults _state_check(){
        if(_warriors_here[red]->Is_Alive()==false&&_warriors_here[blue]->Is_Alive()==false){
            return bothDead;
        }
        if(_warriors_here[red]->Is_Alive()&&_warriors_here[blue]->Is_Alive()==false){
            return redWon;
        }
        if(_warriors_here[red]->Is_Alive()==false&&_warriors_here[blue]->Is_Alive()){
            return blueWon;
        }
        return bothAlive;
    }
    std::pair<BattleResults,bool> _battle(int const& time_,Warrior* attacker,Warrior* defender){
        BattleResults battle_result{};
        bool has_battle_happened{true};
        if(_state_check()!=bothAlive){
            battle_result = _state_check();
            has_battle_happened = false;
        }
        else{
            attacker->backup_hp();
            defender->backup_hp();
            attacker->Sword_Attack(defender,time_);
            if(_state_check()!=bothAlive){
                battle_result = _state_check();
            }
            else{
                defender->Sword_FightBack(attacker,time_);
                if(_state_check()!=bothAlive){
                    battle_result = _state_check();
                } 
            } 
            has_battle_happened = true; 
            battle_result = _state_check();
        }
        if(battle_result==redWon){
            _warriors_here[red]->Loot(*_warriors_here[blue]);
        }
        if(battle_result==blueWon){
            _warriors_here[blue]->Loot(*_warriors_here[red]);
        }
        return std::make_pair(battle_result,has_battle_happened);
    }

public:
    BattleCity(int city_pos):City(city_pos,0){}
    ~BattleCity(){}
    BattleResults Battle(int const& time_);
    void Try_Bomb(int const& time_);
    auto Warriors_here(){return _warriors_here;}
};

void BattleCity::Try_Bomb(int const& time_){
    if(_warriors_here[red]!=nullptr&&_warriors_here[blue]!=nullptr){
        HeadquarterStategy attacker;
        switch(_city_flag){
            case -1:{
                attacker = HeadquarterStategy(1-_city_pos&1);
                break;
            }
            default:{
                attacker = HeadquarterStategy(_city_flag);
            }
        }
        if(_warriors_here[red]->Use_Bomb(_warriors_here[blue],time_,attacker==blue)){
            _warriors_here[red] = nullptr;
            _warriors_here[blue] = nullptr;
        }
        else if(_warriors_here[blue]->Use_Bomb(_warriors_here[red],time_,attacker==red)){
            _warriors_here[red] = nullptr;
            _warriors_here[blue] = nullptr;
        }
    }
}

BattleResults BattleCity::Battle(int const& time_){
    BattleResults battle_result;
    if(_warriors_here[red]!=nullptr&&_warriors_here[blue]!=nullptr){
        Warrior* attacker{};
        Warrior* defender{};
        switch(_city_flag){
            case -1:{
                attacker = _warriors_here[1-_city_pos&1];
                defender = _warriors_here[_city_pos&1];
                break;
            }
            default:{
                attacker = _warriors_here[_city_flag];
                defender = _warriors_here[1-_city_flag];
            }
        }
        auto res=_battle(time_,attacker,defender);
        battle_result = res.first;
        auto battle_happened = res.second;
        switch(battle_result){
            case redWon:{
                if(battle_happened){
                    _warriors_here[red]->Suck_Blood(*_warriors_here[blue]);
                    std::cout<<std::setw(3)<<std::setfill('0')<<time_/60<<std::setfill(' ')<<":40 "
                            <<*_warriors_here[blue]
                            <<" was killed in city "<<city_pos()<<std::endl;
                }
                _warriors_here[red]->Morale_Change(true);
                if(attacker == _warriors_here[red]){
                    _warriors_here[red]->Yell(time_);
                }
                std::cout<<std::setw(3)<<std::setfill('0')<<time_/60<<std::setfill(' ')<<":40 "
                         <<*_warriors_here[red]
                         <<" earned "<<_health<<" elements for his headquarter"<<std::endl;

                if(_latest_won==red&&_city_flag!=red){
                    _city_flag = red;
                    std::cout<<std::setw(3)<<std::setfill('0')<<time_/60<<std::setfill(' ')<<":40 "
                             <<HeadquarterName[red]<<" flag raised in city "<<city_pos()<<std::endl;
                }
                _latest_won = red;

                _warriors_here[blue]=nullptr;
                break;
            }
            case blueWon:{
                if(battle_happened){
                    _warriors_here[blue]->Suck_Blood(*_warriors_here[red]);
                    std::cout<<std::setw(3)<<std::setfill('0')<<time_/60<<std::setfill(' ')<<":40 "
                            <<*_warriors_here[red]
                            <<" was killed in city "<<city_pos()<<std::endl;
                }
                _warriors_here[blue]->Morale_Change(true);
                if(attacker == _warriors_here[blue]){
                    _warriors_here[blue]->Yell(time_);
                }

                std::cout<<std::setw(3)<<std::setfill('0')<<time_/60<<std::setfill(' ')<<":40 "
                         <<*_warriors_here[blue]
                         <<" earned "<<_health<<" elements for his headquarter"<<std::endl;

                if(_latest_won==blue&&_city_flag!=blue){
                    _city_flag = blue;
                    std::cout<<std::setw(3)<<std::setfill('0')<<time_/60<<std::setfill(' ')<<":40 "
                             <<HeadquarterName[blue]<<" flag raised in city "<<city_pos()<<std::endl;
                }
                _latest_won = blue;

                _warriors_here[red]=nullptr;
                break;
            }
            case bothDead:{
                if(battle_happened){
                    std::cout<<std::setw(3)<<std::setfill('0')<<time_/60<<std::setfill(' ')<<":40 "
                            <<*_warriors_here[blue]
                            <<" was killed in city "<<city_pos()<<std::endl;
                    std::cout<<std::setw(3)<<std::setfill('0')<<time_/60<<std::setfill(' ')<<":40 "
                            <<*_warriors_here[red]
                            <<" was killed in city "<<city_pos()<<std::endl;
                    _latest_won = -1;        
                }

                _warriors_here[red]=nullptr;
                _warriors_here[blue]=nullptr;
                break;
            }
            case bothAlive:{
                _warriors_here[red]->Morale_Change(false);
                _warriors_here[blue]->Morale_Change(false);
                _warriors_here[red]->Loyalty_Decline();
                _warriors_here[blue]->Loyalty_Decline();
                attacker->Yell(time_);

                _latest_won = -1;

                break;
            }
        }
    }
    return battle_result;
}



////////////////////////////////
////         games          ////
////////////////////////////////


class Game{
private:
    int _time;
    int _city_sum;
    std::vector<City*> _cities;

public:
    Game(int city_sum):_time(0),_city_sum(city_sum+2){
        _cities.resize(city_sum+2);
        _cities[0]=new Headquarter(HQHealth,red);
        for(int i=1;i<=city_sum;++i){
            _cities[i]=new BattleCity(i);
        }
        _cities[city_sum+1]=new Headquarter(HQHealth,blue);
    }
    ~Game(){
        for(auto iter = _cities.begin();iter!=_cities.end();++iter){
            
            delete *iter;
        }
        _cities.clear();
    }
    void PlayGame();
};

void Game::PlayGame(){
    //一个小时的循环
    Headquarter* red_hq = dynamic_cast<Headquarter*>(_cities[0]);
    Headquarter* blue_hq = dynamic_cast<Headquarter*>(_cities[_city_sum-1]);
    while(_time<=MaxTime){
        /*???:00 words*/
        red_hq->Give_Birth(_time);
        blue_hq->Give_Birth(_time);
        _time+=5;
        if(_time>MaxTime)break;

        /*???:05 words*/
        for(auto iter = _cities.begin();iter != _cities.end();++iter){
            if((*iter)->warriors_here()[red]!=nullptr){
                if((*iter)->warriors_here()[red]->Runaway(_time)){
                    (*iter)->warriors_here()[red]=nullptr;
                }
            }
            if((*iter)->warriors_here()[blue]!=nullptr){
                if((*iter)->warriors_here()[blue]->Runaway(_time)){
                    (*iter)->warriors_here()[blue]=nullptr;
                }
            }
        }
        _time+=5;
        if(_time>MaxTime)break;

        /*???:10 words*/
        for(int i=_city_sum-2;i>=0;--i){
            _cities[i]->Warrior_March(_cities[i+1],red);
        }
        for(int i=1;i<_city_sum;++i){
            _cities[i]->Warrior_March(_cities[i-1],blue);
        }
        for(auto iter = _cities.begin();iter != _cities.end();++iter){
            (*iter)->Warrior_Arrive(_time);
        }
        if(red_hq->Is_Overtaken()||blue_hq->Is_Overtaken()){
            break;
        }
        _time+=10;
        if(_time>MaxTime)break;
        
        for(auto iter = _cities.begin()+1;iter!=_cities.end()-1;++iter){
            (*iter)->Generate_Element();
        }
        _time+=10;
        if(_time>MaxTime)break;


        /*???:30 words*/
        for(auto iter = _cities.begin()+1;iter!=_cities.end()-1;++iter){
            if((*iter)->warriors_here()[red]!=nullptr&&(*iter)->warriors_here()[blue]==nullptr){
                std::cout<<std::setw(3)<<std::setfill('0')<<_time/60<<std::setfill(' ')<<":30 "
                         <<*(*iter)->warriors_here()[red]
                         <<" earned "<<(*iter)->health()<<" elements for his headquarter"<<std::endl;
                red_hq->GetElements((*iter),_time);
            }
            if((*iter)->warriors_here()[blue]!=nullptr&&(*iter)->warriors_here()[red]==nullptr){
                std::cout<<std::setw(3)<<std::setfill('0')<<_time/60<<std::setfill(' ')<<":30 "
                         <<*(*iter)->warriors_here()[blue]
                         <<" earned "<<(*iter)->health()<<" elements for his headquarter"<<std::endl;
                blue_hq->GetElements((*iter),_time);
            }
        }
        _time+=5;
        if(_time>MaxTime)break;
        
        /*???:35 words*/
        auto iter = _cities.begin()+1;
        if((*iter)->warriors_here()[red]!=nullptr&&(*(iter+1))->warriors_here()[blue]!=nullptr){
            (*(iter+1))->warriors_here()[blue]->backup_hp();
            (*iter)->warriors_here()[red]->Shoot_Arrow((*(iter+1))->warriors_here()[blue],_time); 
        }
        ++iter;
        for(;iter < _cities.end()-2;++iter){
            if((*iter)->warriors_here()[red]!=nullptr&&(*(iter+1))->warriors_here()[blue]!=nullptr){
                (*(iter+1))->warriors_here()[blue]->backup_hp();
                (*iter)->warriors_here()[red]->Shoot_Arrow((*(iter+1))->warriors_here()[blue],_time);
            }
            if((*iter)->warriors_here()[blue]!=nullptr&&(*(iter-1))->warriors_here()[red]!=nullptr){
                (*(iter-1))->warriors_here()[red]->backup_hp();
                (*iter)->warriors_here()[blue]->Shoot_Arrow((*(iter-1))->warriors_here()[red],_time);
            }
        }
        iter = _cities.end()-2;
        if((*iter)->warriors_here()[blue]!=nullptr&&(*(iter-1))->warriors_here()[red]!=nullptr){
            (*(iter-1))->warriors_here()[red]->backup_hp();
            (*iter)->warriors_here()[blue]->Shoot_Arrow((*(iter-1))->warriors_here()[red],_time);
        }

        iter = _cities.begin()+1;
        if((*iter)->warriors_here()[red]!=nullptr&&(*(iter+1))->warriors_here()[blue]!=nullptr){
            if((*(iter+1))->warriors_here()[red]==nullptr&&(*(iter+1))->warriors_here()[blue]->Is_Alive()==false){
                (*(iter+1))->warriors_here()[blue] = nullptr;
            }  
        }
        ++iter;
        for(;iter < _cities.end()-2;++iter){
            if((*iter)->warriors_here()[red]!=nullptr&&(*(iter+1))->warriors_here()[blue]!=nullptr){
                if((*(iter+1))->warriors_here()[red]==nullptr&&(*(iter+1))->warriors_here()[blue]->Is_Alive()==false){
                    (*(iter+1))->warriors_here()[blue] = nullptr;
                }
            }
            if((*iter)->warriors_here()[blue]!=nullptr&&(*(iter-1))->warriors_here()[red]!=nullptr){
                if((*(iter-1))->warriors_here()[blue]==nullptr&&(*(iter-1))->warriors_here()[red]->Is_Alive()==false){
                    (*(iter-1))->warriors_here()[red] = nullptr;
                }
            }
        }
        iter = _cities.end()-2;
        if((*iter)->warriors_here()[blue]!=nullptr&&(*(iter-1))->warriors_here()[red]!=nullptr){
            if((*(iter-1))->warriors_here()[blue]==nullptr&&(*(iter-1))->warriors_here()[red]->Is_Alive()==false){
                (*(iter-1))->warriors_here()[red] = nullptr;
            }
        }
        _time+=3;
        if(_time>MaxTime)break;
        
        /*???:38 words*/

        for(auto iter = _cities.begin()+1 ; iter != _cities.end()-1 ; ++iter){
            (*iter)->Try_Bomb(_time);
        }
        _time+=2;
        if(_time>MaxTime)break;

        /*???:40 words*/
        std::vector<City*> redWon_cities;
        std::vector<City*> blueWon_cities;
        std::priority_queue<Warrior*,std::vector<Warrior*>,warrior_cmp<red>> redReward_warriors;
        std::priority_queue<Warrior*,std::vector<Warrior*>,warrior_cmp<blue>> blueReward_warriors;

        for(auto iter = _cities.begin()+1;iter != _cities.end()-1;++iter){
            BattleResults res =(*iter)->Battle(_time);
            switch(res){
                case redWon:{
                    redWon_cities.push_back(*iter);
                    redReward_warriors.push((*iter)->warriors_here()[red]);
                    break;
                }
                case blueWon:{
                    blueWon_cities.push_back(*iter);
                    blueReward_warriors.push((*iter)->warriors_here()[blue]);
                    break;
                }
            }
        }
        while(not redReward_warriors.empty()){
            bool res = red_hq->Reward(redReward_warriors.top(),_time);
            redReward_warriors.pop();
            if(res == false){
                break;
            }
        }
        while(not blueReward_warriors.empty()){
            bool res = blue_hq->Reward(blueReward_warriors.top(),_time);
            blueReward_warriors.pop();
            if(res == false){
                break;
            }
        }
        for(auto iter = redWon_cities.begin() ; iter != redWon_cities.end() ; ++iter){
            red_hq->GetElements(*iter,_time);
        }
        for(auto iter = blueWon_cities.begin() ; iter != blueWon_cities.end() ; ++iter){
            blue_hq->GetElements(*iter,_time);
        }

        for(auto iter = _cities.begin();iter!=_cities.end();++iter){
            if((*iter)->warriors_here()[red] != nullptr){
                if(not (*iter)->warriors_here()[red]->Is_Alive()){
                    (*iter)->warriors_here()[red] = nullptr;
                }
            }
            if((*iter)->warriors_here()[blue] != nullptr){
                if(not (*iter)->warriors_here()[blue]->Is_Alive()){
                    (*iter)->warriors_here()[blue] = nullptr;
                }
            }
        }

        _time+=10;
        if(_time>MaxTime)break;

        /*???:50 words*/
        red_hq->Report(_time);
        blue_hq->Report(_time);
        _time+=5;
        if(_time>MaxTime)break;

        /*???:55 words*/
        
        for(auto iter = _cities.begin()+1;iter != _cities.end()-1;++iter){
            if((*iter)->warriors_here()[red]!=nullptr){
                (*iter)->warriors_here()[red]->Report(_time);
            }
        }
        if(blue_hq->storage_enemy()!=nullptr){
            blue_hq->storage_enemy()->Report(_time);
        }
        if(red_hq->storage_enemy()!=nullptr){
            red_hq->storage_enemy()->Report(_time);
        }
        for(auto iter = _cities.begin()+1;iter != _cities.end()-1;++iter){
            if((*iter)->warriors_here()[blue]!=nullptr){
                (*iter)->warriors_here()[blue]->Report(_time);
            }
        }
        _time+=5;
        if(_time>MaxTime)break;
    }
}



////////////////////////////////
////          main          ////
////////////////////////////////


int main(){
    int nCase{};
    std::cin>>nCase;
    for(int i=0;i<nCase;++i){
        std::cout<<"Case "<<i+1<<':'<<std::endl;
        std::cin>>HQHealth>>CitySum>>ArrowAtk>>LoyaltyDeclineRate>>MaxTime;
        for(int j=0;j<5;++j){
            std::cin>>WarriorHp[j];
        }
        for(int j=0;j<5;++j){
            std::cin>>WarriorATK[j];
        }
        HQPosition[blue]=CitySum+1;
        Game game(CitySum);
        game.PlayGame();
    }
    return 0;
}