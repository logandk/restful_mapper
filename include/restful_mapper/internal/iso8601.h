#ifndef RESTFUL_MAPPER_ISO8601_H_20130314
#define RESTFUL_MAPPER_ISO8601_H_20130314

#include <cstdio>
#include <cctype>
#include <ctime>
#include <string>
#include <stdexcept>

/**
 * @brief Calculate the current UTC time stamp
 *
 * @return the UTC time stamp
 */
inline std::time_t utc_time()
{
  std::time_t now = std::time(NULL);

  std::tm tm_local(*std::localtime(&now));
  std::time_t t_local = std::mktime(&tm_local);

  std::tm tm_utc(*std::gmtime(&t_local));
  std::time_t t_utc = std::mktime(&tm_utc);

  return now - (t_utc - t_local);
}

/**
 * @brief Produces an ISO-8601 string representation of the timestamp
 *
 * @param timestamp the epoch time in seconds
 * @param include_timezone appends Z UTC flag at end of string if true
 *
 * @return a string representing the timestamp in UTC
 */
inline std::string to_iso8601(std::time_t timestamp, const bool &include_timezone = true)
{
  std::tm exploded_time(*std::gmtime(&timestamp));
  std::string ret;

  if (include_timezone)
  {
    char buf[sizeof "1970-01-01T00:00:00Z"];
    std::strftime(buf, sizeof buf, "%Y-%m-%dT%H:%M:%SZ", &exploded_time);
    ret = buf;
  }
  else
  {
    char buf[sizeof "1970-01-01T00:00:00"];
    std::strftime(buf, sizeof buf, "%Y-%m-%dT%H:%M:%S", &exploded_time);
    ret = buf;
  }

  return ret;
}

/**
 * @brief Parses an ISO-8601 formatted string into epoch time
 *
 * @param descriptor the ISO-8601
 *
 * @return the UTC timestamp
 */
inline std::time_t from_iso8601(std::string descriptor)
{
  // Parse according to ISO 8601
  std::tm t;

  const char *value = descriptor.c_str();
  const char *c     = value;
  int year          = 0;
  int month         = 0;
  int seconds       = 0;
  int minutes       = 0;
  int hours         = 0;
  int days          = 0;

  // NOTE: we have to check for the extended format first,
  // because otherwise the separting '-' will be interpreted
  // by std::sscanf as signs of a 1 digit integer .... :-(
  if (std::sscanf(value, "%4u-%2u-%2u", &year, &month, &days) == 3)
  {
    c += 10;
  }
  else if (std::sscanf(value, "%4u%2u%2u", &year, &month, &days) == 3)
  {
    c += 8;
  }
  else
  {
    throw std::runtime_error(std::string("Invalid date format: ") + value);
  }

  t.tm_year = year - 1900;
  t.tm_mon  = month - 1;
  t.tm_mday = days;

  // Check if time is supplied
  if (*c == '\0')
  {
    t.tm_hour = 0;
    t.tm_sec  = 0;
    t.tm_min  = 0;
  }
  else if (*c == 'T')
  {
    // Time of day part
    c++;

    if (std::sscanf(c, "%2d%2d", &hours, &minutes) == 2)
    {
      c += 4;
    }
    else if (std::sscanf(c, "%2d:%2d", &hours, &minutes) == 2)
    {
      c += 5;
    }
    else
    {
      throw std::runtime_error(std::string("Invalid date format: ") + value);
    }

    if (*c == ':')
    {
      c++;
    }

    if (*c != '\0')
    {
      if (std::sscanf(c, "%2d", &seconds) == 1)
      {
        c += 2;
      }
      else
      {
        throw std::runtime_error(std::string("Invalid date format: ") + value);
      }
    }

    t.tm_hour = hours;
    t.tm_min  = minutes;
    t.tm_sec  = seconds;
  }
  else
  {
    throw std::runtime_error(std::string("Invalid date format: ") + value);
  }

  // Drop microsecond information
  if (*c == '.')
  {
    c++;

    while (std::isdigit(*c) && *c != '\0')
    {
      c++;
    }
  }

  // Parse time zone information
  int tz_offset = 0;

  if (*c == 'Z')
  {
    c++;
  }

  if (*c != '\0')
  {
    int tz_direction = 0;

    if (*c == '+')
    {
      tz_direction = 1;
    }
    else if (*c == '-')
    {
      tz_direction = -1;
    }
    else
    {
      throw std::runtime_error(std::string("Invalid date format: ") + value);
    }

    // Offset part
    int tz_hours   = 0;
    int tz_minutes = 0;

    c++;

    if (std::sscanf(c, "%2d", &tz_hours) == 1)
    {
      c += 2;
    }
    else
    {
      throw std::runtime_error(std::string("Invalid date format: ") + value);
    }

    if (*c == ':')
    {
      c++;
    }

    if (*c != '\0')
    {
      if (std::sscanf(c, "%2d", &tz_minutes) == 1)
      {
        c += 2;
      }
      else
      {
        throw std::runtime_error(std::string("Invalid date format: ") + value);
      }
    }

    tz_offset = tz_direction * (tz_hours * 3600 + tz_minutes * 60);
  }

  // Determine DST automatically
  t.tm_isdst = -1;

  // Correct for local time zone
  std::time_t t_local = std::mktime(&t);
  std::tm tm_utc(*std::gmtime(&t_local));
  std::time_t t_utc = std::mktime(&tm_utc);
  tz_offset += (t_utc - t_local);

  return std::mktime(&t) - tz_offset;
}

#endif // RESTFUL_MAPPER_ISO8601_H_20130314

