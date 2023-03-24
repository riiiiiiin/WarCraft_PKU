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
int WarriorHp[5]{};
int WarriorATK[5]{};
// 红方顺序:3,4,5,2,1;蓝方顺序:4,1,2,3,5
constexpr WarriorType statgies[2][5]{{iceman,lion,wolf,ninja,dragon},{lion,dragon,ninja,iceman,wolf}};
int RoyaltyDeclineRate{};
int HQPosition[2]{};
int MaxTime{};
int HQHealth{};



////////////////////////////////
////        weapons         ////
////////////////////////////////


class Weapon{
protected:
    WeaponType _weapontype;
    int _durability;
    double _dmgScale;

public:
    Weapon(WeaponType wtype,int dur,double dsca):_weapontype(wtype),_durability(dur),_dmgScale(dsca){}
    virtual ~Weapon() = default;

    WeaponType weapontype(){return _weapontype;}
    int& durability(){return _durability;}
    virtual std::pair<double,double> attackDmg(double atk){
        double attack=atk*_dmgScale;
        return std::make_pair(attack,0);
    }

    friend bool comparator_tidy(Weapon const& lhs,Weapon const& rhs);
    friend bool comparator_loot(Weapon const& lhs,Weapon const& rhs);
    //for debug
    friend std::ostream& operator<<(std::ostream& os,Weapon const& dhis){
        os<<dhis._weapontype<<std::endl;
        return os;
    }
};
//战前整理
bool comparator_tidy(Weapon const& lhs,Weapon const& rhs){
    if(lhs._weapontype>rhs._weapontype){
        return true;
    }
    if(lhs._weapontype<rhs._weapontype){
        return false;
    }
    return lhs._durability>rhs._durability;
}
//掠夺前整理
bool comparator_loot(Weapon const& lhs,Weapon const& rhs){
    if(lhs._weapontype>rhs._weapontype){
        return true;
    }
    if(lhs._weapontype<rhs._weapontype){
        return false;
    }
    return lhs._durability<rhs._durability;
}

class Sword:public Weapon{
public:
    Sword():Weapon(sword,-1,0.2){}
    ~Sword() = default;
};

class Bomb:public Weapon{
public:
    Bomb():Weapon(bomb,1,0.4){}
    ~Bomb() = default;
    virtual std::pair<double,double> attackDmg(double atk){
        double attack=atk*_dmgScale;
        return std::make_pair(attack,attack/2);
    }
};

class Arrow:public Weapon{
public:
    Arrow():Weapon(arrow,2,0.3){}
    ~Arrow() = default;
};



/////////////////////////////////
////        warriors         ////
/////////////////////////////////


class Warrior{
protected:
    int _warrior_id;
    int _hp;
    int _atk;
    int _position;
    int _weapon_roll;
    int _weapon_cnt[3];
    WarriorType _warriortype;
    std::vector<Weapon> _weapons;
    HeadquarterStategy _HQ;
    //整理背包，需传入comparator
    virtual void _settle_inventory(bool (*comp)(Weapon const& lhs,Weapon const& rhs)){
        for(auto iter = _weapons.begin();iter != _weapons.end();){
            if(iter->durability()==0){
                --_weapon_cnt[iter->weapontype()];
                iter = _weapons.erase(iter);
            }
            else{
                iter++;
            } 
        }
        sort(_weapons.begin(),_weapons.end(),comp);
        return;      
    }
    //确定初始化将哪种武器装进背包
    virtual void _initial_weapon(int roll){
        switch(roll){
            case sword:{
                Sword newSword;
                _weapons.push_back(newSword);
                break;
            }               
            case bomb:{
                Bomb newBomb;
                _weapons.push_back(newBomb);
                break;
            }                
            case arrow:{
                Arrow newArrow;
                _weapons.push_back(newArrow);
                break;
            }               
        }
        ++_weapon_cnt[roll];
    }

public:
    Warrior(int wid,int hp,int atk,int pos,WarriorType wtype,HeadquarterStategy HQ_):_warrior_id(wid),_hp(hp),_atk(atk),
                                                              _position(pos),_warriortype(wtype),_HQ(HQ_),_weapon_cnt{}{}
    virtual ~Warrior() = default;

    HeadquarterStategy HQ() const& {return _HQ;}
    bool Is_Alive() const {return _hp>0;}
    std::vector<Weapon> const& weapons(){return _weapons;}
    int hp() const& {return _hp;}
    WarriorType const& warriorType(){return _warriortype;}
    //无意义多态：为Ninja设计//
    inline virtual bool Hurt_oneself(){return 1;}
    //无意义多态：为Iceman&&Lion预留//
    virtual void March(){
        _position+=(_HQ==red?1:-1);
        /*march words*/
    }
    virtual void Reach_HQ(){
        _position+=(_HQ==red?1:-1);
        /*reach words*/
    }
    //无意义多态：为lion预留
    virtual void Runaway(){}
    virtual void PreFight_Loot(Warrior& enemy){}
    virtual void Prepare_For_Fight(){
        _settle_inventory(comparator_tidy);
        _weapon_roll=0;
        //undone
    }
    virtual bool Attack(Warrior& enemy){
        if(_weapons.size()==0){
            return false;//failed
        }
        //attack
        std::pair<double,double> Dmg=_weapons[_weapon_roll].attackDmg(_atk);
        enemy._hp-=Dmg.first;
        if(Dmg.second){
            _hp-=Dmg.second*Hurt_oneself();
        }
        if(_weapons[_weapon_roll].durability()>0){
            --_weapons[_weapon_roll].durability();
            if(_weapons[_weapon_roll].durability()==0){
                --_weapon_cnt[_weapons[_weapon_roll].weapontype()];
                _weapons.erase(_weapons.begin()+_weapon_roll);
            }
            else{
                ++_weapon_roll;
            }
        }
        else{
            ++_weapon_roll;
        }
        _weapon_roll%=_weapons.size();
        return true;//success
    } 
    //抢夺对方背包，wolf或战后用
    virtual void Loot(Warrior& enemy){
        _settle_inventory(comparator_tidy);
        enemy._settle_inventory(comparator_loot);
        for(auto iter=enemy._weapons.begin();iter!=enemy._weapons.end();){
            if(_weapons.size()>=10){
                break;
            }
            ++_weapon_cnt[iter->weapontype()];
            _weapons.push_back(*iter);
            iter=enemy._weapons.erase(iter);
        }
    }
    virtual void Yell(){}
    virtual void Report(){
        /*reports weapons and hp*/
    }
};


class Dragon:public Warrior{
public:
    Dragon(int pos,int wid,HeadquarterStategy HQ_):Warrior(wid,WarriorHp[dragon],WarriorATK[dragon],pos,dragon,HQ_){
        _weapons.clear();
        _initial_weapon(wid%3);
    }
    ~Dragon(){}
    void Yell(){
        /*dragon yells*/
    }
};

class Ninja:public Warrior{
public:
    Ninja(int pos,int wid,HeadquarterStategy HQ_):Warrior(wid,WarriorHp[ninja],WarriorATK[ninja],pos,ninja,HQ_){
        _weapons.clear();
        _initial_weapon(wid%3);
        _initial_weapon(wid%3+1);
    }
    ~Ninja(){}
    inline bool hurt_oneself(){return 0;}
};

class Iceman:public Warrior{
public:
    Iceman(int pos,int wid,HeadquarterStategy HQ_):Warrior(wid,WarriorHp[iceman],WarriorATK[iceman],pos,iceman,HQ_){
        _weapons.clear();
        _initial_weapon(wid%3);
    }
    ~Iceman(){}
    void March(){
        _position+=(_HQ==red?1:-1);
        _hp*=0.9;
        /*march words*/
    }
};

class Lion:public Warrior{
private:
    int _royalty;
public:
    Lion(int pos,int wid,HeadquarterStategy HQ_,int HQhealth):Warrior(wid,WarriorHp[lion],WarriorATK[lion],pos,lion,HQ_),
                                                            _royalty(HQhealth)
    {
        _weapons.clear();
        _initial_weapon(wid%3);
    }
    ~Lion(){}
    void March(){
        _position+=(_HQ==red?1:-1);
        _royalty-=RoyaltyDeclineRate;
        /*march words*/
    }
    void Runaway(){
        if(_position!=HQPosition[1-_HQ]){
            /*lion runs away*/
        }
    }
};

class Wolf:public Warrior{
public:
    Wolf(int pos,int wid,HeadquarterStategy HQ_):Warrior(wid,WarriorHp[wolf],WarriorATK[wolf],pos,wolf,HQ_){
        _weapons.clear();
    }
    ~Wolf()=default;
    void PreFight_Loot(Warrior& enemy){
        if(enemy.warriorType()==wolf) return;
        Loot(enemy);
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
    virtual void Warrior_March(){
        //恶心死了：输出和事件应当不同步
        (this+1)->_warriors_here[red]=_warriors_here[red];
        (this-1)->_warriors_here[blue]=_warriors_here[blue];
        _warriors_here[red]=nullptr;
        _warriors_here[blue]=nullptr;
    }
    virtual void Warrior_Arrive(){
        if(_warriors_here[red]!=nullptr)
            _warriors_here[red]->March();
        if(_warriors_here[blue]!=nullptr)
            _warriors_here[blue]->March();
    }
    virtual bool Give_Birth(){}
    virtual void Battle(){}
    virtual void Report(){}
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
    bool Give_Birth();
    void Warrior_March(){
        (this+(_stategy==red?1:-1))->_warriors_here[_stategy]=_warriors_here[_stategy];
        _warriors_here[_stategy]=nullptr;
    }
    void Warrior_Arrive(){
        if(_warriors_here[1-_stategy]!=nullptr){
            _warriors_here[1-_stategy]->Reach_HQ();
            _is_overtaken=true;
        }
    }
    void Report(){
        /*reports hp*/
    }
};

bool Headquarter::Give_Birth(){
    if(_health<WarriorHp[statgies[_stategy][_warrior_roll]]){
        return false;//failed
    }
    _health-=WarriorHp[statgies[_stategy][_warrior_roll]];
    ++_warrior_sum;
    Warrior* newWarrior;
    switch(_warrior_roll){
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
    /*give birth words*/
    _own_warriors.push_back(newWarrior);
    _warriors_here[_stategy]=newWarrior;
    _warrior_roll=(_warrior_roll+1)%5;
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
        if(_warriors_here[red]->Is_Alive()&&_warriors_here[blue]->Is_Alive()){
            return bothAlive;
        }
    }
    BattleResults _battle(){
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

            std::vector<Weapon> _weapons_bak_red = _warriors_here[red]->weapons();
            std::vector<Weapon> _weapons_bak_blue = _warriors_here[blue]->weapons();
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
            if((_weapons_bak_red==_warriors_here[red]->weapons())&&(_weapons_bak_blue == _warriors_here[blue]->weapons())&&
               (hp_bak_red == _warriors_here[red]->hp())&&(hp_bak_blue == _warriors_here[blue]->hp())){
                battle_result = bothAlive;
                break;
            }
        }
        if(battle_result==redWon){
            _warriors_here[red]->Loot(*_warriors_here[blue]);
            _warriors_here[red]->Yell();
        }
        if(battle_result==blueWon){
            _warriors_here[blue]->Loot(*_warriors_here[red]);
            _warriors_here[blue]->Yell();
        }
        return battle_result;
    }

public:
    BattleCity(int city_pos):City(city_pos){}
    ~BattleCity(){}
    void Battle(){
        if(_warriors_here[red]!=nullptr&&_warriors_here[blue]!=nullptr){
            BattleResults res=_battle();
            switch(res){
                case redWon:{
                    /*battle result words*/
                    _warriors_here[blue]=nullptr;
                    break;
                }
                case blueWon:{
                    /*battle result words*/
                    _warriors_here[red]=nullptr;
                    break;
                }
                case bothDead:{
                    /*battle result words*/
                    _warriors_here[red]=nullptr;
                    _warriors_here[blue]=nullptr;
                    break;
                }
                case bothAlive:{
                    /*battle result words*/
                    break;
                }
            }
        }
    }
};

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
    void PlayGame(){
        //一个小时的循环
        while(_time<=MaxTime){
            /*???:00 words*/
            _cities[0]->Give_Birth();
            _cities[_city_sum-1]->Give_Birth();
            _time+=5;
            if(_time>MaxTime)break;

            /*???:05 words*/
            for(auto iter = _cities.begin();iter != _cities.end();++iter){
                (*iter)->warriors_here()[red]->Runaway();
                (*iter)->warriors_here()[blue]->Runaway();
            }
            _time+=5;
            if(_time>MaxTime)break;

            /*???:10 words*/
            for(auto iter = _cities.begin();iter != _cities.end();++iter){
                (*iter)->Warrior_March();
                (*iter)->Warrior_Arrive();
            }
            _time+=25;
            if(_time>MaxTime)break;

            /*???:35 words*/
            for(auto iter = _cities.begin()+1;iter != _cities.end()-1;++iter){
                if((*iter)->warriors_here()[red]!=nullptr&&(*iter)->warriors_here()[blue]!=nullptr){
                    (*iter)->warriors_here()[red]->PreFight_Loot(*(*iter)->warriors_here()[blue]);
                    (*iter)->warriors_here()[blue]->PreFight_Loot(*(*iter)->warriors_here()[red]);
                }
            }
            _time+=5;
            if(_time>MaxTime)break;

            /*???:40 words*/
            for(auto iter = _cities.begin()+1;iter != _cities.end()-1;++iter){
                (*iter)->Battle();
            }
            _time+=10;
            if(_time>MaxTime)break;

            /*???:50 words*/
            _cities[0]->Report();
            _cities[_city_sum-1]->Report();
            _time+=5;
            if(_time>MaxTime)break;

            /*???:55 words*/
            for(auto iter = _cities.begin()+1;iter != _cities.end()-1;++iter){
                if((*iter)->warriors_here()[red]!=nullptr){
                    (*iter)->warriors_here()[red]->Report();
                }
                if((*iter)->warriors_here()[blue]!=nullptr){
                    (*iter)->warriors_here()[blue]->Report();
                }
            }
            _time+=5;
            if(_time>MaxTime)break;
        }
    }
};






int main(){
    
}