#include "app_calendar.h"


int cal_is_day_profile_exists(const cal_scheduler_t *sched, uint8_t id, uint8_t *index)
{
    int ret = RET_ERR;

    for (uint8_t i = 0U; i < sched->day_profiles_size; i++)
    {
        if (sched->day_profiles[i].day_profile_id == id)
        {
            *index = i;
            ret = RET_OK;
        }
    }
    return ret;
}

int cal_get_day_profile_by_index(const cal_scheduler_t *sched, const uint8_t index, cal_day_profile_t *dp)
{
    int ret = RET_ERR;

    if (index < sched->day_profiles_size)
    {
        *dp = sched->day_profiles[index];
        ret = RET_OK;
    }
    return ret;
}

int cal_get_day_profile_by_id(const cal_scheduler_t *sched, uint8_t id, cal_day_profile_t *dp)
{
    int ret = RET_ERR;

    uint8_t index = 0U;
    if (cal_is_day_profile_exists(sched, id, &index))
    {
        *dp = sched->day_profiles[index];
        ret = RET_OK;
    }
    return ret;
}


int cal_is_day_profile_valid(const cal_day_profile_t *dp)
{
    int ret = RET_ERR;
    uint8_t size = dp->switching_actions_size;

    if (size <= CAL_MAX_SWITCHING_ACTIONS)
    {
        // Test the chronological order of the switching actions and the script id range validity
        for (uint8_t timeIndex = 0U; timeIndex < size; timeIndex++)
        {
            uint8_t hours = dp->switching_actions[timeIndex].start_hour;
            uint8_t minutes = dp->switching_actions[timeIndex].start_min;

            /*
            // We do not check the script ID, we don't care of the value (meter specific)
            // Check if time is valid
            if (alchemy::util::Time::IsValidTime(hours, minutes, alchemy::util::Seconds(0U)))
            {
                // b. If the index > 0 -> Check the chronological order (strictly chronological)
                if (timeIndex > 0U)
                {
                    alchemy::util::Time timeCurrent(hours, minutes, alchemy::util::Seconds(0U));
                    alchemy::util::Time timePrevious(alchemy::util::Hours(i_dayProfile.switchingActions[timeIndex - 1U].startHour),
                                                     alchemy::util::Minutes(i_dayProfile.switchingActions[timeIndex - 1U].startMin),
                                                     alchemy::util::Seconds(0U));

                    if (timeCurrent <= timePrevious)
                    {
                        ret = RET_ERR;
                        AL_LOG_E("[SCHED] Bad day profile, must be set in chronological order");
                    }
                    else
                    {
                        ret = RET_OK;
                    }
                }

            }
            else
            {
                ret = RET_ERR;
                AL_LOG_E("[SCHED] Not a valid Time format");
            }
             */
        }
    }
    return ret;
}



int cal_append_day_profile(cal_scheduler_t *sched, const cal_day_profile_t *dp)
{
    int ret = RET_OK;
    uint8_t size = sched->day_profiles_size;

    if ((size < CAL_MAX_DAY_PROFILES) && (cal_is_day_profile_valid(dp)))
    {
        for (uint8_t i = 0U; i < size; i++)
        {
            // Check the uniqueness of the identifiers
            if (sched->day_profiles[i].day_profile_id == dp->day_profile_id)
            {
                ret = RET_ERR;
            }
        }

        if (ret)
        {
            sched->day_profiles[size] = *dp;
            sched->day_profiles_size++;
        }
    }
    else
    {
        ret = RET_ERR;
    }
    return ret;
}
