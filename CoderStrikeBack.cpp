#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

struct Coordinates
{
    int x;
    int y;
};

float distance(int startX, int startY, int targetX, int targetY)
{
    return sqrt(abs((targetX - startX) * (targetX - startX) -
                (targetY - startY) * (targetY - startY)));
}

float distance(Coordinates start, Coordinates target)
{
    return distance(start.x, start.y, target.x, target.y);
}

int lerp(int start, int end, float interpolationPoint)
{
    //cerr << "inter point: " << interpolationPoint << endl;
    return start + interpolationPoint * (end - start);
}

Coordinates lerp(Coordinates start, Coordinates end, float interpolationPoint)
{
    Coordinates result = 
    {
        .x = lerp(start.x, end.x, interpolationPoint),
        .y = lerp(start.y, end.y, interpolationPoint)
    };
    //cerr << "Lerp results: " << start.x << " " << end.x << " " << result.x<<endl;
    return result;
}

//Booleans
bool boostUsed = false;

int main()
{

    // game loop
    while (1) {
        int x;
        int y;
        int nextCheckpointX; // x position of the next check point
        int nextCheckpointY; // y position of the next check point
        int nextCheckpointDist; // distance to the next checkpoint
        int nextCheckpointAngle; // angle between your pod orientation and the direction of the next checkpoint
        cin >> x >> y >> nextCheckpointX >> nextCheckpointY >> nextCheckpointDist >> nextCheckpointAngle; cin.ignore();
        int opponentX;
        int opponentY;
        cin >> opponentX >> opponentY; cin.ignore();

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;


        // You have to output the target position
        // followed by the power (0 <= thrust <= 100)
        // i.e.: "x y thrust"

        // Convert to coordinates
        Coordinates position = {.x = x, .y = y};
        Coordinates opponentPos = {.x = opponentX, .y = opponentY};
        Coordinates nextCheckpointPos {.x = nextCheckpointX, .y = nextCheckpointY};

        
        // The 3 output variables
        Coordinates target = nextCheckpointPos;
        string thrust = "100";

        // Possible improvement : adapt thrust setting to lerp according to the angle.
        // Maybe implement a condition for faster target change (for faster steering) if boost was used in the past X turns
        if(nextCheckpointAngle > 90 || nextCheckpointAngle < -90)
        {
            if(nextCheckpointDist > 600)
                thrust = "30";
            else
                thrust = "100";
        }

        // Try to intercept the enemy to knock him off his course, if he is ahead of us,
        // by changing our target to be between the checkpoint and the enemy
        if(distance(position, nextCheckpointPos) > distance(opponentPos, nextCheckpointPos) && nextCheckpointDist > 300)
        {
            target = lerp(opponentPos, nextCheckpointPos, 0.85f);
        }
        // Use the boost when going straight to the checkpoint an the distance is large enough
        // Could be improved by remembering the past checkpoints and using it for the most distant 2 checkpoints
        if(nextCheckpointAngle ==  0 && nextCheckpointDist > 5000 && boostUsed == false)
        {
            thrust = "BOOST";
            boostUsed = true;
        }

        cout << target.x << " " << target.y << " " << thrust << endl;
    }
}