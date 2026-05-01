/**
 * @file vaccine_system.h
 * @brief Header file containing all definitions and interfaces for the vaccine management system
 * @author ist114255 Joana Melo
 */
#ifndef VACCINE_SYSTEM_H
#define VACCINE_SYSTEM_H

/** Maximum number of vaccine batches that can be stored in the system */
#define MAX_BATCHES 1000
/** Maximum length for batch ID strings */
#define ID_LEN 21
/** Maximum length for vaccine name strings */
#define NAME_LEN 51
/** Maximum length for user input */
#define MAX_INPUT 1000
/** Size of the hash table for user lookup */
#define HASH_SIZE 256
/** Initial capacity for dynamic arrays */
#define INIT_CAP 10

/**
 * @brief Structure to represent a calendar date
 */
typedef struct {
    int day, month, year;
} Date;

/**
 * @brief Structure to represent a vaccine batch
 */
typedef struct {
    char id[ID_LEN];          /**< Unique batch identifier */
    char name[NAME_LEN];      /**< Name of the vaccine */
    Date expiry;              /**< Expiration date */
    int total, used, available; /**< Dose counts and availability flag */
} Batch;

/**
 * @brief Structure to represent a vaccination record
 */
typedef struct {
    char* user;               /**< Name of the vaccinated person */
    char batch_id[ID_LEN];    /**< ID of the batch used */
    Date date;                /**< Date of vaccination */
} Vaccination;

/**
 * @brief Structure for hash table entry to track user vaccinations
 */
typedef struct {
    char* user;               /**< User name */
    int* indices;             /**< Array of indices to vaccination records */
    int count, capacity;      /**< Current count and capacity of indices array */
} UserEntry;

/**
 * @brief Hash table structure for efficient user lookup
 */
typedef struct {
    UserEntry** entries;      /**< Array of user entry pointers */
    int size, count;          /**< Size of table and number of entries */
} HashTable;

/**
 * @brief Enumeration of possible error conditions
 */
typedef enum {
    ERR_TOO_MANY_VACCINES,
    ERR_DUPLICATE_BATCH,
    ERR_INVALID_BATCH,
    ERR_INVALID_NAME,
    ERR_INVALID_DATE,
    ERR_INVALID_QUANTITY,
    ERR_NO_SUCH_VACCINE,
    ERR_NO_STOCK,
    ERR_ALREADY_VACCINATED,
    ERR_NO_SUCH_BATCH,
    ERR_NO_SUCH_USER,
    ERR_OUT_OF_MEMORY
} Error;

/**
 * @brief Main structure containing the complete vaccination system state
 */
typedef struct {
    Batch batches[MAX_BATCHES];   /**< Array of vaccine batches */
    int batch_count;              /**< Number of batches currently stored */
    Vaccination* vaccinations;    /**< Dynamic array of vaccination records */
    int vax_count, vax_capacity;  /**< Count and capacity of vaccinations array */
    Date current_date;            /**< Current system date */
    int language;                 /**< Language setting (0=EN, 1=PT) */
    HashTable* users;             /**< Hash table for user lookup */
} VaccineSystem;

/* Function declarations - Hash Table Operations */
HashTable* create_table(int size);
void free_table(HashTable *table);
UserEntry* find_user(HashTable *table, const char *user);
UserEntry* add_user(HashTable *table, const char *user);
int add_vax_index(UserEntry *user, int index);

/* Function declarations - Core Vaccine System Functions */
void error(VaccineSystem *sys, Error code, const char *param);
void cleanup_resources(char *batch, char *name);
void init_system(VaccineSystem *sys);
int date_cmp(Date a, Date b);
int date_valid(Date d, Date current, int strict);
int valid_id(const char *id);
int valid_name(const char *name);
int validate_batch_inputs(VaccineSystem *sys, const char *batch, const char *name, Date expiry, int doses);
int find_batch(VaccineSystem *sys, const char *id);
void print_batch(const Batch *b);
void print_user_vaccination(Vaccination *vax);
void sort_batches(VaccineSystem *sys);
void add_batch(VaccineSystem *sys, const char *batch, const char *name, Date expiry, int doses);
void list_vaccine(VaccineSystem *sys, const char *vax);
void list_all_batches(VaccineSystem *sys);
int ensure_vax_capacity(VaccineSystem *sys);
int is_vaccinated(VaccineSystem *sys, HashTable *table, const char *user, const char *vax, Date date);
int find_best_batch(VaccineSystem *sys, const char *vax_name);
void register_vaccination(VaccineSystem *sys, const char *user, int batch_idx);
char* extract_quoted_string(const char *str);
char* extract_unquoted_string(const char *str, const char *delimiter);
char* get_user_name(const char *line, int type);
char* read_input_line();
void parse_date_from_token(char *token, Date *date, int *has_date);
int validate_user_batch(VaccineSystem *sys, const char *user, const char *id, int *has_id);
int process_delete_parameters(VaccineSystem *sys, char *line, char **user, Date *date, char *id, int *has_date, int *has_id);
int should_delete_vaccination(Vaccination *vax, const char *user, const Date *date, const char *id, int has_date, int has_id);
void rebuild_user_table(VaccineSystem *sys);

#endif /* VACCINE_SYSTEM_H */
