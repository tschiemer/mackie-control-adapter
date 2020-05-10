#include <time_helpers.h>

#if __APPLE__
#include <CoreServices/CoreServices.h>
#include <mach/mach_time.h>
#elif __linux__
#include <sys/time.h>
#else
#error "Unknown compiler"
#endif


namespace RMETotalMixMidiAdapter {

#if __APPLE__

    // https://stackoverflow.com/questions/2165726/how-do-i-record-timestamps-in-a-mac-os-x-c-program
    // https://stackoverflow.com/questions/1450737/what-is-mach-absolute-time-based-on-on-iphone/4753909#4753909
    timestamp_t getMicrosecondTimestamp(){
      const int64_t kOneMillion = 1000; // * 1000
      static mach_timebase_info_data_t s_timebase_info;

      static dispatch_once_t onceToken;
      dispatch_once(&onceToken, ^{
          (void) mach_timebase_info(&s_timebase_info);
      });

      // mach_absolute_time() returns billionth of seconds,
      // so divide by one million to get milliseconds
      return (int)((mach_absolute_time() * s_timebase_info.numer) / (kOneMillion * s_timebase_info.denom));
    }

#elif __linux__

    timestamp_t getMicrosecondTimestamp(){
      struct timeval tp;
      gettimeofday(&tp, NULL);
      return tp.tv_sec * 1000000 + tp.tv_usec;
    }

#endif

}
