/**
 * @file main.c
 * @brief Main program entry point and command processing for vaccine management system
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vaccine_system.h"

/**
 * @brief Handles the batch creation command
 * @param sys Pointer to the vaccine system
 */
static void command_create_batch(VaccineSystem *sys) {
    char *batch = malloc(MAX_INPUT);
    char *name = malloc(MAX_INPUT);
    int day, month, year, doses;
    Date expiry;
    
    if (!batch || !name) {
        cleanup_resources(batch, name);
        error(sys, ERR_OUT_OF_MEMORY, NULL);
        return;
    }
    
    if (scanf("%s %d-%d-%d %d %s", batch, &day, &month, &year, 
             &doses, name) != 6) {
        cleanup_resources(batch, name);
        return;
    }

    expiry.day = day;
    expiry.month = month;
    expiry.year = year;

    if (!validate_batch_inputs(sys, batch, name, expiry, doses)) {
        cleanup_resources(batch, name);
        return;
    }

    add_batch(sys, batch, name, expiry, doses);
    cleanup_resources(batch, name);
}

/**
 * @brief Handles the set time command
 * @param sys Pointer to the vaccine system
 */
static void command_set_time(VaccineSystem *sys) {
    Date new;
    if (scanf("%d-%d-%d", &new.day, &new.month, &new.year) != 3) return;
    if (date_valid(new, sys->current_date, 0)) {
        sys->current_date = new;
        printf("%02d-%02d-%d\n", new.day, new.month, new.year);
    } else {
        error(sys, ERR_INVALID_DATE, NULL);
    }
}

/**
 * @brief Handles the list command for vaccines
 * @param sys Pointer to the vaccine system
 */
static void command_list(VaccineSystem *sys) {
    char *input = read_input_line();
    if (!input) return;
    
    char *token = strtok(input, " ");
    if (!token) {
        list_all_batches(sys);
    } else {
        do {
            list_vaccine(sys, token);
        } while ((token = strtok(NULL, " ")));
    }
    free(input);
}

/**
 * @brief Handles the add vaccination command
 * @param sys Pointer to the vaccine system
 */
static void command_add_vaccination(VaccineSystem *sys) {
    char *input = read_input_line();
    if (!input) return;

    char *last_space = strrchr(input, ' ');
    if (!last_space) { 
        free(input); 
        return; 
    }
    
    char *vax_name = strdup(last_space + 1);
    char *user = get_user_name(input, 0);
    if (!vax_name || !user) { 
        free(input); 
        free(vax_name); 
        free(user); 
        return; 
    }

    if (is_vaccinated(sys, sys->users, user, vax_name, sys->current_date)) {
        error(sys, ERR_ALREADY_VACCINATED, NULL);
        free(input); 
        free(vax_name); 
        free(user);
        return;
    }

    int batch_idx = find_best_batch(sys, vax_name);
    
    if (batch_idx == -1) {
        error(sys, ERR_NO_STOCK, NULL);
        free(input); 
        free(vax_name); 
        free(user);
        return;
    }

    if (!ensure_vax_capacity(sys)) {
        free(input); 
        free(vax_name); 
        free(user);
        return;
    }

    register_vaccination(sys, user, batch_idx);
    free(input);
    free(vax_name);
    free(user);
}

/**
 * @brief Handles the list users command
 * @param sys Pointer to the vaccine system
 */
static void command_list_users(VaccineSystem *sys) {
    char *line = read_input_line();
    if (!line) return;

    char *user = get_user_name(line, 1);
    if (user) {
        int found = 0;
        for (int i = 0; i < sys->vax_count; i++) {
            if (!strcmp(sys->vaccinations[i].user, user)) {
                print_user_vaccination(&sys->vaccinations[i]);
                found = 1;
            }
        }
        if (!found) error(sys, ERR_NO_SUCH_USER, user);
        free(user);
    } else {
        for (int i = 0; i < sys->vax_count; i++) {
            print_user_vaccination(&sys->vaccinations[i]);
        }
    }
    free(line);
}

/**
 * @brief Handles the remove batch command
 * @param sys Pointer to the vaccine system
 */
static void command_remove_batch(VaccineSystem *sys) {
    char id[ID_LEN];
    scanf("%20s", id);
    int idx = find_batch(sys, id);
    if (idx == -1) { 
        error(sys, ERR_NO_SUCH_BATCH, id); 
        return; 
    }
    printf("%d\n", sys->batches[idx].used);
    sys->batches[idx].available = 0;
    if (sys->batches[idx].used) sys->batches[idx].total = sys->batches[idx].used;
}

/**
 * @brief Handles the delete vaccinations command
 * @param sys Pointer to the vaccine system
 */
static void command_delete_vaccinations(VaccineSystem *sys) {
    char *line = read_input_line();
    if (!line) return;

    char *user = NULL;
    char id[ID_LEN] = "";
    Date date = {0};
    int has_date = 0, has_id = 0;
    
    if (!process_delete_parameters(sys, line, &user, &date, id, 
                                 &has_date, &has_id)) {
        free(line);
        return;
    }

    int *delete = calloc(sys->vax_count, sizeof(int));
    if (!delete) { 
        error(sys, ERR_OUT_OF_MEMORY, NULL); 
        free(line); 
        free(user); 
        return; 
    }

    int total = 0;
    for (int i = 0; i < sys->vax_count; i++) {
        if (should_delete_vaccination(&sys->vaccinations[i], user, 
                                   &date, id, has_date, has_id)) {
            delete[i] = 1; 
            total++; 
        }
    }

    int j = 0;
    for (int i = 0; i < sys->vax_count; i++) {
        if (!delete[i]) {
            if (j != i) sys->vaccinations[j] = sys->vaccinations[i];
            j++;
        } else {
            free(sys->vaccinations[i].user);
        }
    }
    sys->vax_count = j;
    
    if (sys->users) {
        rebuild_user_table(sys);
    }

    printf("%d\n", total);
    free(delete);
    free(line);
    free(user);
}

/**
 * @brief Handles the quit command and cleanup
 * @param sys Pointer to the vaccine system
 */
static void command_quit(VaccineSystem *sys) {
    for (int i = 0; i < sys->vax_count; i++) free(sys->vaccinations[i].user);
    free(sys->vaccinations);
    if (sys->users) free_table(sys->users);
    exit(0);
}

/**
 * @brief Processes commands from standard input
 * @param sys Pointer to the vaccine system
 */
static void process_command(VaccineSystem *sys) {
    char cmd;
    while (scanf(" %c", &cmd) == 1) {
        switch (cmd) {
            case 'q': command_quit(sys); break;
            case 'c': command_create_batch(sys); break;
            case 't': command_set_time(sys); break;
            case 'l': command_list(sys); break;
            case 'a': command_add_vaccination(sys); break;
            case 'u': command_list_users(sys); break;
            case 'r': command_remove_batch(sys); break;
            case 'd': command_delete_vaccinations(sys); break;
        }
    }
}

/**
 * @brief Main function - entry point of the program
 * @param argc Argument count
 * @param argv Array of argument strings
 * @return Exit status code
 */
int main(int argc, char *argv[]) {
    VaccineSystem sys;
    init_system(&sys);
    if (argc > 1 && !strcmp(argv[1], "pt")) sys.language = 1;
    process_command(&sys);
    return 0;
}
