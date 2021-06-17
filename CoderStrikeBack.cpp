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

 // Initially named Coordinates, this struct is used to represent both coordinates and vectors.

struct Pair
{
    int x;
    int y;
};

Pair operator +(Pair a, Pair b)
{
    Pair Sum;
    Sum.x = a.x + b.x;
    Sum.y = a.y + b.y;
    return Sum;
}

float distance(int startX, int startY, int targetX, int targetY)
{
    return sqrt(abs((targetX - startX) * (targetX - startX) -
        (targetY - startY) * (targetY - startY)));
}

float distance(Pair start, Pair target)
{
    return distance(start.x, start.y, target.x, target.y);
}

int lerp(int start, int end, float interpolationPoint)
{
    //cerr << "inter point: " << interpolationPoint << endl;
    return start + interpolationPoint * (end - start);
}

Pair lerp(Pair start, Pair end, float interpolationPoint)
{
    Pair result =
    {
        .x = lerp(start.x, end.x, interpolationPoint),
        .y = lerp(start.y, end.y, interpolationPoint)
    };
    //cerr << "Lerp results: " << start.x << " " << end.x << " " << result.x<<endl;
    return result;
}

float findAngle(Pair vector1, Pair vector2)
{
    // angle = arccos(sum of products of Xs and Ys/product of magnitudes)
    float numerator = vector1.x * vector2.x + vector1.y * vector2.y;
    float denominator = sqrt(vector1.x * vector1.x + vector1.y * vector1.y) * sqrt(vector2.x * vector2.x + vector2.y * vector2.y);
    return acos(numerator / denominator);
}

Pair rotateVectorByAngle(Pair vector, float angle)
{
    float sine = sin(angle);
    float cosine = cos(angle);
    int newX = floor(vector.x * cosine - vector.y * sine);
    int newY = floor(vector.x * sine + vector.y * cosine);
    Pair newVector = { .x = newX, .y = newY };
    return newVector;
}


// Information about the last 3 positions, used to predict the next one
Pair previousPositions[3]; //0th element is the previous position
Pair previousOpponentPositions[3]; //0th element is the previous position

void initialSetup()
{
    for (int index = 0; index < 3; index++)
        previousPositions[index] = { .x = -1, .y = -1 };
}

// UpdatePositions needs to be changed to a general use instead of hard-coded, if the position prediction is expanded.
void updatePositions(Pair position, Pair opponentPos)
{
    previousPositions[2] = previousPositions[1];
    previousPositions[1] = previousPositions[0];
    previousPositions[0] = position;

    previousOpponentPositions[2] = previousOpponentPositions[1];
    previousOpponentPositions[1] = previousOpponentPositions[0];
    previousOpponentPositions[0] = opponentPos;
}

// Use the last 3 positions to estimate a future position. We don't know the turning rate of the racer so we can only guess using it's most recent trajectory
// Tests show that the predicted position is reasonably accurate but not exact. Could be made exact if we're allowed to give our own turn rate.

Pair predictFuturePosition(Pair previousPositions[])
{
    // calculate the vector between the last 2 positions
    // calculate the vector between positions 2 and 3
    Pair lastVector = { .x = previousPositions[0].x - previousPositions[1].x, .y = previousPositions[0].y - previousPositions[1].y };
    Pair secondLastVector = { .x = previousPositions[1].x - previousPositions[2].x, .y = previousPositions[1].y - previousPositions[2].y };

    // find angle between vector 1 and 2 -- dot product
    float angle = findAngle(lastVector, secondLastVector);

    // find left or right orientation between vector 1 and 2 -- compare cross product to 0
    float crossProduct = lastVector.x * secondLastVector.y - secondLastVector.x * lastVector.y;
    if (crossProduct > 0) // turning towards the left.
        angle = -angle;

    // rotate predicted vector by angle to right or left
    Pair predictedVector = rotateVectorByAngle(lastVector, angle);

    // predicted position is the current position + the predicted vector
    Pair predictedPosition = previousPositions[0] + predictedVector;
    return predictedPosition;
}

// Variables used between steps
bool boostUsed = false;
int stepCounter = 0;

int main()
{

    initialSetup();

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
        Pair position = { .x = x, .y = y };
        Pair opponentPos = { .x = opponentX, .y = opponentY };
        Pair nextCheckpointPos{ .x = nextCheckpointX, .y = nextCheckpointY };

        // Add the new position for the opponent
        updatePositions(position, opponentPos);
        stepCounter++;

        // The 3 output variables
        Pair target = nextCheckpointPos;
        string thrust = "100";


        // Slow down if we need to turn by more than 90 degree  for the next checkpoint
        float arbitraryAngle = 90.0f;
        float angleRatio;
        if (nextCheckpointAngle > arbitraryAngle || nextCheckpointAngle < -arbitraryAngle)
            angleRatio = 1 - (abs(nextCheckpointAngle) - 90.0f) / 90.0f;
        else
            angleRatio = 1;

        float distanceToCheckpoint = distance(position, nextCheckpointPos);

        // Slow down as we get closer to target, because we often have to turn around right after
        float checkpointRadius = 400.0f;
        float arbitraryDistance = 1000.0f;
        // Clamp the ratio to use in lerp
        float distanceRatio = clamp(distanceToCheckpoint / (checkpointRadius + arbitraryDistance), 0.0f, 1.0f);

        int adjustedThrust = lerp(0, 100, angleRatio * distanceRatio);

        thrust = to_string(adjustedThrust);


        // Try to intercept the enemy to knock him off his course, if he is ahead of us,
        // by changing our target to be between the checkpoint and the enemy
        if (distanceToCheckpoint > distance(opponentPos, nextCheckpointPos) && nextCheckpointDist > 500)
        {
            target = lerp(opponentPos, nextCheckpointPos, 0.80f);

            // Predict the next position using the previous 3 positions
            if (previousPositions[2].x != -1)
            {
                Pair predictedPos = predictFuturePosition(previousPositions);
                Pair predictedOppponentPos = predictFuturePosition(previousOpponentPositions);
                cerr << "MyPredPos:" << predictedPos.x << " " << predictedPos.y << endl;
                cerr << "OppPredPos:" << predictedOppponentPos.x << " " << predictedOppponentPos.y << endl;

                // If the racers are about to crash in the next position, maybe we can use the shield
                // This is not a very good approach because we lose too much momentum from not using our engines
                // It seems the moments when it would beneficial to use a shield are very specific. It would be 
                // easier to find them if we knew the locations and order of the checkpoints from the start, it won't be as effective
                // if we learn them after the first lap because the bot will likely have too big of a lead.
                /*
                float oppDistance = distance(predictedPos, predictedOppponentPos);
                cerr << "Distance = " << oppDistance << endl;
                if (oppDistance < 600)
                    thrust = "SHIELD";
                */
            }
        }

        // Use the boost when going straight to the checkpoint an the distance is large enough
        // Could be improved by remembering the past checkpoints and using it for the most distant 2 checkpoints
        if (nextCheckpointAngle == 0 && nextCheckpointDist > 7500 && boostUsed == false && stepCounter > 30)
        {
            thrust = "BOOST";
            boostUsed = true;
        }


        cout << target.x << " " << target.y << " " << thrust << endl;
    }
}