////////////////////////////////////////////
/*你说的话没错，但是《魔兽世界》中的世界更为广阔
   ，充满神秘的魔法和神力。在这个奇妙的世界里，
  你将扮演一个勇敢的冒险家，掌握强大的魔法力量，
     和各种性格迥异、技能卓越的盟友并肩战斗。
你将面临各种挑战和敌人，用你的力量和智慧打败它们，
 完成众多任务和使命，探索这个神秘而又危险的世界，
            成为真正的英雄。*/
////////////////////////////////////////////

////////////////////////////////////////////
/*要求真tmd复杂，全tmd写成一点也不优雅的补丁了
         放弃补完注释和重构代码了*/
////////////////////////////////////////////


#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>



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
constexpr WarriorType statgies[2][5]{{iceman,lion,wolf,ninja,dragon},{lion,dragon,ninja,iceman,wolf}};
int LoyaltyDeclineRate{};
int HQPosition[2]{};
int MaxTime{};
int HQHealth{};
int CitySum{};



////////////////////////////////
////        weapons         ////
////////////////////////////////


class Weapon{
protected:
    WeaponType _weapontype;
    int _durability;
    int _dmgScale;

public:
    Weapon(WeaponType wtype,int dur,int dsca):_weapontype(wtype),_durability(dur),_dmgScale(dsca){}
    virtual ~Weapon() = default;

    WeaponType weapontype(){return _weapontype;}
    int& durability(){return _durability;}
    virtual std::pair<int,int> attackDmg(int atk){
        int attack=atk*_dmgScale/10;
        std::cout<<"oops,base atkdmg used"<<std::endl;
        return std::make_pair(attack,0);
    }

    friend bool comparator_tidy(Weapon * lhs,Weapon * rhs);
    friend bool comparator_loot(Weapon * lhs,Weapon * rhs);
    friend bool operator==(Weapon const& lhs,Weapon const& rhs){
        if((lhs._weapontype==rhs._weapontype)&&(lhs._durability==rhs._durability)){
            return true;
        }
        return false;
    }
    //for debug
    friend std::ostream& operator<<(std::ostream& os,Weapon const& dhis){
        os<<dhis._weapontype<<std::endl;
        return os;
    }
};
//战前整理
bool comparator_tidy(Weapon* lhs,Weapon* rhs){
    if(lhs->_weapontype>rhs->_weapontype){
        return true;
    }
    if(lhs->_weapontype<rhs->_weapontype){
        return false;
    }
    return lhs->_durability>rhs->_durability;
}
//掠夺前整理
bool comparator_loot(Weapon* lhs,Weapon* rhs){
    if(lhs->_weapontype>rhs->_weapontype){
        return true;
    }
    if(lhs->_weapontype<rhs->_weapontype){
        return false;
    }
    return lhs->_durability<rhs->_durability;
}

class Sword:public Weapon{
public:
    Sword():Weapon(sword,-1,2){}
    ~Sword() = default;
    std::pair<int,int> attackDmg(int atk){
        int attack=atk/5;
        return std::make_pair(attack,0);
    }
};

class Bomb:public Weapon{
public:
    Bomb():Weapon(bomb,1,4){}
    ~Bomb() = default;
    std::pair<int,int> attackDmg(int atk){
        int attack=atk*2/5;
        return std::make_pair(attack,attack/2);
    }
};

class Arrow:public Weapon{
public:
    Arrow():Weapon(arrow,2,3){}
    ~Arrow() = default;
    std::pair<int,int> attackDmg(int atk){
        int attack=atk*3/10;
        return std::make_pair(attack,0);
    }
};



/////////////////////////////////
////        warriors         ////
/////////////////////////////////


class Warrior{
protected:
    int _warrior_id_;
    int _hp_;
    int _atk;
    int _position_;
    int _weapon_roll_;
    int _weapon_cnt[3];
    int _hurt_oneself_;
    WarriorType _warriortype_;
    std::vector<Weapon*> _weapons;
    HeadquarterStategy _HQ;
    //整理背包，需传入comparator
    virtual void _settle_inventory(bool (*comp)(Weapon* lhs,Weapon* rhs)){
        for(auto iter = _weapons.begin();iter != _weapons.end();){
            if((*iter)->durability()==0){
                --_weapon_cnt[(*iter)->weapontype()];
                iter = _weapons.erase(iter);
            }
            else{
                iter++;
            } 
        }
        //sort() is not useful for vector<T*>
        //considering the tiny data 
        //use bubble instead
        Weapon* tempptr= nullptr;
        //std::cout<<_weapons.size();
        if(_weapons.size()==0){
            return;
        }
        for(int i=0;i<_weapons.size()-1;++i){
            for(int j=0;j<_weapons.size()-i-1;++j){
                if(comp(_weapons[j],_weapons[j+1])){
                    tempptr = _weapons[j+1];
                    _weapons[j+1] = _weapons[j];
                    _weapons[j] = tempptr;
                }
            }
        }
        tempptr=nullptr;
        return;      
    }
    //确定初始化将哪种武器装进背包
    virtual void _initial_weapon(int roll){
        Weapon* newWeapon;
        switch(roll){
            case sword:{
                newWeapon=new Sword;
                break;
            }               
            case bomb:{
                newWeapon = new Bomb;
                break;
            }                
            case arrow:{
                newWeapon = new Arrow;
                break;
            }               
        }
        _weapons.push_back(newWeapon);
        ++_weapon_cnt[roll];
    }

public:
    Warrior(int wid,int hp,int atk,int pos,WarriorType wtype,HeadquarterStategy HQ_,int hurt_oneself=1):_warrior_id_(wid),_hp_(hp),_atk(atk),
                                                              _position_(pos),_warriortype_(wtype),_HQ(HQ_),_weapon_cnt{},_hurt_oneself_(hurt_oneself){}
    virtual ~Warrior(){
        for(auto iter=_weapons.begin();iter!=_weapons.end();++iter){
            
            delete *iter; 
            
        }
        _weapons.clear();
    }
    int* weapon_cnt(){return _weapon_cnt;}
    HeadquarterStategy HQ() const& {return _HQ;}
    bool Is_Alive() const {return _hp_>0;}
    std::vector<Weapon*> const& weapons(){return _weapons;}
    int hp() const& {return _hp_;}
    WarriorType const& warriorType(){return _warriortype_;}
    //iceman and lion should override this
    virtual void March(int const& time_){
        _position_+=(_HQ==red?1:-1);
        std::cout<<std::setw(3)<<std::setfill('0')<<time_/60<<std::setfill(' ')<<":10 "
                 <<HeadquarterName[_HQ]<<' '<<WarriorName[_warriortype_]<<' '
                 <<_warrior_id_<<" marched to city "<<_position_<<" with "
                 <<_hp_<<" elements and force "<<_atk<<std::endl;
        return;
    }
    virtual void Reach_HQ(int const& time_){
        _position_+=(_HQ==red?1:-1);
        std::cout<<std::setw(3)<<std::setfill('0')<<time_/60<<std::setfill(' ')<<":10 "
                 <<HeadquarterName[_HQ]<<' '<<WarriorName[_warriortype_]<<' '
                 <<_warrior_id_<<" reached "<<HeadquarterName[1-_HQ]<<" headquarter with "
                 <<_hp_<<" elements and force "<<_atk<<std::endl;
    }
    //lion should override this
    virtual bool Runaway(int const& time_){return false;}
    virtual void PreFight_Loot(Warrior& enemy,int const& time_){}
    virtual void Prepare_For_Fight(){
        _settle_inventory(comparator_tidy);
        _weapon_roll_=0;
        //undone
    }
    virtual bool Attack(Warrior& enemy){
        if(_weapons.size()==0){
            return false;//failed
        }
        //attack
        
        std::pair<int,int> Dmg=_weapons[_weapon_roll_]->attackDmg(_atk);
        
        enemy._hp_-=Dmg.first;
        if(Dmg.second){
            _hp_-=Dmg.second*_hurt_oneself_;
        }       
        if(_weapons[_weapon_roll_]->durability()>0){
            --_weapons[_weapon_roll_]->durability();
            
            if(_weapons[_weapon_roll_]->durability()==0){
                --_weapon_cnt[_weapons[_weapon_roll_]->weapontype()];
                delete *(_weapons.begin()+_weapon_roll_);
                _weapons.erase(_weapons.begin()+_weapon_roll_);
            }
            else{
                ++_weapon_roll_;
            }
        }
        else{
            ++_weapon_roll_;
        }
        if(_weapons.size()){
            _weapon_roll_%=_weapons.size(); 
        }
        return true;//success
    } 
    //抢夺对方背包，wolf或战后用
    virtual bool Loot(Warrior& enemy,bool loot_once=false){
        bool has_looted{false};
        _settle_inventory(comparator_tidy);
        enemy._settle_inventory(comparator_loot);
        WeaponType lastlooted = sword;
        for(auto iter=enemy._weapons.begin();iter!=enemy._weapons.end();){
            if(_weapons.size()>=10){
                break;
            }
            if((*iter)->weapontype()>lastlooted&&loot_once&&has_looted){
                break;
            }
            ++_weapon_cnt[(*iter)->weapontype()];
            --enemy._weapon_cnt[(*iter)->weapontype()];
            _weapons.push_back(*iter);
            iter=enemy._weapons.erase(iter);
            has_looted=true;
        }
        return has_looted;
    }
    virtual void Yell(int const& time_){}
    virtual void Report(int const& time_){
        std::cout<<std::setw(3)<<std::setfill('0')<<time_/60<<std::setfill(' ')<<":55 ";
        ShowBasic();
        std::cout<<" has "<<_weapon_cnt[sword]<<' '<<WeaponName[sword]<<' '
                 <<_weapon_cnt[bomb]<<' '<<WeaponName[bomb]<<' '
                 <<_weapon_cnt[arrow]<<' '<<WeaponName[arrow]<<' '
                 <<"and "<<_hp_<<" elements"<<std::endl;
    }
    virtual void ShowBasic(){
        std::cout<<HeadquarterName[_HQ]<<' '<<WarriorName[_warriortype_]<<' '
                 <<_warrior_id_;
    }
};


class Dragon:public Warrior{
public:
    Dragon(int pos,int wid,HeadquarterStategy HQ_):Warrior(wid,WarriorHp[dragon],WarriorATK[dragon],pos,dragon,HQ_){
        _weapons.clear();
        _initial_weapon(wid%3);
    }
    ~Dragon(){}
    void Yell(int const& time_){
        std::cout<<std::setw(3)<<std::setfill('0')<<time_/60<<std::setfill(' ')<<":40 ";
        ShowBasic();
        std::cout<<" yelled in city "<<_position_<<std::endl;
    }
};

class Ninja:public Warrior{
public:
    Ninja(int pos,int wid,HeadquarterStategy HQ_):Warrior(wid,WarriorHp[ninja],WarriorATK[ninja],pos,ninja,HQ_,0){
        _weapons.clear();
        _initial_weapon(wid%3);
        _initial_weapon((wid+1)%3);
    }
    ~Ninja(){}
};

class Iceman:public Warrior{
public:
    Iceman(int pos,int wid,HeadquarterStategy HQ_):Warrior(wid,WarriorHp[iceman],WarriorATK[iceman],pos,iceman,HQ_){
        _weapons.clear();
        _initial_weapon(wid%3);
    }
    ~Iceman(){}
    void March(int const& time_){
        _position_+=(_HQ==red?1:-1);
        _hp_-=_hp_/10;
        std::cout<<std::setw(3)<<std::setfill('0')<<time_/60<<std::setfill(' ')<<":10 "
                 <<HeadquarterName[_HQ]<<' '<<WarriorName[_warriortype_]<<' '
                 <<_warrior_id_<<" marched to city "<<_position_<<" with "
                 <<_hp_<<" elements and force "<<_atk<<std::endl;
        return;
    }
    virtual void Reach_HQ(int const& time_){
        _position_+=(_HQ==red?1:-1);
        _hp_-=_hp_/10;
        std::cout<<std::setw(3)<<std::setfill('0')<<time_/60<<std::setfill(' ')<<":10 "
                 <<HeadquarterName[_HQ]<<' '<<WarriorName[_warriortype_]<<' '
                 <<_warrior_id_<<" reached "<<HeadquarterName[1-_HQ]<<" headquarter with "
                 <<_hp_<<" elements and force "<<_atk<<std::endl;
    }
};

class Lion:public Warrior{
private:
    int _loyalty;
public:
    Lion(int pos,int wid,HeadquarterStategy HQ_,int HQhealth):Warrior(wid,WarriorHp[lion],WarriorATK[lion],pos,lion,HQ_),
                                                            _loyalty(HQhealth)
    {
        _weapons.clear();
        _initial_weapon(wid%3);
    }
    ~Lion(){}
    void March(int const& time_){
        _position_+=(_HQ==red?1:-1);
        _loyalty-=LoyaltyDeclineRate;
        std::cout<<std::setw(3)<<std::setfill('0')<<time_/60<<std::setfill(' ')<<":10 "
                 <<HeadquarterName[_HQ]<<' '<<WarriorName[_warriortype_]<<' '
                 <<_warrior_id_<<" marched to city "<<_position_<<" with "
                 <<_hp_<<" elements and force "<<_atk<<std::endl;
        return;
    }
    bool Runaway(int const& time_){
        if((_position_!=HQPosition[1-_HQ])&&(_loyalty<=0)){
            std::cout<<std::setw(3)<<std::setfill('0')<<time_/60<<std::setfill(' ')<<":05 "
             <<HeadquarterName[_HQ]<<' '
             <<WarriorName[_warriortype_]<<' '
             <<_warrior_id_<<" ran away"<<std::endl;
            return true;
        }
        return false;
    }
};

class Wolf:public Warrior{
public:
    Wolf(int pos,int wid,HeadquarterStategy HQ_):Warrior(wid,WarriorHp[wolf],WarriorATK[wolf],pos,wolf,HQ_){
        _weapons.clear();
    }
    ~Wolf()=default;
    void PreFight_Loot(Warrior& enemy,int const& time_){
        if(enemy.warriorType()==wolf){
            return;
        }
        int weapon_cnt_bak[3] {_weapon_cnt[sword],_weapon_cnt[bomb],_weapon_cnt[arrow]};
        if(Loot(enemy,1)){
            std::cout<<std::setw(3)<<std::setfill('0')<<time_/60<<std::setfill(' ')<<":35 ";
            ShowBasic(); 
            std::cout<<" took ";
            for(int i=0;i<3;++i){
                if(_weapon_cnt[i]-weapon_cnt_bak[i]){
                    std::cout<<_weapon_cnt[i]-weapon_cnt_bak[i]<<' '<<WeaponName[i]<<' ';
                }
            }
            std::cout<<"from ";
            enemy.ShowBasic();
            std::cout<<" in city "<<_position_<<std::endl;
        }
        return;
    }
};



//////////////////////////////
////        cities        ////
//////////////////////////////


class City{
protected:
    int _city_pos;
    std::vector<Warrior*> _warriors_here;

public:
    City(int city_pos):_city_pos(city_pos){
        _warriors_here.resize(2);
        _warriors_here[red]=nullptr;
        _warriors_here[blue]=nullptr;
    }
    virtual ~City(){
        _warriors_here.clear();
    }
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
    virtual void Battle(int const& time_){}
    virtual void Report(int const& time_){}
    virtual bool Is_Overtaken(){return false;}
};


class Headquarter:public City{
private:
    int _health;
    int _warrior_roll;
    int _warrior_sum;
    bool _is_overtaken;
    HeadquarterStategy _stategy;
    std::vector<Warrior*> _own_warriors;

public:
    Headquarter(int health,HeadquarterStategy stategy):_health(health),_warrior_roll(0),_warrior_sum(0),
                                                      _stategy(stategy),City(HQPosition[stategy]),_is_overtaken(false)
                                                      {
                                                        _own_warriors.clear();
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
            std::cout<<std::setw(3)<<std::setfill('0')<<time_/60<<std::setfill(' ')<<":10 "
                     <<HeadquarterName[_stategy]<<" headquarter was taken"<<std::endl;
            _is_overtaken=true;
        }
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
            newWarrior = new Dragon(HQPosition[_stategy],_warrior_sum,_stategy);
            break;
        }
        case ninja:{
            newWarrior = new Ninja(HQPosition[_stategy],_warrior_sum,_stategy);
            break;
        }
        case iceman:{
            newWarrior = new Iceman(HQPosition[_stategy],_warrior_sum,_stategy);
            break;
        }
        case lion:{
            newWarrior = new Lion(HQPosition[_stategy],_warrior_sum,_stategy,_health);
            break;
        }
        case wolf:{
            newWarrior = new Wolf(HQPosition[_stategy],_warrior_sum,_stategy);            
            break;
        }
    }
    _own_warriors.push_back(newWarrior);
    _warriors_here[_stategy]=newWarrior;
    _warrior_roll=(_warrior_roll+1)%5;
    //output
    std::cout<<std::setw(3)<<std::setfill('0')<<time_/60<<std::setfill(' ')<<":00 "
             <<HeadquarterName[_stategy]<<' '
             <<WarriorName[newWarrior->warriorType()]<<' '
             <<_warrior_sum<<" born"<<std::endl;
    //far from elegant lol
    if(newWarrior->warriorType()==lion){
        std::cout<<"Its loyalty is "<<_health<<std::endl;
    }
    return true;//success
}


class BattleCity:public City{
private:
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
    BattleResults _battle(int const& time_){
        _warriors_here[red]->Prepare_For_Fight();
        _warriors_here[blue]->Prepare_For_Fight();
        BattleResults battle_result{};

        while(1){
            
            //先手：1-_city_pos&1 后手：_city_pos&1
            bool attack_result[2]{};
            //检查战局情况
            if(_state_check()!=bothAlive){
                battle_result = _state_check();
                break;
            }

            int hp_bak_red = _warriors_here[red]->hp();
            int hp_bak_blue = _warriors_here[blue]->hp();

            
            //先手攻击
            attack_result[1-_city_pos&1] = _warriors_here[1-_city_pos&1]->Attack(*_warriors_here[_city_pos&1]);
            if(_state_check()!=bothAlive){
                battle_result = _state_check();
                break;
            }
                       
            //后手攻击
            attack_result[_city_pos&1] = _warriors_here[_city_pos&1]->Attack(*_warriors_here[1-_city_pos&1]);
            if(_state_check()!=bothAlive){
                battle_result = _state_check();
                break;
            }


            //双方都没有武器了
            if(attack_result[red]==false&&attack_result[blue]==false){
                battle_result = bothAlive;
                break;
            }

            //双方都在刮痧
            if((_warriors_here[red]->weapon_cnt()[bomb]==0)&&(_warriors_here[red]->weapon_cnt()[arrow]==0)&&
               (_warriors_here[blue]->weapon_cnt()[bomb]==0)&&(_warriors_here[blue]->weapon_cnt()[arrow]==0)&&
               (_warriors_here[red]->hp()==hp_bak_red)&&(_warriors_here[blue]->hp()==hp_bak_blue)){
                battle_result=bothAlive;
                break;
            }
        }
        if(battle_result==redWon){
            _warriors_here[red]->Loot(*_warriors_here[blue]);
        }
        if(battle_result==blueWon){
            _warriors_here[blue]->Loot(*_warriors_here[red]);
        }
        return battle_result;
    }

public:
    BattleCity(int city_pos):City(city_pos){}
    ~BattleCity(){}
    void Battle(int const& time_);
};

void BattleCity::Battle(int const& time_){
    if(_warriors_here[red]!=nullptr&&_warriors_here[blue]!=nullptr){
        BattleResults res=_battle(time_);
        switch(res){
            case redWon:{
                std::cout<<std::setw(3)<<std::setfill('0')<<time_/60<<std::setfill(' ')<<":40 ";
                _warriors_here[red]->ShowBasic();
                std::cout<<" killed ";
                _warriors_here[blue]->ShowBasic();
                std::cout<<" in city "<<_city_pos<<" remaining "<<_warriors_here[red]->hp()
                         <<" elements"<<std::endl;
                _warriors_here[red]->Yell(time_);

                _warriors_here[blue]=nullptr;
                break;
            }
            case blueWon:{
                std::cout<<std::setw(3)<<std::setfill('0')<<time_/60<<std::setfill(' ')<<":40 ";
                _warriors_here[blue]->ShowBasic();
                std::cout<<" killed ";
                _warriors_here[red]->ShowBasic();
                std::cout<<" in city "<<_city_pos<<" remaining "<<_warriors_here[blue]->hp()
                         <<" elements"<<std::endl;
                _warriors_here[blue]->Yell(time_);

                _warriors_here[red]=nullptr;
                break;
            }
            case bothDead:{
                std::cout<<std::setw(3)<<std::setfill('0')<<time_/60<<std::setfill(' ')<<":40 both ";
                _warriors_here[red]->ShowBasic();
                std::cout<<" and ";
                _warriors_here[blue]->ShowBasic();
                std::cout<<" died in city "<<_city_pos<<std::endl;

                _warriors_here[red]=nullptr;
                _warriors_here[blue]=nullptr;
                break;
            }
            case bothAlive:{
                std::cout<<std::setw(3)<<std::setfill('0')<<time_/60<<std::setfill(' ')<<":40 both ";
                _warriors_here[red]->ShowBasic();
                std::cout<<" and ";
                _warriors_here[blue]->ShowBasic();
                std::cout<<" were alive in city "<<_city_pos<<std::endl;
                _warriors_here[red]->Yell(time_);
                _warriors_here[blue]->Yell(time_);
                break;
            }
        }
    }
    return;
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
    while(_time<=MaxTime){
        /*???:00 words*/
        _cities[0]->Give_Birth(_time);
        _cities[_city_sum-1]->Give_Birth(_time);
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
        if(_cities[0]->Is_Overtaken()||_cities[_city_sum-1]->Is_Overtaken()){
            break;
        }
        _time+=25;
        if(_time>MaxTime)break;

        

        /*???:35 words*/
        for(auto iter = _cities.begin()+1;iter != _cities.end()-1;++iter){
            if((*iter)->warriors_here()[red]!=nullptr&&(*iter)->warriors_here()[blue]!=nullptr){
                (*iter)->warriors_here()[red]->PreFight_Loot(*(*iter)->warriors_here()[blue],_time);
                (*iter)->warriors_here()[blue]->PreFight_Loot(*(*iter)->warriors_here()[red],_time);
            }
        }
        _time+=5;
        if(_time>MaxTime)break;

        /*???:40 words*/
        for(auto iter = _cities.begin()+1;iter != _cities.end()-1;++iter){
            (*iter)->Battle(_time);
        }
        _time+=10;
        if(_time>MaxTime)break;

        /*???:50 words*/
        _cities[0]->Report(_time);
        _cities[_city_sum-1]->Report(_time);
        _time+=5;
        if(_time>MaxTime)break;

        /*???:55 words*/
        for(auto iter = _cities.begin()+1;iter != _cities.end()-1;++iter){
            if((*iter)->warriors_here()[red]!=nullptr){
                (*iter)->warriors_here()[red]->Report(_time);
            }
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
        std::cin>>HQHealth>>CitySum>>LoyaltyDeclineRate>>MaxTime;
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