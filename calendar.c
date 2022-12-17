// Project 1 for COSE101
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum cal_day_t;
typedef enum cal_day_t cal_day_t;

enum cal_month_t;
typedef enum cal_month_t cal_month_t;

struct cal_event_t;
typedef struct cal_event_t cal_event_t;

enum cal_day_t {
    CAL_DAY_SUN,
    CAL_DAY_MON,
    CAL_DAY_TUE,
    CAL_DAY_WED,
    CAL_DAY_THU,
    CAL_DAY_FRI,
    CAL_DAY_SAT,
};

enum cal_month_t {
    CAL_MONTH_JAN = 1,
    CAL_MONTH_FEB,
    CAL_MONTH_MAR,
    CAL_MONTH_APR,
    CAL_MONTH_MAY,
    CAL_MONTH_JUN,
    CAL_MONTH_JUL,
    CAL_MONTH_AUG,
    CAL_MONTH_SEP,
    CAL_MONTH_OCT,
    CAL_MONTH_NOV,
    CAL_MONTH_DEC,
};

struct cal_event_t {
    cal_event_t *next;
    int year;
    cal_month_t month;
    cal_day_t day;
    int type;
    bool gone;
    char content[1000];
};

int cal_month_lengths[CAL_MONTH_DEC + 1] = {
    [0] = 31,
    [CAL_MONTH_JAN] = 31,
    [CAL_MONTH_FEB] = 28,
    [CAL_MONTH_MAR] = 31,
    [CAL_MONTH_APR] = 30,
    [CAL_MONTH_MAY] = 31,
    [CAL_MONTH_JUN] = 30,
    [CAL_MONTH_JUL] = 31,
    [CAL_MONTH_AUG] = 31,
    [CAL_MONTH_SEP] = 30,
    [CAL_MONTH_OCT] = 31,
    [CAL_MONTH_NOV] = 30,
    [CAL_MONTH_DEC] = 31,
};

const char *cal_day_to_str(cal_day_t day) {
    switch (day) {
    case CAL_DAY_SUN:
        return "Sun";
    case CAL_DAY_MON:
        return "Mon";
    case CAL_DAY_TUE:
        return "Tue";
    case CAL_DAY_WED:
        return "Wed";
    case CAL_DAY_THU:
        return "Thu";
    case CAL_DAY_FRI:
        return "Fri";
    case CAL_DAY_SAT:
        return "Sat";
    }
}

const char *cal_month_to_str(cal_month_t month) {
    switch (month) {
    case CAL_MONTH_JAN:
        return "Jan";
    case CAL_MONTH_FEB:
        return "Feb";
    case CAL_MONTH_MAR:
        return "Mar";
    case CAL_MONTH_APR:
        return "Apr";
    case CAL_MONTH_MAY:
        return "May";
    case CAL_MONTH_JUN:
        return "Jun";
    case CAL_MONTH_JUL:
        return "Jul";
    case CAL_MONTH_AUG:
        return "Aug";
    case CAL_MONTH_SEP:
        return "Sep";
    case CAL_MONTH_OCT:
        return "Oct";
    case CAL_MONTH_NOV:
        return "Nov";
    case CAL_MONTH_DEC:
        return "Dec";
    }
}

int cal_year_length(int year) {
    if (year % 4 == 0) {
        return 366;
    } else {
        return 365;
    }
}

cal_day_t cal_year_first_day_of_week(int year) {
    int days = CAL_DAY_THU;
    for (int cur_year = 1970; cur_year < year; cur_year++) {
        days += cal_year_length(cur_year);
    }
    return days % 7;
}

cal_day_t cal_first_day_of_month(int year, cal_month_t month) {
    int day = cal_year_first_day_of_week(year);
    for (cal_month_t cur_month = CAL_MONTH_JAN; cur_month < month;
         cur_month++) {
        day += cal_month_lengths[cur_month];
    }
    return day % 7;
}

const char *cal_day_suffix(int day) {
    if (day == 1 || day == 21 || day == 31) {
        return "st";
    }
    if (day == 2 || day == 22) {
        return "nd";
    }
    if (day == 3 || day == 23) {
        return "rd";
    }
    return "th";
}

void cal_handle_print_command(int year, cal_month_t month,
                              cal_event_t *events) {
    cal_day_t day = cal_first_day_of_month(year, month);
    printf("\n");
    for (int i = 0; i < 14; i++) {
        printf(" ");
    }
    printf("%i-%i", year, month);
    printf("\n\n");
    cal_day_t days[7];
    for (int i = day; i < day + 7; i++) {
        days[i - day] = (i - 1) % 7;
    }
    for (int i = 0; i < 7; i++) {
        printf("  %s", cal_day_to_str(i));
    }
    printf("\n");
    int dates[7][6] = {};
    int prints = 0;
    for (int i = day - 1; i >= 0; i--) {
        printf("%5i", (cal_month_lengths[(month + 11) % 12] - i));
        prints++;
    }
    while (prints < 7 * 6) {
        printf("%5i", (prints - day) % cal_month_lengths[month] + 1);
        prints++;
        if (prints % 7 == 0) {
            printf("\n");
        }
    }
    printf("\n");
    while (events != NULL) {
        if (events->year == year && events->month == month && !events->gone) {
            printf("%i%s", events->day, cal_day_suffix(events->day));
            printf(" %s %i", cal_month_to_str(events->month), events->year);
            if (events->type == 0) {
                printf(" (private)");
            }
            printf(": ");
            printf("%s\n", events->content);
        }
        events = events->next;
    }
}

void cal_handle_commands(void) {
    cal_event_t *events = NULL;
    while (true) {
        char command[10] = {};
        printf("input command ==> ");
        scanf("%s", command);
        if (!strcmp(command, "print")) {
            int year, month;
            scanf("%i %i", &year, &month);
            cal_handle_print_command(year, month, events);
            continue;
        }
        if (!strcmp(command, "add")) {
            cal_event_t *new_events = calloc(1, sizeof(cal_event_t));
            new_events->next = events;
            new_events->gone = false;
            scanf("%i %i %i %i", &new_events->year, &new_events->month,
                  &new_events->day, &new_events->type);
            for (int i = 0; i < 1000; i++) {
                char c = getchar();
                if (c == '\n') {
                    new_events->content[i] = '\0';
                    break;
                }
                new_events->content[i] = c;
                new_events->content[i + 1] = '\0';
            }
            events = new_events;
            continue;
        }
        if (!strcmp(command, "remove")) {
            int year, month, day;
            scanf("%i %i %i", &year, &month, &day);
            cal_event_t *ev = events;
            while (true) {
                if (ev == NULL) {
                    printf("error: could not find any events on that day\n");
                    break;
                }
                if (!ev->gone && ev->year == year && ev->month == month &&
                    ev->day == day) {
                    ev->gone = true;
                    break;
                }
                ev = ev->next;
            }
            continue;
        }
        if (!strcmp(command, "save")) {
            char name[FILENAME_MAX];
            scanf("%s", name);
            FILE *save_file = fopen(name, "w");
            cal_event_t *ev = events;
            while (ev != NULL) {
                if (!ev->gone) {
                    fprintf(save_file, "%i %i %i %i%s\n", ev->year, ev->month,
                            ev->day, ev->type, ev->content);
                }
                ev = ev->next;
            }
            fclose(save_file);
            continue;
        }
        if (!strcmp(command, "load")) {
            char name[FILENAME_MAX];
            scanf("%s", name);
            FILE *in_file = fopen(name, "r");
            if (in_file == NULL) {
                printf("error: cannot open file: %s", name);
            }
            while (!feof(in_file)) {
                cal_event_t *new_events = calloc(1, sizeof(cal_event_t));
                new_events->next = events;
                new_events->gone = false;
                fscanf(in_file, "%i %i %i %i", &new_events->year,
                       &new_events->month, &new_events->day, &new_events->type);
                for (int i = 0; i < 1000; i++) {
                    char c = fgetc(in_file);
                    if (c == '\n') {
                        new_events->content[i] = '\0';
                        break;
                    }
                    new_events->content[i] = c;
                    new_events->content[i + 1] = '\0';
                }
                events = new_events;
            }
            continue;
        }
        if (!strcmp(command, "exit")) {
            printf("goodbye!\n");
            return;
        }
        printf("unknown command: %s\n", command);
    }
}

int main() { cal_handle_commands(); }
