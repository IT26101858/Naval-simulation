#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_SHIPS 100
#define MAX_POINTS 100
#define GRAVITY 9.81
#define PI 3.14159265358979323846
#define INF_TIME 1.0e12


/* =====================================
             BATTLESHIP
   ===================================== */

typedef struct
{
    char type;

    double x;
    double y;

    double maxVelocity;

    /*
       Part 2-A

       Time between two consecutive
       gun firings of B
    */

    double firingInterval;

    /*
       Used when redoing Part 1-C
    */

    double cumulativeImpact;

    int alive;

} Battleship;


/* =====================================
             ESCORT SHIP
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
       In Part 2-A an Escort
       can still fire only once.
    */

    int hasFired;

} EscortShip;


/* =====================================
          RANDOM DOUBLE
   ===================================== */

double randomDouble(double min,
                    double max)
{
    return min +
           ((double)rand() / RAND_MAX)
           * (max - min);
}


/* =====================================
       DISTANCE BETWEEN SHIPS
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
       SET ESCORT PROPERTIES
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
       Generate minimum angle
    */

    ship->minAngle =
        randomDouble(
            1.0,
            89.0 - angleRange
        );


    /*
       max angle =
       min angle + angle range
    */

    ship->maxAngle =
        ship->minAngle
        + angleRange;


    /*
       EA maximum velocity
    */

    if(type == 'A')
    {
        ship->maxVelocity =
            1.2 *
            battleshipMaxVelocity;
    }

    else
    {
        /*
           Student implementation
           choice.

           Random velocity less
           than B maximum velocity.
        */

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


    ship->alive =
        1;


    ship->hasFired =
        0;
}


/* =====================================
          PROJECTILE CHECK
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
           Convert degrees
           into radians
        */

        radians =
            angle *
            PI /
            180.0;


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
           R = u^2 sin(2theta) / g

           Rearrange:

           u = sqrt(
                   Rg /
                   sin(2theta)
               )
        */

        requiredVelocity =
            sqrt(
                (
                    distance *
                    GRAVITY
                )
                /
                sinValue
            );


        /*
           Check whether velocity
           is inside gun limits
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
                    2.0 *
                    requiredVelocity *
                    sin(radians)
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

    double minAngle,
    double maxAngle,

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

        minAngle,
        maxAngle,

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
          ATTACK STRATEGY
   ===================================== */

/*
   Higher score =
   higher attack priority.

   Strategy:

   1. If E can hit B,
      give very high priority.

   2. Higher impact power
      gets higher priority.

   3. Faster enemy hit
      gets slightly higher priority.

   4. Non-dangerous ships
      are ranked using distance.
*/

double threatScore(
    Battleship B,
    EscortShip E
)
{
    double enemyHitTime;

    double enemyAngle;

    double enemyVelocity;

    double distance;


    distance =
        distanceBetween(
            B.x,
            B.y,

            E.x,
            E.y
        );


    /*
       Check whether E
       can attack B
    */

    if(
        escortCanHit(
            B,

            E,

            &enemyHitTime,

            &enemyAngle,

            &enemyVelocity
        )
    )
    {
        return
            10000.0

            +

            E.impactPower *
            1000.0

            -

            enemyHitTime;
    }


    /*
       If E cannot hit B,
       closer ships get slightly
       higher priority.
    */

    return
        1000.0
        -
        distance;
}


/* =====================================
       BUILD B ATTACK ORDER
   ===================================== */

int buildAttackOrder(
    Battleship B,

    EscortShip E[],

    int N,

    double bMinAngle,
    double bMaxAngle,

    int order[]
)
{
    int count;


    count =
        0;


    /*
       First find all Escorts
       that B can attack.
    */

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
                B,

                E[i],

                bMinAngle,

                bMaxAngle,

                &time,

                &angle,

                &velocity
            )
        )
        {
            order[count] =
                i;


            count++;
        }
    }


    /*
       Sort by threat score.

       Highest threat first.
    */

    for(int i = 0;
        i < count - 1;
        i++)
    {
        int best;


        best =
            i;


        for(int j = i + 1;
            j < count;
            j++)
        {
            if(
                threatScore(
                    B,
                    E[order[j]]
                )

                >

                threatScore(
                    B,
                    E[order[best]]
                )
            )
            {
                best =
                    j;
            }
        }


        /*
           Swap
        */

        if(best != i)
        {
            int temp;


            temp =
                order[i];


            order[i] =
                order[best];


            order[best] =
                temp;
        }
    }


    return count;
}


/* =====================================
       APPLY ESCORT DAMAGE TO B
   ===================================== */

void applyEscortDamage(
    Battleship *B,

    EscortShip *E,

    int cumulativeMode,

    FILE *output,

    double impactTime
)
{
    /*
       E has now used its
       single attack.
    */

    E->hasFired =
        1;


    fprintf(
        output,

        "Escort E%d type E%c "
        "hit B at %.2f seconds\n",

        E->id,

        E->type,

        impactTime
    );


    fprintf(
        output,

        "Impact power = %.2f\n",

        E->impactPower
    );


    /* =================================
       Part 1-A / 1-B behaviour
       ================================= */

    if(cumulativeMode == 0)
    {
        /*
           One Escort shell
           destroys B.
        */

        B->alive =
            0;


        fprintf(
            output,

            "Battleship destroyed "
            "by Escort E%d\n",

            E->id
        );
    }


    /* =================================
       Part 1-C behaviour
       ================================= */

    else
    {
        /*
           Add impact power
        */

        B->cumulativeImpact +=
            E->impactPower;


        fprintf(
            output,

            "Cumulative impact "
            "on B = %.2f\n",

            B->cumulativeImpact
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

                "Battleship destroyed.\n"
            );


            fprintf(
                output,

                "Cumulative impact "
                "reached 100%%.\n"
            );
        }
    }
}


/* =====================================
       PROCESS ESCORT ATTACKS
   ===================================== */

void resolveEscortAttacksUntil(
    Battleship *B,

    EscortShip E[],

    int N,

    double timeLimit,

    int cumulativeMode,

    FILE *output
)
{
    while(B->alive)
    {
        int earliestEscort;


        double earliestTime;


        earliestEscort =
            -1;


        earliestTime =
            INF_TIME;


        /*
           Find the Escort whose shell
           reaches B first.
        */

        for(int i = 0;
            i < N;
            i++)
        {
            double time;

            double angle;

            double velocity;


            /*
               Destroyed E cannot attack
            */

            if(E[i].alive == 0)
            {
                continue;
            }


            /*
               E can only fire once
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
                if(
                    time <= timeLimit
                    &&
                    time < earliestTime
                )
                {
                    earliestTime =
                        time;


                    earliestEscort =
                        i;
                }
            }
        }


        /*
           No Escort shell reaches
           B before time limit.
        */

        if(earliestEscort == -1)
        {
            break;
        }


        applyEscortDamage(
            B,

            &E[earliestEscort],

            cumulativeMode,

            output,

            earliestTime
        );
    }
}


/* =====================================
        ONE BATTLE ITERATION
   ===================================== */

void runBattleAtCurrentPosition(
    Battleship *B,

    EscortShip E[],

    int N,

    double bMinAngle,
    double bMaxAngle,

    int cumulativeMode,

    FILE *output,

    int iteration
)
{
    int attackOrder[MAX_SHIPS];

    int attackCount;


    fprintf(
        output,

        "\n================================\n"
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


    fprintf(
        output,

        "B firing interval "
        "T_B = %.2f seconds\n",

        B->firingInterval
    );


    fprintf(
        output,

        "B vertical angle range "
        "= %.2f - %.2f\n",

        bMinAngle,

        bMaxAngle
    );


    /* =================================
       CREATE ATTACK ORDER
       ================================= */

    attackCount =
        buildAttackOrder(
            *B,

            E,

            N,

            bMinAngle,

            bMaxAngle,

            attackOrder
        );


    fprintf(
        output,

        "Attack order: "
    );


    if(attackCount == 0)
    {
        fprintf(
            output,

            "No reachable Escort ships\n"
        );
    }

    else
    {
        for(int i = 0;
            i < attackCount;
            i++)
        {
            fprintf(
                output,

                "E%d",

                E[attackOrder[i]].id
            );


            if(
                i <
                attackCount - 1
            )
            {
                fprintf(
                    output,

                    " -> "
                );
            }
        }


        fprintf(
            output,

            "\n"
        );
    }


    /* =================================
       B ATTACKS USING T_B
       ================================= */

    for(int shot = 0;

        shot < attackCount
        &&
        B->alive;

        shot++)
    {
        int index;


        double flightTime;

        double angle;

        double velocity;

        double firingTime;

        double impactTime;


        index =
            attackOrder[shot];


        if(E[index].alive == 0)
        {
            continue;
        }


        if(
            battleshipCanHit(
                *B,

                E[index],

                bMinAngle,

                bMaxAngle,

                &flightTime,

                &angle,

                &velocity
            )
        )
        {
            /*
               PART 2-A IMPORTANT:

               Shot 1 -> 0 seconds

               Shot 2 -> T_B

               Shot 3 -> 2 * T_B

               Shot 4 -> 3 * T_B
            */

            firingTime =
                shot *
                B->firingInterval;


            /*
               Total time until
               shell hits target.
            */

            impactTime =
                firingTime
                +
                flightTime;


            /*
               Before B shell reaches E,
               check whether any Escort
               shell reaches B.
            */

            resolveEscortAttacksUntil(
                B,

                E,

                N,

                impactTime,

                cumulativeMode,

                output
            );


            /*
               B may have been destroyed
               before its shell reached E.
            */

            if(B->alive == 0)
            {
                break;
            }


            /*
               Successful B shell
               destroys E.
            */

            E[index].alive =
                0;


            fprintf(
                output,

                "\nB attacked Escort E%d\n",

                E[index].id
            );


            fprintf(
                output,

                "B firing time "
                "= %.2f seconds\n",

                firingTime
            );


            fprintf(
                output,

                "Shell flight time "
                "= %.2f seconds\n",

                flightTime
            );


            fprintf(
                output,

                "Impact time "
                "= %.2f seconds\n",

                impactTime
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
                "= %.2f\n",

                velocity
            );


            fprintf(
                output,

                "Escort E%d destroyed\n",

                E[index].id
            );
        }
    }


    /*
       Remaining alive Escorts
       can use their one attack.
    */

    if(B->alive)
    {
        resolveEscortAttacksUntil(
            B,

            E,

            N,

            INF_TIME,

            cumulativeMode,

            output
        );
    }


    if(
        cumulativeMode
        &&
        B->alive
    )
    {
        fprintf(
            output,

            "\nB survived iteration.\n"
        );


        fprintf(
            output,

            "Cumulative impact "
            "= %.2f\n",

            B->cumulativeImpact
        );
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
            "Could not open %s\n",

            filename
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

        "B type = %c\n",

        B.type
    );


    fprintf(
        file,

        "B position = "
        "(%.2f, %.2f)\n",

        B.x,

        B.y
    );


    fprintf(
        file,

        "B max velocity = %.2f\n",

        B.maxVelocity
    );


    fprintf(
        file,

        "B firing interval T_B "
        "= %.2f\n",

        B.firingInterval
    );


    fprintf(
        file,

        "B cumulative impact "
        "= %.2f\n",

        B.cumulativeImpact
    );


    fprintf(
        file,

        "B alive = %d\n\n",

        B.alive
    );


    for(int i = 0;
        i < N;
        i++)
    {
        fprintf(
            file,

            "Escort E%d\n",

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

            "Angles = "
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


/* =====================================
       STATIONARY SIMULATION
   ===================================== */

void runStationarySimulation(
    const char *resultFilename,

    const char *finalFilename,

    Battleship initialB,

    EscortShip initialE[],

    int N,

    double D,

    int cumulativeMode
)
{
    Battleship B;


    EscortShip E[MAX_SHIPS];


    FILE *output;


    B =
        initialB;


    copyEscorts(
        E,

        initialE,

        N
    );


    output =
        fopen(
            resultFilename,

            "w"
        );


    if(output == NULL)
    {
        printf(
            "Error opening result file.\n"
        );

        return;
    }


    /*
       One stationary battle
    */

    runBattleAtCurrentPosition(
        &B,

        E,

        N,

        0.1,

        89.9,

        cumulativeMode,

        output,

        1
    );


    fprintf(
        output,

        "\nFinal B status = %s\n",

        B.alive
        ?
        "ALIVE"
        :
        "DESTROYED"
    );


    if(cumulativeMode)
    {
        fprintf(
            output,

            "Final cumulative impact "
            "= %.2f\n",

            B.cumulativeImpact
        );
    }


    fclose(output);


    saveBattlefield(
        finalFilename,

        B,

        E,

        N,

        D
    );
}


/* =====================================
          PATH SIMULATION
   ===================================== */

void runPathSimulation(
    const char *resultFilename,

    const char *finalFilename,

    Battleship initialB,

    EscortShip initialE[],

    int N,

    double pathX[],

    double pathY[],

    int k,

    int jamMode,

    int t,

    double thetaMin,

    double D,

    int cumulativeMode
)
{
    Battleship B;


    EscortShip E[MAX_SHIPS];


    FILE *output;


    B =
        initialB;


    copyEscorts(
        E,

        initialE,

        N
    );


    output =
        fopen(
            resultFilename,

            "w"
        );


    if(output == NULL)
    {
        printf(
            "Error opening result file.\n"
        );

        return;
    }


    /*
       Repeat for k path points
       or until B is destroyed.
    */

    for(int step = 0;

        step < k
        &&
        B.alive;

        step++)
    {
        double bMinAngle;


        B.x =
            pathX[step];


        B.y =
            pathY[step];


        /*
           Normal B angle
        */

        bMinAngle =
            0.1;


        /*
           Part 1-B Simulation 2:

           After t iterations,
           B gun is jammed.
        */

        if(
            jamMode
            &&
            (step + 1) > t
        )
        {
            bMinAngle =
                thetaMin;
        }


        runBattleAtCurrentPosition(
            &B,

            E,

            N,

            bMinAngle,

            89.9,

            cumulativeMode,

            output,

            step + 1
        );
    }


    fprintf(
        output,

        "\nFinal B status = %s\n",

        B.alive
        ?
        "ALIVE"
        :
        "DESTROYED"
    );


    if(cumulativeMode)
    {
        fprintf(
            output,

            "Final cumulative impact "
            "= %.2f\n",

            B.cumulativeImpact
        );
    }


    fclose(output);


    saveBattlefield(
        finalFilename,

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


    double pathX[MAX_POINTS];

    double pathY[MAX_POINTS];


    int N;

    int k;

    int t;


    double D;

    double thetaMin;


    unsigned int seed;


    printf(
        "========== PART 2-A ==========\n"
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
        N < 1
        ||
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
        k < 2
        ||
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
       Battleship max velocity
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
        initialB.maxVelocity <= 0.0
    );


    /* =================================
       PART 2-A IMPORTANT

       T_B = time between
       consecutive B firings
       ================================= */

    do
    {
        printf(
            "Enter Battleship firing "
            "interval T_B in seconds: "
        );


        scanf(
            "%lf",

            &initialB.firingInterval
        );

    }
    while(
        initialB.firingInterval
        <= 0.0
    );


    /* =================================
       Initial B X position
       ================================= */

    do
    {
        printf(
            "Enter initial Battleship "
            "X position: "
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
       Initial B Y position
       ================================= */

    do
    {
        printf(
            "Enter initial Battleship "
            "Y position: "
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


    initialB.alive =
        1;


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
           Random E type A-E
        */

        type =
            'A'
            +
            rand() % 5;


        setEscortProperties(
            &initialE[i],

            type,

            initialB.maxVelocity
        );
    }


    /* =================================
       Generate path
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
       Jam settings for 1-B Sim 2
       ================================= */

    do
    {
        printf(
            "Enter t for gun jam "
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
       Save initial conditions
       ================================= */

    saveBattlefield(
        "part2A_initial.txt",

        initialB,

        initialE,

        N,

        D
    );


    /* =================================
       REDO PART 1-A
       ================================= */

    printf(
        "\n--- 2-A: Redo Part 1-A ---\n"
    );


    runStationarySimulation(
        "part2A_part1A_results.txt",

        "part2A_part1A_final.txt",

        initialB,

        initialE,

        N,

        D,

        0
    );


    /* =================================
       REDO PART 1-B
       SIMULATION 1
       ================================= */

    printf(
        "--- 2-A: Redo Part 1-B "
        "Simulation 1 ---\n"
    );


    runPathSimulation(
        "part2A_part1B_sim1_results.txt",

        "part2A_part1B_sim1_final.txt",

        initialB,

        initialE,

        N,

        pathX,

        pathY,

        k,

        0,

        t,

        thetaMin,

        D,

        0
    );


    /* =================================
       REDO PART 1-B
       SIMULATION 2
       ================================= */

    printf(
        "--- 2-A: Redo Part 1-B "
        "Simulation 2 ---\n"
    );


    runPathSimulation(
        "part2A_part1B_sim2_results.txt",

        "part2A_part1B_sim2_final.txt",

        initialB,

        initialE,

        N,

        pathX,

        pathY,

        k,

        1,

        t,

        thetaMin,

        D,

        0
    );


    /* =================================
       REDO PART 1-C
       STATIONARY VERSION
       ================================= */

    printf(
        "--- 2-A: Redo Part 1-C / "
        "1-A ---\n"
    );


    runStationarySimulation(
        "part2A_part1C_A_results.txt",

        "part2A_part1C_A_final.txt",

        initialB,

        initialE,

        N,

        D,

        1
    );


    /* =================================
       REDO PART 1-C
       PART 1-B SIMULATION 1
       ================================= */

    printf(
        "--- 2-A: Redo Part 1-C / "
        "1-B Simulation 1 ---\n"
    );


    runPathSimulation(
        "part2A_part1C_B1_results.txt",

        "part2A_part1C_B1_final.txt",

        initialB,

        initialE,

        N,

        pathX,

        pathY,

        k,

        0,

        t,

        thetaMin,

        D,

        1
    );


    /* =================================
       REDO PART 1-C
       PART 1-B SIMULATION 2
       ================================= */

    printf(
        "--- 2-A: Redo Part 1-C / "
        "1-B Simulation 2 ---\n"
    );


    runPathSimulation(
        "part2A_part1C_B2_results.txt",

        "part2A_part1C_B2_final.txt",

        initialB,

        initialE,

        N,

        pathX,

        pathY,

        k,

        1,

        t,

        thetaMin,

        D,

        1
    );


    printf(
        "\nPart 2-A completed.\n"
    );


    return 0;
}
