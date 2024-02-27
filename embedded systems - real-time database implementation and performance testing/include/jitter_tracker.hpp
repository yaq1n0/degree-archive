#ifndef jitter_tracker_hpp
#define jitter_tracker_hpp

#include "time_helpers.hpp"

#include <cmath>
#include <iostream>
#include <cfloat>

class JitterTracker
{
public:
    struct jitter_stats_t
    {
        double d_sample;
        double total_n;
        double total_error_min;
        double total_error_max;
        double total_error_mean;
        double total_error_stddev;
        double sample_n;
        double sample_error_min;
        double sample_error_max;
        double sample_error_mean;
        double sample_error_stddev;
    };

    JitterTracker(double d_sample=0)
    {
        Reset(d_sample);
    }

    void Reset(double d_sample_new)
    {
        m_d_sample=d_sample_new;
        m_total_error_n=0;
        m_total_error_min=DBL_MAX;
        m_total_error_max=-DBL_MAX;
        m_total_error_sum = 0;
        m_total_error_sum_square = 0;
        m_sample_error_n=0;
        m_sample_error_min=DBL_MAX;
        m_sample_error_max=-DBL_MAX;
        m_sample_error_sum = 0;
        m_sample_error_sum_square = 0;
    }

    void OnExecute(double tNow, double tTarget)
    {
        double error=tNow-tTarget;

        //std::cerr<<"  gap="<<gap<<", error="<<error<<"\n";
        
        m_total_error_n += 1;
        m_total_error_min = std::min(m_total_error_min, error);
        m_total_error_max = std::max(m_total_error_max, error);
        m_total_error_sum += error;
        m_total_error_sum_square += error*error;

        m_sample_error_n += 1;
        m_sample_error_min = std::min(m_sample_error_min, error);
        m_sample_error_max = std::max(m_sample_error_max, error);
        m_sample_error_sum += error;
        m_sample_error_sum_square += error*error;
    }

    jitter_stats_t CollectStats()
    {
        jitter_stats_t res;
        res.d_sample=m_d_sample;
        res.total_n=m_total_error_n;
        res.total_error_min=m_total_error_min;
        res.total_error_max=m_total_error_max;
        res.total_error_mean=m_total_error_sum/m_total_error_n;
        res.total_error_stddev=sqrt( m_total_error_sum_square/m_total_error_n - res.total_error_mean*res.total_error_mean  );

        res.sample_n=m_sample_error_n;
        res.sample_error_min=m_sample_error_min;
        res.sample_error_max=m_sample_error_max;
        res.sample_error_mean=m_sample_error_sum/m_sample_error_n;
        res.sample_error_stddev=sqrt( m_sample_error_sum_square/m_sample_error_n - res.sample_error_mean*res.sample_error_mean  );

        m_sample_error_n=0;
        m_sample_error_min=DBL_MAX;
        m_sample_error_max=-DBL_MAX;
        m_sample_error_sum=0;
        m_sample_error_sum_square=0;
        return res;
    }

    static std::ostream &print_header(std::ostream &dst)
    {
        return dst<<"d_sample,error_n,error_min,error_mean,error_max,error_stddev";
    }

private:
    double m_d_sample;

    double m_total_error_n;
    double m_total_error_min;
    double m_total_error_max;
    double m_total_error_sum;
    double m_total_error_sum_square;

    double m_sample_error_n;
    double m_sample_error_min;
    double m_sample_error_max;
    double m_sample_error_sum;
    double m_sample_error_sum_square;
};

std::ostream &operator<<(std::ostream &dst, const JitterTracker::jitter_stats_t &stats)
{
    dst<<stats.d_sample<<", ";
    //dst<<stats.total_n<<", "<<stats.total_error_min<<", "<<stats.total_error_mean<<", "<<stats.total_error_max<<", "<<stats.total_error_stddev;
    dst<<stats.sample_n<<", "<<stats.sample_error_min<<", "<<stats.sample_error_mean<<", "<<stats.sample_error_max<<", "<<stats.sample_error_stddev;
    return dst;
}

#endif
