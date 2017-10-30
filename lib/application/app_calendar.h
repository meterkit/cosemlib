#ifndef APP_CALENDAR_H
#define APP_CALENDAR_H

#include <stdint.h>

#include "app_definitions.h"
#include "os_util.h"

typedef struct
{
    uint16_t action_script_id;
    uint8_t start_hour;
    uint8_t start_min;
} cal_switching_action_t;

typedef struct
{
    uint8_t day_profile_id;
    cal_switching_action_t switching_actions[CAL_MAX_SWITCHING_ACTIONS];
    uint8_t switching_actions_size;
} cal_day_profile_t;

typedef struct
{
    uint16_t special_day_id;
    uint8_t day_profile_id;
    uint8_t date[5];
} cal_special_day_t;

typedef struct
{
    uint8_t week_profile_od;
    uint8_t day_of_week_profile_id[7]; // one per week day
} cal_week_profile_t;

typedef struct
{
    uint8_t season_profile_id;
    uint8_t week_profile_id;
    uint8_t date_time[CLK_DATE_TIME_SIZE];
} cal_season_profile_t;

typedef struct
{
    uint8_t date_time[CLK_DATE_TIME_SIZE];
    uint8_t date_time_enable;
} cal_sched_date_time_t;

typedef struct
{
    cal_day_profile_t day_profiles[CAL_MAX_DAY_PROFILES];
    uint8_t day_profiles_size;
    cal_week_profile_t week_profiles[CAL_MAX_WEEK_PROFILES];
    uint8_t week_profiles_size;
    cal_season_profile_t season_profiles[CAL_MAX_SEASON_PROFILES];
    uint8_t season_profiles_size;
    uint8_t mName[CAL_MAX_NAME_SIZE];
    cal_sched_date_time_t sched_date_time;
} cal_scheduler_t;

typedef struct
{
    cal_special_day_t list[CAL_MAX_SPECIAL_DAYS];
    uint16_t special_days_size;
} cal_special_days_t;


// =================================================================================================
// FUNCTIONS
// =================================================================================================

// General component functions
int cal_init(cal_scheduler_t *sched);

// Day profiles functions
int cal_append_day_profile(cal_scheduler_t *sched, const cal_day_profile_t *dp);
int cal_is_day_profile_valid(const cal_day_profile_t *dp);
int cal_get_day_profile_by_id(const cal_scheduler_t *sched, uint8_t id, cal_day_profile_t *dp);
int cal_get_day_profile_by_index(const cal_scheduler_t *sched, const uint8_t index, cal_day_profile_t *dp);
int cal_is_day_profile_exists(const cal_scheduler_t *sched, uint8_t id, uint8_t *index);


#endif // APP_CALENDAR_H
