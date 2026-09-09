#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_SHIPS 100
#define MAX_POINTS 100
#define GRAVITY 9.81
#define PI 3.14159265358979323846

typedef struct
{
    char type;
    double x;
    double y;
    double maxVelocity;
    int alive;

} Battleship;


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

} EscortShip;


/* ================================
   Generate random double value
   ================================ */

double randomDouble(double min, double max)
{
    return min +
           ((double)rand() / RAND_MAX)
           * (max - min);
}


/* ================================
   Distance between two points
   ================================ */

double distanceBetween(double x1,
                       double y1,
                       double x2,
                       double y2)
{
    double dx = x2 - x1;
    double dy = y2 - y1;

    return sqrt(dx * dx + dy * dy);
}


/* ================================
   Escort angle range
   ================================ */

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


/* ================================
   Escort impact power
   ================================ */

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


/* ================================
   Set Escort properties
   ================================ */

void setEscortProperties(EscortShip *ship,
                         char type,
                         double battleshipMaxVelocity)
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
       Angle range =
       max angle - min angle
    */

    ship->maxAngle =
        ship->minAngle
        + angleRange;


    /*
       Set maximum velocity
    */

    if(type == 'A')
    {
        /*
           EA maximum velocity
           = 1.2 * B maximum velocity
        */

        ship->maxVelocity =
            1.2
            * battleshipMaxVelocity;
    }

    else
    {
        ship->maxVelocity =
            randomDouble(
                0.55
                * battleshipMaxVelocity,

                0.95
                * battleshipMaxVelocity
            );
    }


    /*
       Set minimum velocity
    */

    ship->minVelocity =
        randomDouble(
            0.30
            * ship->maxVelocity,

            0.60
            * ship->maxVelocity
        );


    ship->alive =
        1;
}


/* ================================
   Check projectile hit
   ================================ */

int canHit(double distance,
           double minVelocity,
           double maxVelocity,
           double minAngle,
           double maxAngle,
           double *hitTime,
           double *selectedAngle,
           double *selectedVelocity)
{
    double angle;


    for(angle = minAngle;
        angle <= maxAngle;
        angle += 0.1)
    {
        double radians;

        double sinValue;

        double requiredVelocity;

        double time;


        radians =
            angle
            * PI
            / 180.0;


        sinValue =
            sin(
                2.0
                * radians
            );


        if(sinValue <= 0.0)
        {
            continue;
        }


        /*
           Projectile equation

           R = u^2 sin(2 theta) / g

           Rearranged:

           u = sqrt(Rg / sin(2 theta))
        */

        requiredVelocity =
            sqrt(
                (distance * GRAVITY)
                / sinValue
            );


        /*
           Check whether required velocity
           is inside gun velocity range
        */

        if(requiredVelocity >= minVelocity &&
           requiredVelocity <= maxVelocity)
        {
            /*
               Flight time

               t = 2u sin(theta) / g
            */

            time =
                (
                    2.0
                    * requiredVelocity
                    * sin(radians)
                )
                / GRAVITY;


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


/* ================================
   Battleship attacks Escort
   ================================ */

int battleshipCanHit(Battleship B,
                     EscortShip ship,
                     double minAngle,
                     double maxAngle,
                     double *time,
                     double *angle,
                     double *velocity)
{
    double distance;


    distance =
        distanceBetween(
            B.x,
            B.y,
            ship.x,
            ship.y
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


/* ================================
   Escort attacks Battleship
   ================================ */

int escortCanHit(Battleship B,
                 EscortShip ship,
                 double *time,
                 double *angle,
                 double *velocity)
{
    double distance;


    distance =
        distanceBetween(
            ship.x,
            ship.y,
            B.x,
            B.y
        );


    return canHit(
        distance,

        ship.minVelocity,

        ship.maxVelocity,

        ship.minAngle,

        ship.maxAngle,

        time,

        angle,

        velocity
    );
}


/* ================================
   Copy Escort array
   ================================ */

void copyEscorts(EscortShip destination[],
                 EscortShip source[],
                 int N)
{
    for(int i = 0;
        i < N;
        i++)
    {
        destination[i] =
            source[i];
    }
}


/* ================================
   Save battlefield
   ================================ */

void saveBattlefield(char filename[],
                     Battleship B,
                     EscortShip E[],
                     int N,
                     double D)
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
        "CANVAS SIZE = %.2f x %.2f\n\n",
        D,
        D
    );


    fprintf(
        file,
        "BATTLESHIP\n"
    );


    fprintf(
        file,
        "Type = %c\n",
        B.type
    );


    fprintf(
        file,
        "Position = (%.2f, %.2f)\n",
        B.x,
        B.y
    );


    fprintf(
        file,
        "Maximum Velocity = %.2f\n",
        B.maxVelocity
    );


    fprintf(
        file,
        "Alive = %d\n\n",
        B.alive
    );


    fprintf(
        file,
        "ESCORT SHIPS\n\n"
    );


    for(int i = 0;
        i < N;
        i++)
    {
        fprintf(
            file,
            "Escort ID = %d\n",
            E[i].id
        );


        fprintf(
            file,
            "Type = E%c\n",
            E[i].type
        );


        fprintf(
            file,
            "Position = (%.2f, %.2f)\n",
            E[i].x,
            E[i].y
        );


        fprintf(
            file,
            "Minimum Velocity = %.2f\n",
            E[i].minVelocity
        );


        fprintf(
            file,
            "Maximum Velocity = %.2f\n",
            E[i].maxVelocity
        );


        fprintf(
            file,
            "Minimum Angle = %.2f\n",
            E[i].minAngle
        );


        fprintf(
            file,
            "Maximum Angle = %.2f\n",
            E[i].maxAngle
        );


        fprintf(
            file,
            "Impact Power = %.2f\n",
            E[i].impactPower
        );


        fprintf(
            file,
            "Alive = %d\n\n",
            E[i].alive
        );
    }


    fclose(file);
}


/* ================================
   Save path
   ================================ */

void savePath(char filename[],
              double pathX[],
              double pathY[],
              int k)
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
            "Error opening path file.\n"
        );

        return;
    }


    for(int i = 0;
        i < k;
        i++)
    {
        fprintf(
            file,
            "Point %d = (%.2f, %.2f)\n",

            i + 1,

            pathX[i],

            pathY[i]
        );
    }


    fclose(file);
}


/* =======================================
        PART 1-B SIMULATION 1
   ======================================= */

void runSimulation1(Battleship *B,
                    EscortShip E[],
                    int N,
                    double pathX[],
                    double pathY[],
                    int k)
{
    FILE *file;


    file =
        fopen(
            "part1B_sim1_results.txt",
            "w"
        );


    if(file == NULL)
    {
        printf(
            "Error opening Simulation 1 file.\n"
        );

        return;
    }


    fprintf(
        file,
        "PART 1-B - SIMULATION 1\n\n"
    );


    /*
       Repeat simulation for
       all k points
    */

    for(int step = 0;
        step < k &&
        B->alive;
        step++)
    {
        int sinkingEscort =
            -1;


        double earliestHitTime =
            999999999.0;


        /*
           Move Battleship to
           current path point
        */

        B->x =
            pathX[step];


        B->y =
            pathY[step];


        fprintf(
            file,
            "=============================\n"
        );


        fprintf(
            file,
            "ITERATION %d\n",
            step + 1
        );


        fprintf(
            file,
            "Battleship position = "
            "(%.2f, %.2f)\n",

            B->x,

            B->y
        );


        /*
           Check whether an alive Escort
           can destroy B.
        */

        for(int i = 0;
            i < N;
            i++)
        {
            double time;

            double angle;

            double velocity;


            /*
               Destroyed Escorts are not
               used again.
            */

            if(E[i].alive == 0)
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
                   Find earliest Escort
                   shell impact.
                */

                if(time <
                   earliestHitTime)
                {
                    earliestHitTime =
                        time;


                    sinkingEscort =
                        i;
                }
            }
        }


        /*
           B is destroyed
        */

        if(sinkingEscort != -1)
        {
            B->alive =
                0;


            fprintf(
                file,
                "Battleship destroyed.\n"
            );


            fprintf(
                file,
                "Escort E%d sank B.\n",

                E[sinkingEscort].id
            );


            fprintf(
                file,
                "Escort type = E%c\n",

                E[sinkingEscort].type
            );


            fprintf(
                file,
                "Time to impact = %.2f seconds\n",

                earliestHitTime
            );


            printf(
                "\nSimulation 1 stopped "
                "at iteration %d.\n",

                step + 1
            );


            printf(
                "Battleship destroyed "
                "by Escort E%d.\n",

                E[sinkingEscort].id
            );


            break;
        }


        /*
           If B survives,
           B attacks all reachable
           alive Escorts.
        */

        else
        {
            int hitCount =
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


                /*
                   Normal B gun range:
                   approximately
                   0 to 90 degrees.
                */

                if(
                    battleshipCanHit(
                        *B,

                        E[i],

                        0.1,

                        89.9,

                        &time,

                        &angle,

                        &velocity
                    )
                )
                {
                    /*
                       One B hit destroys
                       one Escort.
                    */

                    E[i].alive =
                        0;


                    hitCount++;


                    fprintf(
                        file,
                        "B destroyed Escort E%d\n",

                        E[i].id
                    );


                    fprintf(
                        file,
                        "Type = E%c\n",

                        E[i].type
                    );


                    fprintf(
                        file,
                        "Time to hit = %.2f seconds\n",

                        time
                    );


                    fprintf(
                        file,
                        "Firing angle = %.2f degrees\n",

                        angle
                    );


                    fprintf(
                        file,
                        "Shell velocity = %.2f\n\n",

                        velocity
                    );
                }
            }


            fprintf(
                file,
                "Escort ships destroyed "
                "in iteration = %d\n\n",

                hitCount
            );
        }
    }


    if(B->alive)
    {
        fprintf(
            file,
            "Simulation 1 completed "
            "all %d iterations.\n",

            k
        );


        printf(
            "Simulation 1 completed "
            "all %d iterations.\n",

            k
        );
    }


    fclose(file);
}


/* =======================================
        PART 1-B SIMULATION 2
   ======================================= */

void runSimulation2(Battleship *B,
                    EscortShip E[],
                    int N,
                    double pathX[],
                    double pathY[],
                    int k,
                    int jamAfter,
                    double thetaMin)
{
    FILE *file;


    file =
        fopen(
            "part1B_sim2_results.txt",
            "w"
        );


    if(file == NULL)
    {
        printf(
            "Error opening Simulation 2 file.\n"
        );

        return;
    }


    fprintf(
        file,
        "PART 1-B - SIMULATION 2\n\n"
    );


    fprintf(
        file,
        "Gun jams after %d iterations.\n",

        jamAfter
    );


    fprintf(
        file,
        "After jamming angle range = "
        "%.2f to 90 degrees.\n\n",

        thetaMin
    );


    for(int step = 0;
        step < k &&
        B->alive;
        step++)
    {
        int sinkingEscort =
            -1;


        double earliestHitTime =
            999999999.0;


        double bMinAngle;


        /*
           Move B to current path point.
        */

        B->x =
            pathX[step];


        B->y =
            pathY[step];


        /*
           First t iterations =
           normal gun.

           After t iterations =
           jammed gun.
        */

        if((step + 1) >
           jamAfter)
        {
            bMinAngle =
                thetaMin;
        }

        else
        {
            bMinAngle =
                0.1;
        }


        fprintf(
            file,
            "=============================\n"
        );


        fprintf(
            file,
            "ITERATION %d\n",

            step + 1
        );


        fprintf(
            file,
            "Battleship position = "
            "(%.2f, %.2f)\n",

            B->x,

            B->y
        );


        if((step + 1) >
           jamAfter)
        {
            fprintf(
                file,
                "Gun status = JAMMED\n"
            );


            fprintf(
                file,
                "B angle range = "
                "%.2f - 89.9\n",

                bMinAngle
            );
        }

        else
        {
            fprintf(
                file,
                "Gun status = NORMAL\n"
            );
        }


        /*
           Check Escort attacks
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
                escortCanHit(
                    *B,

                    E[i],

                    &time,

                    &angle,

                    &velocity
                )
            )
            {
                if(time <
                   earliestHitTime)
                {
                    earliestHitTime =
                        time;


                    sinkingEscort =
                        i;
                }
            }
        }


        /*
           Battleship sinks
        */

        if(sinkingEscort != -1)
        {
            B->alive =
                0;


            fprintf(
                file,
                "Battleship destroyed.\n"
            );


            fprintf(
                file,
                "Escort E%d sank B.\n",

                E[sinkingEscort].id
            );


            fprintf(
                file,
                "Escort type = E%c\n",

                E[sinkingEscort].type
            );


            fprintf(
                file,
                "Time to impact = %.2f seconds\n",

                earliestHitTime
            );


            printf(
                "\nSimulation 2 stopped "
                "at iteration %d.\n",

                step + 1
            );


            printf(
                "Battleship destroyed "
                "by Escort E%d.\n",

                E[sinkingEscort].id
            );


            break;
        }


        /*
           Battleship survives
           and attacks Escorts.
        */

        else
        {
            int hitCount =
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


                /*
                   If gun is jammed,
                   bMinAngle = thetaMin.

                   Otherwise,
                   bMinAngle = 0.1.
                */

                if(
                    battleshipCanHit(
                        *B,

                        E[i],

                        bMinAngle,

                        89.9,

                        &time,

                        &angle,

                        &velocity
                    )
                )
                {
                    E[i].alive =
                        0;


                    hitCount++;


                    fprintf(
                        file,
                        "B destroyed Escort E%d\n",

                        E[i].id
                    );


                    fprintf(
                        file,
                        "Type = E%c\n",

                        E[i].type
                    );


                    fprintf(
                        file,
                        "Time to hit = %.2f seconds\n",

                        time
                    );


                    fprintf(
                        file,
                        "Firing angle = %.2f degrees\n",

                        angle
                    );


                    fprintf(
                        file,
                        "Shell velocity = %.2f\n\n",

                        velocity
                    );
                }
            }


            fprintf(
                file,
                "Escort ships destroyed "
                "in iteration = %d\n\n",

                hitCount
            );
        }
    }


    if(B->alive)
    {
        fprintf(
            file,
            "Simulation 2 completed "
            "all %d iterations.\n",

            k
        );


        printf(
            "Simulation 2 completed "
            "all %d iterations.\n",

            k
        );
    }


    fclose(file);
}


/* =========================================
                 MAIN FUNCTION
   ========================================= */

int main()
{
    Battleship initialB;

    Battleship sim1B;

    Battleship sim2B;


    EscortShip initialE[MAX_SHIPS];

    EscortShip sim1E[MAX_SHIPS];

    EscortShip sim2E[MAX_SHIPS];


    double pathX[MAX_POINTS];

    double pathY[MAX_POINTS];


    int N;

    int k;

    int t;

    double thetaMin;

    double D;

    unsigned int seed;


    printf(
        "========== PART 1-B ==========\n"
    );


    /* ================================
       Canvas size
       ================================ */

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



    /* ================================
       Number of Escort ships
       ================================ */

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
    while(N < 1 ||
          N > MAX_SHIPS);



    /* ================================
       Number of path points
       ================================ */

    do
    {
        printf(
            "Enter number of path points k: "
        );


        scanf(
            "%d",
            &k
        );

    }
    while(k < 2 ||
          k > MAX_POINTS);



    /* ================================
       Battleship type
       ================================ */

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
        initialB.type != 'U' &&
        initialB.type != 'M' &&
        initialB.type != 'R' &&
        initialB.type != 'S'
    );



    /* ================================
       Battleship maximum velocity
       ================================ */

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
    while(initialB.maxVelocity <= 0.0);



    /* ================================
       Initial B X position
       ================================ */

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
        initialB.x < 0.0 ||
        initialB.x > D
    );



    /* ================================
       Initial B Y position
       ================================ */

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
        initialB.y < 0.0 ||
        initialB.y > D
    );


    initialB.alive =
        1;



    /* ================================
       Random seed
       ================================ */

    printf(
        "Enter random seed: "
    );


    scanf(
        "%u",
        &seed
    );


    srand(seed);



    /* ================================
       Generate Escort ships
       ================================ */

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
            'A'
            + rand() % 5;


        setEscortProperties(
            &initialE[i],

            type,

            initialB.maxVelocity
        );
    }



    /* =================================
       Generate k path points
       ================================= */


    /*
       First point = initial B position
    */

    pathX[0] =
        initialB.x;


    pathY[0] =
        initialB.y;



    /*
       Generate remaining path points
    */

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



    /*
       Save initial battlefield
    */

    saveBattlefield(
        "part1B_initial.txt",

        initialB,

        initialE,

        N,

        D
    );


    /*
       Save generated path
    */

    savePath(
        "part1B_path.txt",

        pathX,

        pathY,

        k
    );



    /* =================================
           SIMULATION 1
       ================================= */


    /*
       Copy initial B
    */

    sim1B =
        initialB;


    /*
       Copy all initial Escort data
    */

    copyEscorts(
        sim1E,

        initialE,

        N
    );


    printf(
        "\n--- Starting Simulation 1 ---\n"
    );


    runSimulation1(
        &sim1B,

        sim1E,

        N,

        pathX,

        pathY,

        k
    );


    /*
       Save Simulation 1 final state
    */

    saveBattlefield(
        "part1B_sim1_final.txt",

        sim1B,

        sim1E,

        N,

        D
    );



    /* =================================
       SIMULATION 2 SETTINGS
       ================================= */


    /*
       t must be less than k
    */

    do
    {
        printf(
            "\nEnter t "
            "(gun jams after t iterations, "
            "0 < t < k): "
        );


        scanf(
            "%d",
            &t
        );

    }
    while(
        t <= 0 ||
        t >= k
    );



    /*
       PDF requirement:

       0 < theta_min < 30
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
        thetaMin <= 0.0 ||
        thetaMin >= 30.0
    );



    /* =================================
           SIMULATION 2
       ================================= */


    /*
       IMPORTANT:

       Simulation 2 must use
       SAME INITIAL CONDITIONS
       as Simulation 1.
    */

    sim2B =
        initialB;


    copyEscorts(
        sim2E,

        initialE,

        N
    );


    printf(
        "\n--- Starting Simulation 2 ---\n"
    );


    runSimulation2(
        &sim2B,

        sim2E,

        N,

        pathX,

        pathY,

        k,

        t,

        thetaMin
    );


    /*
       Save Simulation 2 final state
    */

    saveBattlefield(
        "part1B_sim2_final.txt",

        sim2B,

        sim2E,

        N,

        D
    );


    printf(
        "\nPart 1-B completed.\n"
    );


    return 0;
}
