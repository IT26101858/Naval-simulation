#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_SHIPS 100


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
       Initial impact power.

       Previous simulations:
       B impact power = 1.0
    */
    double initialImpactPower;


    /*
       Current degraded impact power
    */
    double currentImpactPower;


    /*
       Gamma degradation rate
    */
    double gamma;


    /*
       Number of gun firings
    */
    int shotsFired;


    /*
       Damage received by B
    */
    double damage;


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


    /*
       Original impact power
       from Table 1
    */
    double initialImpactPower;


    /*
       Current degraded impact power
    */
    double currentImpactPower;


    /*
       Gamma value
    */
    double gamma;


    /*
       Number of gun firings
    */
    int shotsFired;


    /*
       Damage received from B
    */
    double damage;


    int alive;

} EscortShip;


/* =====================================
        INITIAL IMPACT POWER OF E
   ===================================== */

double getEscortImpactPower(char type)
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
        CALCULATE IMPACT POWER

        IPn = IP0 * e^(-gamma*n)
   ===================================== */

double calculateImpactPower(
    double IP0,
    double gamma,
    int n
)
{
    return
        IP0 *
        exp(
            -gamma * n
        );
}


/* =====================================
        UPDATE B IMPACT POWER
   ===================================== */

void updateBattleshipImpactPower(
    Battleship *B
)
{
    B->currentImpactPower =
        calculateImpactPower(
            B->initialImpactPower,
            B->gamma,
            B->shotsFired
        );
}


/* =====================================
        UPDATE E IMPACT POWER
   ===================================== */

void updateEscortImpactPower(
    EscortShip *E
)
{
    E->currentImpactPower =
        calculateImpactPower(
            E->initialImpactPower,
            E->gamma,
            E->shotsFired
        );
}


/* =====================================
         B ATTACKS AN ESCORT
   ===================================== */

void battleshipAttack(
    Battleship *B,
    EscortShip *E,
    FILE *file
)
{
    /*
       Calculate impact power
       before this firing.
    */

    updateBattleshipImpactPower(B);


    printf(
        "\nB fires at Escort E%d\n",
        E->id
    );


    printf(
        "B current impact power = %.4f\n",
        B->currentImpactPower
    );


    fprintf(
        file,
        "\nB fires at Escort E%d\n",
        E->id
    );


    fprintf(
        file,
        "B shots fired before shot = %d\n",
        B->shotsFired
    );


    fprintf(
        file,
        "B current impact power = %.4f\n",
        B->currentImpactPower
    );


    /*
       Add damage to Escort.
    */

    E->damage +=
        B->currentImpactPower;


    /*
       B has now fired one more shot.
    */

    B->shotsFired++;


    fprintf(
        file,
        "Escort E%d cumulative damage = %.4f\n",
        E->id,
        E->damage
    );


    /*
       1.0 = 100% damage
    */

    if(E->damage >= 1.0)
    {
        E->alive =
            0;


        printf(
            "Escort E%d destroyed.\n",
            E->id
        );


        fprintf(
            file,
            "Escort E%d DESTROYED\n",
            E->id
        );
    }

    else
    {
        printf(
            "Escort E%d survived this hit.\n",
            E->id
        );


        fprintf(
            file,
            "Escort E%d still alive.\n",
            E->id
        );
    }


    /*
       Save impact power after
       number of firings changes.
    */

    updateBattleshipImpactPower(B);


    fprintf(
        file,
        "B impact power after firing = %.4f\n",
        B->currentImpactPower
    );
}


/* =====================================
          ESCORT ATTACKS B
   ===================================== */

void escortAttack(
    EscortShip *E,
    Battleship *B,
    FILE *file
)
{
    /*
       Calculate current IP
    */

    updateEscortImpactPower(E);


    printf(
        "\nEscort E%d attacks B\n",
        E->id
    );


    printf(
        "Escort current impact power = %.4f\n",
        E->currentImpactPower
    );


    fprintf(
        file,
        "\nEscort E%d attacks B\n",
        E->id
    );


    fprintf(
        file,
        "Escort shots fired before shot = %d\n",
        E->shotsFired
    );


    fprintf(
        file,
        "Escort current impact power = %.4f\n",
        E->currentImpactPower
    );


    /*
       Add E current impact
       to B cumulative damage.
    */

    B->damage +=
        E->currentImpactPower;


    /*
       E has fired one more time.
    */

    E->shotsFired++;


    fprintf(
        file,
        "Battleship cumulative damage = %.4f\n",
        B->damage
    );


    /*
       100% damage
    */

    if(B->damage >= 1.0)
    {
        B->alive =
            0;


        printf(
            "Battleship destroyed.\n"
        );


        fprintf(
            file,
            "BATTLESHIP DESTROYED\n"
        );
    }

    else
    {
        printf(
            "Battleship survived.\n"
        );


        fprintf(
            file,
            "Battleship still alive.\n"
        );
    }


    /*
       Current impact power
       after firing.
    */

    updateEscortImpactPower(E);


    fprintf(
        file,
        "Escort current impact power "
        "after firing = %.4f\n",
        E->currentImpactPower
    );
}


/* =====================================
        SAVE CURRENT CONDITIONS
   ===================================== */

void saveCurrentConditions(
    Battleship B,
    EscortShip E[],
    int N
)
{
    FILE *file;


    file =
        fopen(
            "part2C_current_conditions.txt",
            "w"
        );


    if(file == NULL)
    {
        printf(
            "Error opening file.\n"
        );

        return;
    }


    /*
       Battleship information
    */

    fprintf(
        file,
        "========== BATTLESHIP ==========\n"
    );


    fprintf(
        file,
        "Type = %c\n",
        B.type
    );


    fprintf(
        file,
        "Initial impact power IP0 = %.4f\n",
        B.initialImpactPower
    );


    fprintf(
        file,
        "Gamma = %.4f\n",
        B.gamma
    );


    fprintf(
        file,
        "Shots fired = %d\n",
        B.shotsFired
    );


    fprintf(
        file,
        "Current impact power = %.4f\n",
        B.currentImpactPower
    );


    fprintf(
        file,
        "Damage received = %.4f\n",
        B.damage
    );


    fprintf(
        file,
        "Alive = %d\n\n",
        B.alive
    );


    /*
       Escort information
    */

    fprintf(
        file,
        "========== ESCORT SHIPS ==========\n\n"
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
            "Initial impact power IP0 = %.4f\n",
            E[i].initialImpactPower
        );


        fprintf(
            file,
            "Gamma = %.4f\n",
            E[i].gamma
        );


        fprintf(
            file,
            "Shots fired = %d\n",
            E[i].shotsFired
        );


        fprintf(
            file,
            "Current impact power = %.4f\n",
            E[i].currentImpactPower
        );


        fprintf(
            file,
            "Damage received = %.4f\n",
            E[i].damage
        );


        fprintf(
            file,
            "Alive = %d\n\n",
            E[i].alive
        );
    }


    fclose(file);
}


/* =====================================
                MAIN
   ===================================== */

int main()
{
    Battleship B;

    EscortShip E[MAX_SHIPS];


    int N;

    int rounds;


    FILE *resultFile;


    printf(
        "========== PART 2-C ==========\n"
    );


    /* =================================
       NUMBER OF ESCORT SHIPS
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
        N < 1 ||
        N > MAX_SHIPS
    );


    /* =================================
       BATTLESHIP TYPE
       ================================= */

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
        B.type != 'U'
        &&
        B.type != 'M'
        &&
        B.type != 'R'
        &&
        B.type != 'S'
    );


    /* =================================
       B INITIAL IMPACT POWER

       PDF says previous B impact
       power was 1.
       ================================= */

    B.initialImpactPower =
        1.0;


    B.currentImpactPower =
        B.initialImpactPower;


    B.shotsFired =
        0;


    B.damage =
        0.0;


    B.alive =
        1;


    /* =================================
       B GAMMA

       PDF:
       Gamma is close to zero for B.
       ================================= */

    do
    {
        printf(
            "Enter Battleship gamma "
            "(close to zero): "
        );


        scanf(
            "%lf",
            &B.gamma
        );

    }
    while(B.gamma < 0.0);


    /* =================================
       GENERATE / SET ESCORT SHIPS
       ================================= */

    for(int i = 0;
        i < N;
        i++)
    {
        char type;


        E[i].id =
            i + 1;


        /*
           Random E type A-E
        */

        type =
            'A' +
            rand() % 5;


        E[i].type =
            type;


        /*
           Initial impact power
           according to Table 1.
        */

        E[i].initialImpactPower =
            getEscortImpactPower(
                type
            );


        E[i].currentImpactPower =
            E[i].initialImpactPower;


        E[i].shotsFired =
            0;


        E[i].damage =
            0.0;


        E[i].alive =
            1;


        /*
           PDF:
           Gamma for E is relatively
           higher than B.

           User enters gamma.
        */

        do
        {
            printf(
                "Enter gamma for Escort E%d "
                "(type E%c): ",
                E[i].id,
                E[i].type
            );


            scanf(
                "%lf",
                &E[i].gamma
            );

        }
        while(E[i].gamma < 0.0);
    }


    /* =================================
       NUMBER OF SIMULATION ROUNDS
       ================================= */

    do
    {
        printf(
            "Enter number of battle rounds: "
        );


        scanf(
            "%d",
            &rounds
        );

    }
    while(rounds <= 0);


    /* =================================
       OPEN RESULT FILE
       ================================= */

    resultFile =
        fopen(
            "part2C_results.txt",
            "w"
        );


    if(resultFile == NULL)
    {
        printf(
            "Error opening result file.\n"
        );

        return 1;
    }


    fprintf(
        resultFile,
        "========== PART 2-C ==========\n\n"
    );


    /* =================================
       SIMULATION
       ================================= */

    for(int round = 0;

        round < rounds
        &&
        B.alive;

        round++)
    {
        printf(
            "\n========== ROUND %d ==========\n",
            round + 1
        );


        fprintf(
            resultFile,
            "\n========== ROUND %d ==========\n",
            round + 1
        );


        /* =================================
           B ATTACKS FIRST ALIVE ESCORT
           ================================= */

        int target =
            -1;


        for(int i = 0;
            i < N;
            i++)
        {
            if(E[i].alive)
            {
                target =
                    i;

                break;
            }
        }


        if(target == -1)
        {
            printf(
                "All Escort ships destroyed.\n"
            );


            fprintf(
                resultFile,
                "All Escort ships destroyed.\n"
            );


            break;
        }


        battleshipAttack(
            &B,
            &E[target],
            resultFile
        );


        /* =================================
           ALIVE ESCORTS ATTACK B
           ================================= */

        for(int i = 0;

            i < N
            &&
            B.alive;

            i++)
        {
            if(E[i].alive)
            {
                escortAttack(
                    &E[i],
                    &B,
                    resultFile
                );
            }
        }
    }


    /* =================================
       FINAL CURRENT IMPACT FACTORS
       ================================= */

    updateBattleshipImpactPower(
        &B
    );


    for(int i = 0;
        i < N;
        i++)
    {
        updateEscortImpactPower(
            &E[i]
        );
    }


    fprintf(
        resultFile,
        "\n========== FINAL RESULTS ==========\n"
    );


    fprintf(
        resultFile,
        "B status = %s\n",
        B.alive
        ?
        "ALIVE"
        :
        "DESTROYED"
    );


    fprintf(
        resultFile,
        "B shots fired = %d\n",
        B.shotsFired
    );


    fprintf(
        resultFile,
        "B current impact factor = %.4f\n",
        B.currentImpactPower
    );


    fprintf(
        resultFile,
        "B cumulative damage = %.4f\n\n",
        B.damage
    );


    for(int i = 0;
        i < N;
        i++)
    {
        fprintf(
            resultFile,
            "Escort E%d\n",
            E[i].id
        );


        fprintf(
            resultFile,
            "Type = E%c\n",
            E[i].type
        );


        fprintf(
            resultFile,
            "Shots fired = %d\n",
            E[i].shotsFired
        );


        fprintf(
            resultFile,
            "Current impact factor = %.4f\n",
            E[i].currentImpactPower
        );


        fprintf(
            resultFile,
            "Damage = %.4f\n",
            E[i].damage
        );


        fprintf(
            resultFile,
            "Alive = %d\n\n",
            E[i].alive
        );
    }


    fclose(
        resultFile
    );


    /*
       PDF specifically asks to save
       current impact factor of each ship.
    */

    saveCurrentConditions(
        B,
        E,
        N
    );


    printf(
        "\nPart 2-C completed.\n"
    );


    return 0;
}
