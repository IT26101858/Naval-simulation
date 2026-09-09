#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define G 9.81
#define PI 3.14159265

struct Battleship {
    char notation;
    char name[30];
    char gun_name[40];
    int x, y;
    float max_speed;
    float gamma;
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
    float min_speed, max_speed;
    float min_angle, max_angle;
    float impact_power;
    float gamma;
    float health;
    float distance;
    float reload_time;
    int has_attacked;
};

char setup_b_notation = 'U';
int setup_canvas_size = 5000;
int setup_escort_count = 5;
int setup_k_points = 10;
int setup_jam_iteration = 5;
float setup_theta_min = 20.0;
float setup_b_gamma = 0.01;
float setup_e_gamma = 0.05;
float setup_b_reload = 10.0;
float setup_e_reload = 5.0;


void setupBattleshipProperties(struct Battleship *b) {

    printf("\n--- Battleship Properties Setup ---\n");

    printf("Available Notations: U (USS Iowa), M (King George V), R (Richelieu), S (Sovetsky Soyuz)\n");

    printf("Enter Battleship Notation: ");
    scanf(" %c", &setup_b_notation);

    b->notation = setup_b_notation;

    if (b->notation == 'U') {

        sprintf(b->name, "USS Iowa (BB-61)");
        sprintf(b->gun_name, "50-caliber Mark 7 gun");

    } else if (b->notation == 'M') {

        sprintf(b->name, "MS King George V");
        sprintf(b->gun_name, "(356 mm) Mark VII gun");

    } else if (b->notation == 'R') {

        sprintf(b->name, "Richelieu");
        sprintf(b->gun_name, "(15 inch) Mle 1935 gun");

    } else {

        sprintf(b->name, "Sovetsky Soyuz-class");
        sprintf(b->gun_name, "(16 inch) B-37 gun");
    }

    printf("Enter Battleship Gamma value (close to zero): ");
    scanf("%f", &setup_b_gamma);

    b->gamma = setup_b_gamma;

    printf("Enter Battleship Reload Time (T_B): ");
    scanf("%f", &setup_b_reload);

    b->reload_time = setup_b_reload;

    printf("--> Battleship configured successfully.\n");
}


void setupEscortShipsSettings() {

    printf("\n--- Escort Ships Settings & Battlefield Setup ---\n");

    printf("Enter Canvas Size (D) for DxD square canvas: ");
    scanf("%d", &setup_canvas_size);

    printf("Enter Number of Escort Ships (N): ");
    scanf("%d", &setup_escort_count);

    printf("Enter Escort Reload Time (T_E): ");
    scanf("%f", &setup_e_reload);

    printf("Enter Simulation Iterations (k points): ");
    scanf("%d", &setup_k_points);

    printf("Enter Jamming Iteration (t < k) and Min Angle (theta_min): ");
    scanf("%d %f", &setup_jam_iteration, &setup_theta_min);

    printf("--> Escort and Battlefield settings saved.\n");
}


void setupMenu(struct Battleship *b) {

    int choice;

    do {

        printf("\n--- Setup Submenu Options ---\n");
        printf("1. Battleship Properties (Notation, Gamma, Reload)\n");
        printf("2. Escort ships Settings (Canvas, N, Vmax, Angles, Impact, Gamma)\n");
        printf("3. Seed value\n");
        printf("4. Return to Main Menu\n");

        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {

            while (getchar() != '\n');

            choice = 0;
        }

        switch (choice) {

            case 1:
                setupBattleshipProperties(b);
                break;

            case 2:
                setupEscortShipsSettings();
                break;

            case 3: {

                int seed;

                printf("Enter random seed value: ");
                scanf("%d", &seed);

                srand(seed);

                printf("--> Seed configured.\n");

                break;
            }

            case 4:
                printf("--> Returning to Main Menu...\n");
                break;

            default:
                printf("--> Invalid choice!\n");
        }

    } while (choice != 4);
}


void runSimulation() {

    printf("\n=== Part 1-C Naval Battle Simulation ===\n");

    struct Battleship b1;

    b1.notation = setup_b_notation;

    b1.x = rand() % (setup_canvas_size + 1);
    b1.y = rand() % (setup_canvas_size + 1);

    b1.max_speed = 300.0;

    b1.health = 100.0;

    b1.impact_power = 1.0;

    b1.gamma = setup_b_gamma;

    b1.shots_fired = 0;

    b1.reload_time = setup_b_reload;


    struct EscortShip enemies[setup_escort_count];

    char notations[5] = {'A', 'B', 'C', 'D', 'E'};

    float fixed_impacts[5] = {
        0.08,
        0.06,
        0.07,
        0.05,
        0.04
    };


    for (int i = 0; i < setup_escort_count; i++) {

        enemies[i].id = i + 1;

        int type_idx = rand() % 5;

        enemies[i].notation = notations[type_idx];

        enemies[i].impact_power = fixed_impacts[type_idx];

        enemies[i].x = rand() % (setup_canvas_size + 1);

        enemies[i].y = rand() % (setup_canvas_size + 1);

        enemies[i].min_speed = 20.0 + (rand() % 30);

        enemies[i].max_speed = b1.max_speed * 1.2;

        enemies[i].min_angle = 10.0 + (rand() % 10);

        enemies[i].max_angle =
            enemies[i].min_angle +
            (type_idx == 0 ? 20 :
            (type_idx == 1 ? 30 : 25));

        enemies[i].health = 100.0;

        enemies[i].gamma = setup_e_gamma;

        enemies[i].reload_time = setup_e_reload;

        enemies[i].has_attacked = 0;

        float dx = enemies[i].x - b1.x;
        float dy = enemies[i].y - b1.y;

        enemies[i].distance =
            sqrt((dx * dx) + (dy * dy));
    }


    FILE *init_file = fopen("initial_conditions.txt", "w");

    if (init_file != NULL) {

        fprintf(init_file,
                "=== Initial Conditions ===\n");

        fprintf(init_file,
                "Battleship: %c | Position: (%d, %d)\n",
                b1.notation,
                b1.x,
                b1.y);

        fprintf(init_file,
                "ID\tNotation\tX\tY\tDistance\tImpactPower\n");

        for (int i = 0; i < setup_escort_count; i++) {

            fprintf(init_file,
                    "%d\t%c\t\t%d\t%d\t%.2f\t\t%.2f\n",
                    enemies[i].id,
                    enemies[i].notation,
                    enemies[i].x,
                    enemies[i].y,
                    enemies[i].distance,
                    enemies[i].impact_power);
        }

        fclose(init_file);
    }


    FILE *log_file = fopen("combat_log.txt", "w");

    if (log_file == NULL) {

        printf("Error opening combat_log.txt\n");

        return;
    }


    fprintf(log_file,
            "=== PART 1-C NAVAL BATTLE COMBAT LOG ===\n\n");


    int alive_enemies = setup_escort_count;

    int loop_iteration = 1;

    float total_time = 0.0;

    float cumulative_impact = 0.0;

    int killer_id = -1;


    while (
        b1.health > 0 &&
        alive_enemies > 0 &&
        loop_iteration <= setup_k_points
    ) {


        if (loop_iteration > 1) {

            b1.x =
                rand() % (setup_canvas_size + 1);

            b1.y =
                rand() % (setup_canvas_size + 1);


            for (int i = 0;
                 i < setup_escort_count;
                 i++) {

                if (enemies[i].health > 0) {

                    float dx =
                        enemies[i].x - b1.x;

                    float dy =
                        enemies[i].y - b1.y;

                    enemies[i].distance =
                        sqrt((dx * dx) +
                             (dy * dy));
                }
            }
        }


        total_time += b1.reload_time;


        printf(
            "\n--- Iteration %d ---\n",
            loop_iteration
        );

        printf(
            "Time: %.1fs | Battleship Location: (%d, %d)\n",
            total_time,
            b1.x,
            b1.y
        );


        fprintf(
            log_file,
            "Iteration %d | Time %.1fs | Location (%d, %d)\n",
            loop_iteration,
            total_time,
            b1.x,
            b1.y
        );


        int is_jammed =
            (loop_iteration >= setup_jam_iteration);


        if (is_jammed) {

            printf(
                "--> [JAMMED GUN] Vertical angle restricted to >= %.1f deg\n",
                setup_theta_min
            );

            fprintf(
                log_file,
                "  - Gun Jammed. Minimum angle: %.1f degrees\n",
                setup_theta_min
            );
        }


        int target_idx = -1;

        float min_dist = 9999999.0;

        float target_angle = 0.0;


        for (int i = 0;
             i < setup_escort_count;
             i++) {

            if (enemies[i].health > 0) {

                float dist =
                    enemies[i].distance;


                float sine_val =
                    (dist * G) /
                    (b1.max_speed *
                     b1.max_speed);


                if (sine_val <= 1.0) {

                    float angle_deg =
                        (asin(sine_val) / 2.0) *
                        (180.0 / PI);


                    if (
                        is_jammed &&
                        angle_deg < setup_theta_min
                    ) {

                        continue;
                    }


                    if (dist < min_dist) {

                        min_dist = dist;

                        target_idx = i;

                        target_angle = angle_deg;
                    }
                }
            }
        }


        if (target_idx != -1) {

            b1.shots_fired++;


            float damage =
                b1.impact_power * 100.0;


            enemies[target_idx].health = 0;


            printf(
                "[Battleship] Fired at Escort ID %d (%c)\n",
                enemies[target_idx].id,
                enemies[target_idx].notation
            );

            printf(
                "  Angle: %.2f deg | Damage: %.2f%%\n",
                target_angle,
                damage
            );


            fprintf(
                log_file,
                "  - Battleship hit Escort ID %d (%c). Angle: %.2f | Damage: %.2f%%\n",
                enemies[target_idx].id,
                enemies[target_idx].notation,
                target_angle,
                damage
            );


            alive_enemies--;


            printf(
                "  -> Escort ID %d DESTROYED!\n",
                enemies[target_idx].id
            );


            fprintf(
                log_file,
                "  -> Escort ID %d DESTROYED.\n",
                enemies[target_idx].id
            );


        } else {

            printf(
                "[Battleship] No valid targets in range or angle restriction!\n"
            );

            fprintf(
                log_file,
                "  - No valid target found.\n"
            );
        }


        /*
         * Part 1-C:
         * Each E ship can attack only once.
         * Impact Power determines damage to Battleship.
         */

        for (int i = 0;
             i < setup_escort_count;
             i++) {

            if (
                enemies[i].health > 0 &&
                enemies[i].has_attacked == 0
            ) {

                float escort_damage =
                    enemies[i].impact_power * 100.0;


                b1.health -= escort_damage;


                cumulative_impact +=
                    escort_damage;


                enemies[i].has_attacked = 1;


                if (b1.health < 0) {

                    b1.health = 0;
                }


                printf(
                    "[Escort AI] ID %d (%c) attacked once\n",
                    enemies[i].id,
                    enemies[i].notation
                );

                printf(
                    "  Impact Power: %.2f | Damage: %.2f%% | Battleship HP: %.2f%%\n",
                    enemies[i].impact_power,
                    escort_damage,
                    b1.health
                );


                fprintf(
                    log_file,
                    "  - Escort ID %d (%c) attacked once. Impact Power: %.2f | Damage: %.2f%% | Battleship HP: %.2f%%\n",
                    enemies[i].id,
                    enemies[i].notation,
                    enemies[i].impact_power,
                    escort_damage,
                    b1.health
                );


                if (b1.health <= 0) {

                    killer_id =
                        enemies[i].id;


                    printf(
                        "  -> Battleship DESTROYED by Escort ID %d (%c)!\n",
                        enemies[i].id,
                        enemies[i].notation
                    );


                    fprintf(
                        log_file,
                        "  -> Battleship DESTROYED by Escort ID %d (%c).\n",
                        enemies[i].id,
                        enemies[i].notation
                    );


                    break;
                }
            }
        }


        loop_iteration++;
    }


    printf("\n=== Simulation Finished ===\n");


    fprintf(
        log_file,
        "\n=== Final Status ===\n"
    );


    fprintf(
        log_file,
        "Total Time: %.1fs\n",
        total_time
    );


    fprintf(
        log_file,
        "Battleship HP: %.2f%%\n",
        b1.health
    );


    fprintf(
        log_file,
        "Cumulative Impact on Battleship: %.2f%%\n",
        cumulative_impact
    );


    if (b1.health <= 0) {

        printf(
            "Battleship was DESTROYED.\n"
        );

        printf(
            "Destroyed by Escort ID: %d\n",
            killer_id
        );

        printf(
            "Cumulative Impact: %.2f%%\n",
            cumulative_impact
        );


        fprintf(
            log_file,
            "Battleship destroyed by Escort ID: %d\n",
            killer_id
        );


    } else {

        printf(
            "Battleship survived the simulation.\n"
        );

        printf(
            "Cumulative Impact on Battleship: %.2f%%\n",
            cumulative_impact
        );


        fprintf(
            log_file,
            "Battleship survived the simulation.\n"
        );


        fprintf(
            log_file,
            "Cumulative Impact on Battleship: %.2f%%\n",
            cumulative_impact
        );
    }


    fprintf(
        log_file,
        "\n=== Final Escort Ship Status ===\n"
    );


    fprintf(
        log_file,
        "ID\tNotation\tHealth\tAttacked\n"
    );


    for (int i = 0;
         i < setup_escort_count;
         i++) {

        fprintf(
            log_file,
            "%d\t%c\t\t%.2f\t%d\n",
            enemies[i].id,
            enemies[i].notation,
            enemies[i].health,
            enemies[i].has_attacked
        );
    }


    fclose(log_file);


    printf(
        "--> Results saved to 'combat_log.txt' and 'initial_conditions.txt'.\n"
    );
}


void displayStatistics() {

    printf(
        "\n--- Simulation Statistics (Loading combat_log.txt) ---\n"
    );


    FILE *f =
        fopen("combat_log.txt", "r");


    if (f == NULL) {

        printf(
            "--> No previous simulation logs found. Run a simulation first.\n"
        );

        return;
    }


    char ch;


    while ((ch = fgetc(f)) != EOF) {

        putchar(ch);
    }


    fclose(f);


    printf(
        "\n----------------------------------------------------\n"
    );
}


void displayInstructions() {

    printf(
        "\n================ INSTRUCTIONS ================\n"
    );

    printf(
        "1. Automated AI Naval Battle Simulator.\n"
    );

    printf(
        "2. Part 1-C uses percentage-based Escort Ship impact power.\n"
    );

    printf(
        "3. Each Escort Ship can attack only once.\n"
    );

    printf(
        "4. Battleship can destroy an Escort Ship with one attack.\n"
    );

    printf(
        "5. Cumulative impact on Battleship is recorded.\n"
    );

    printf(
        "==============================================\n"
    );
}


int main() {

    struct Battleship b1;


    b1.notation = 'U';

    sprintf(
        b1.name,
        "USS Iowa (BB-61)"
    );

    sprintf(
        b1.gun_name,
        "50-caliber Mark 7 gun"
    );

    b1.gamma = 0.01;

    b1.reload_time = 10.0;


    int choice;


    do {

        printf(
            "\n=== Main Menu Options ===\n"
        );

        printf(
            "1. Start Simulation (Setup & Show simulation)\n"
        );

        printf(
            "2. View Instructions\n"
        );

        printf(
            "3. Simulation Statistics\n"
        );

        printf(
            "4. Exit\n"
        );


        printf(
            "Enter your choice: "
        );


        if (scanf("%d", &choice) != 1) {

            while (getchar() != '\n');

            choice = 0;
        }


        switch (choice) {

            case 1: {

                int sub;


                printf(
                    "\n--- Start Simulation Submenu ---\n"
                );

                printf(
                    "1. Setup\n"
                );

                printf(
                    "2. Show simulation (Run & Display)\n"
                );


                printf(
                    "Enter choice: "
                );

                scanf("%d", &sub);


                if (sub == 1) {

                    setupMenu(&b1);

                } else if (sub == 2) {

                    runSimulation();
                }


                break;
            }


            case 2:

                displayInstructions();

                break;


            case 3:

                displayStatistics();

                break;


            case 4:

                printf(
                    "\n--> Exiting simulator. Goodbye!\n"
                );

                break;


            default:

                printf(
                    "--> Invalid choice!\n"
                );
        }


    } while (choice != 4);


    return 0;
}
