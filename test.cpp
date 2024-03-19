#include <bits/stdc++.h>
#include<fstream>
using namespace std;
fstream f1;

const int n = 200;
const int robot_num = 10;
const int berth_num = 10;
const int N = 210;

int money, boat_capacity, id;
char ch[N][N];
int gds[N][N];
int zhen;
int park_item[10]={0};

struct GridLocation {
    int x, y;
};
std::map<GridLocation,int> item_to_idx;
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
struct SquareGrid {
    static std::array<GridLocation, 4> DIRS;

    int width, height;

    SquareGrid(int width_, int height_)
        : width(width_), height(height_) {}

    bool in_bounds(GridLocation id) const {
        return 0 <= id.x && id.x < width
            && 0 <= id.y && id.y < height;
    }

    bool passable(GridLocation id) const {
        return ch[id.x+1][id.y+1]!='*'&&ch[id.x+1][id.y+1]!='#'&&ch[id.x+1][id.y+1]!='A';
    }

    std::vector<GridLocation> neighbors(GridLocation id) const {
        std::vector<GridLocation> results;

        for (GridLocation dir : DIRS) {
            GridLocation next{ id.x + dir.x, id.y + dir.y };
            if (in_bounds(next) && passable(next)) {
                results.push_back(next);
            }
        }

        if ((id.x + id.y) % 2 == 0) {
            // aesthetic improvement on square grids
            std::reverse(results.begin(), results.end());
        }

        return results;
    }
    int cost(GridLocation from_node, GridLocation to_node) const {
        return 1;
    }
};
std::array<GridLocation, 4> SquareGrid::DIRS =
{ GridLocation{ 1, 0 }, GridLocation{ 0, -1 }, GridLocation{ -1, 0 }, GridLocation{ 0, 1 } };
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
void addItem(int zhen,int x,int y,int val)
{
    ItemNode* in = new ItemNode(x,y,zhen+1000,val);
    ItemNode* pn = dummyhead->prevnode;
    pn->nextnode=in;
    in->nextnode=dummyhead;
    dummyhead->prevnode=in;
    in->prevnode=pn;
    
}
void deleteItem(int zhen)
{
    ItemNode* cur = dummyhead->nextnode;
    ItemNode* pre = dummyhead;
    while(cur!=dummyhead&&cur->expire_time<=zhen)
    {
        ItemNode* ne = cur->nextnode;
        pre->nextnode=ne;
        ne->prevnode=pre;
        if(item_to_idx.find(GridLocation{cur->x,cur->y})!=item_to_idx.end())
            item_to_idx.erase(GridLocation{cur->x,cur->y});
        delete cur;
        cur=ne;
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
    }
    scanf("%d", &boat_capacity);
    char okk[100];
    scanf("%s", okk);
    printf("OK\n");
    fflush(stdout);
}

int Input()
{
    scanf("%d%d", &id, &money);
    int num;
    scanf("%d", &num);
    
    for(int i = 1; i <= num; i ++)
    {
        int x, y, val;
        scanf("%d%d%d", &x, &y, &val);
        addItem(zhen,x,y,val);
    }
    
    for(int i = 0; i < robot_num; i ++)
    {
        int sts;
        scanf("%d%d%d%d", &robot[i].goods, &robot[i].x, &robot[i].y, &sts);
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
void a_star_search(int i,GridLocation start,GridLocation goal)
{
    PriorityQueue<pair<GridLocation,int>, int> frontier;
    frontier.put(pair<GridLocation,int>{start,0}, 0);

    came_from[i][start] = start;
    cost_so_far[i][start] = 0;
    int cnt=1;
    while (!frontier.empty()) {
        pair<GridLocation,int> currentpair = frontier.get();
        GridLocation current = currentpair.first;
        int curstep = currentpair.second;

        if (current == goal) {
            break;
        }

        for (GridLocation next : grid.neighbors(current)) {
            int new_cost = cost_so_far[i][current] + grid.cost(current, next);
            if (cost_so_far[i].find(next) == cost_so_far[i].end()
                || new_cost < cost_so_far[i][next]) {
                if(judge_rb_crash(next,curstep,i))
                    continue;
                cost_so_far[i][next] = new_cost;
                int priority = new_cost + heuristic(next, goal);
                frontier.put({next,curstep+1}, priority);
                cnt++;
                if(cnt>600) return;
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
    return path;
}

int robot_status[10] = {0};
int move_robot(int i,int zhen)
{
    return move_dic[path[i][zhen-from_zhen[i]+1]-path[i][zhen-from_zhen[i]]];
}
int dir[10] = {-1};

int boat_status[5] = {0};
bool robot_idle[10];




int my_abs(int a)
{
    return a>=0?a:-a;
}
bool judge_rb_park[10][10]={false};
int to_which_park(int j,Robot& rb,GridLocation* pp)
{
    for(int i=0;i<10;i++)
    {
        if((my_abs(rb.x-pp[i].x)+my_abs(rb.y-pp[i].y))<=90&&judge_rb_park[j][i]==false)
            return i;
    }
    return j;//random number
}

int to_park[10];
map<int,set<uintptr_t>> rb_wall;
void process_robot()
{
    int max_astar = 1;
    for(int i=0;i<10;i++)
    {
        if(robot[i].rs==-1)
            continue;
        dir[i]=-1;
        if(robot_status[i]==0||robot_status[i]==4)
        {
            robot_status[i]=0;
            if(max_astar<=0)
                continue;
            ItemNode* cur = dummyhead->prevnode;
            int maxsearch = 20;
            while(cur!=dummyhead&&maxsearch>0&&(cur->is_chosen==true||((my_abs(cur->x-robot[i].x)+my_abs(cur->y-robot[i].y))>=80)||(rb_wall[i].find((uintptr_t)cur)!=rb_wall[i].end())))
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
                dir[i] = move_robot(i,zhen);
                robot_status[i]=1;
            }
        }
        else if(robot_status[i]==1)
        {
            dir[i] = move_robot(i,zhen);
            if((zhen-from_zhen[i]+2)==path[i].size())
            {
                robot_status[i]=2;
                path[i].clear();
            }
        }
        else if(robot_status[i]==2)
        {
            robot_status[i]=5;//not going to the park
            if(max_astar<=0)
                continue;
            int park_idx = to_which_park(i,robot[i],park_pos);
            cur_goal=park_pos[park_idx];
            to_park[i]=park_idx;
            GridLocation start_pos = {robot[i].x,robot[i].y};
            came_from[i].clear();
            cost_so_far[i].clear();
            a_star_search(i,start_pos,cur_goal);
            max_astar--;
            path[i]=reconstruct_path(start_pos, cur_goal, came_from[i]);
            if(path[i].size()==0)
            {
                judge_rb_park[i][park_idx]=true;
                continue;
            }
            from_zhen[i]=zhen;
            dir[i] = move_robot(i,zhen);
            robot_status[i]=3;
            for(int j=0;j<10;j++)
                judge_rb_park[i][j]=false;
        }
        else if(robot_status[i]==3)
        {
            dir[i] = move_robot(i,zhen);
            if((zhen-from_zhen[i]+2)==path[i].size())
            {
                robot_status[i]=4;
                park_item[to_park[i]]++;
            }
        }

    }


}

int to_zhen[5]={-1};
int finish_upboat[5]={-1};
bool on_virtual[5]={false};
bool to_virtual[5]={false};
void process_boat()
{
    for(int i=0;i<5;i++) {
        on_virtual[i]=false;
        to_virtual[i]=false;
        if(boat_status[i]==0)
        {
            to_zhen[i] = zhen+berth[i*2].transport_time;
            boat_status[i] = 1;
            on_virtual[i]=true;
        }
        else if(boat_status[i]==1)
        {
            if(to_zhen[i]==zhen)
            {
                int num = boat_capacity<park_item[i*2]?boat_capacity:park_item[i*2];
                finish_upboat[i]=zhen+(num-1)/(berth[i*2].loading_speed)+1;
                boat_status[i]=2;
            }
            
        }
        else if(boat_status[i]==2)
        {
            if(zhen==finish_upboat[i])
            {
                boat_status[i]=3;
                to_zhen[i]=zhen+berth[i*2].transport_time;
                to_virtual[i]=true;
            }
        }
        else if(boat_status[i]==3&&to_zhen[i]==zhen)
        {
            to_zhen[i] = zhen+berth[i*2].transport_time;
            boat_status[i] = 1;
            on_virtual[i]=true;
        }
    }
    
}
int main()
{
    dummyhead = new ItemNode();
    dummyhead->nextnode=dummyhead;
    dummyhead->prevnode=dummyhead;


    f1.open("data.txt",ios::out);
    Init();
    for(zhen = 1; zhen <= 15000; zhen ++)
    {
        int id = Input();
        deleteItem(zhen);
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
                printf("move %d %d\n",i,dir[i]);
                printf("get %d\n",i);
            }
            else if(robot_status[i]==3)
            {
                printf("move %d %d\n",i,dir[i]);
            }
            else if(robot_status[i]==4)
            {
                printf("move %d %d\n",i,dir[i]);
                printf("pull %d\n",i);
            }
        }
        for(int i=0;i<5;i++)
        {
            if(on_virtual[i])
            {
                printf("ship %d %d\n",i,i*2);
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


