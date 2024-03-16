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

struct GridLocation {
    int x, y;
};
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

std::map<GridLocation, GridLocation> came_from;
std::map<GridLocation, int> cost_so_far;
std::set<GridLocation> park_set;

struct Robot
{
    int x, y, goods;
    int status;
    int mbx, mby;
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
        park_set.insert(GridLocation{berth[id].x,berth[id].y});
        park_x=berth[id].x;park_y=berth[id].y;
    }
    park_goal={park_x,park_y+2};
    f1<<park_x<<" "<<park_y+2<<endl;
    scanf("%d", &boat_capacity);
    char okk[100];
    scanf("%s", okk);
    printf("OK\n");
    fflush(stdout);
}

int item_x;
int item_y;
GridLocation item_goal;
int Input()
{
    scanf("%d%d", &id, &money);
    int num;
    scanf("%d", &num);
    for(int i = 1; i <= num; i ++)
    {
        int x, y, val;
        scanf("%d%d%d", &x, &y, &val);
        item_x=x;
        item_y=y;
    }
    item_goal={item_x,item_y};
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

void a_star_search(GridLocation start,GridLocation goal)
{
    PriorityQueue<GridLocation, int> frontier;
    frontier.put(start, 0);

    came_from[start] = start;
    cost_so_far[start] = 0;

    while (!frontier.empty()) {
        GridLocation current = frontier.get();

        if (current == goal) {
            break;
        }

        for (GridLocation next : grid.neighbors(current)) {
            int new_cost = cost_so_far[current] + grid.cost(current, next);
            if (cost_so_far.find(next) == cost_so_far.end()
                || new_cost < cost_so_far[next]) {
                cost_so_far[next] = new_cost;
                int priority = new_cost + heuristic(next, goal);
                frontier.put(next, priority);
                came_from[next] = current;
            }
        }
    }
}


std::vector<GridLocation> reconstruct_path(
    GridLocation start, GridLocation goal,
    std::map<GridLocation, GridLocation> came_from
) {
    std::vector<GridLocation> path;
    GridLocation current = goal;
    while (current != start) {
        path.push_back(current);
        current = came_from[current];
    }
    path.push_back(start); // optional
    std::reverse(path.begin(), path.end());
    return path;
}

int robot_status = 0;
std::vector<GridLocation> path;
int from_zhen;
int move_robot(int zhen)
{
    return move_dic[path[zhen-from_zhen+1]-path[zhen-from_zhen]];
}
int dir = -1;
bool if_get = false;
int end_load;
int item_num=1;
int boat_status = 0;
void process_robot(int zhen)
{
    dir=-1;
    if(robot_status==0)
    {
        cur_goal=item_goal;
        GridLocation start_pos = {robot[0].x,robot[0].y};
        came_from.clear();
        cost_so_far.clear();
        a_star_search(start_pos,cur_goal);
        path=reconstruct_path(start_pos, cur_goal, came_from);
        from_zhen=zhen;
        dir = move_robot(zhen);
        robot_status=1;
    }
    else if(robot_status==1)
    {
        dir = move_robot(zhen);
        if((zhen-from_zhen+1)==path.size())
        {
            robot_status=2;
            if_get=true;
        }    
    }
    else if(robot_status==2)
    {
        cur_goal=park_goal;
        GridLocation start_pos = {robot[0].x,robot[0].y};
        came_from.clear();
        cost_so_far.clear();
        a_star_search(start_pos,cur_goal);
        path=reconstruct_path(start_pos, cur_goal, came_from);
        from_zhen=zhen;
        dir = move_robot(zhen);
        robot_status=3;
    }
    else if(robot_status==3)
    {
        if((zhen-from_zhen+1)==path.size())
        {
            robot_status=4;
        } 
        else
            dir = move_robot(zhen);    
    }
    else if(robot_status==4)
    {
        if(boat_status==2)
        {
            end_load=zhen+(item_num-1)/berth[9].loading_speed+1;
            robot_status=5;
        }
    }
    else if(robot_status==5)
    {
        if(zhen==end_load)
        {
            robot_status=0;
        }
    }
}

int to_zhen=-1;
void process_boat(int zhen)
{
    if(boat_status==0)
    {
        to_zhen = zhen+berth[9].transport_time;
        boat_status = 1;
    }
    else if(boat_status==1)
    {
        if(to_zhen==zhen)
        {
            boat_status=2;
        }
            
    }
    else if(boat_status==2)
    {
        if(zhen==end_load)
        {
            boat_status=3;
            to_zhen=zhen+berth[9].transport_time;
        }
    }
    else if(boat_status==3)
    {
        if(to_zhen==zhen)
        {
            boat_status=0;
        }
    }
}
int main()
{
    f1.open("data.txt",ios::out);
    Init();
    for(int zhen = 1; zhen <= 15000; zhen ++)
    {
        int id = Input();
        process_boat(zhen);
        process_robot(zhen);
        if(zhen==1||robot_status==1||robot_status==0)
        {
            printf("move 0 %d\n",dir);
        }
        if(if_get)
        {
            if_get=false;
            printf("get 0\n");
        }
        else if(robot_status==2||robot_status==3)
        {
            printf("move 0 %d\n",dir);
        }
        if(robot_status==4)
        {
            printf("pull 0\n");
        }
        if(zhen==1||boat_status==0)
            printf("ship 0 9\n");
        if(boat_status==3)
            printf("go 0\n");
        puts("OK");
        fflush(stdout);
    }
    f1.close();
    return 0;
}


