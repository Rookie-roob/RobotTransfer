 #include <bits/stdc++.h>
#include<fstream>
using namespace std;
fstream f1;

bool if_open_itemlist = false;
bool if_rb_itemlist[10]={false};
const int n = 200;
const int robot_num = 10;
const int berth_num = 10;
const int N = 210;
//int urgent_rb_id = -1;

int money, boat_capacity, id;
char ch[N][N];
int gds[N][N];
int zhen;
int park_item[10]={0};
bool in_recover[10]={false};
int robot_status[10] = {0};
//int rb_idle_time[10]={0};


struct GridLocation {
    int x, y;
};
bool is_park_pos(int x,int y)
{
    if(x>0&&ch[x-1+1][y+1]=='.')
        return true;
    else if(x<199&&ch[x+1+1][y+1]=='.')
        return true;
    else if(y>0&&ch[x+1][y-1+1]=='.')
        return true;
    else if(y<199&&ch[x+1][y+1+1]=='.')
        return true;
    else
        return false;
}
int my_abs(int a)
{
    return a>=0?a:-a;
}
struct Robot
{
    int x, y, goods;
    int status;
    int mbx, mby;
    int max_search = 20;
    int rs = 0;//0:initial    1:not in maze    -1:in maze
    Robot() {}
    Robot(int startX, int startY) {
        x = startX;
        y = startY;
    }
}robot[robot_num + 10];
struct SquareGrid {
    static std::array<GridLocation, 4> DIRS0;
    static std::array<GridLocation, 4> DIRS1;
    static std::array<GridLocation, 4> DIRS2;
    static std::array<GridLocation, 4> DIRS3;

    int width, height;

    SquareGrid(int width_, int height_)
        : width(width_), height(height_) {}

    bool in_bounds(GridLocation id) const {
        return 0 <= id.x && id.x < width
            && 0 <= id.y && id.y < height;
    }

    bool passable(int rb_id,GridLocation id) const {
        bool j =  ch[id.x+1][id.y+1]!='*'&&ch[id.x+1][id.y+1]!='#';
        if(!j) return false;
        /*
            for(int i=0;i<10;i++)
        {
            if(i==rb_id) continue;
            if(robot[i].x==id.x&&robot[i].y==id.y) return false;
        }
        */
        
        return true;
    }

    std::vector<GridLocation> neighbors(int rb_id,GridLocation id,int dir_status) const {
        std::vector<GridLocation> results;
        if(dir_status==0)
        {
            for (GridLocation dir : DIRS0) {
                GridLocation next{ id.x + dir.x, id.y + dir.y };
                if (in_bounds(next) && passable(rb_id,next)) {
                    results.push_back(next);
                }
            }
            if ((id.x + id.y) % 2 == 0) {
                // aesthetic improvement on square grids
                if(results.size()<=2)
                {
                    std::reverse(results.begin(), results.end());
                }
                else
                {
                    std::reverse(results.begin(), results.begin()+2);
                    std::reverse(results.begin()+2, results.end());
                }
            }
        }
        else if(dir_status==1)
        {
            for (GridLocation dir : DIRS1) {
                GridLocation next{ id.x + dir.x, id.y + dir.y };
                if (in_bounds(next) && passable(rb_id,next)) {
                    results.push_back(next);
                }
            }
            if ((id.x + id.y) % 2 == 0) {
                // aesthetic improvement on square grids
                if(results.size()<=2)
                {
                    std::reverse(results.begin(), results.end());
                }
                else
                {
                    std::reverse(results.begin(), results.begin()+2);
                    std::reverse(results.begin()+2, results.end());
                }
            }
        }
        else if(dir_status==2)
        {
            for (GridLocation dir : DIRS2) {
                GridLocation next{ id.x + dir.x, id.y + dir.y };
                if (in_bounds(next) && passable(rb_id,next)) {
                    results.push_back(next);
                }
            }
            if ((id.x + id.y) % 2 == 0) {
                // aesthetic improvement on square grids
                if(results.size()<=2)
                {
                    std::reverse(results.begin(), results.end());
                }
                else
                {
                    std::reverse(results.begin(), results.begin()+2);
                    std::reverse(results.begin()+2, results.end());
                }
            }
        }
        else
        {
            for (GridLocation dir : DIRS3) {
                GridLocation next{ id.x + dir.x, id.y + dir.y };
                if (in_bounds(next) && passable(rb_id,next)) {
                    results.push_back(next);
                }
            }
            if ((id.x + id.y) % 2 == 0) {
                // aesthetic improvement on square grids
                if(results.size()<=2)
                {
                    std::reverse(results.begin(), results.end());
                }
                else
                {
                    std::reverse(results.begin(), results.begin()+2);
                    std::reverse(results.begin()+2, results.end());
                }
            }
        }
        return results;
    }
    int cost(GridLocation from_node, GridLocation to_node) const {
        return 1;
    }
};
std::array<GridLocation, 4> SquareGrid::DIRS0 =
{ GridLocation{ -1, 0 }, GridLocation{ 0, -1 }, GridLocation{ 1, 0 }, GridLocation{ 0, 1 } };
std::array<GridLocation, 4> SquareGrid::DIRS1 =
{ GridLocation{ -1, 0 }, GridLocation{ 0, 1 }, GridLocation{0, -1}, GridLocation{ 1, 0} };
std::array<GridLocation, 4> SquareGrid::DIRS2 =
{ GridLocation{ 1, 0 }, GridLocation{ 0, 1 }, GridLocation{0, -1}, GridLocation{ -1, 0} };
std::array<GridLocation, 4> SquareGrid::DIRS3 =
{ GridLocation{ 1, 0 }, GridLocation{ 0, -1 }, GridLocation{-1, 0}, GridLocation{ 0, 1} };
SquareGrid grid(200, 200);

GridLocation operator - (GridLocation a, GridLocation b) {
    return GridLocation{a.x-b.x,a.y-b.y};
}

bool operator == (GridLocation a, GridLocation b) {
    return a.x == b.x && a.y == b.y;
}

bool operator != (GridLocation a, GridLocation b) {
    return !(a == b);
}

bool operator < (GridLocation a, GridLocation b) {
    return std::tie(a.x, a.y) < std::tie(b.x, b.y);
}
inline int heuristic(GridLocation a, GridLocation b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

std::map<GridLocation, GridLocation> came_from[10];
std::map<GridLocation, int> cost_so_far[10];
GridLocation park_pos[10];
GridLocation park_pos2[10];
GridLocation park_pos3[10];
GridLocation park_pos4[10];



struct Berth
{
    int x;
    int y;
    int transport_time;
    int loading_speed;
    Berth(){}
    Berth(int x, int y, int transport_time, int loading_speed) {
        this -> x = x;
        this -> y = y;
        this -> transport_time = transport_time;
        this -> loading_speed = loading_speed;
    }
}berth[berth_num + 10];

struct Boat
{
    int num, pos, status;
}boat[10];

struct ItemNode {
    int x;
    int y;
    int expire_time;
    int val;
    int dist;
    ItemNode* nextnode;
    ItemNode* prevnode;
    bool is_chosen;
    int max_search=5;
    ItemNode(){}
    ItemNode(int x1,int t1,int et,int v):x(x1),y(t1),expire_time(et),val(v),nextnode(nullptr),prevnode(nullptr),is_chosen(false)
    {
    }
    ItemNode(ItemNode* nextnode,ItemNode* prevnode):nextnode(nullptr),prevnode(nullptr),is_chosen(false)
    {
    }
    ItemNode(int x1,int t1,int et,int v,ItemNode* nn,ItemNode* pn):x(x1),y(t1),expire_time(et),val(v),nextnode(nn),prevnode(pn),is_chosen(false)
    {
    }
};
ItemNode* dummyhead;
ItemNode* park_dummyhead[10];

void addItem(int zhen,int x,int y,int val)
{
    if((!if_open_itemlist)||(zhen%2==0))
    {
        ItemNode* in = new ItemNode(x,y,zhen+1000,val);
        ItemNode* pn = dummyhead->prevnode;
        ItemNode* cur = dummyhead;
        while(pn!=dummyhead&&pn->val>val)
        {
            pn=pn->prevnode;
            cur=cur->prevnode;
        }
        pn->nextnode=in;
        in->nextnode=cur;
        cur->prevnode=in;
        in->prevnode=pn;
    }
    else
    {
        int dis = (my_abs(x-park_pos[0].x)+my_abs(y-park_pos[0].y));
        int index = 0;
        for(int i=1;i<10;i++)
        {
            if((my_abs(x-park_pos[i].x)+my_abs(y-park_pos[i].y))<dis)
            {
                dis=(my_abs(x-park_pos[i].x)+my_abs(y-park_pos[i].y));
                index=i;
            }
        }
        ItemNode* in = new ItemNode(x,y,zhen+1000,val);
        //in->dist=my_abs(x-park_pos[index].x)+my_abs(y-park_pos[index].y);
        ItemNode* pn = park_dummyhead[index]->prevnode;
        ItemNode* cur = park_dummyhead[index];
        while(pn!=park_dummyhead[index]&&pn->val>val)
        {
            pn=pn->prevnode;
            cur=cur->prevnode;
        }
        pn->nextnode=in;
        in->nextnode=cur;
        cur->prevnode=in;
        in->prevnode=pn;
    }
    
}
void deleteItem(int zhen)
{
    ItemNode* cur = dummyhead->nextnode;
    ItemNode* pre = dummyhead;
    while(cur!=dummyhead&&(cur->expire_time<=zhen||cur->is_chosen==true))
    {
        ItemNode* ne = cur->nextnode;
        pre->nextnode=ne;
        ne->prevnode=pre;
        delete cur;
        cur=ne;
    }
    for(int u=0;u<10;u++)
    {
        cur = park_dummyhead[u]->nextnode;
        pre = park_dummyhead[u];
        while(cur!=park_dummyhead[u]&&(cur->expire_time<=zhen||cur->is_chosen==true))
        {
            ItemNode* ne = cur->nextnode;
            pre->nextnode=ne;
            ne->prevnode=pre;
            delete cur;
            cur=ne;
        }
    }
}

void deleteNode(ItemNode* cur)
{
    ItemNode* pre = cur->prevnode;
    ItemNode* ne = cur->nextnode;
    pre->nextnode=ne;
    ne->prevnode=pre;
    delete cur;
}

int park_x;
int park_y;
GridLocation park_goal;
std::map<GridLocation,int> move_dic;
void Init()
{
    move_dic[GridLocation{0,1}]=0;
    move_dic[GridLocation{0,-1}]=1;
    move_dic[GridLocation{-1,0}]=2;
    move_dic[GridLocation{1,0}]=3;
    for(int i = 1; i <= n; i ++)
        scanf("%s", ch[i] + 1);
    for(int i = 0; i < berth_num; i ++)
    {
        int id;
        scanf("%d", &id);
        scanf("%d%d%d%d", &berth[id].x, &berth[id].y, &berth[id].transport_time, &berth[id].loading_speed);
        if(is_park_pos(berth[id].x,berth[id].y))
        {
            park_pos[i]=GridLocation{berth[id].x,berth[id].y};
        }
        else if(is_park_pos(berth[id].x+3,berth[id].y+3))
        {
            park_pos[i]=GridLocation{berth[id].x+3,berth[id].y+3};
        }
        else if(is_park_pos(berth[id].x+3,berth[id].y))
        {
            park_pos[i]=GridLocation{berth[id].x+3,berth[id].y};
        }
        else
        {
            park_pos[i]=GridLocation{berth[id].x,berth[id].y+3};
        }


        if(is_park_pos(berth[id].x,berth[id].y)&&(park_pos[i].x!=berth[id].x||park_pos[i].y!=berth[id].y))
        {
            park_pos2[i]=GridLocation{berth[id].x,berth[id].y};
        }
        else if(is_park_pos(berth[id].x+3,berth[id].y+3)&&(park_pos[i].x!=(berth[id].x+3)||park_pos[i].y!=(berth[id].y+3)))
        {
            park_pos2[i]=GridLocation{berth[id].x+3,berth[id].y+3};
        }
        else if(is_park_pos(berth[id].x+3,berth[id].y)&&(park_pos[i].x!=(berth[id].x+3)||park_pos[i].y!=(berth[id].y)))
        {
            park_pos2[i]=GridLocation{berth[id].x+3,berth[id].y};
        }
        else if(is_park_pos(berth[id].x,berth[id].y+3)&&(park_pos[i].x!=(berth[id].x)||park_pos[i].y!=(berth[id].y+3)))
        {
            park_pos2[i]=GridLocation{berth[id].x,berth[id].y+3};
        }
        else if(is_park_pos(berth[id].x,berth[id].y+1)&&(park_pos[i].x!=(berth[id].x)||park_pos[i].y!=(berth[id].y+1)))
        {
            park_pos2[i]=GridLocation{berth[id].x,berth[id].y+1};
        }
        else if(is_park_pos(berth[id].x,berth[id].y+2)&&(park_pos[i].x!=(berth[id].x)||park_pos[i].y!=(berth[id].y+2)))
        {
            park_pos2[i]=GridLocation{berth[id].x+3,berth[id].y};
        }
        else if(is_park_pos(berth[id].x+3,berth[id].y+1)&&(park_pos[i].x!=(berth[id].x+3)||park_pos[i].y!=(berth[id].y+1)))
        {
            park_pos2[i]=GridLocation{berth[id].x+3,berth[id].y+1};
        }
        else if(is_park_pos(berth[id].x+3,berth[id].y+2)&&(park_pos[i].x!=(berth[id].x+3)||park_pos[i].y!=(berth[id].y+2)))
        {
            park_pos2[i]=GridLocation{berth[id].x+3,berth[id].y+2};
        }
        else if(is_park_pos(berth[id].x+1,berth[id].y+3)&&(park_pos[i].x!=(berth[id].x+1)||park_pos[i].y!=(berth[id].y+3)))
        {
            park_pos2[i]=GridLocation{berth[id].x+1,berth[id].y+3};
        }
        else if(is_park_pos(berth[id].x+2,berth[id].y+3)&&(park_pos[i].x!=(berth[id].x+2)||park_pos[i].y!=(berth[id].y+3)))
        {
            park_pos2[i]=GridLocation{berth[id].x+2,berth[id].y+3};
        }
        else if(is_park_pos(berth[id].x+1,berth[id].y)&&(park_pos[i].x!=(berth[id].x+1)||park_pos[i].y!=(berth[id].y)))
        {
            park_pos2[i]=GridLocation{berth[id].x+1,berth[id].y};
        }
        else if(is_park_pos(berth[id].x+2,berth[id].y)&&(park_pos[i].x!=(berth[id].x+2)||park_pos[i].y!=(berth[id].y)))
        {
            park_pos2[i]=GridLocation{berth[id].x+2,berth[id].y};
        }


        if(is_park_pos(berth[id].x+3,berth[id].y)&&(park_pos[i].x!=(berth[id].x+3)||park_pos[i].y!=(berth[id].y))&&(park_pos2[i].x!=(berth[id].x+3)||park_pos2[i].y!=(berth[id].y)))
        {
            park_pos3[i]=GridLocation{berth[id].x+3,berth[id].y};
        }
        else if(is_park_pos(berth[id].x,berth[id].y+3)&&(park_pos[i].x!=(berth[id].x)||park_pos[i].y!=(berth[id].y+3))&&(park_pos2[i].x!=(berth[id].x)||park_pos2[i].y!=(berth[id].y+3)))
        {
            park_pos3[i]=GridLocation{berth[id].x,berth[id].y+3};
        }
        else if(is_park_pos(berth[id].x,berth[id].y+1)&&(park_pos[i].x!=(berth[id].x)||park_pos[i].y!=(berth[id].y+1))&&(park_pos2[i].x!=(berth[id].x)||park_pos2[i].y!=(berth[id].y+1)))
        {
            park_pos3[i]=GridLocation{berth[id].x,berth[id].y+1};
        }
        else if(is_park_pos(berth[id].x,berth[id].y+2)&&(park_pos[i].x!=(berth[id].x)||park_pos[i].y!=(berth[id].y+2))&&(park_pos2[i].x!=(berth[id].x)||park_pos2[i].y!=(berth[id].y+2)))
        {
            park_pos3[i]=GridLocation{berth[id].x+3,berth[id].y};
        }
        else if(is_park_pos(berth[id].x+3,berth[id].y+1)&&(park_pos[i].x!=(berth[id].x+3)||park_pos[i].y!=(berth[id].y+1))&&(park_pos2[i].x!=(berth[id].x+3)||park_pos2[i].y!=(berth[id].y+1)))
        {
            park_pos3[i]=GridLocation{berth[id].x+3,berth[id].y+1};
        }
        else if(is_park_pos(berth[id].x+3,berth[id].y+2)&&(park_pos[i].x!=(berth[id].x+3)||park_pos[i].y!=(berth[id].y+2))&&(park_pos2[i].x!=(berth[id].x+3)||park_pos2[i].y!=(berth[id].y+2)))
        {
            park_pos3[i]=GridLocation{berth[id].x+3,berth[id].y+2};
        }
        else if(is_park_pos(berth[id].x+1,berth[id].y+3)&&(park_pos[i].x!=(berth[id].x+1)||park_pos[i].y!=(berth[id].y+3))&&(park_pos2[i].x!=(berth[id].x+1)||park_pos2[i].y!=(berth[id].y+3)))
        {
            park_pos3[i]=GridLocation{berth[id].x+1,berth[id].y+3};
        }
        else if(is_park_pos(berth[id].x+2,berth[id].y+3)&&(park_pos[i].x!=(berth[id].x+2)||park_pos[i].y!=(berth[id].y+3))&&(park_pos2[i].x!=(berth[id].x+2)||park_pos2[i].y!=(berth[id].y+3)))
        {
            park_pos3[i]=GridLocation{berth[id].x+2,berth[id].y+3};
        }
        else if(is_park_pos(berth[id].x+1,berth[id].y)&&(park_pos[i].x!=(berth[id].x+1)||park_pos[i].y!=(berth[id].y))&&(park_pos2[i].x!=(berth[id].x+1)||park_pos2[i].y!=(berth[id].y)))
        {
            park_pos3[i]=GridLocation{berth[id].x+1,berth[id].y};
        }
        else if(is_park_pos(berth[id].x+2,berth[id].y)&&(park_pos[i].x!=(berth[id].x+2)||park_pos[i].y!=(berth[id].y))&&(park_pos2[i].x!=(berth[id].x+2)||park_pos2[i].y!=(berth[id].y)))
        {
            park_pos3[i]=GridLocation{berth[id].x+2,berth[id].y};
        }


        if(is_park_pos(berth[id].x+3,berth[id].y)&&(park_pos[i].x!=(berth[id].x+3)||park_pos[i].y!=(berth[id].y))&&(park_pos2[i].x!=(berth[id].x+3)||park_pos2[i].y!=(berth[id].y))&&(park_pos3[i].x!=(berth[id].x+3)||park_pos3[i].y!=(berth[id].y)))
        {
            park_pos4[i]=GridLocation{berth[id].x+3,berth[id].y};
        }
        else if(is_park_pos(berth[id].x,berth[id].y+3)&&(park_pos[i].x!=(berth[id].x)||park_pos[i].y!=(berth[id].y+3))&&(park_pos2[i].x!=(berth[id].x)||park_pos2[i].y!=(berth[id].y+3))&&(park_pos3[i].x!=(berth[id].x)||park_pos3[i].y!=(berth[id].y+3)))
        {
            park_pos4[i]=GridLocation{berth[id].x,berth[id].y+3};
        }
        else if(is_park_pos(berth[id].x,berth[id].y+1)&&(park_pos[i].x!=(berth[id].x)||park_pos[i].y!=(berth[id].y+1))&&(park_pos2[i].x!=(berth[id].x)||park_pos2[i].y!=(berth[id].y+1))&&(park_pos3[i].x!=(berth[id].x)||park_pos3[i].y!=(berth[id].y+1)))
        {
            park_pos4[i]=GridLocation{berth[id].x,berth[id].y+1};
        }
        else if(is_park_pos(berth[id].x,berth[id].y+2)&&(park_pos[i].x!=(berth[id].x)||park_pos[i].y!=(berth[id].y+2))&&(park_pos2[i].x!=(berth[id].x)||park_pos2[i].y!=(berth[id].y+2))&&(park_pos3[i].x!=(berth[id].x)||park_pos3[i].y!=(berth[id].y+2)))
        {
            park_pos4[i]=GridLocation{berth[id].x+3,berth[id].y};
        }
        else if(is_park_pos(berth[id].x+3,berth[id].y+1)&&(park_pos[i].x!=(berth[id].x+3)||park_pos[i].y!=(berth[id].y+1))&&(park_pos2[i].x!=(berth[id].x+3)||park_pos2[i].y!=(berth[id].y+1))&&(park_pos3[i].x!=(berth[id].x+3)||park_pos3[i].y!=(berth[id].y+1)))
        {
            park_pos4[i]=GridLocation{berth[id].x+3,berth[id].y+1};
        }
        else if(is_park_pos(berth[id].x+3,berth[id].y+2)&&(park_pos[i].x!=(berth[id].x+3)||park_pos[i].y!=(berth[id].y+2))&&(park_pos2[i].x!=(berth[id].x+3)||park_pos2[i].y!=(berth[id].y+2))&&(park_pos3[i].x!=(berth[id].x+3)||park_pos3[i].y!=(berth[id].y+2)))
        {
            park_pos4[i]=GridLocation{berth[id].x+3,berth[id].y+2};
        }
        else if(is_park_pos(berth[id].x+1,berth[id].y+3)&&(park_pos[i].x!=(berth[id].x+1)||park_pos[i].y!=(berth[id].y+3))&&(park_pos2[i].x!=(berth[id].x+1)||park_pos2[i].y!=(berth[id].y+3))&&(park_pos3[i].x!=(berth[id].x+1)||park_pos3[i].y!=(berth[id].y+3)))
        {
            park_pos4[i]=GridLocation{berth[id].x+1,berth[id].y+3};
        }
        else if(is_park_pos(berth[id].x+2,berth[id].y+3)&&(park_pos[i].x!=(berth[id].x+2)||park_pos[i].y!=(berth[id].y+3))&&(park_pos2[i].x!=(berth[id].x+2)||park_pos2[i].y!=(berth[id].y+3))&&(park_pos3[i].x!=(berth[id].x+2)||park_pos3[i].y!=(berth[id].y+3)))
        {
            park_pos4[i]=GridLocation{berth[id].x+2,berth[id].y+3};
        }
        else if(is_park_pos(berth[id].x+1,berth[id].y)&&(park_pos[i].x!=(berth[id].x+1)||park_pos[i].y!=(berth[id].y))&&(park_pos2[i].x!=(berth[id].x+1)||park_pos2[i].y!=(berth[id].y))&&(park_pos3[i].x!=(berth[id].x+1)||park_pos3[i].y!=(berth[id].y)))
        {
            park_pos4[i]=GridLocation{berth[id].x+1,berth[id].y};
        }
        else if(is_park_pos(berth[id].x+2,berth[id].y)&&(park_pos[i].x!=(berth[id].x+2)||park_pos[i].y!=(berth[id].y))&&(park_pos2[i].x!=(berth[id].x+2)||park_pos2[i].y!=(berth[id].y))&&(park_pos3[i].x!=(berth[id].x+2)||park_pos3[i].y!=(berth[id].y)))
        {
            park_pos4[i]=GridLocation{berth[id].x+2,berth[id].y};
        }
    }
    scanf("%d", &boat_capacity);
    char okk[100];
    scanf("%s", okk);
    printf("OK\n");
    fflush(stdout);
}

bool canAddItem(int x,int y)
{
    for(int i=0;i<10;i++)
    {
        if((my_abs(x-park_pos[i].x)+my_abs(y-park_pos[i].y))<=50)
            return true;
    }
    return false;
}
int recover_zhen[10]={0};
int Input()
{
    if(!if_open_itemlist)
    {
        int number = 0;
        for(int i=0;i<10;i++)
        {
            if(if_rb_itemlist[i]==true||robot[i].rs==-1)
            {
                number++;
            }    
        }
        if(number>=8)
        {
            if_open_itemlist=true;
        }
    }
    scanf("%d%d", &id, &money);
    int num;
    scanf("%d", &num);
    
    for(int i = 1; i <= num; i ++)
    {
        int x, y, val;
        scanf("%d%d%d", &x, &y, &val);
        if(canAddItem(x,y))
            addItem(zhen,x,y,val);
    }
    
    for(int i = 0; i < robot_num; i ++)
    {
        int sts;
        scanf("%d%d%d%d", &robot[i].goods, &robot[i].x, &robot[i].y, &sts);
        if(in_recover[i]==false&&sts==0){
            in_recover[i]=true;
            if(robot[i].goods==0)
            {
                robot_status[i]=10;
                recover_zhen[i]=zhen+18;
            }
            else if(robot[i].goods==1)
            {
                robot_status[i]=11;
                if(ch[robot[i].x+1][robot[i].y+1]!='B')
                    recover_zhen[i]=zhen+18;
                else 
                    recover_zhen[i]=zhen+19;
            }
        }
    }
    for(int i = 0; i < 5; i ++)
        scanf("%d%d\n", &boat[i].status, &boat[i].pos);
    char okk[100];
    scanf("%s", okk);
    return id;
}

template<typename T, typename priority_t>
struct PriorityQueue {
    typedef std::pair<priority_t, T> PQElement;
    std::priority_queue<PQElement, std::vector<PQElement>,
        std::greater<PQElement>> elements;

    inline bool empty() const {
        return elements.empty();
    }

    inline void put(T item, priority_t priority) {
        elements.emplace(priority, item);
    }

    T get() {
        T best_item = elements.top().second;
        elements.pop();
        return best_item;
    }
};

GridLocation cur_goal;
vector<vector<GridLocation>> path(10,vector<GridLocation>());
int from_zhen[10]={0};

bool judge_rb_crash(GridLocation& next,int curstep,int idx)
{
    for(int i=0;i<10;i++)
    {
        if(i==idx)
            continue;
        if(path[i].size()==0)
            continue;
        int judge_idx1 = curstep+zhen-from_zhen[i];
        int judge_idx2 = curstep+zhen+1-from_zhen[i];
        if(judge_idx1<path[i].size()&&path[i][judge_idx1]==next) return true;
        if(judge_idx2<path[i].size()&&path[i][judge_idx2]==next) return true;
    }

    return false;
}
int cnt=1;
void a_star_search(int i,GridLocation& start,GridLocation& goal)
{
    
    PriorityQueue<pair<GridLocation,int>, int> frontier;
    frontier.put(pair<GridLocation,int>{start,0}, 0);
    came_from[i].clear();
    cost_so_far[i].clear();
    came_from[i][start] = start;
    cost_so_far[i][start] = 0;
    if(cnt>3000) return;
    int dir_status = -1;
    while (!frontier.empty()) {
        pair<GridLocation,int> currentpair = frontier.get();
        GridLocation current = currentpair.first;
        int curstep = currentpair.second;

        if(current.x>=goal.x&&current.y>=goal.y)
        {
            dir_status=0;
        }
        else if(current.x>=goal.x&&current.y<=goal.y)
        {
            dir_status=1;
        }
        else if(current.x<=goal.x&&current.y>=goal.y)
        {
            dir_status=3;
        }
        else
        {
            dir_status=2;
        }
        if (current == goal) {
            break;
        }
        for (GridLocation next : grid.neighbors(i,current,dir_status)) {
            int new_cost = cost_so_far[i][current] + grid.cost(current, next);
            if (cost_so_far[i].find(next) == cost_so_far[i].end()
                || new_cost < cost_so_far[i][next]) {
                if(judge_rb_crash(next,curstep,i))
                    continue;
                cost_so_far[i][next] = new_cost;
                int priority = new_cost + heuristic(next, goal);
                frontier.put({next,curstep+1}, priority);
                cnt++;
                if(cnt>3000) {
                    return;
                }
                
                came_from[i][next] = current;
            }
        }
    }
}

std::vector<GridLocation> reconstruct_path(
    GridLocation start, GridLocation goal,
    std::map<GridLocation, GridLocation>& came_from1
) {
        
    std::vector<GridLocation> path;
    if(came_from1.find(goal)==came_from1.end())
        return path;
    GridLocation current = goal;
    while (current != start) {
        path.push_back(current);
        current = came_from1[current];
    }
    path.push_back(start); // optional
    std::reverse(path.begin(), path.end());
    //if(zhen==80)
        //printpath(path);
    return path;
}


int move_robot(int i,int zhen)
{
    return move_dic[path[i][zhen-from_zhen[i]+1]-path[i][zhen-from_zhen[i]]];
}
int dir[10] = {-1};

int boat_status[5] = {0};






int to_which_park2(int j,Robot& rb,GridLocation pp[10])
{
    int dis = (my_abs(rb.x-pp[0].x)+my_abs(rb.y-pp[0].y));
    int index = 0;
    for(int t=zhen;t<(zhen+10);t++)
    {
        int i = t%10;
        if((my_abs(rb.x-pp[i].x)+my_abs(rb.y-pp[i].y))<dis)
        {
            index=i;
            dis=my_abs(rb.x-pp[i].x)+my_abs(rb.y-pp[i].y);
        }
            
    }
    return index;
}
int to_which_park(int j,Robot& rb,GridLocation* pp)
{
    for(int t=zhen;t<(zhen+10);t++)
    {
        int i = t%10;
        if((my_abs(rb.x-pp[i].x)+my_abs(rb.y-pp[i].y))<=80)
            return i;
    }
    return j;//random number
}

int to_park[10];
map<int,set<uintptr_t>> rb_wall;
int limited_times[10]={0};
bool is_already_park(int x,int y)
{
    if(ch[x+1][y+1]!='B') return false;
    if(x>0&&ch[x-1+1][y+1]=='.') return true;
    if(x<199&&ch[x+1+1][y+1]=='.') return true;
    if(y>0&&ch[x+1][y-1+1]=='.') return true;
    if(y<199&&ch[x+1][y+1+1]=='.') return true;
    return false;
}
void process_robot()
{
    int max_astar = 5;
         for(int q=zhen;q<(zhen+10);q++)
        {
            int i = q%10;
            if(robot[i].rs==-1)
                continue;
            dir[i]=-1;
            if(robot_status[i]==0||robot_status[i]==4)
            {
                robot_status[i]=0;
                if(max_astar<=0)
                    continue;
                if(if_open_itemlist)
                {
                    ItemNode* cur = park_dummyhead[to_park[i]]->prevnode;
                    int maxsearch = 30;
                    while(cur!=park_dummyhead[to_park[i]]&&maxsearch>0&&(cur->is_chosen==true||((my_abs(cur->x-robot[i].x)+my_abs(cur->y-robot[i].y))>=70)||(rb_wall[i].find((uintptr_t)cur)!=rb_wall[i].end())))
                    {
                        cur=cur->prevnode;
                        maxsearch--;
                    }
                    if(cur==park_dummyhead[to_park[i]]||maxsearch==0)
                    {
                        cur = dummyhead->prevnode;
                        maxsearch = 20;
                        while(cur!=dummyhead&&maxsearch>0&&(cur->is_chosen==true||((my_abs(cur->x-robot[i].x)+my_abs(cur->y-robot[i].y))>=80)||(rb_wall[i].find((uintptr_t)cur)!=rb_wall[i].end())||((my_abs(cur->x-robot[i].x)+my_abs(cur->y-robot[i].y)+8)>(cur->expire_time-zhen))))
                        {
                            cur=cur->prevnode;
                            maxsearch--;
                        }
                        if(cur==dummyhead||maxsearch==0)
                        {
                            continue;
                        }
                        else
                        {
                            cur_goal=GridLocation{cur->x,cur->y};
                            cur->is_chosen=true;
                            deleteNode(cur);
                        }
                    }
                    else
                    {
                            cur_goal=GridLocation{cur->x,cur->y};
                            cur->is_chosen=true;
                            deleteNode(cur);
                    }
                    came_from[i].clear();
                    cost_so_far[i].clear();
                    GridLocation start_pos = {robot[i].x,robot[i].y};                
                    a_star_search(i,start_pos,cur_goal);
                    max_astar--;
                    path[i]=reconstruct_path(start_pos, cur_goal, came_from[i]);
                    if(path[i].size()==0)
                    {
                        rb_wall[i].insert((uintptr_t)cur);
                        robot[i].max_search--;
                        if(robot[i].max_search<=0&&robot[i].rs==0)
                            robot[i].rs=-1;
                        cur->max_search--;
                        if(cur->max_search==0)
                        {
                            deleteNode(cur);
                            rb_wall[i].erase((uintptr_t)cur);
                        }
                    }
                    else
                    {
                        robot[i].rs=1;
                        from_zhen[i]=zhen;
                        rb_wall[i].clear();
                        bool jdg = false;
                        for(int m=0;m<10;m++)
                        {
                            if(m==i) continue;
                            if(path[i][zhen-from_zhen[i]+1]==GridLocation{robot[m].x,robot[m].y})
                            {
                                jdg=true;
                                break;
                            }
                        }
                        if(jdg)
                        {
                            path[i].clear();
                            robot_status[i]=0;
                            continue;
                        }
                        dir[i] = move_robot(i,zhen);
                        robot_status[i]=1;
                    }
                }
                else
                {
                    ItemNode* cur = dummyhead->prevnode;
                    int maxsearch = 30;
                    while(cur!=dummyhead&&maxsearch>0&&(cur->is_chosen==true||((my_abs(cur->x-robot[i].x)+my_abs(cur->y-robot[i].y))>=50)||(rb_wall[i].find((uintptr_t)cur)!=rb_wall[i].end())))
                    {
                        cur=cur->prevnode;
                        maxsearch--;
                    }
                    if(cur==dummyhead||maxsearch==0)
                        continue;
                    else
                    {
                        cur_goal=GridLocation{cur->x,cur->y};
                        cur->is_chosen=true;
                        deleteNode(cur);
                    }
                    came_from[i].clear();
                    cost_so_far[i].clear();
                    GridLocation start_pos = {robot[i].x,robot[i].y};                
                    a_star_search(i,start_pos,cur_goal);
                    max_astar--;
                    path[i]=reconstruct_path(start_pos, cur_goal, came_from[i]);
                    if(path[i].size()==0)
                    {
                        rb_wall[i].insert((uintptr_t)cur);
                        robot[i].max_search--;
                        if(robot[i].max_search<=0&&robot[i].rs==0)
                            robot[i].rs=-1;
                        cur->max_search--;
                        if(cur->max_search==0)
                        {
                            deleteNode(cur);
                            rb_wall[i].erase((uintptr_t)cur);
                        }
                    }
                    else
                    {
                        robot[i].rs=1;
                        from_zhen[i]=zhen;
                        rb_wall[i].clear();
                        bool jdg = false;
                        for(int m=0;m<10;m++)
                        {
                            if(m==i) continue;
                            if(path[i][zhen-from_zhen[i]+1]==GridLocation{robot[m].x,robot[m].y})
                            {
                                jdg=true;
                                break;
                            }
                        }
                        if(jdg)
                        {
                            path[i].clear();
                            robot_status[i]=0;
                            continue;
                        }
                        dir[i] = move_robot(i,zhen);
                        robot_status[i]=1;
                    }
                }
            }
            else if(robot_status[i]==1)
            {
                bool jdg = false;
                for(int m=0;m<10;m++)
                {
                    if(m==i) continue;
                    if(path[i][zhen-from_zhen[i]+1]==GridLocation{robot[m].x,robot[m].y})
                    {
                        jdg=true;
                        break;
                    }
                }
                if(jdg)
                {
                    path[i].clear();
                    robot_status[i]=0;
                    continue;
                }
                dir[i] = move_robot(i,zhen);
                if((zhen-from_zhen[i]+2)==path[i].size())
                {
                    robot_status[i]=2;
                    path[i].clear();
                }
            }
            else if(robot_status[i]==2||robot_status[i]==5)
            {

                robot_status[i]=5;//not going to the park
                if(max_astar<=0)
                    continue;
                if((limited_times[i]%5)==0) {
                    int park_idx = to_which_park2(i,robot[i],park_pos);

                    cur_goal=park_pos[park_idx];
                    to_park[i]=park_idx;
                    GridLocation start_pos = {robot[i].x,robot[i].y};
                    came_from[i].clear();
                    cost_so_far[i].clear();

                    a_star_search(i,start_pos,cur_goal);
                    max_astar--;
                    path[i]=reconstruct_path(start_pos, cur_goal, came_from[i]);
                    limited_times[i]++;
                    if(path[i].size()==0)
                    {
                        continue;
                    }   
                }
                else if((limited_times[i]%5)==1)
                {
                    int park_idx = to_which_park2(i,robot[i],park_pos2);
                    cur_goal=park_pos2[park_idx];
                    to_park[i]=park_idx;
                    GridLocation start_pos = {robot[i].x,robot[i].y};
                    came_from[i].clear();
                    cost_so_far[i].clear();
                    a_star_search(i,start_pos,cur_goal);
                    max_astar--;
                    path[i]=reconstruct_path(start_pos, cur_goal, came_from[i]);
                    limited_times[i]++;
                    if(path[i].size()==0)
                    {
                        continue;
                    }  
                }
                else if((limited_times[i]%5)==2)
                {
                    int park_idx = to_which_park2(i,robot[i],park_pos3);
                    cur_goal=park_pos3[park_idx];
                    to_park[i]=park_idx;
                    GridLocation start_pos = {robot[i].x,robot[i].y};
                    came_from[i].clear();
                    cost_so_far[i].clear();
                    a_star_search(i,start_pos,cur_goal);
                    max_astar--;
                    path[i]=reconstruct_path(start_pos, cur_goal, came_from[i]);
                    limited_times[i]++;
                    if(path[i].size()==0)
                    {
                        
                        continue;
                    } 
                }
                else if((limited_times[i]%5)==3)
                {
                   int park_idx = to_which_park2(i,robot[i],park_pos4);
                    cur_goal=park_pos4[park_idx];
                    to_park[i]=park_idx;
                    GridLocation start_pos = {robot[i].x,robot[i].y};
                    came_from[i].clear();
                    cost_so_far[i].clear();
                    a_star_search(i,start_pos,cur_goal);
                    max_astar--;
                    path[i]=reconstruct_path(start_pos, cur_goal, came_from[i]);
                    limited_times[i]++;
                    if(path[i].size()==0)
                    {
                        continue;
                    }  
                }
                else
                {
                    int park_idx = to_which_park(i,robot[i],park_pos);
                    cur_goal=park_pos[park_idx];
                    to_park[i]=park_idx;
                    GridLocation start_pos = {robot[i].x,robot[i].y};
                    came_from[i].clear();
                    cost_so_far[i].clear();
                    a_star_search(i,start_pos,cur_goal);
                    max_astar--;
                    path[i]=reconstruct_path(start_pos, cur_goal, came_from[i]);
                    limited_times[i]++;
                    if(path[i].size()==0)
                    {
                        continue;
                    }
                }
                from_zhen[i]=zhen;
                bool jdg = false;
                for(int m=0;m<10;m++)
                {
                    if(m==i) continue;
                    if(path[i][zhen-from_zhen[i]+1]==GridLocation{robot[m].x,robot[m].y})
                    {
                        jdg=true;
                        break;
                    }
                }
                if(jdg)
                {
                    path[i].clear();
                    robot_status[i]=5;
                    continue;
                }
                dir[i] = move_robot(i,zhen);
                robot_status[i]=3;
                limited_times[i]=0;
            }
            else if(robot_status[i]==3)
            {
                
                bool jdg = false;
                for(int m=0;m<10;m++)
                {
                    if(m==i) continue;
                    if(path[i][zhen-from_zhen[i]+1]==GridLocation{robot[m].x,robot[m].y})
                    {
                        jdg=true;
                        break;
                    }
                }
                if(jdg)
                {
                    
                    if(is_already_park(robot[i].x,robot[i].y))
                    {
                        robot_status[i]=4;
                        park_item[to_park[i]]++;
                        path[i].clear();
                        dir[i]=4;
                        continue;
                    }
                    path[i].clear();
                    robot_status[i]=5;
                    continue;
                }
                dir[i] = move_robot(i,zhen);
                if((zhen-from_zhen[i]+2)==path[i].size())
                {
                    robot_status[i]=4;
                    park_item[to_park[i]]++;
                    if_rb_itemlist[i]=true;
                    path[i].clear();
                }
            }
            else if(robot_status[i]==10)
            {
                if(zhen==recover_zhen[i])
                {
                    robot_status[i]=4;
                    in_recover[i]=false;
                }
            }
            else if(robot_status[i]==11)
            {
                if(zhen==recover_zhen[i])
                {
                    if(ch[robot[i].x+1][robot[i].y+1]!='B')
                    {
                        robot_status[i]=5;
                        in_recover[i]=false;
                    }
                    else
                    {
                        robot_status[i]=4;dir[i]=4;
                        park_item[to_park[i]]++;
                        in_recover[i]=false;
                    }
                }
                
            }
        }

   
}

int to_zhen[5]={-1};
int finish_upboat[5]={-1};
bool on_virtual[5]={false};
bool to_virtual[5]={false};
bool boat_to_park[10]={false};
int boat_to_parkid[5]={0};
int bt_capacity[5]={0};
void process_boat()
{
    for(int i=0;i<5;i++) {
        on_virtual[i]=false;
        to_virtual[i]=false;
        if(boat_status[i]==0||(boat_status[i]==3&&to_zhen[i]==zhen))
        {
            bt_capacity[i]=boat_capacity;
            int index = -1;
            double maxval = 0.0;
            for(int r=0;r<10;r++)
            {
                if(((berth[r].transport_time)*2+park_item[r]/berth[r].loading_speed)>=(15000-zhen))
                    continue;
                if(boat_to_park[r]) continue;
                double tmp = (double)berth[r].loading_speed*park_item[r]/((double)(berth[r].loading_speed)*(berth[r].transport_time)+park_item[r]);
                if(tmp>=maxval)
                {
                    maxval=tmp;
                    index=r;
                }
            }
            to_zhen[i] = zhen+berth[index].transport_time;
            boat_to_park[index]=true;
            boat_status[i] = 1;
            on_virtual[i]=true;
            boat_to_parkid[i]=index;
            if(index==-1)
                on_virtual[i]=false;
        }
        else if(boat_status[i]==1)
        {
            if(to_zhen[i]==zhen)
            {
                int num = bt_capacity[i]<park_item[boat_to_parkid[i]]?bt_capacity[i]:park_item[boat_to_parkid[i]];
                finish_upboat[i]=zhen+(num-1)/(berth[boat_to_parkid[i]].loading_speed)+1;
                bt_capacity[i]-=(finish_upboat[i]-zhen)*(berth[boat_to_parkid[i]].loading_speed);
                park_item[boat_to_parkid[i]]-=num;
                boat_status[i]=2;
            }
            
        }
        else if(boat_status[i]==2)
        {
            if(zhen==finish_upboat[i])
            {
                int origin_pkid=boat_to_parkid[i];
                boat_to_park[origin_pkid]=false;

                boat_to_parkid[i]=-1;
                double score = (double)(boat_capacity-bt_capacity[i])/berth[origin_pkid].transport_time;
                for(int r=0;r<10;r++)
                {
                    if(((berth[r].transport_time)+200)>=(15000-zhen)||r==origin_pkid)
                        continue;
                    if(boat_to_park[r]) continue;
                    double tmp = (double)(park_item[r]+boat_capacity-bt_capacity[i])/(berth[r].transport_time+200);
                    if(tmp>=score)
                    {
                        score=tmp;
                        boat_to_parkid[i]=r;
                    }
                }
                if(boat_to_parkid[i]==-1)
                {
                    boat_status[i]=3;
                    to_zhen[i]=zhen+berth[origin_pkid].transport_time;
                    to_virtual[i]=true;
                }
                else
                {
                    to_zhen[i] = zhen+200;
                    boat_to_park[boat_to_parkid[i]]=true;
                    boat_status[i] = 1;
                    on_virtual[i]=true;
                }

            }
        }
    }
    
}
int main()
{
    dummyhead = new ItemNode();
    dummyhead->nextnode=dummyhead;
    dummyhead->prevnode=dummyhead;
    for(int i=0;i<10;i++)
    {
        park_dummyhead[i] = new ItemNode();
        park_dummyhead[i]->nextnode=park_dummyhead[i];
        park_dummyhead[i]->prevnode=park_dummyhead[i];
    }

   f1.open("data.txt",ios::out);
    Init();
    for(zhen = 1; zhen <= 15000; zhen ++)
    {
        
        int id = Input();
        deleteItem(zhen);
        cnt=1;
        process_boat();
        process_robot();
        
        for(int i=0;i<10;i++)
        {
            if(robot[i].rs==-1) continue;
            if(zhen==1||robot_status[i]==0||robot_status[i]==1)
            {
                if(dir[i]!=-1)
                    printf("move %d %d\n",i,dir[i]);
            }
            else if(robot_status[i]==2)
            {
                if(dir[i]!=-1)
                {
                    printf("move %d %d\n",i,dir[i]);
                    printf("get %d\n",i);
                } 
            }
            else if(robot_status[i]==3)
            {
                if(dir[i]!=-1)
                    printf("move %d %d\n",i,dir[i]);
            }
            else if(robot_status[i]==4)
            {
                if(dir[i]==4)
                {
                    printf("pull %d\n",i);
                }
                else if(dir[i]!=-1)
                {
                    printf("move %d %d\n",i,dir[i]);
                    printf("pull %d\n",i);
                }
            }
        }
        for(int i=0;i<5;i++)
        {
            if(on_virtual[i])
            {
                printf("ship %d %d\n",i,boat_to_parkid[i]);
            }
            else if(to_virtual[i])
            {
                printf("go %d\n",i);
            }
        }
        puts("OK");
        fflush(stdout);
    }
    f1.close();
    return 0;
}
