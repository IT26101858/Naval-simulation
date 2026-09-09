#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_SHIPS 100
#define MAX_POINTS 100
#define GRAVITY 9.81
#define PI 3.14159265358979323846
#define MAX_SIM_TIME 1000.0


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
       Part 2-A / 2-B

       Time between two consecutive
       Battleship firings
    */
    double firingInterval;

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

    /*
       PART 2-B

       Time between two consecutive
       firings of this Escort ship.
    */
    double firingInterval;

    /*
       Next time this Escort
       is allowed to fire.
    */
    double nextFireTime;

    int alive;

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
          ANGLE RANGE
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
          IMPACT POWER
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
      ESCORT FIRING INTERVAL
   ===================================== */

double getEscortFiringInterval(
    char type,
    double TE_A,
    double TE_B,
    double TE_C,
    double TE_D,
    double TE_E
)
{
    switch(type)
    {
        case 'A':
            return TE_A;

        case 'B':
            return TE_B;

        case 'C':
            return TE_C;

        case 'D':
            return TE_D;

        case 'E':
            return TE_E;

        default:
            return 1.0;
    }
}


/* =====================================
       SET ESCORT PROPERTIES
   ===================================== */

void setEscortProperties(
    EscortShip *ship,
    char type,
    double battleshipMaxVelocity,
    double TE_A,
    double TE_B,
    double TE_C,
    double TE_D,
    double TE_E
)
{
    double angleRange;

    ship->type =
        type;


    ship->impactPower =
        getImpactPower(type);


    angleRange =
        getAngleRange(type);


    ship->minAngle =
        randomDouble(
            1.0,
            89.0 - angleRange
        );


    ship->maxAngle =
        ship->minAngle +
        angleRange;


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
           Random maximum velocity.

           This range is our
           implementation choice.
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


    /*
       PART 2-B

       Give the correct TE according
       to Escort type.
    */

    ship->firingInterval =
        getEscortFiringInterval(
            type,
            TE_A,
            TE_B,
            TE_C,
            TE_D,
            TE_E
        );


    /*
       Every Escort can fire
       immediately at time 0.
    */

    ship->nextFireTime =
        0.0;


    ship->alive =
        1;
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

           Therefore:

           u = sqrt(Rg / sin(2theta))
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


        if(
            requiredVelocity >=
                minVelocity
            &&
            requiredVelocity <=
                maxVelocity
        )
        {
            /*
               Projectile flight time

               t = 2u sin(theta) / g
            */

            *hitTime =
                (
                    2.0 *
                    requiredVelocity *
                    sin(radians)
                )
                /
                GRAVITY;


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
        B CAN HIT ESCORT?
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
        ESCORT CAN HIT B?
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
           THREAT SCORE
   ===================================== */

/*
   Strategy:

   E that can attack B
   gets higher priority.

   Higher impact power
   also gets higher priority.
*/

double threatScore(
    Battleship B,
    EscortShip E
)
{
    double time;
    double angle;
    double velocity;

    double distance;


    if(
        escortCanHit(
            B,
            E,

            &time,
            &angle,
            &velocity
        )
    )
    {
        return
            10000.0

            +

            E.impactPower *
            1000.0

            -

            time;
    }


    distance =
        distanceBetween(
            B.x,
            B.y,

            E.x,
            E.y
        );


    return
        1000.0 -
        distance;
}


/* =====================================
        BUILD ATTACK ORDER
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
       Find Escorts B can hit
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
       Sort:

       Highest threat first
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
       PROCESS ESCORT FIRINGS
   ===================================== */

void processEscortFirings(
    Battleship *B,

    EscortShip E[],

    int N,

    double currentTime,

    int cumulativeMode,

    FILE *output
)
{
    /*
       Check every alive Escort
    */

    for(int i = 0;
        i < N &&
        B->alive;
        i++)
    {
        double flightTime;
        double angle;
        double velocity;


        if(E[i].alive == 0)
        {
            continue;
        }


        /*
           PART 2-B

           Escort can fire whenever
           currentTime reaches its
           nextFireTime.
        */

        if(
            currentTime + 0.000001
            <
            E[i].nextFireTime
        )
        {
            continue;
        }


        /*
           Check whether E can hit B
        */

        if(
            escortCanHit(
                *B,

                E[i],

                &flightTime,

                &angle,

                &velocity
            )
        )
        {
            fprintf(
                output,

                "Time %.2f: "
                "Escort E%d fired at B\n",

                currentTime,

                E[i].id
            );


            fprintf(
                output,

                "Type = E%c\n",

                E[i].type
            );


            fprintf(
                output,

                "TE = %.2f seconds\n",

                E[i].firingInterval
            );


            fprintf(
                output,

                "Impact power = %.2f\n",

                E[i].impactPower
            );


            /*
               In simplified event model,
               damage is applied for
               this successful shot.
            */

            if(cumulativeMode == 0)
            {
                /*
                   Part 1-A / 1-B rule:

                   One successful E hit
                   destroys B.
                */

                B->alive =
                    0;


                fprintf(
                    output,

                    "Battleship destroyed "
                    "by Escort E%d\n\n",

                    E[i].id
                );
            }

            else
            {
                /*
                   Part 1-C rule
                */

                B->cumulativeImpact +=
                    E[i].impactPower;


                fprintf(
                    output,

                    "Cumulative impact "
                    "= %.2f\n",

                    B->cumulativeImpact
                );


                if(
                    B->cumulativeImpact
                    >= 1.0
                )
                {
                    B->alive =
                        0;


                    fprintf(
                        output,

                        "Battleship "
                        "destroyed.\n\n"
                    );
                }
            }
        }


        /*
           IMPORTANT PART 2-B:

           Whether it hit or not,
           schedule next firing time.

           next shot =
           current shot + TE
        */

        E[i].nextFireTime +=
            E[i].firingInterval;
    }
}


/* =====================================
          RESET NEXT FIRE TIMES
   ===================================== */

void resetEscortFireTimes(
    EscortShip E[],
    int N
)
{
    for(int i = 0;
        i < N;
        i++)
    {
        E[i].nextFireTime =
            0.0;
    }
}


/* =====================================
       ONE PART 2-B BATTLE
   ===================================== */

void runBattle(
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
    double currentTime;

    double nextBFireTime;


    int attackOrder[MAX_SHIPS];

    int attackCount;

    int attackPosition;


    currentTime =
        0.0;


    nextBFireTime =
        0.0;


    attackPosition =
        0;


    resetEscortFireTimes(
        E,
        N
    );


    fprintf(
        output,

        "\n=============================\n"
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

        "B firing interval T_B "
        "= %.2f seconds\n",

        B->firingInterval
    );


    /* =================================
       BUILD INITIAL ATTACK ORDER
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

        "Attack Order: "
    );


    for(int i = 0;
        i < attackCount;
        i++)
    {
        fprintf(
            output,

            "E%d",

            E[attackOrder[i]].id
        );


        if(i < attackCount - 1)
        {
            fprintf(
                output,

                " -> "
            );
        }
    }


    fprintf(
        output,

        "\n\n"
    );


    /* =================================
           TIME SIMULATION LOOP
       ================================= */

    while(
        currentTime <= MAX_SIM_TIME
        &&
        B->alive
    )
    {
        /*
           --------------------------------
           ESCORT SHIPS FIRE CONTINUOUSLY
           --------------------------------
        */

        processEscortFirings(
            B,

            E,

            N,

            currentTime,

            cumulativeMode,

            output
        );


        if(B->alive == 0)
        {
            break;
        }


        /*
           --------------------------------
             B FIRES EVERY T_B SECONDS
           --------------------------------
        */

        if(
            currentTime + 0.000001
            >=
            nextBFireTime
        )
        {
            /*
               Find next alive target
               in attack order.
            */

            while(
                attackPosition <
                    attackCount

                &&

                E[
                    attackOrder[
                        attackPosition
                    ]
                ].alive == 0
            )
            {
                attackPosition++;
            }


            if(
                attackPosition <
                attackCount
            )
            {
                int index;

                double flightTime;
                double angle;
                double velocity;


                index =
                    attackOrder[
                        attackPosition
                    ];


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
                    fprintf(
                        output,

                        "Time %.2f: "
                        "B fired at Escort E%d\n",

                        currentTime,

                        E[index].id
                    );


                    fprintf(
                        output,

                        "Angle = %.2f degrees\n",

                        angle
                    );


                    fprintf(
                        output,

                        "Velocity = %.2f\n",

                        velocity
                    );


                    /*
                       B still destroys
                       E with one successful
                       attack in Part 2-B.
                    */

                    E[index].alive =
                        0;


                    fprintf(
                        output,

                        "Escort E%d "
                        "destroyed.\n\n",

                        E[index].id
                    );


                    attackPosition++;
                }
            }


            /*
               Schedule B's next shot
            */

            nextBFireTime +=
                B->firingInterval;
        }


        /*
           Check whether all reachable
           Escorts are destroyed.
        */

        int aliveReachable;

        aliveReachable =
            0;


        for(int i = 0;
            i < N;
            i++)
        {
            double time;
            double angle;
            double velocity;


            if(
                E[i].alive
                &&
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
                aliveReachable =
                    1;

                break;
            }
        }


        if(
            aliveReachable == 0
        )
        {
            fprintf(
                output,

                "No more reachable "
                "Escort ships.\n"
            );

            break;
        }


        /*
           Increase simulation time.

           0.1 s time step.
        */

        currentTime +=
            0.1;
    }


    fprintf(
        output,

        "\nBattle ended at "
        "%.2f seconds\n",

        currentTime
    );


    fprintf(
        output,

        "B status = %s\n",

        B->alive
        ?
        "ALIVE"
        :
        "DESTROYED"
    );


    if(cumulativeMode)
    {
        fprintf(
            output,

            "Cumulative impact "
            "= %.2f\n",

            B->cumulativeImpact
        );
    }
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
       STATIONARY SIMULATION
   ===================================== */

void runStationarySimulation(
    const char *filename,

    Battleship initialB,

    EscortShip initialE[],

    int N,

    int cumulativeMode
)
{
    Battleship B;

    EscortShip E[MAX_SHIPS];

    FILE *file;


    B =
        initialB;


    copyEscorts(
        E,

        initialE,

        N
    );


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


    runBattle(
        &B,

        E,

        N,

        0.1,

        89.9,

        cumulativeMode,

        file,

        1
    );


    fclose(file);
}


/* =====================================
          PATH SIMULATION
   ===================================== */

void runPathSimulation(
    const char *filename,

    Battleship initialB,

    EscortShip initialE[],

    int N,

    double pathX[],
    double pathY[],

    int k,

    int jamMode,

    int t,

    double thetaMin,

    int cumulativeMode
)
{
    Battleship B;

    EscortShip E[MAX_SHIPS];

    FILE *file;


    B =
        initialB;


    copyEscorts(
        E,

        initialE,

        N
    );


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
           Normal gun
        */

        bMinAngle =
            0.1;


        /*
           Part 1-B Simulation 2
           gun jam
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


        runBattle(
            &B,

            E,

            N,

            bMinAngle,

            89.9,

            cumulativeMode,

            file,

            step + 1
        );
    }


    fclose(file);
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


    double D;

    int N;

    int k;

    int t;

    double thetaMin;


    /*
       PART 2-B

       Escort firing intervals
    */

    double TE_A;
    double TE_B;
    double TE_C;
    double TE_D;
    double TE_E;


    unsigned int seed;


    printf(
        "========== PART 2-B ==========\n"
    );


    /* =================================
       Canvas
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
            "Enter number of Escort ships: "
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
       Path points
       ================================= */

    do
    {
        printf(
            "Enter number of path "
            "points k: "
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
        initialB.maxVelocity
        <= 0.0
    );


    /* =================================
       B FIRING INTERVAL T_B
       ================================= */

    do
    {
        printf(
            "Enter Battleship "
            "firing interval T_B: "
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
       ESCORT FIRING INTERVALS T_E
       ================================= */

    do
    {
        printf(
            "Enter TE for EA: "
        );

        scanf(
            "%lf",
            &TE_A
        );

    }
    while(TE_A <= 0.0);


    do
    {
        printf(
            "Enter TE for EB: "
        );

        scanf(
            "%lf",
            &TE_B
        );

    }
    while(TE_B <= 0.0);


    do
    {
        printf(
            "Enter TE for EC: "
        );

        scanf(
            "%lf",
            &TE_C
        );

    }
    while(TE_C <= 0.0);


    do
    {
        printf(
            "Enter TE for ED: "
        );

        scanf(
            "%lf",
            &TE_D
        );

    }
    while(TE_D <= 0.0);


    do
    {
        printf(
            "Enter TE for EE: "
        );

        scanf(
            "%lf",
            &TE_E
        );

    }
    while(TE_E <= 0.0);


    /* =================================
       Initial B position
       ================================= */

    do
    {
        printf(
            "Enter initial B X: "
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


    do
    {
        printf(
            "Enter initial B Y: "
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
       Generate Escorts
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


        type =
            'A' +
            rand() % 5;


        setEscortProperties(
            &initialE[i],

            type,

            initialB.maxVelocity,

            TE_A,
            TE_B,
            TE_C,
            TE_D,
            TE_E
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
       Gun jam settings
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
       REDO PART 1-A
       ================================= */

    printf(
        "\n2-B: Running Part 1-A...\n"
    );


    runStationarySimulation(
        "part2B_part1A.txt",

        initialB,

        initialE,

        N,

        0
    );


    /* =================================
       REDO PART 1-B SIMULATION 1
       ================================= */

    printf(
        "2-B: Running Part 1-B Sim 1...\n"
    );


    runPathSimulation(
        "part2B_part1B_sim1.txt",

        initialB,

        initialE,

        N,

        pathX,
        pathY,

        k,

        0,

        t,

        thetaMin,

        0
    );


    /* =================================
       REDO PART 1-B SIMULATION 2
       ================================= */

    printf(
        "2-B: Running Part 1-B Sim 2...\n"
    );


    runPathSimulation(
        "part2B_part1B_sim2.txt",

        initialB,

        initialE,

        N,

        pathX,
        pathY,

        k,

        1,

        t,

        thetaMin,

        0
    );


    /* =================================
       REDO PART 1-C
       ================================= */

    printf(
        "2-B: Running Part 1-C...\n"
    );


    runStationarySimulation(
        "part2B_part1C.txt",

        initialB,

        initialE,

        N,

        1
    );


    printf(
        "\nPart 2-B completed.\n"
    );


    return 0;
}
