#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define G 9.81
#define PI 3.14159265
#define MAX_SHIPS 100

struct Battleship {
    char notation;
    char name[30];
    char gun_name[40];

    int x, y;

    float max_speed;
    float health;
    float impact_power;

    int shots_fired;
    float reload_time;
};

struct EscortShip {
    int id;
    char notation;
    char type_name[30];

    int x, y;

    float min_speed;
    float max_speed;

    float min_angle;
    float max_angle;

    float impact_power;
    float health;
    float distance;

    int has_attacked;
};

char setup_b_notation = 'U';

int setup_canvas_size = 5000;
int setup_escort_count = 5;
int setup_k_points = 10;

int setup_jam_iteration = 5;
float setup_theta_min = 20.0;

float setup_b_reload = 10.0;

int random_seed = 1;


/* ---------------------------------------------------------
   Battleship setup
   --------------------------------------------------------- */

void setupBattleshipProperties()
{
    printf("\n--- Battleship Properties ---\n");

    printf("U - USS Iowa\n");
    printf("M - King George V\n");
    printf("R - Richelieu\n");
    printf("S - Sovetsky Soyuz\n");

    printf("Enter Battleship notation: ");
    scanf(" %c", &setup_b_notation);

    printf("Enter B firing time T_B: ");
    scanf("%f", &setup_b_reload);

    printf("Battleship setup completed.\n");
}


/* ---------------------------------------------------------
   Battlefield setup
   --------------------------------------------------------- */

void setupBattlefield()
{
    printf("\n--- Battlefield Settings ---\n");

    printf("Enter Canvas Size D: ");
    scanf("%d", &setup_canvas_size);

    printf("Enter Number of Escort Ships: ");
    scanf("%d", &setup_escort_count);

    if (setup_escort_count > MAX_SHIPS)
        setup_escort_count = MAX_SHIPS;

    printf("Enter Number of Simulation Points k: ");
    scanf("%d", &setup_k_points);

    printf("Enter Jam Iteration t: ");
    scanf("%d", &setup_jam_iteration);

    printf("Enter Minimum Angle theta_min: ");
    scanf("%f", &setup_theta_min);

    printf("Battlefield setup completed.\n");
}


/* ---------------------------------------------------------
   Seed setup
   --------------------------------------------------------- */

void setupSeed()
{
    printf("\nEnter random seed: ");
    scanf("%d", &random_seed);

    srand(random_seed);

    printf("Seed configured.\n");
}


/* ---------------------------------------------------------
   Setup menu
   --------------------------------------------------------- */

void setupMenu()
{
    int choice;

    do
    {
        printf("\n================ SETUP MENU ================\n");

        printf("1. Battleship Properties\n");
        printf("2. Battlefield / Escort Settings\n");
        printf("3. Seed Value\n");
        printf("4. Return\n");

        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                setupBattleshipProperties();
                break;

            case 2:
                setupBattlefield();
                break;

            case 3:
                setupSeed();
                break;

            case 4:
                printf("Returning...\n");
                break;

            default:
                printf("Invalid choice.\n");
        }

    } while (choice != 4);
}


/* ---------------------------------------------------------
   Get battleship name
   --------------------------------------------------------- */

void getBattleshipDetails(struct Battleship *b)
{
    if (b->notation == 'U')
    {
        sprintf(b->name, "USS Iowa (BB-61)");
        sprintf(b->gun_name, "50-caliber Mark 7 gun");
    }
    else if (b->notation == 'M')
    {
        sprintf(b->name, "King George V");
        sprintf(b->gun_name, "356 mm Mark VII gun");
    }
    else if (b->notation == 'R')
    {
        sprintf(b->name, "Richelieu");
        sprintf(b->gun_name, "380 mm Mle 1935 gun");
    }
    else
    {
        sprintf(b->name, "Sovetsky Soyuz");
        sprintf(b->gun_name, "B-37 gun");
    }
}


/* ---------------------------------------------------------
   Get escort ship type name
   --------------------------------------------------------- */

void getEscortTypeName(struct EscortShip *e)
{
    if (e->notation == 'A')
        sprintf(e->type_name, "Escort Type A");

    else if (e->notation == 'B')
        sprintf(e->type_name, "Escort Type B");

    else if (e->notation == 'C')
        sprintf(e->type_name, "Escort Type C");

    else if (e->notation == 'D')
        sprintf(e->type_name, "Escort Type D");

    else
        sprintf(e->type_name, "Escort Type E");
}


/* ---------------------------------------------------------
   Calculate distance
   --------------------------------------------------------- */

float calculateDistance(int x1, int y1, int x2, int y2)
{
    float dx = x2 - x1;
    float dy = y2 - y1;

    return sqrt((dx * dx) + (dy * dy));
}


/* ---------------------------------------------------------
   Initialize battlefield
   --------------------------------------------------------- */

void initializeBattlefield(
    struct Battleship *b,
    struct EscortShip enemies[]
)
{
    char notations[5] = {'A', 'B', 'C', 'D', 'E'};

    float impactPower[5] =
    {
        0.08,
        0.06,
        0.07,
        0.05,
        0.04
    };

    int i;

    b->notation = setup_b_notation;

    b->x = rand() % (setup_canvas_size + 1);
    b->y = rand() % (setup_canvas_size + 1);

    b->max_speed = 300.0;
    b->health = 100.0;

    /*
       In Part 2-A we have not introduced
       impact degradation yet.
    */
    b->impact_power = 1.0;

    b->shots_fired = 0;
    b->reload_time = setup_b_reload;

    getBattleshipDetails(b);


    for (i = 0; i < setup_escort_count; i++)
    {
        int typeIndex = rand() % 5;

        enemies[i].id = i + 1;

        enemies[i].notation = notations[typeIndex];

        enemies[i].impact_power =
            impactPower[typeIndex];

        enemies[i].x =
            rand() % (setup_canvas_size + 1);

        enemies[i].y =
            rand() % (setup_canvas_size + 1);

        enemies[i].min_speed =
            20.0 + (rand() % 30);

        enemies[i].max_speed =
            b->max_speed * 1.2;

        enemies[i].min_angle =
            10.0 + (rand() % 10);

        enemies[i].max_angle =
            enemies[i].min_angle + 25.0;

        enemies[i].health = 100.0;

        enemies[i].has_attacked = 0;

        enemies[i].distance =
            calculateDistance(
                b->x,
                b->y,
                enemies[i].x,
                enemies[i].y
            );

        getEscortTypeName(&enemies[i]);
    }
}


/* ---------------------------------------------------------
   Update distances
   --------------------------------------------------------- */

void updateDistances(
    struct Battleship *b,
    struct EscortShip enemies[]
)
{
    int i;

    for (i = 0; i < setup_escort_count; i++)
    {
        if (enemies[i].health > 0)
        {
            enemies[i].distance =
                calculateDistance(
                    b->x,
                    b->y,
                    enemies[i].x,
                    enemies[i].y
                );
        }
    }
}


/* ---------------------------------------------------------
   Check whether B can hit E
   --------------------------------------------------------- */

int canBattleshipHit(
    struct Battleship *b,
    struct EscortShip *e,
    int jammed
)
{
    float sineValue;
    float angle;

    sineValue =
        (e->distance * G) /
        (b->max_speed * b->max_speed);

    if (sineValue > 1.0)
        return 0;

    angle =
        (asin(sineValue) / 2.0) *
        (180.0 / PI);

    if (jammed && angle < setup_theta_min)
        return 0;

    return 1;
}


/* ---------------------------------------------------------
   CUSTOM STRATEGY
   ---------------------------------------------------------

   Strategy:

   1. Highest Impact Power first
   2. If impact power is equal,
      nearest E ship first

   This helps B destroy the most dangerous
   escort ships first.
   --------------------------------------------------------- */

void sortAttackOrder(
    struct Battleship *b,
    struct EscortShip enemies[],
    int order[],
    int count
)
{
    int i, j;

    for (i = 0; i < count; i++)
    {
        order[i] = i;
    }

    for (i = 0; i < count - 1; i++)
    {
        for (j = i + 1; j < count; j++)
        {
            int a = order[i];
            int c = order[j];

            int swapNeeded = 0;

            if (enemies[c].impact_power >
                enemies[a].impact_power)
            {
                swapNeeded = 1;
            }
            else if (
                enemies[c].impact_power ==
                enemies[a].impact_power &&
                enemies[c].distance <
                enemies[a].distance
            )
            {
                swapNeeded = 1;
            }

            if (swapNeeded)
            {
                int temp = order[i];

                order[i] = order[j];
                order[j] = temp;
            }
        }
    }
}


/* ---------------------------------------------------------
   Save initial battlefield
   --------------------------------------------------------- */

void saveInitialConditions(
    struct Battleship *b,
    struct EscortShip enemies[]
)
{
    FILE *file;

    int i;

    file = fopen(
        "2A_initial_conditions.txt",
        "w"
    );

    if (file == NULL)
    {
        printf("Error opening file.\n");
        return;
    }

    fprintf(
        file,
        "========== PART 2-A INITIAL CONDITIONS ==========\n\n"
    );

    fprintf(
        file,
        "Battleship: %s\n",
        b->name
    );

    fprintf(
        file,
        "Notation: %c\n",
        b->notation
    );

    fprintf(
        file,
        "Position: (%d, %d)\n",
        b->x,
        b->y
    );

    fprintf(
        file,
        "B Firing Time (T_B): %.2f seconds\n\n",
        b->reload_time
    );

    fprintf(
        file,
        "ID\tType\tX\tY\tDistance\tImpact Power\n"
    );

    for (i = 0; i < setup_escort_count; i++)
    {
        fprintf(
            file,
            "%d\t%c\t%d\t%d\t%.2f\t%.2f\n",
            enemies[i].id,
            enemies[i].notation,
            enemies[i].x,
            enemies[i].y,
            enemies[i].distance,
            enemies[i].impact_power
        );
    }

    fclose(file);
}


/* ---------------------------------------------------------
   Run Part 2-A Simulation
   --------------------------------------------------------- */

void runSimulation(int simulationType)
{
    struct Battleship b;

    struct EscortShip enemies[MAX_SHIPS];

    int attackOrder[MAX_SHIPS];

    int aliveEnemies;

    int iteration;

    int orderCount;

    float totalTime = 0.0;

    float cumulativeImpact = 0.0;

    int killerID = -1;

    FILE *logFile;

    int i;


    printf("\n============================================\n");

    if (simulationType == 1)
        printf("       PART 2-A - SIMULATION 1\n");

    else if (simulationType == 2)
        printf("       PART 2-A - SIMULATION 2\n");

    else
        printf("       PART 2-A - SIMULATION 3\n");

    printf("============================================\n");


    initializeBattlefield(
        &b,
        enemies
    );


    saveInitialConditions(
        &b,
        enemies
    );


    if (simulationType == 1)
    {
        logFile =
            fopen(
                "2A_part1A_results.txt",
                "w"
            );
    }
    else if (simulationType == 2)
    {
        logFile =
            fopen(
                "2A_part1B_results.txt",
                "w"
            );
    }
    else
    {
        logFile =
            fopen(
                "2A_part1C_results.txt",
                "w"
            );
    }


    if (logFile == NULL)
    {
        printf("Unable to create result file.\n");
        return;
    }


    fprintf(
        logFile,
        "========== PART 2-A SIMULATION ==========\n\n"
    );

    fprintf(
        logFile,
        "Battleship: %s\n",
        b.name
    );

    fprintf(
        logFile,
        "B Firing Time T_B: %.2f seconds\n\n",
        b.reload_time
    );


    aliveEnemies =
        setup_escort_count;


    iteration = 1;


    while (
        aliveEnemies > 0 &&
        b.health > 0 &&
        iteration <= setup_k_points
    )
    {

        /*
           Part 1-B uses movement through
           generated points.

           Part 1-A keeps the same position.
        */

        if (simulationType == 2)
        {
            b.x =
                rand() %
                (setup_canvas_size + 1);

            b.y =
                rand() %
                (setup_canvas_size + 1);

            updateDistances(
                &b,
                enemies
            );
        }


        printf(
            "\n--- Iteration %d ---\n",
            iteration
        );

        printf(
            "B Position: (%d, %d)\n",
            b.x,
            b.y
        );


        fprintf(
            logFile,
            "\n--- Iteration %d ---\n",
            iteration
        );

        fprintf(
            logFile,
            "B Position: (%d, %d)\n",
            b.x,
            b.y
        );


        /*
           Time between B gun firings.
        */

        totalTime += b.reload_time;


        int jammed = 0;


        if (
            simulationType == 2 &&
            iteration >= setup_jam_iteration
        )
        {
            jammed = 1;

            printf(
                "B gun JAMMED. Minimum angle = %.2f\n",
                setup_theta_min
            );

            fprintf(
                logFile,
                "B gun JAMMED. Minimum angle = %.2f\n",
                setup_theta_min
            );
        }


        /*
           Find E ships inside B's
           attack range.
        */

        orderCount = 0;


        for (i = 0; i < setup_escort_count; i++)
        {
            if (enemies[i].health > 0)
            {
                if (
                    canBattleshipHit(
                        &b,
                        &enemies[i],
                        jammed
                    )
                )
                {
                    attackOrder[orderCount] = i;

                    orderCount++;
                }
            }
        }


        /*
           Apply custom strategy.
        */

        if (orderCount > 0)
        {
            sortAttackOrder(
                &b,
                enemies,
                attackOrder,
                orderCount
            );
        }


        /*
           Save attack order.
        */

        fprintf(
            logFile,
            "B Attack Order: "
        );

        for (i = 0; i < orderCount; i++)
        {
            int index =
                attackOrder[i];

            fprintf(
                logFile,
                "E%d",
                enemies[index].id
            );

            if (i < orderCount - 1)
                fprintf(
                    logFile,
                    " -> "
                );
        }

        fprintf(
            logFile,
            "\n"
        );


        printf(
            "B Attack Order: "
        );


        for (i = 0; i < orderCount; i++)
        {
            int index =
                attackOrder[i];

            printf(
                "E%d",
                enemies[index].id
            );

            if (i < orderCount - 1)
                printf(
                    " -> "
                );
        }

        printf("\n");


        /*
           B fires one shell.

           In Part 1-A/B/C,
           B can destroy an E with
           one attack.
        */

        if (orderCount > 0)
        {
            int target =
                attackOrder[0];

            b.shots_fired++;

            enemies[target].health = 0;

            aliveEnemies--;


            printf(
                "B fired at E%d.\n",
                enemies[target].id
            );

            printf(
                "E%d destroyed.\n",
                enemies[target].id
            );


            fprintf(
                logFile,
                "B fired at E%d.\n",
                enemies[target].id
            );

            fprintf(
                logFile,
                "E%d destroyed.\n",
                enemies[target].id
            );
        }


        /*
           Escort ships attack.

           Part 1-A/B:
           One successful E attack destroys B.

           Part 1-C:
           Each E damages B according to
           its impact power and each E
           can attack only once.
        */

        for (i = 0; i < setup_escort_count; i++)
        {
            if (
                enemies[i].health > 0 &&
                enemies[i].has_attacked == 0
            )
            {
                /*
                   For this simulation we assume
                   an E can attack when its
                   projectile is physically possible.
                */

                float sineValue =
                    (enemies[i].distance * G) /
                    (300.0 * 300.0);

                if (sineValue <= 1.0)
                {
                    /*
                       Part 1-C:
                       percentage damage.
                    */

                    if (simulationType == 3)
                    {
                        float damage =
                            enemies[i].impact_power *
                            100.0;

                        b.health -= damage;

                        cumulativeImpact += damage;

                        enemies[i].has_attacked = 1;


                        printf(
                            "E%d attacked B: %.2f%% damage\n",
                            enemies[i].id,
                            damage
                        );

                        fprintf(
                            logFile,
                            "E%d attacked B: %.2f%% damage\n",
                            enemies[i].id,
                            damage
                        );
                    }

                    /*
                       Part 1-A/B:
                       One E attack destroys B.
                    */

                    else
                    {
                        b.health = 0;

                        killerID =
                            enemies[i].id;

                        enemies[i].has_attacked = 1;


                        printf(
                            "E%d destroyed B!\n",
                            enemies[i].id
                        );

                        fprintf(
                            logFile,
                            "E%d destroyed B!\n",
                            enemies[i].id
                        );

                        break;
                    }
                }
            }
        }


        if (b.health < 0)
            b.health = 0;


        printf(
            "B Health: %.2f%%\n",
            b.health
        );

        printf(
            "Elapsed Time: %.2f seconds\n",
            totalTime
        );


        fprintf(
            logFile,
            "B Health: %.2f%%\n",
            b.health
        );

        fprintf(
            logFile,
            "Elapsed Time: %.2f seconds\n",
            totalTime
        );


        iteration++;
    }


    /*
       Final results
    */

    fprintf(
        logFile,
        "\n========== FINAL RESULTS ==========\n"
    );

    fprintf(
        logFile,
        "Battle Duration: %.2f seconds\n",
        totalTime
    );

    fprintf(
        logFile,
        "B Health: %.2f%%\n",
        b.health
    );

    fprintf(
        logFile,
        "B Shots Fired: %d\n",
        b.shots_fired
    );


    if (b.health <= 0)
    {
        printf(
            "\nBATTLESHIP DESTROYED!\n"
        );

        printf(
            "E%d sank the Battleship.\n",
            killerID
        );

        fprintf(
            logFile,
            "Battleship destroyed by E%d.\n",
            killerID
        );
    }
    else
    {
        printf(
            "\nBattleship survived.\n"
        );

        printf(
            "Cumulative impact on B: %.2f%%\n",
            cumulativeImpact
        );

        fprintf(
            logFile,
            "Cumulative Impact on B: %.2f%%\n",
            cumulativeImpact
        );
    }


    fprintf(
        logFile,
        "\nFinal Escort Ship Status:\n"
    );


    for (i = 0; i < setup_escort_count; i++)
    {
        fprintf(
            logFile,
            "E%d | Type %c | Health %.2f%% | "
            "Position (%d,%d)\n",
            enemies[i].id,
            enemies[i].notation,
            enemies[i].health,
            enemies[i].x,
            enemies[i].y
        );
    }


    fclose(logFile);


    printf(
        "\nSimulation completed.\n"
    );

    printf(
        "Results saved to text file.\n"
    );
}


/* ---------------------------------------------------------
   Part 2-A menu
   --------------------------------------------------------- */

void simulationMenu()
{
    int choice;

    do
    {
        printf("\n============================================\n");
        printf("             PART 2-A MENU\n");
        printf("============================================\n");

        printf("1. Simulate Part 1-A\n");
        printf("2. Simulate Part 1-B\n");
        printf("3. Simulate Part 1-C\n");
        printf("4. Return\n");

        printf("Enter choice: ");
        scanf("%d", &choice);


        switch (choice)
        {
            case 1:
                runSimulation(1);
                break;

            case 2:
                runSimulation(2);
                break;

            case 3:
                runSimulation(3);
                break;

            case 4:
                break;

            default:
                printf("Invalid choice.\n");
        }

    } while (choice != 4);
}


/* ---------------------------------------------------------
   Instructions
   --------------------------------------------------------- */

void displayInstructions()
{
    printf("\n============================================\n");
    printf("              PART 2-A INSTRUCTIONS\n");
    printf("============================================\n");

    printf("\n1. Battleship has one gun.\n");

    printf(
        "2. T_B represents the time between "
        "two consecutive B gun firings.\n"
    );

    printf(
        "3. T_B is entered by the user.\n"
    );

    printf(
        "4. B uses a custom attack strategy.\n"
    );

    printf(
        "5. Highest Impact Power E ships are "
        "selected first.\n"
    );

    printf(
        "6. If Impact Power is equal, "
        "the nearest E is selected first.\n"
    );

    printf(
        "7. Part 1-A, 1-B and 1-C simulations "
        "are supported.\n"
    );

    printf(
        "8. Attack order is saved in the "
        "simulation result files.\n"
    );

    printf(
        "9. Part 1-C uses percentage damage "
        "from E ships.\n"
    );

    printf(
        "10. Each E ship can attack only once "
        "in Part 1-C.\n"
    );
}


/* ---------------------------------------------------------
   Statistics
   --------------------------------------------------------- */

void displayStatistics()
{
    FILE *file;

    char filename[100];

    int choice;

    printf("\n--- Simulation Statistics ---\n");

    printf("1. Part 1-A results\n");
    printf("2. Part 1-B results\n");
    printf("3. Part 1-C results\n");

    printf("Enter choice: ");
    scanf("%d", &choice);


    if (choice == 1)
    {
        sprintf(
            filename,
            "2A_part1A_results.txt"
        );
    }
    else if (choice == 2)
    {
        sprintf(
            filename,
            "2A_part1B_results.txt"
        );
    }
    else if (choice == 3)
    {
        sprintf(
            filename,
            "2A_part1C_results.txt"
        );
    }
    else
    {
        printf("Invalid choice.\n");
        return;
    }


    file = fopen(filename, "r");


    if (file == NULL)
    {
        printf(
            "No simulation result found.\n"
        );

        return;
    }


    char ch;

    while ((ch = fgetc(file)) != EOF)
    {
        putchar(ch);
    }


    fclose(file);
}


/* ---------------------------------------------------------
   Main
   --------------------------------------------------------- */

int main()
{
    int choice;

    /*
       Default random seed
    */

    srand((unsigned int)time(NULL));


    do
    {
        printf("\n");
        printf("============================================\n");
        printf("        NAVAL BATTLE SIMULATOR\n");
        printf("                 PART 2-A\n");
        printf("============================================\n");

        printf("1. Setup\n");
        printf("2. Start Simulation\n");
        printf("3. View Instructions\n");
        printf("4. Simulation Statistics\n");
        printf("5. Exit\n");

        printf("Enter choice: ");
        scanf("%d", &choice);


        switch (choice)
        {
            case 1:
                setupMenu();
                break;

            case 2:
                simulationMenu();
                break;

            case 3:
                displayInstructions();
                break;

            case 4:
                displayStatistics();
                break;

            case 5:
                printf(
                    "\nExiting simulator...\n"
                );
                break;

            default:
                printf(
                    "Invalid choice.\n"
                );
        }

    } while (choice != 5);


    return 0;
}
