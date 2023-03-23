#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

using namespace std;
enum WeaponType{sword=0,bomb=1,arrow=2};
enum WarriorType{dragon=0,ninja=1,iceman=2,lion=3,wolf=4};
enum HeadquarterStategy{red=0,blue=1};
const string WeaponName[3]{"sword", "bomb","arrow"};
const string WarriorName[5]{"dragon","ninja","iceman","lion","wolf"};
int WarriorHp[5]{};
// 红方顺序:3,4,5,2,1;蓝方顺序:4,1,2,3,5;红方策略==0,蓝方策略==1
const int statgies[2][5]{{2,3,4,1,0},{3,0,1,2,4}};

int minCost{20000},iHour{};

class Headquarter;
class Warrior{
private:
    //shared attributes
    int hp;
    WeaponType weapon;
    WarriorType warriorType;
    //morale for dragon,secondWeapon for ninja,null for iceman,loyalty for lion,null for wolf
    double secondAttribute;

public: 
    Warrior()=default;
    Warrior(WarriorType wtype_,int warriorSum_,int hqHealth):hp(WarriorHp[wtype_]),warriorType(wtype_),weapon((WeaponType)(warriorSum_%3))                                               
                                                 {                                                                                                      
                                                    switch(warriorType){
                                                        case dragon:secondAttribute=(double)hqHealth/hp;break;
                                                        case ninja:secondAttribute=(warriorSum_+1)%3;break;
                                                        case lion:secondAttribute= hqHealth;break;
                                                    }
                                                 }
    void ShowExtra(){
        switch(warriorType){
            case dragon:cout<<"It has a "<<WeaponName[weapon]<<",and it's morale is "
                            <<fixed<<setprecision(2)<<secondAttribute<<endl;
                        break;
            case ninja :cout<<"It has a "<<WeaponName[weapon]<<" and a "<<
                           WeaponName[(int)(secondAttribute)]<<endl;
                        break;
            case iceman:cout<<"It has a "<<WeaponName[weapon]<<endl;
                        break;
            case lion  :cout<<"It's loyalty is "<<(int)secondAttribute<<endl;
                        break;
        }
    }
};

class Headquarter{
private:
    int health,roll,warriorCnt[5],warriorSum;
    HeadquarterStategy hqStategy;
    vector<vector<Warrior*>> warriors;
public:
    Headquarter(int health_,HeadquarterStategy stategy_):health(health_),hqStategy(stategy_),
                                                           roll(0),warriorCnt{0},warriorSum(0)
                                                           {
                                                                warriors.resize(5);
                                                           }
    ~Headquarter(){
        for (int i = 0; i < 5; ++i) {
            for (int j = 0; j < warriors[i].size(); ++j) {
                delete warriors[i][j];
            }
        }
    }
    int GiveBirth();                                                   
};

int Headquarter::GiveBirth(){
    if (roll == -1)
    {
        return 0;
    }
    else if (health < minCost)
    {
        cout << setfill('0') << setw(3) << iHour << setfill(' ') << ' '
             << (hqStategy == red ? "red" : "blue")
             << " headquarter stops making warriors" << endl;
        roll=-1;
        return 0;
    }
    else
    {
        while (health < (WarriorHp[statgies[hqStategy][roll]]))
        {
            roll = (roll + 1) % 5;
        }

        health-= (WarriorHp[statgies[hqStategy][roll]]);
        ++warriorSum;
        ++warriorCnt[roll];
        Warrior* newWarrior=new Warrior((WarriorType)(statgies[hqStategy][roll]),warriorSum,health);
        warriors[statgies[hqStategy][roll]].push_back(newWarrior);
        cout << setfill('0') << setw(3) << iHour << setfill(' ') << ' '
             << (hqStategy == red ? "red" : "blue") << ' '
             << WarriorName[statgies[hqStategy][roll]]<< ' ' << warriorSum << ' '
             << "born with strength " << WarriorHp[statgies[hqStategy][roll]] << ','
             << warriorCnt[roll] << ' ' << WarriorName[statgies[hqStategy][roll]] << " in "
             << (hqStategy == red ? "red" : "blue") << " headquarter" << endl;
        newWarrior->ShowExtra();
        roll=(roll+1)%5;
        return 1;
    }
}

int main(){
    int nCase{};
    cin>>nCase;
    for(int cnt{};cnt<nCase;++cnt){
        cout<<"Case:"<<cnt+1<<endl;
		minCost=20000;
        int maxHealth{};
        cin>>maxHealth;
        Headquarter redH(maxHealth,red),blueH(maxHealth,blue);
        for(int i=0;i<5;++i){
            cin>>WarriorHp[i];
            minCost=minCost<WarriorHp[i]?minCost:WarriorHp[i];
        }
        iHour=0;
        int retR{},retB{};
        while(1){
            retR=redH.GiveBirth();
            retB=blueH.GiveBirth();
            if(retR+retB){
                iHour++;
            }
            else{
                break;
            }
        }
    }
    return 0;
}