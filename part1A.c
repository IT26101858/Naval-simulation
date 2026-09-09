#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_SHIPS 100
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


/* --------------------------------
   Random double number
   -------------------------------- */

double randomDouble(double min, double max)
{
    return min +
           ((double)rand() / RAND_MAX)
           * (max - min);
}


/* --------------------------------
   Distance between two ships
   -------------------------------- */

double distanceBetween(double x1,
                       double y1,
                       double x2,
                       double y2)
{
    double dx = x2 - x1;
    double dy = y2 - y1;

    return sqrt(dx * dx + dy * dy);
}


/* --------------------------------
   Escort angle range
   -------------------------------- */

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


/* --------------------------------
   Escort impact power
   -------------------------------- */

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


/* --------------------------------
   Set Escort ship properties
   -------------------------------- */

void setEscortProperties(EscortShip *ship,
                         char type,
                         double battleshipMaxVelocity)
{
    double angleRange;

    ship->type = type;

    ship->impactPower =
        getImpactPower(type);

    angleRange =
        getAngleRange(type);


    /*
       Random minimum angle
    */

    ship->minAngle =
        randomDouble(1.0,
                     89.0 - angleRange);


    ship->maxAngle =
        ship->minAngle + angleRange;


    /*
       Set velocity range
    */

    if(type == 'A')
    {
        ship->maxVelocity =
            1.2 * battleshipMaxVelocity;
    }
    else
    {
        ship->maxVelocity =
            randomDouble(
                0.55 * battleshipMaxVelocity,
                0.95 * battleshipMaxVelocity
            );
    }


    ship->minVelocity =
        randomDouble(
            0.30 * ship->maxVelocity,
            0.60 * ship->maxVelocity
        );


    ship->alive = 1;
}


/* --------------------------------
   Check if a projectile can reach
   the given distance
   -------------------------------- */

int canHit(double distance,
           double minVelocity,
           double maxVelocity,
           double minAngle,
           double maxAngle,
           double *hitTime,
           double *selectedAngle,
           double *selectedVelocity)
{
    for(double angle = minAngle;
        angle <= maxAngle;
        angle += 0.1)
    {
        double radians;

        double sinValue;

        double requiredVelocity;

        double time;


        radians =
            angle * PI / 180.0;


        sinValue =
            sin(2.0 * radians);


        if(sinValue <= 0.0)
        {
            continue;
        }


        /*
           R = u^2 sin(2 theta) / g

           Therefore,

           u = sqrt(Rg / sin(2 theta))
        */

        requiredVelocity =
            sqrt(
                (distance * GRAVITY)
                / sinValue
            );


        if(requiredVelocity >= minVelocity &&
           requiredVelocity <= maxVelocity)
        {
            /*
               Projectile flight time

               t = 2u sin(theta) / g
            */

            time =
                (2.0 *
                 requiredVelocity *
                 sin(radians))
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


/* --------------------------------
   Check whether Battleship can hit
   an Escort
   -------------------------------- */

int battleshipCanHit(Battleship B,
                     EscortShip ship,
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


    /*
       Battleship angle range = 0 - 90

       We use 0.1 - 89.9 to avoid
       mathematical problems at
       exactly 0 and 90 degrees.
    */

    return canHit(
        distance,
        0.0,
        B.maxVelocity,
        0.1,
        89.9,
        time,
        angle,
        velocity
    );
}


/* --------------------------------
   Check whether Escort can hit B
   -------------------------------- */

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


/* --------------------------------
   Save battlefield conditions
   -------------------------------- */

void saveBattlefield(char filename[],
                     Battleship B,
                     EscortShip E[],
                     int N,
                     double D)
{
    FILE *file;


    file =
        fopen(filename, "w");


    if(file == NULL)
    {
        printf("Error opening file.\n");

        return;
    }


    fprintf(file,
            "CANVAS SIZE = %.2f x %.2f\n\n",
            D,
            D);


    fprintf(file,
            "BATTLESHIP\n");


    fprintf(file,
            "Type = %c\n",
            B.type);


    fprintf(file,
            "Position = (%.2f, %.2f)\n",
            B.x,
            B.y);


    fprintf(file,
            "Maximum Velocity = %.2f\n",
            B.maxVelocity);


    fprintf(file,
            "Alive = %d\n\n",
            B.alive);



    fprintf(file,
            "ESCORT SHIPS\n\n");


    for(int i = 0; i < N; i++)
    {
        fprintf(file,
                "Escort ID = %d\n",
                E[i].id);


        fprintf(file,
                "Type = E%c\n",
                E[i].type);


        fprintf(file,
                "Position = (%.2f, %.2f)\n",
                E[i].x,
                E[i].y);


        fprintf(file,
                "Minimum Velocity = %.2f\n",
                E[i].minVelocity);


        fprintf(file,
                "Maximum Velocity = %.2f\n",
                E[i].maxVelocity);


        fprintf(file,
                "Minimum Angle = %.2f\n",
                E[i].minAngle);


        fprintf(file,
                "Maximum Angle = %.2f\n",
                E[i].maxAngle);


        fprintf(file,
                "Impact Power = %.2f\n",
                E[i].impactPower);


        fprintf(file,
                "Alive = %d\n\n",
                E[i].alive);
    }


    fclose(file);
}


/* =========================================
                 MAIN FUNCTION
   ========================================= */

int main()
{
    Battleship B;

    EscortShip E[MAX_SHIPS];

    int N;

    double D;

    unsigned int seed;


    printf(
        "========== PART 1-A ==========\n"
    );


    /* --------------------------------
       Canvas size
       -------------------------------- */

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
    while(D <= 0);



    /* --------------------------------
       Number of Escort ships
       -------------------------------- */

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



    /* --------------------------------
       Battleship type
       -------------------------------- */

    do
    {
        printf(
            "Enter Battleship type "
            "(U/M/R/S): "
        );

        scanf(
            " %c",
            &B.type
        );

    }
    while(
        B.type != 'U' &&
        B.type != 'M' &&
        B.type != 'R' &&
        B.type != 'S'
    );



    /* --------------------------------
       Battleship maximum velocity
       -------------------------------- */

    do
    {
        printf(
            "Enter Battleship maximum "
            "shell velocity: "
        );

        scanf(
            "%lf",
            &B.maxVelocity
        );

    }
    while(B.maxVelocity <= 0);



    /* --------------------------------
       Battleship X position
       -------------------------------- */

    do
    {
        printf(
            "Enter Battleship X "
            "position: "
        );

        scanf(
            "%lf",
            &B.x
        );

    }
    while(B.x < 0 ||
          B.x > D);



    /* --------------------------------
       Battleship Y position
       -------------------------------- */

    do
    {
        printf(
            "Enter Battleship Y "
            "position: "
        );

        scanf(
            "%lf",
            &B.y
        );

    }
    while(B.y < 0 ||
          B.y > D);



    /*
       Battleship is alive
       at the beginning.
    */

    B.alive = 1;



    /* --------------------------------
       Random seed
       -------------------------------- */

    printf(
        "Enter random seed: "
    );

    scanf(
        "%u",
        &seed
    );


    srand(seed);



    /* --------------------------------
       Generate Escort ships
       -------------------------------- */

    for(int i = 0;
        i < N;
        i++)
    {
        /*
           Unique identifier
        */

        E[i].id =
            i + 1;


        /*
           Random position
        */

        E[i].x =
            randomDouble(
                0.0,
                D
            );


        E[i].y =
            randomDouble(
                0.0,
                D
            );


        /*
           Random type

           0 -> A
           1 -> B
           2 -> C
           3 -> D
           4 -> E
        */

        char type =
            'A' + rand() % 5;


        /*
           Set all properties
        */

        setEscortProperties(
            &E[i],
            type,
            B.maxVelocity
        );
    }



    /*
       Save initial battlefield
       conditions.
    */

    saveBattlefield(
        "part1A_initial.txt",
        B,
        E,
        N,
        D
    );



    /* =================================
       CHECK WHETHER B WILL SINK
       ================================= */

    int sinkingEscort =
        -1;


    double earliestHitTime =
        999999999.0;



    for(int i = 0;
        i < N;
        i++)
    {
        double time;

        double angle;

        double velocity;


        if(
            escortCanHit(
                B,
                E[i],
                &time,
                &angle,
                &velocity
            )
        )
        {
            /*
               Find the Escort whose shell
               reaches B first.
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
       Open result file.
    */

    FILE *resultFile;


    resultFile =
        fopen(
            "part1A_results.txt",
            "w"
        );


    if(resultFile == NULL)
    {
        printf(
            "Error opening result file.\n"
        );

        return 1;
    }



    /* =================================
       CASE 1 - B SINKS
       ================================= */

    if(sinkingEscort != -1)
    {
        B.alive =
            0;


        printf(
            "\nBattleship was destroyed.\n"
        );


        printf(
            "Escort E%d sank "
            "the Battleship.\n",
            E[sinkingEscort].id
        );


        printf(
            "Time to impact = %.2f seconds\n",
            earliestHitTime
        );



        fprintf(
            resultFile,
            "Battleship was destroyed.\n"
        );


        fprintf(
            resultFile,
            "Escort E%d sank "
            "the Battleship.\n",
            E[sinkingEscort].id
        );


        fprintf(
            resultFile,
            "Escort type = E%c\n",
            E[sinkingEscort].type
        );


        fprintf(
            resultFile,
            "Time to impact = %.2f seconds\n",
            earliestHitTime
        );
    }


    /* =================================
       CASE 2 - B SURVIVES
       ================================= */

    else
    {
        int hitCount =
            0;


        double battleEndTime =
            0.0;



        fprintf(
            resultFile,
            "Battleship survived.\n\n"
        );



        for(int i = 0;
            i < N;
            i++)
        {
            double time;

            double angle;

            double velocity;


            if(
                battleshipCanHit(
                    B,
                    E[i],
                    &time,
                    &angle,
                    &velocity
                )
            )
            {
                /*
                   One successful B shell
                   destroys an Escort in
                   Part 1-A.
                */

                E[i].alive =
                    0;


                hitCount++;



                if(time >
                   battleEndTime)
                {
                    battleEndTime =
                        time;
                }



                fprintf(
                    resultFile,
                    "Escort E%d was hit.\n",
                    E[i].id
                );


                fprintf(
                    resultFile,
                    "Type = E%c\n",
                    E[i].type
                );


                fprintf(
                    resultFile,
                    "Time to hit = %.2f seconds\n",
                    time
                );


                fprintf(
                    resultFile,
                    "Firing angle = %.2f degrees\n",
                    angle
                );


                fprintf(
                    resultFile,
                    "Shell velocity = %.2f\n\n",
                    velocity
                );
            }
        }



        printf(
            "\nBattleship survived.\n"
        );


        printf(
            "Escort ships hit by B = %d\n",
            hitCount
        );


        printf(
            "Battle end time = %.2f seconds\n",
            battleEndTime
        );



        fprintf(
            resultFile,
            "\nNumber of Escort ships hit = %d\n",
            hitCount
        );


        fprintf(
            resultFile,
            "Battle end time = %.2f seconds\n",
            battleEndTime
        );
    }



    fclose(
        resultFile
    );



    /*
       Save final battlefield
       conditions.
    */

    saveBattlefield(
        "part1A_final.txt",
        B,
        E,
        N,
        D
    );



    printf(
        "\nPart 1-A simulation completed.\n"
    );


    return 0;
}
