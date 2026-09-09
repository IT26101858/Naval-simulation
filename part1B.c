#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define G 9.81
#define PI 3.14159265

struct Battleship {
    char notation;
    char name[40];

    float x;
    float y;

    float maxVelocity;
    int alive;
};

struct EscortShip {
    int id;

    char notation;
    char name[40];

    float x;
    float y;

    float minVelocity;
    float maxVelocity;

    float minAngle;
    float maxAngle;

    float impactPower;

    int alive;
};


/* -----------------------------------------
   FUNCTION PROTOTYPES
----------------------------------------- */

float calculateDistance(float x1, float y1,
                        float x2, float y2);

float calculateMaxRange(float velocity);

float calculateProjectileAngle(float distance,
                               float velocity);

float calculateHitTime(float distance,
                       float velocity,
                       float angle);

void setupBattleship(struct Battleship *b);

void createEscortShip(struct EscortShip *e,
                      int id,
                      int canvasSize,
                      float bMaxVelocity);

void copyEscorts(struct EscortShip destination[],
                 struct EscortShip source[],
                 int n);

void saveInitialConditions(
    struct Battleship b,
    struct EscortShip escorts[],
    int n,
    int canvasSize,
    int k);

void saveIterationResult(
    FILE *file,
    int iteration,
    struct Battleship b,
    struct EscortShip escorts[],
    int n,
    int destroyedCount,
    int battleshipSunk,
    int sinkingEscort,
    int jammed,
    float thetaMin);

void runSimulation1(
    struct Battleship initialBattleship,
    struct EscortShip initialEscorts[],
    int n,
    int canvasSize,
    int k);

void runSimulation2(
    struct Battleship initialBattleship,
    struct EscortShip initialEscorts[],
    int n,
    int canvasSize,
    int k,
    int jamIteration,
    float thetaMin);


/* -----------------------------------------
   DISTANCE
----------------------------------------- */

float calculateDistance(float x1, float y1,
                        float x2, float y2)
{
    float dx = x2 - x1;
    float dy = y2 - y1;

    return sqrt(dx * dx + dy * dy);
}


/* -----------------------------------------
   MAXIMUM PROJECTILE RANGE
   R = V² / g
----------------------------------------- */

float calculateMaxRange(float velocity)
{
    return (velocity * velocity) / G;
}


/* -----------------------------------------
   PROJECTILE ANGLE

   R = V² sin(2θ) / g

   θ = asin(Rg/V²) / 2
----------------------------------------- */

float calculateProjectileAngle(float distance,
                               float velocity)
{
    float value =
        (distance * G) /
        (velocity * velocity);

    if (value > 1.0)
    {
        return -1;
    }

    float angle =
        asin(value) / 2.0;

    return angle * 180.0 / PI;
}


/* -----------------------------------------
   TIME TO HIT

   T = 2V sin(θ) / g
----------------------------------------- */

float calculateHitTime(float distance,
                       float velocity,
                       float angle)
{
    float angleRadians =
        angle * PI / 180.0;

    float time =
        (2 * velocity *
         sin(angleRadians)) / G;

    return time;
}


/* -----------------------------------------
   BATTLESHIP SETUP
----------------------------------------- */

void setupBattleship(struct Battleship *b)
{
    printf("\nSelect Battleship Type\n");

    printf("U - USS Iowa (BB-61)\n");
    printf("M - MS King George V\n");
    printf("R - Richelieu\n");
    printf("S - Sovetsky Soyuz-class\n");

    printf("Enter Battleship Type: ");

    scanf(" %c",
          &b->notation);

    switch (b->notation)
    {
        case 'U':
        case 'u':

            b->notation = 'U';

            sprintf(
                b->name,
                "USS Iowa (BB-61)");

            break;


        case 'M':
        case 'm':

            b->notation = 'M';

            sprintf(
                b->name,
                "MS King George V");

            break;


        case 'R':
        case 'r':

            b->notation = 'R';

            sprintf(
                b->name,
                "Richelieu");

            break;


        case 'S':
        case 's':

            b->notation = 'S';

            sprintf(
                b->name,
                "Sovetsky Soyuz-class");

            break;


        default:

            printf(
                "Invalid type. "
                "USS Iowa selected.\n");

            b->notation = 'U';

            sprintf(
                b->name,
                "USS Iowa (BB-61)");
    }


    printf(
        "Enter Battleship Maximum "
        "Shell Velocity: ");

    scanf("%f",
          &b->maxVelocity);


    b->alive = 1;
}


/* -----------------------------------------
   CREATE RANDOM ESCORT SHIP
----------------------------------------- */

void createEscortShip(
    struct EscortShip *e,
    int id,
    int canvasSize,
    float bMaxVelocity)
{
    int type =
        rand() % 5;


    e->id = id;

    e->x =
        rand() % (canvasSize + 1);

    e->y =
        rand() % (canvasSize + 1);

    e->alive = 1;


    switch (type)
    {
        case 0:

            e->notation = 'A';

            sprintf(
                e->name,
                "1936A-class Destroyer");

            e->impactPower = 0.08;

            e->minAngle = 20;
            e->maxAngle = 40;

            e->minVelocity =
                50 + rand() % 100;

            e->maxVelocity =
                1.2 * bMaxVelocity;

            break;


        case 1:

            e->notation = 'B';

            sprintf(
                e->name,
                "Gabbiano-class Corvette");

            e->impactPower = 0.06;

            e->minAngle = 30;
            e->maxAngle = 60;

            e->minVelocity =
                50 + rand() % 100;

            e->maxVelocity =
                50 +
                rand() %
                (int)(bMaxVelocity - 50);

            break;


        case 2:

            e->notation = 'C';

            sprintf(
                e->name,
                "Matsu-class Destroyer");

            e->impactPower = 0.07;

            e->minAngle = 25;
            e->maxAngle = 50;

            e->minVelocity =
                50 + rand() % 100;

            e->maxVelocity =
                50 +
                rand() %
                (int)(bMaxVelocity - 50);

            break;


        case 3:

            e->notation = 'D';

            sprintf(
                e->name,
                "F-class Escort Ship");

            e->impactPower = 0.05;

            e->minAngle = 50;
            e->maxAngle = 75;

            e->minVelocity =
                50 + rand() % 100;

            e->maxVelocity =
                50 +
                rand() %
                (int)(bMaxVelocity - 50);

            break;


        case 4:

            e->notation = 'E';

            sprintf(
                e->name,
                "Japanese Kaibokan");

            e->impactPower = 0.04;

            e->minAngle = 70;
            e->maxAngle = 90;

            e->minVelocity =
                50 + rand() % 100;

            e->maxVelocity =
                50 +
                rand() %
                (int)(bMaxVelocity - 50);

            break;
    }
}


/* -----------------------------------------
   COPY ESCORT ARRAY

   Simulation 1 and Simulation 2 must start
   with the same initial conditions.
----------------------------------------- */

void copyEscorts(
    struct EscortShip destination[],
    struct EscortShip source[],
    int n)
{
    for (int i = 0;
         i < n;
         i++)
    {
        destination[i] =
            source[i];
    }
}


/* -----------------------------------------
   SAVE INITIAL CONDITIONS
----------------------------------------- */

void saveInitialConditions(
    struct Battleship b,
    struct EscortShip escorts[],
    int n,
    int canvasSize,
    int k)
{
    FILE *file =
        fopen(
            "initial_conditions_1B.txt",
            "w");


    if (file == NULL)
    {
        printf(
            "Error creating initial file.\n");

        return;
    }


    fprintf(
        file,
        "===== PART 1-B INITIAL CONDITIONS =====\n\n");


    fprintf(
        file,
        "Canvas: %d x %d\n",
        canvasSize,
        canvasSize);


    fprintf(
        file,
        "Number of Escorts: %d\n",
        n);


    fprintf(
        file,
        "Path Points (k): %d\n\n",
        k);


    fprintf(
        file,
        "BATTLESHIP\n");

    fprintf(
        file,
        "Type: %c\n",
        b.notation);

    fprintf(
        file,
        "Name: %s\n",
        b.name);

    fprintf(
        file,
        "Initial Position: "
        "(%.2f, %.2f)\n",
        b.x,
        b.y);

    fprintf(
        file,
        "Maximum Velocity: %.2f\n\n",
        b.maxVelocity);


    fprintf(
        file,
        "ESCORT SHIPS\n\n");


    for (int i = 0;
         i < n;
         i++)
    {
        fprintf(
            file,
            "ID: %d\n",
            escorts[i].id);

        fprintf(
            file,
            "Type: E%c\n",
            escorts[i].notation);

        fprintf(
            file,
            "Position: "
            "(%.2f, %.2f)\n",
            escorts[i].x,
            escorts[i].y);

        fprintf(
            file,
            "Velocity: %.2f - %.2f\n",
            escorts[i].minVelocity,
            escorts[i].maxVelocity);

        fprintf(
            file,
            "Angle: %.2f - %.2f\n",
            escorts[i].minAngle,
            escorts[i].maxAngle);

        fprintf(
            file,
            "Impact Power: %.2f\n\n",
            escorts[i].impactPower);
    }


    fclose(file);
}


/* -----------------------------------------
   SAVE ITERATION RESULT
----------------------------------------- */

void saveIterationResult(
    FILE *file,
    int iteration,
    struct Battleship b,
    struct EscortShip escorts[],
    int n,
    int destroyedCount,
    int battleshipSunk,
    int sinkingEscort,
    int jammed,
    float thetaMin)
{
    fprintf(
        file,
        "\n====================================\n");


    fprintf(
        file,
        "ITERATION %d\n",
        iteration);


    fprintf(
        file,
        "Battleship Position: "
        "(%.2f, %.2f)\n",
        b.x,
        b.y);


    if (jammed)
    {
        fprintf(
            file,
            "Gun Status: JAMMED\n");

        fprintf(
            file,
            "Allowed Vertical Angle: "
            "%.2f - 90 degrees\n",
            thetaMin);
    }
    else
    {
        fprintf(
            file,
            "Gun Status: NORMAL\n");
    }


    if (battleshipSunk)
    {
        fprintf(
            file,
            "Battleship Status: SUNK\n");

        fprintf(
            file,
            "Sunk By Escort ID: %d\n",
            sinkingEscort);
    }
    else
    {
        fprintf(
            file,
            "Battleship Status: ACTIVE\n");

        fprintf(
            file,
            "Total Escorts Destroyed "
            "So Far: %d\n",
            destroyedCount);
    }


    fprintf(
        file,
        "\nEscort Status:\n");


    for (int i = 0;
         i < n;
         i++)
    {
        fprintf(
            file,
            "Escort ID %d "
            "(E%c): ",
            escorts[i].id,
            escorts[i].notation);


        if (escorts[i].alive)
        {
            fprintf(
                file,
                "ACTIVE\n");
        }
        else
        {
            fprintf(
                file,
                "DESTROYED\n");
        }
    }
}


/* -----------------------------------------
   SIMULATION 1

   Battleship moves through k points.
----------------------------------------- */

void runSimulation1(
    struct Battleship initialBattleship,
    struct EscortShip initialEscorts[],
    int n,
    int canvasSize,
    int k)
{
    struct Battleship b =
        initialBattleship;


    struct EscortShip escorts[n];

    copyEscorts(
        escorts,
        initialEscorts,
        n);


    FILE *file =
        fopen(
            "simulation1_1B.txt",
            "w");


    if (file == NULL)
    {
        printf(
            "Error creating Simulation 1 file.\n");

        return;
    }


    fprintf(
        file,
        "===== PART 1-B SIMULATION 1 =====\n");


    int destroyedCount = 0;

    int battleshipSunk = 0;

    int sinkingEscort = -1;


    for (int iteration = 1;
         iteration <= k;
         iteration++)
    {
        /*
           Generate a new random point.
           Battleship follows these k points.
        */

        b.x =
            rand() %
            (canvasSize + 1);

        b.y =
            rand() %
            (canvasSize + 1);


        printf(
            "\n[Simulation 1] "
            "Iteration %d\n",
            iteration);


        printf(
            "Battleship Position: "
            "(%.0f, %.0f)\n",
            b.x,
            b.y);


        /*
           STEP 1

           Check if any active Escort
           can hit the Battleship.
        */

        for (int i = 0;
             i < n;
             i++)
        {
            if (!escorts[i].alive)
            {
                continue;
            }


            float distance =
                calculateDistance(
                    escorts[i].x,
                    escorts[i].y,
                    b.x,
                    b.y);


            float escortRange =
                calculateMaxRange(
                    escorts[i].maxVelocity);


            if (distance <= escortRange)
            {
                battleshipSunk = 1;

                sinkingEscort =
                    escorts[i].id;

                printf(
                    "Battleship SUNK by "
                    "Escort ID %d\n",
                    sinkingEscort);

                break;
            }
        }


        /*
           If Battleship is destroyed,
           simulation stops immediately.
        */

        if (battleshipSunk)
        {
            saveIterationResult(
                file,
                iteration,
                b,
                escorts,
                n,
                destroyedCount,
                battleshipSunk,
                sinkingEscort,
                0,
                0);

            break;
        }


        /*
           STEP 2

           Battleship attacks all Escorts
           inside its attack range.
        */

        float battleRange =
            calculateMaxRange(
                b.maxVelocity);


        for (int i = 0;
             i < n;
             i++)
        {
            if (!escorts[i].alive)
            {
                continue;
            }


            float distance =
                calculateDistance(
                    b.x,
                    b.y,
                    escorts[i].x,
                    escorts[i].y);


            if (distance <= battleRange)
            {
                escorts[i].alive = 0;

                destroyedCount++;


                printf(
                    "Escort ID %d "
                    "DESTROYED\n",
                    escorts[i].id);
            }
        }


        saveIterationResult(
            file,
            iteration,
            b,
            escorts,
            n,
            destroyedCount,
            battleshipSunk,
            sinkingEscort,
            0,
            0);


        /*
           If all escorts are destroyed,
           simulation can end.
        */

        if (destroyedCount == n)
        {
            break;
        }
    }


    fprintf(
        file,
        "\n===== SIMULATION 1 FINISHED =====\n");


    if (battleshipSunk)
    {
        fprintf(
            file,
            "Battleship was sunk "
            "by Escort ID %d\n",
            sinkingEscort);
    }
    else
    {
        fprintf(
            file,
            "Battleship survived.\n");
    }


    fprintf(
        file,
        "Total Escorts Destroyed: %d\n",
        destroyedCount);


    fclose(file);


    printf(
        "\nSimulation 1 results saved "
        "to simulation1_1B.txt\n");
}


/* -----------------------------------------
   SIMULATION 2

   After t iterations the gun is jammed.

   Vertical angle:
   theta_min <= theta <= 90
----------------------------------------- */

void runSimulation2(
    struct Battleship initialBattleship,
    struct EscortShip initialEscorts[],
    int n,
    int canvasSize,
    int k,
    int jamIteration,
    float thetaMin)
{
    struct Battleship b =
        initialBattleship;


    struct EscortShip escorts[n];

    copyEscorts(
        escorts,
        initialEscorts,
        n);


    FILE *file =
        fopen(
            "simulation2_1B.txt",
            "w");


    if (file == NULL)
    {
        printf(
            "Error creating Simulation 2 file.\n");

        return;
    }


    fprintf(
        file,
        "===== PART 1-B SIMULATION 2 =====\n");


    fprintf(
        file,
        "Gun jams after iteration: %d\n",
        jamIteration);


    fprintf(
        file,
        "Minimum Angle after jam: "
        "%.2f degrees\n",
        thetaMin);


    int destroyedCount = 0;

    int battleshipSunk = 0;

    int sinkingEscort = -1;


    for (int iteration = 1;
         iteration <= k;
         iteration++)
    {
        b.x =
            rand() %
            (canvasSize + 1);

        b.y =
            rand() %
            (canvasSize + 1);


        int gunJammed = 0;


        if (iteration > jamIteration)
        {
            gunJammed = 1;
        }


        printf(
            "\n[Simulation 2] "
            "Iteration %d\n",
            iteration);


        printf(
            "Battleship Position: "
            "(%.0f, %.0f)\n",
            b.x,
            b.y);


        if (gunJammed)
        {
            printf(
                "Gun JAMMED! "
                "Minimum angle = %.2f\n",
                thetaMin);
        }


        /*
           STEP 1

           Escort attacks Battleship.
        */

        for (int i = 0;
             i < n;
             i++)
        {
            if (!escorts[i].alive)
            {
                continue;
            }


            float distance =
                calculateDistance(
                    escorts[i].x,
                    escorts[i].y,
                    b.x,
                    b.y);


            float escortRange =
                calculateMaxRange(
                    escorts[i].maxVelocity);


            if (distance <= escortRange)
            {
                battleshipSunk = 1;

                sinkingEscort =
                    escorts[i].id;

                printf(
                    "Battleship SUNK by "
                    "Escort ID %d\n",
                    sinkingEscort);

                break;
            }
        }


        if (battleshipSunk)
        {
            saveIterationResult(
                file,
                iteration,
                b,
                escorts,
                n,
                destroyedCount,
                battleshipSunk,
                sinkingEscort,
                gunJammed,
                thetaMin);

            break;
        }


        /*
           STEP 2

           Battleship attacks escorts.

           If gun is jammed,
           projectile angle must be >= thetaMin.
        */

        for (int i = 0;
             i < n;
             i++)
        {
            if (!escorts[i].alive)
            {
                continue;
            }


            float distance =
                calculateDistance(
                    b.x,
                    b.y,
                    escorts[i].x,
                    escorts[i].y);


            float angle =
                calculateProjectileAngle(
                    distance,
                    b.maxVelocity);


            if (angle < 0)
            {
                continue;
            }


            if (gunJammed)
            {
                if (angle < thetaMin)
                {
                    continue;
                }
            }


            escorts[i].alive = 0;

            destroyedCount++;


            printf(
                "Escort ID %d "
                "DESTROYED "
                "(Angle %.2f)\n",
                escorts[i].id,
                angle);
        }


        saveIterationResult(
            file,
            iteration,
            b,
            escorts,
            n,
            destroyedCount,
            battleshipSunk,
            sinkingEscort,
            gunJammed,
            thetaMin);


        if (destroyedCount == n)
        {
            break;
        }
    }


    fprintf(
        file,
        "\n===== SIMULATION 2 FINISHED =====\n");


    if (battleshipSunk)
    {
        fprintf(
            file,
            "Battleship was sunk "
            "by Escort ID %d\n",
            sinkingEscort);
    }
    else
    {
        fprintf(
            file,
            "Battleship survived.\n");
    }


    fprintf(
        file,
        "Total Escorts Destroyed: %d\n",
        destroyedCount);


    fclose(file);


    printf(
        "\nSimulation 2 results saved "
        "to simulation2_1B.txt\n");
}


/* -----------------------------------------
   MAIN
----------------------------------------- */

int main()
{
    srand(time(NULL));


    struct Battleship battleship;


    int canvasSize;

    int numberOfEscorts;

    int k;

    int jamIteration;

    float thetaMin;


    printf(
        "====================================\n");

    printf(
        "ADVANCED NAVAL BATTLE SIMULATOR\n");

    printf(
        "PART 1 - B\n");

    printf(
        "====================================\n");


    printf(
        "\nEnter Canvas Size (D): ");

    scanf(
        "%d",
        &canvasSize);


    printf(
        "Enter Number of Escort Ships (N): ");

    scanf(
        "%d",
        &numberOfEscorts);


    printf(
        "Enter Number of Path Points (k): ");

    scanf(
        "%d",
        &k);


    do
    {
        printf(
            "Enter Gun Jamming Iteration "
            "(t < k): ");

        scanf(
            "%d",
            &jamIteration);

    }
    while (
        jamIteration <= 0 ||
        jamIteration >= k
    );


    do
    {
        printf(
            "Enter Minimum Angle after "
            "Gun Jam (0 < theta < 30): ");

        scanf(
            "%f",
            &thetaMin);

    }
    while (
        thetaMin <= 0 ||
        thetaMin >= 30
    );


    setupBattleship(
        &battleship);


    /*
       Generate escort ships.
    */

    struct EscortShip escorts[
        numberOfEscorts];


    for (int i = 0;
         i < numberOfEscorts;
         i++)
    {
        createEscortShip(
            &escorts[i],
            i + 1,
            canvasSize,
            battleship.maxVelocity);
    }


    /*
       Generate first path point.
    */

    battleship.x =
        rand() %
        (canvasSize + 1);


    battleship.y =
        rand() %
        (canvasSize + 1);


    /*
       Save same initial conditions
       for both simulations.
    */

    saveInitialConditions(
        battleship,
        escorts,
        numberOfEscorts,
        canvasSize,
        k);


    printf(
        "\nInitial conditions saved "
        "to initial_conditions_1B.txt\n");


    /*
       SIMULATION 1
    */

    runSimulation1(
        battleship,
        escorts,
        numberOfEscorts,
        canvasSize,
        k);


    /*
       SIMULATION 2

       Same initial escorts.
       Same starting conditions.
    */

    runSimulation2(
        battleship,
        escorts,
        numberOfEscorts,
        canvasSize,
        k,
        jamIteration,
        thetaMin);


    printf(
        "\n====================================\n");

    printf(
        "PART 1-B COMPLETED\n");

    printf(
        "====================================\n");


    printf(
        "\nFiles Generated:\n");

    printf(
        "1. initial_conditions_1B.txt\n");

    printf(
        "2. simulation1_1B.txt\n");

    printf(
        "3. simulation2_1B.txt\n");


    return 0;
}
