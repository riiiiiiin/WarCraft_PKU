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
enum IsWeaponReusable{unreuseable=0,reuseable=1};//unused
const std::string WeaponName[3]{"sword", "bomb","arrow"};
const std::string WarriorName[5]{"dragon","ninja","iceman","lion","wolf"};
int WarriorHp[5]{};
int WarriorATK[5]{};
// 红方顺序:3,4,5,2,1;蓝方顺序:4,1,2,3,5
constexpr WarriorType statgies[2][5]{{iceman,lion,wolf,ninja,dragon},{lion,dragon,ninja,iceman,wolf}};
int RoyaltyDown{};
int HQPosition[2]{};



////////////////////////////////
////         weapons        ////
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



////////////////////////////
////      warriors      ////
////////////////////////////


class Warrior{
protected:
    int _warrior_id;
    int _hp;
    int _atk;
    int _position;
    int _weapon_roll;
    WarriorType _warriortype;
    std::vector<Weapon> _weapons;
    HeadquarterStategy _HQ;
    //整理背包，需传入comparator
    virtual void _settle_inventory(bool (*comp)(Weapon const& lhs,Weapon const& rhs)){
        for(auto iter = _weapons.begin();iter != _weapons.end();){
            if(iter->durability()==0){
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
    }
public:
    Warrior(int wid,int hp,int atk,int pos,WarriorType wtype,HeadquarterStategy HQ):_warrior_id(wid),_hp(hp),_atk(atk),
                                                              _position(pos),_warriortype(wtype),_HQ(HQ){}
    virtual ~Warrior() = default;
    //无意义多态：为Ninja设计//
    inline virtual bool Hurt_oneself(){return 1;}
    //无意义多态：为Iceman&&Lion预留//
    virtual void March(){
        _position+=(_HQ==red?1:-1);
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
                _weapons.erase(_weapons.begin()+_weapon_roll);
            }
            else{
                ++_weapon_roll;
            }
        }
        else{
            ++_weapon_roll;
        }
        _weapon_roll/=_weapons.size();
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
            _weapons.push_back(*iter);
            iter=enemy._weapons.erase(iter);
        }
    }
    virtual void Yell(){}
};


class Dragon:public Warrior{
public:
    Dragon(int pos,int wid,HeadquarterStategy HQ):Warrior(wid,WarriorHp[dragon],WarriorATK[dragon],pos,dragon,HQ){
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
    Ninja(int pos,int wid,HeadquarterStategy HQ):Warrior(wid,WarriorHp[ninja],WarriorATK[ninja],pos,ninja,HQ){
        _weapons.clear();
        _initial_weapon(wid%3);
        _initial_weapon(wid%3+1);
    }
    ~Ninja(){}
    inline bool hurt_oneself(){return 0;}
};

class Iceman:public Warrior{
public:
    Iceman(int pos,int wid,HeadquarterStategy HQ):Warrior(wid,WarriorHp[iceman],WarriorATK[iceman],pos,iceman,HQ){
        _weapons.clear();
        _initial_weapon(wid%3);
    }
    ~Iceman(){}
    void March(){
        _position+=(_HQ==red?1:-1);
        _hp*=0.9;
    }
};

class Lion:public Warrior{
private:
    int _royalty;
public:
    Lion(int pos,int wid,HeadquarterStategy HQ,int HQhealth):Warrior(wid,WarriorHp[lion],WarriorATK[lion],pos,lion,HQ)
                                                              ,_royalty(HQhealth-WarriorHp[lion])
    {
        _weapons.clear();
        _initial_weapon(wid%3);
    }
    ~Lion(){}
    void March(){
        _position+=(_HQ==red?1:-1);
        _royalty-=RoyaltyDown;
    }
    void Runaway(){
        if(_position!=HQPosition[2-_HQ]){
            /*lion runs away*/
        }
    }
};

class Wolf:public Warrior{
public:
    Wolf(int pos,int wid,HeadquarterStategy HQ):Warrior(wid,WarriorHp[wolf],WarriorATK[wolf],pos,wolf,HQ){
        _weapons.clear();
    }
    ~Wolf()=default;
    void PreFight_Loot(Warrior& enemy){
        Loot(enemy);
    }
};



//////////////////////////
////       city       ////
//////////////////////////


class City{
private:
    int _city_pos;
};

class Headqurater:public City{
private:
    int _health;
    int _warrior_roll;
    int warriorSum;
    HeadquarterStategy
}




int main(){
    
}