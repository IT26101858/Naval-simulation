#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_SHIPS 100
#define MAX_POINTS 100
#define GRAVITY 9.81
#define PI 3.14159265358979323846


/* =====================================
            BATTLESHIP STRUCT
   ===================================== */

typedef struct
{
    char type;

    double x;
    double y;

    double maxVelocity;

    int alive;

    /*
       Total damage received by B

       0.00 = 0%
       0.50 = 50%
       1.00 = 100%
    */

    double cumulativeImpact;

} Battleship;


/* =====================================
           ESCORT SHIP STRUCT
   ===================================== */

typedef struct
{
    int id;

    char type;

    double x;
    double y;

    double minVelocity;
    double maxVelocity;

    double minAngle;
    double maxAngle;

    double impactPower;

    int alive;

    /*
       Part 1-C:
       One Escort can fire only once.
    */

    int hasFired;

} EscortShip;


/* =====================================
         RANDOM DOUBLE FUNCTION
   ===================================== */

double randomDouble(double min,
                    double max)
{
    return min +
           ((double)rand() / RAND_MAX)
           * (max - min);
}


/* =====================================
       DISTANCE BETWEEN TWO SHIPS
   ===================================== */

double distanceBetween(double x1,
                       double y1,
                       double x2,
                       double y2)
{
    double dx;
    double dy;

    dx = x2 - x1;

    dy = y2 - y1;


    return sqrt(
        dx * dx +
        dy * dy
    );
}


/* =====================================
          ESCORT ANGLE RANGE
   ===================================== */

double getAngleRange(char type)
{
    switch(type)
    {
        case 'A':
            return 20.0;

        case 'B':
            return 30.0;

        case 'C':
            return 25.0;

        case 'D':
            return 50.0;

        case 'E':
            return 70.0;

        default:
            return 0.0;
    }
}


/* =====================================
          ESCORT IMPACT POWER
   ===================================== */

double getImpactPower(char type)
{
    switch(type)
    {
        case 'A':
            return 0.08;

        case 'B':
            return 0.06;

        case 'C':
            return 0.07;

        case 'D':
            return 0.05;

        case 'E':
            return 0.04;

        default:
            return 0.0;
    }
}


/* =====================================
       SET ESCORT SHIP PROPERTIES
   ===================================== */

void setEscortProperties(
    EscortShip *ship,
    char type,
    double battleshipMaxVelocity
)
{
    double angleRange;


    angleRange =
        getAngleRange(type);


    ship->type =
        type;


    ship->impactPower =
        getImpactPower(type);


    /*
       Generate minimum vertical angle
    */

    ship->minAngle =
        randomDouble(
            1.0,
            89.0 - angleRange
        );


    /*
       Angle range =
       maxAngle - minAngle
    */

    ship->maxAngle =
        ship->minAngle
        + angleRange;


    /*
       EA maximum velocity
       = 1.2 * B max velocity
    */

    if(type == 'A')
    {
        ship->maxVelocity =
            1.2
            * battleshipMaxVelocity;
    }

    else
    {
        ship->maxVelocity =
            randomDouble(
                0.55 *
                battleshipMaxVelocity,

                0.95 *
                battleshipMaxVelocity
            );
    }


    ship->minVelocity =
        randomDouble(
            0.30 *
            ship->maxVelocity,

            0.60 *
            ship->maxVelocity
        );


    /*
       Ship is alive initially
    */

    ship->alive =
        1;


    /*
       Ship has not fired yet
    */

    ship->hasFired =
        0;
}


/* =====================================
         PROJECTILE HIT CHECK
   ===================================== */

int canHit(
    double distance,

    double minVelocity,
    double maxVelocity,

    double minAngle,
    double maxAngle,

    double *hitTime,
    double *selectedAngle,
    double *selectedVelocity
)
{
    double angle;


    for(
        angle = minAngle;

        angle <= maxAngle;

        angle += 0.1
    )
    {
        double radians;

        double sinValue;

        double requiredVelocity;

        double time;


        /*
           Degrees -> radians
        */

        radians =
            angle
            * PI
            / 180.0;


        sinValue =
            sin(
                2.0 *
                radians
            );


        if(sinValue <= 0.0)
        {
            continue;
        }


        /*
           Projectile range:

           R = u^2 sin(2 theta) / g


           Rearrange:

           u = sqrt(
                   Rg /
                   sin(2 theta)
               )
        */

        requiredVelocity =
            sqrt(
                (
                    distance
                    * GRAVITY
                )
                /
                sinValue
            );


        /*
           Required velocity must be
           inside allowed range.
        */

        if(
            requiredVelocity
                >= minVelocity
            &&
            requiredVelocity
                <= maxVelocity
        )
        {
            /*
               Projectile flight time

               t = 2u sin(theta) / g
            */

            time =
                (
                    2.0
                    * requiredVelocity
                    * sin(radians)
                )
                /
                GRAVITY;


            *hitTime =
                time;


            *selectedAngle =
                angle;


            *selectedVelocity =
                requiredVelocity;


            return 1;
        }
    }


    return 0;
}


/* =====================================
          B ATTACKS ESCORT
   ===================================== */

int battleshipCanHit(
    Battleship B,

    EscortShip E,

    double bMinAngle,
    double bMaxAngle,

    double *time,
    double *angle,
    double *velocity
)
{
    double distance;


    distance =
        distanceBetween(
            B.x,
            B.y,

            E.x,
            E.y
        );


    return canHit(
        distance,

        0.0,
        B.maxVelocity,

        bMinAngle,
        bMaxAngle,

        time,
        angle,
        velocity
    );
}


/* =====================================
          ESCORT ATTACKS B
   ===================================== */

int escortCanHit(
    Battleship B,

    EscortShip E,

    double *time,
    double *angle,
    double *velocity
)
{
    double distance;


    distance =
        distanceBetween(
            E.x,
            E.y,

            B.x,
            B.y
        );


    return canHit(
        distance,

        E.minVelocity,
        E.maxVelocity,

        E.minAngle,
        E.maxAngle,

        time,
        angle,
        velocity
    );
}


/* =====================================
           COPY ESCORT ARRAY
   ===================================== */

void copyEscorts(
    EscortShip destination[],
    EscortShip source[],
    int N
)
{
    for(int i = 0;
        i < N;
        i++)
    {
        destination[i] =
            source[i];
    }
}


/* =====================================
          SAVE BATTLEFIELD
   ===================================== */

void saveBattlefield(
    const char *filename,

    Battleship B,

    EscortShip E[],

    int N,

    double D
)
{
    FILE *file;


    file =
        fopen(
            filename,
            "w"
        );


    if(file == NULL)
    {
        printf(
            "Error opening file.\n"
        );

        return;
    }


    fprintf(
        file,

        "Canvas = %.2f x %.2f\n",

        D,
        D
    );


    fprintf(
        file,

        "Battleship type = %c\n",

        B.type
    );


    fprintf(
        file,

        "Battleship position = "
        "(%.2f, %.2f)\n",

        B.x,
        B.y
    );


    fprintf(
        file,

        "Battleship max velocity "
        "= %.2f\n",

        B.maxVelocity
    );


    fprintf(
        file,

        "Battleship alive = %d\n",

        B.alive
    );


    fprintf(
        file,

        "Cumulative impact on B "
        "= %.2f\n\n",

        B.cumulativeImpact
    );


    for(int i = 0;
        i < N;
        i++)
    {
        fprintf(
            file,

            "Escort %d\n",

            E[i].id
        );


        fprintf(
            file,

            "Type = E%c\n",

            E[i].type
        );


        fprintf(
            file,

            "Position = "
            "(%.2f, %.2f)\n",

            E[i].x,
            E[i].y
        );


        fprintf(
            file,

            "Velocity = "
            "%.2f - %.2f\n",

            E[i].minVelocity,

            E[i].maxVelocity
        );


        fprintf(
            file,

            "Angle = "
            "%.2f - %.2f\n",

            E[i].minAngle,

            E[i].maxAngle
        );


        fprintf(
            file,

            "Impact power = %.2f\n",

            E[i].impactPower
        );


        fprintf(
            file,

            "Alive = %d\n",

            E[i].alive
        );


        fprintf(
            file,

            "Has fired = %d\n\n",

            E[i].hasFired
        );
    }


    fclose(file);
}


/* =========================================
        ONE PART 1-C BATTLE ITERATION
   ========================================= */

int runBattleStep(
    Battleship *B,

    EscortShip E[],

    int N,

    double bMinAngle,
    double bMaxAngle,

    FILE *output,

    int iteration
)
{
    fprintf(
        output,

        "\n========================\n"
    );


    fprintf(
        output,

        "ITERATION %d\n",

        iteration
    );


    fprintf(
        output,

        "B position = "
        "(%.2f, %.2f)\n",

        B->x,
        B->y
    );


    /* ==================================
       ESCORT SHIPS ATTACK B
       ================================== */

    for(int i = 0;
        i < N &&
        B->alive;
        i++)
    {
        double time;

        double angle;

        double velocity;


        /*
           Destroyed Escort cannot fire.
        */

        if(E[i].alive == 0)
        {
            continue;
        }


        /*
           Part 1-C:
           one E can fire only once.
        */

        if(E[i].hasFired == 1)
        {
            continue;
        }


        if(
            escortCanHit(
                *B,

                E[i],

                &time,

                &angle,

                &velocity
            )
        )
        {
            /*
               E has now used its
               one attack.
            */

            E[i].hasFired =
                1;


            /*
               Add Escort impact power
               to total damage on B.
            */

            B->cumulativeImpact +=
                E[i].impactPower;


            fprintf(
                output,

                "Escort E%d hit B\n",

                E[i].id
            );


            fprintf(
                output,

                "Escort type = E%c\n",

                E[i].type
            );


            fprintf(
                output,

                "Impact power = %.2f\n",

                E[i].impactPower
            );


            fprintf(
                output,

                "Cumulative impact "
                "= %.2f\n",

                B->cumulativeImpact
            );


            fprintf(
                output,

                "Time to hit "
                "= %.2f seconds\n\n",

                time
            );


            /*
               1.0 = 100% damage
            */

            if(
                B->cumulativeImpact
                >= 1.0
            )
            {
                B->alive =
                    0;


                fprintf(
                    output,

                    "BATTLESHIP DESTROYED\n"
                );


                fprintf(
                    output,

                    "Final cumulative "
                    "impact = %.2f\n",

                    B->cumulativeImpact
                );
            }
        }
    }


    /*
       If B was destroyed,
       stop this battle step.
    */

    if(B->alive == 0)
    {
        return 0;
    }


    /* ==================================
       B ATTACKS ESCORT SHIPS
       ================================== */

    int destroyedThisStep =
        0;


    for(int i = 0;
        i < N;
        i++)
    {
        double time;

        double angle;

        double velocity;


        if(E[i].alive == 0)
        {
            continue;
        }


        if(
            battleshipCanHit(
                *B,

                E[i],

                bMinAngle,

                bMaxAngle,

                &time,

                &angle,

                &velocity
            )
        )
        {
            /*
               Part 1-C:
               B can still destroy
               an E with one hit.
            */

            E[i].alive =
                0;


            destroyedThisStep++;


            fprintf(
                output,

                "B destroyed Escort E%d\n",

                E[i].id
            );


            fprintf(
                output,

                "Escort type = E%c\n",

                E[i].type
            );


            fprintf(
                output,

                "Time to hit "
                "= %.2f seconds\n",

                time
            );


            fprintf(
                output,

                "Firing angle "
                "= %.2f degrees\n",

                angle
            );


            fprintf(
                output,

                "Shell velocity "
                "= %.2f\n\n",

                velocity
            );
        }
    }


    fprintf(
        output,

        "Escorts destroyed "
        "this iteration = %d\n",

        destroyedThisStep
    );


    fprintf(
        output,

        "Cumulative impact on B "
        "= %.2f\n",

        B->cumulativeImpact
    );


    return 1;
}


/* =========================================
       PART 1-C : REDO PART 1-A
   ========================================= */

void runPart1C_A(
    Battleship B,

    EscortShip E[],

    int N,

    double D
)
{
    FILE *output;


    output =
        fopen(
            "part1C_A_results.txt",
            "w"
        );


    if(output == NULL)
    {
        printf(
            "Error opening file.\n"
        );

        return;
    }


    /*
       Stationary B:
       one battle step only.
    */

    runBattleStep(
        &B,

        E,

        N,

        0.1,

        89.9,

        output,

        1
    );


    if(B.alive)
    {
        fprintf(
            output,

            "\nBattleship survived.\n"
        );


        fprintf(
            output,

            "Final cumulative impact "
            "= %.2f\n",

            B.cumulativeImpact
        );
    }

    else
    {
        fprintf(
            output,

            "\nBattleship destroyed.\n"
        );


        fprintf(
            output,

            "Final cumulative impact "
            "= %.2f\n",

            B.cumulativeImpact
        );
    }


    fclose(output);


    saveBattlefield(
        "part1C_A_final.txt",

        B,

        E,

        N,

        D
    );
}


/* =========================================
    PART 1-C : REDO PART 1-B SIMULATION 1
   ========================================= */

void runPart1C_B1(
    Battleship B,

    EscortShip E[],

    int N,

    double pathX[],

    double pathY[],

    int k,

    double D
)
{
    FILE *output;


    output =
        fopen(
            "part1C_B1_results.txt",
            "w"
        );


    if(output == NULL)
    {
        printf(
            "Error opening file.\n"
        );

        return;
    }


    for(int step = 0;
        step < k &&
        B.alive;
        step++)
    {
        /*
           Move B to next path point.
        */

        B.x =
            pathX[step];


        B.y =
            pathY[step];


        runBattleStep(
            &B,

            E,

            N,

            0.1,

            89.9,

            output,

            step + 1
        );
    }


    fprintf(
        output,

        "\nFinal cumulative "
        "impact on B = %.2f\n",

        B.cumulativeImpact
    );


    fclose(output);


    saveBattlefield(
        "part1C_B1_final.txt",

        B,

        E,

        N,

        D
    );
}


/* =========================================
    PART 1-C : REDO PART 1-B SIMULATION 2
   ========================================= */

void runPart1C_B2(
    Battleship B,

    EscortShip E[],

    int N,

    double pathX[],

    double pathY[],

    int k,

    int t,

    double thetaMin,

    double D
)
{
    FILE *output;


    output =
        fopen(
            "part1C_B2_results.txt",
            "w"
        );


    if(output == NULL)
    {
        printf(
            "Error opening file.\n"
        );

        return;
    }


    for(int step = 0;
        step < k &&
        B.alive;
        step++)
    {
        double bMinAngle;


        B.x =
            pathX[step];


        B.y =
            pathY[step];


        /*
           After t iterations,
           gun becomes jammed.
        */

        if(
            (step + 1) > t
        )
        {
            bMinAngle =
                thetaMin;
        }

        else
        {
            bMinAngle =
                0.1;
        }


        if(
            (step + 1) > t
        )
        {
            fprintf(
                output,

                "\nGun status = JAMMED\n"
            );
        }

        else
        {
            fprintf(
                output,

                "\nGun status = NORMAL\n"
            );
        }


        runBattleStep(
            &B,

            E,

            N,

            bMinAngle,

            89.9,

            output,

            step + 1
        );
    }


    fprintf(
        output,

        "\nFinal cumulative "
        "impact on B = %.2f\n",

        B.cumulativeImpact
    );


    fclose(output);


    saveBattlefield(
        "part1C_B2_final.txt",

        B,

        E,

        N,

        D
    );
}


/* =========================================
                  MAIN
   ========================================= */

int main()
{
    Battleship initialB;


    EscortShip initialE[MAX_SHIPS];


    EscortShip partA_E[MAX_SHIPS];

    EscortShip partB1_E[MAX_SHIPS];

    EscortShip partB2_E[MAX_SHIPS];


    double pathX[MAX_POINTS];

    double pathY[MAX_POINTS];


    int N;

    int k;

    int t;


    double D;

    double thetaMin;


    unsigned int seed;


    printf(
        "========== PART 1-C ==========\n"
    );


    /* =================================
       Canvas size
       ================================= */

    do
    {
        printf(
            "Enter battlefield size D: "
        );


        scanf(
            "%lf",
            &D
        );

    }
    while(D <= 0.0);


    /* =================================
       Number of Escorts
       ================================= */

    do
    {
        printf(
            "Enter number of "
            "Escort ships: "
        );


        scanf(
            "%d",
            &N
        );

    }
    while(
        N < 1 ||
        N > MAX_SHIPS
    );


    /* =================================
       Number of path points
       ================================= */

    do
    {
        printf(
            "Enter number of "
            "path points k: "
        );


        scanf(
            "%d",
            &k
        );

    }
    while(
        k < 2 ||
        k > MAX_POINTS
    );


    /* =================================
       Battleship type
       ================================= */

    do
    {
        printf(
            "Enter Battleship type "
            "(U/M/R/S): "
        );


        scanf(
            " %c",
            &initialB.type
        );

    }
    while(
        initialB.type != 'U'
        &&
        initialB.type != 'M'
        &&
        initialB.type != 'R'
        &&
        initialB.type != 'S'
    );


    /* =================================
       Battleship velocity
       ================================= */

    do
    {
        printf(
            "Enter Battleship maximum "
            "shell velocity: "
        );


        scanf(
            "%lf",
            &initialB.maxVelocity
        );

    }
    while(
        initialB.maxVelocity
        <= 0.0
    );


    /* =================================
       Battleship X
       ================================= */

    do
    {
        printf(
            "Enter initial "
            "Battleship X position: "
        );


        scanf(
            "%lf",
            &initialB.x
        );

    }
    while(
        initialB.x < 0.0
        ||
        initialB.x > D
    );


    /* =================================
       Battleship Y
       ================================= */

    do
    {
        printf(
            "Enter initial "
            "Battleship Y position: "
        );


        scanf(
            "%lf",
            &initialB.y
        );

    }
    while(
        initialB.y < 0.0
        ||
        initialB.y > D
    );


    /*
       B is alive initially.
    */

    initialB.alive =
        1;


    /*
       No damage initially.
    */

    initialB.cumulativeImpact =
        0.0;


    /* =================================
       Random seed
       ================================= */

    printf(
        "Enter random seed: "
    );


    scanf(
        "%u",
        &seed
    );


    srand(seed);


    /* =================================
       Generate Escort ships
       ================================= */

    for(int i = 0;
        i < N;
        i++)
    {
        char type;


        initialE[i].id =
            i + 1;


        initialE[i].x =
            randomDouble(
                0.0,
                D
            );


        initialE[i].y =
            randomDouble(
                0.0,
                D
            );


        /*
           Random type A-E
        */

        type =
            'A'
            + rand() % 5;


        setEscortProperties(
            &initialE[i],

            type,

            initialB.maxVelocity
        );
    }


    /* =================================
       Generate B path
       ================================= */

    pathX[0] =
        initialB.x;


    pathY[0] =
        initialB.y;


    for(int i = 1;
        i < k;
        i++)
    {
        pathX[i] =
            randomDouble(
                0.0,
                D
            );


        pathY[i] =
            randomDouble(
                0.0,
                D
            );
    }


    /* =================================
       Simulation 2 settings
       ================================= */

    do
    {
        printf(
            "Enter t "
            "(0 < t < k): "
        );


        scanf(
            "%d",
            &t
        );

    }
    while(
        t <= 0
        ||
        t >= k
    );


    /*
       PDF:
       0 < thetaMin < 30
    */

    do
    {
        printf(
            "Enter theta_min "
            "(0 < theta_min < 30): "
        );


        scanf(
            "%lf",
            &thetaMin
        );

    }
    while(
        thetaMin <= 0.0
        ||
        thetaMin >= 30.0
    );


    /* =================================
       Save original conditions
       ================================= */

    saveBattlefield(
        "part1C_initial.txt",

        initialB,

        initialE,

        N,

        D
    );


    /*
       Each simulation must start
       from original Escort data.
    */

    copyEscorts(
        partA_E,

        initialE,

        N
    );


    copyEscorts(
        partB1_E,

        initialE,

        N
    );


    copyEscorts(
        partB2_E,

        initialE,

        N
    );


    /* =================================
       REDO PART 1-A
       ================================= */

    printf(
        "\n--- Part 1-C: "
        "Redo Part 1-A ---\n"
    );


    runPart1C_A(
        initialB,

        partA_E,

        N,

        D
    );


    /* =================================
       REDO PART 1-B SIMULATION 1
       ================================= */

    printf(
        "--- Part 1-C: "
        "Redo Part 1-B "
        "Simulation 1 ---\n"
    );


    runPart1C_B1(
        initialB,

        partB1_E,

        N,

        pathX,

        pathY,

        k,

        D
    );


    /* =================================
       REDO PART 1-B SIMULATION 2
       ================================= */

    printf(
        "--- Part 1-C: "
        "Redo Part 1-B "
        "Simulation 2 ---\n"
    );


    runPart1C_B2(
        initialB,

        partB2_E,

        N,

        pathX,

        pathY,

        k,

        t,

        thetaMin,

        D
    );


    printf(
        "\nPart 1-C completed.\n"
    );


    return 0;
}
