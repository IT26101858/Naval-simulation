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

    int destroyed;
};

float calculateDistance(float x1, float y1, float x2, float y2)
{
    float dx = x2 - x1;
    float dy = y2 - y1;

    return sqrt(dx * dx + dy * dy);
}

/* Maximum projectile range */
float calculateMaxRange(float velocity)
{
    return (velocity * velocity) / G;
}

/* Time required for projectile */
float calculateHitTime(float distance, float velocity)
{
    float range = calculateMaxRange(velocity);

    if (distance > range)
    {
        return -1;
    }

    float angle = asin((distance * G) /
                       (velocity * velocity)) / 2.0;

    float time =
        (2 * velocity * sin(angle)) / G;

    return time;
}

void setBattleship(struct Battleship *b)
{
    printf("\nSelect Battleship Type\n");
    printf("U - USS Iowa (BB-61)\n");
    printf("M - MS King George V\n");
    printf("R - Richelieu\n");
    printf("S - Sovetsky Soyuz-class\n");

    printf("Enter type: ");
    scanf(" %c", &b->notation);

    switch (b->notation)
    {
        case 'U':
        case 'u':
            sprintf(b->name, "USS Iowa (BB-61)");
            b->notation = 'U';
            break;

        case 'M':
        case 'm':
            sprintf(b->name, "MS King George V");
            b->notation = 'M';
            break;

        case 'R':
        case 'r':
            sprintf(b->name, "Richelieu");
            b->notation = 'R';
            break;

        case 'S':
        case 's':
            sprintf(b->name, "Sovetsky Soyuz-class");
            b->notation = 'S';
            break;

        default:
            printf("Invalid type. USS Iowa selected.\n");

            b->notation = 'U';
            sprintf(b->name, "USS Iowa (BB-61)");
    }

    printf("Enter maximum shell velocity: ");
    scanf("%f", &b->maxVelocity);
}

void createEscortShip(struct EscortShip *e,
                      int id,
                      int canvasSize,
                      float battleshipMaxVelocity)
{
    int type = rand() % 5;

    e->id = id;

    e->x = rand() % (canvasSize + 1);
    e->y = rand() % (canvasSize + 1);

    e->destroyed = 0;

    switch (type)
    {
        case 0:

            e->notation = 'A';

            sprintf(e->name,
                    "1936A-class Destroyer");

            e->impactPower = 0.08;

            e->minAngle = 20;

            e->maxAngle = 40;

            e->minVelocity =
                50 + rand() % 100;

            e->maxVelocity =
                1.2 * battleshipMaxVelocity;

            break;


        case 1:

            e->notation = 'B';

            sprintf(e->name,
                    "Gabbiano-class Corvette");

            e->impactPower = 0.06;

            e->minAngle = 30;

            e->maxAngle = 60;

            e->minVelocity =
                50 + rand() % 100;

            e->maxVelocity =
                50 + rand() %
                (int)(battleshipMaxVelocity - 50);

            break;


        case 2:

            e->notation = 'C';

            sprintf(e->name,
                    "Matsu-class Destroyer");

            e->impactPower = 0.07;

            e->minAngle = 25;

            e->maxAngle = 50;

            e->minVelocity =
                50 + rand() % 100;

            e->maxVelocity =
                50 + rand() %
                (int)(battleshipMaxVelocity - 50);

            break;


        case 3:

            e->notation = 'D';

            sprintf(e->name,
                    "F-class Escort Ship");

            e->impactPower = 0.05;

            e->minAngle = 50;

            e->maxAngle = 75;

            e->minVelocity =
                50 + rand() % 100;

            e->maxVelocity =
                50 + rand() %
                (int)(battleshipMaxVelocity - 50);

            break;


        case 4:

            e->notation = 'E';

            sprintf(e->name,
                    "Japanese Kaibokan");

            e->impactPower = 0.04;

            e->minAngle = 70;

            e->maxAngle = 90;

            e->minVelocity =
                50 + rand() % 100;

            e->maxVelocity =
                50 + rand() %
                (int)(battleshipMaxVelocity - 50);

            break;
    }
}

void saveInitialConditions(
    struct Battleship b,
    struct EscortShip escorts[],
    int numberOfEscorts,
    int canvasSize)
{
    FILE *file;

    file = fopen(
        "initial_conditions_1A.txt",
        "w");

    if (file == NULL)
    {
        printf("Error creating file.\n");
        return;
    }

    fprintf(file,
            "===== INITIAL CONDITIONS =====\n\n");

    fprintf(file,
            "Canvas Size: %d x %d\n\n",
            canvasSize,
            canvasSize);

    fprintf(file,
            "BATTLESHIP\n");

    fprintf(file,
            "Type: %c\n",
            b.notation);

    fprintf(file,
            "Name: %s\n",
            b.name);

    fprintf(file,
            "Position: (%.2f, %.2f)\n",
            b.x,
            b.y);

    fprintf(file,
            "Maximum Velocity: %.2f\n\n",
            b.maxVelocity);

    fprintf(file,
            "ESCORT SHIPS\n\n");

    for (int i = 0;
         i < numberOfEscorts;
         i++)
    {
        fprintf(file,
                "ID: %d\n",
                escorts[i].id);

        fprintf(file,
                "Type: E%c\n",
                escorts[i].notation);

        fprintf(file,
                "Name: %s\n",
                escorts[i].name);

        fprintf(file,
                "Position: (%.2f, %.2f)\n",
                escorts[i].x,
                escorts[i].y);

        fprintf(file,
                "Velocity Range: %.2f - %.2f\n",
                escorts[i].minVelocity,
                escorts[i].maxVelocity);

        fprintf(file,
                "Angle Range: %.2f - %.2f\n",
                escorts[i].minAngle,
                escorts[i].maxAngle);

        fprintf(file,
                "Impact Power: %.2f\n\n",
                escorts[i].impactPower);
    }

    fclose(file);
}

void saveFinalConditions(
    struct Battleship b,
    struct EscortShip escorts[],
    int numberOfEscorts,
    int battleshipDestroyed,
    int sinkingEscort,
    int destroyedEscorts,
    float totalTime)
{
    FILE *file;

    file = fopen(
        "final_conditions_1A.txt",
        "w");

    if (file == NULL)
    {
        printf("Error creating file.\n");
        return;
    }

    fprintf(file,
            "===== FINAL CONDITIONS =====\n\n");

    if (battleshipDestroyed)
    {
        fprintf(file,
                "Battleship Status: SUNK\n");

        fprintf(file,
                "Sunk by Escort ID: %d\n",
                sinkingEscort);
    }
    else
    {
        fprintf(file,
                "Battleship Status: SURVIVED\n");

        fprintf(file,
                "Escort Ships Destroyed: %d\n",
                destroyedEscorts);

        fprintf(file,
                "Battle End Time: %.2f seconds\n",
                totalTime);
    }

    fprintf(file,
            "\nEscort Status\n\n");

    for (int i = 0;
         i < numberOfEscorts;
         i++)
    {
        fprintf(file,
                "Escort ID %d : ",
                escorts[i].id);

        if (escorts[i].destroyed)
        {
            fprintf(file,
                    "DESTROYED\n");
        }
        else
        {
            fprintf(file,
                    "ACTIVE\n");
        }
    }

    fclose(file);
}

void saveHitDetails(
    struct EscortShip escorts[],
    float hitTimes[],
    int numberOfEscorts)
{
    FILE *file;

    file = fopen(
        "hit_details_1A.txt",
        "w");

    if (file == NULL)
    {
        return;
    }

    fprintf(file,
            "===== ESCORT SHIPS HIT =====\n\n");

    for (int i = 0;
         i < numberOfEscorts;
         i++)
    {
        if (escorts[i].destroyed)
        {
            fprintf(file,
                    "Escort ID: %d\n",
                    escorts[i].id);

            fprintf(file,
                    "Type: E%c\n",
                    escorts[i].notation);

            fprintf(file,
                    "Time to Hit: %.2f seconds\n\n",
                    hitTimes[i]);
        }
    }

    fclose(file);
}

int main()
{
    srand(time(NULL));

    struct Battleship battleship;

    int canvasSize;
    int numberOfEscorts;

    printf("====================================\n");
    printf("ADVANCED NAVAL BATTLE SIMULATOR\n");
    printf("PART 1 - A\n");
    printf("====================================\n");

    printf("\nEnter Canvas Size (D): ");
    scanf("%d", &canvasSize);

    printf("Enter Number of Escort Ships (N): ");
    scanf("%d", &numberOfEscorts);

    struct EscortShip escorts[numberOfEscorts];

    float hitTimes[numberOfEscorts];

    setBattleship(&battleship);

    printf("\nBattleship Position\n");

    printf("Enter X coordinate: ");
    scanf("%f",
          &battleship.x);

    printf("Enter Y coordinate: ");
    scanf("%f",
          &battleship.y);

    for (int i = 0;
         i < numberOfEscorts;
         i++)
    {
        createEscortShip(
            &escorts[i],
            i + 1,
            canvasSize,
            battleship.maxVelocity);

        hitTimes[i] = 0;
    }

    saveInitialConditions(
        battleship,
        escorts,
        numberOfEscorts,
        canvasSize);

    printf("\nInitial conditions saved.\n");

    float battleRange =
        calculateMaxRange(
            battleship.maxVelocity);

    printf("\nBattleship Maximum Attack Range: %.2f\n",
           battleRange);

    int battleshipDestroyed = 0;

    int sinkingEscort = -1;

    /*
       First check whether any Escort
       can hit the Battleship.
    */

    for (int i = 0;
         i < numberOfEscorts;
         i++)
    {
        float distance =
            calculateDistance(
                escorts[i].x,
                escorts[i].y,
                battleship.x,
                battleship.y);

        float escortRange =
            calculateMaxRange(
                escorts[i].maxVelocity);

        if (distance <= escortRange)
        {
            battleshipDestroyed = 1;

            sinkingEscort =
                escorts[i].id;

            break;
        }
    }

    int destroyedEscorts = 0;

    float totalTime = 0;

    if (battleshipDestroyed)
    {
        printf("\n====================================\n");

        printf("BATTLESHIP HAS SUNK!\n");

        printf("Escort ID %d sank the Battleship.\n",
               sinkingEscort);

        printf("====================================\n");
    }
    else
    {
        printf("\nBattleship survived.\n");

        for (int i = 0;
             i < numberOfEscorts;
             i++)
        {
            float distance =
                calculateDistance(
                    battleship.x,
                    battleship.y,
                    escorts[i].x,
                    escorts[i].y);

            if (distance <= battleRange)
            {
                escorts[i].destroyed = 1;

                destroyedEscorts++;

                float hitTime =
                    calculateHitTime(
                        distance,
                        battleship.maxVelocity);

                hitTimes[i] = hitTime;

                if (hitTime > totalTime)
                {
                    totalTime =
                        hitTime;
                }

                printf(
                    "Escort ID %d was HIT. "
                    "Time: %.2f seconds\n",
                    escorts[i].id,
                    hitTime);
            }
        }

        printf("\n====================================\n");

        printf("Battleship Survived!\n");

        printf("Escort Ships Destroyed: %d\n",
               destroyedEscorts);

        printf("Battle End Time: %.2f seconds\n",
               totalTime);

        printf("====================================\n");

        saveHitDetails(
            escorts,
            hitTimes,
            numberOfEscorts);
    }

    saveFinalConditions(
        battleship,
        escorts,
        numberOfEscorts,
        battleshipDestroyed,
        sinkingEscort,
        destroyedEscorts,
        totalTime);

    printf("\nFiles created successfully:\n");

    printf("1. initial_conditions_1A.txt\n");
    printf("2. hit_details_1A.txt\n");
    printf("3. final_conditions_1A.txt\n");

    return 0;
}
