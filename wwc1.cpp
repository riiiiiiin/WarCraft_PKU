#include <iostream>
#include <cstring>
#include <iomanip>
using namespace std;

class Warrior
{
public:
    char name[7];
    int healthPoint;
    Warrior(){}
    Warrior(int hp_,char* name_){
        strcpy(name,name_);
        healthPoint=hp_;
    }
    void Show(){
        cout<<healthPoint<<' '<<name<<endl;
    }
};

// 红方顺序:3,4,5,2,1;蓝方顺序:4,1,2,3,5;红方策略==0,蓝方策略==1
Warrior* warriorIn[5]{};
int statgies[2][5]{{2,3,4,1,0},{3,0,1,2,4}};
char names[5][7]{
    {"dragon"},{"ninja"},{"iceman"},{"lion"},{"wolf"}
};

int minCost{20000}, iHour;
// 战士构造顺序：iceman、lion、wolf、ninja、dragon

// 用stategy区分红蓝方
class Headquarter
{
private:
    int health;
    int stategy;
    int roll;
    int warriors[5], warriorSum;

public:
    Headquarter(int health_, int stategy_)  
    {
		health=health_;
		stategy=stategy_;
		roll=0;
		warriorSum=0;
        for (int i = 0; i < 5; i++)
        {
            warriors[i] = 0;
        }
    }
    int GiveBirth();
};

int Headquarter::GiveBirth()
{
    if (roll == -1)
    {
        return 0;
    }
    else if (health < minCost)
    {
        cout << setfill('0') << setw(3) << iHour << setfill(' ') << ' '
             << (stategy == 0 ? "red" : "blue")
             << " headquarter stops making warriors" << endl;
        roll=-1;
        return 0;
    }
    else
    {
        while (health < (warriorIn[statgies[stategy][roll]]->healthPoint))
        {
            roll = (roll + 1) % 5;
        }

        health-= (warriorIn[statgies[stategy][roll]]->healthPoint);
        warriorSum++;
        warriors[roll]++;
        cout << setfill('0') << setw(3) << iHour << setfill(' ') << ' '
             << (stategy == 0 ? "red" : "blue") << ' '
             << warriorIn[statgies[stategy][roll]]->name << ' ' << warriorSum << ' '
             << "born with strength " << warriorIn[statgies[stategy][roll]]->healthPoint << ','
             << warriors[roll] << ' ' << warriorIn[statgies[stategy][roll]]->name << " in "
             << (stategy == 0 ? "red" : "blue") << " headquarter" << endl;
        roll=(roll+1)%5;
        return 1;
    }
}

int main()
{
    int nCase{};
    cin >> nCase;
	
    for(int cnt=0;cnt<nCase;cnt++)
		
    {
        cout<<"Case:"<<cnt+1<<endl;
		minCost=20000;
        int maxHealth{};
        cin>>maxHealth;
        Headquarter redH(maxHealth,0),blueH(maxHealth,1);
        int maxHp{};
		
        for(int i=0;i<5;i++){
            cin>>maxHp;
            warriorIn[i]=new Warrior(maxHp,names[i]);
            minCost=minCost<maxHp?minCost:maxHp;
        }
        iHour=0;
        int retR{},retB{};
        while(1){
			retR=0;
			retB=0;
            retR=redH.GiveBirth();
            retB=blueH.GiveBirth();
            if(retR+retB){
                iHour++;
            }
            else{
                break;
            }
        }
for(int i=0;i<5;i++){
        delete [] warriorIn[i]; 
    }
    }
    return 0;
}