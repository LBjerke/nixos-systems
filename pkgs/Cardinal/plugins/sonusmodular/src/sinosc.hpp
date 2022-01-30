/******************************************************************************
 * Copyright 2017-2019 Valerio Orlandini / Sonus Dept. <sonusdept@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/


#ifndef SINOSC_HPP_
#define SINOSC_HPP_

#include <cmath>

template <class fp_sample_t>
class SinOsc
{
    public:
    SinOsc(fp_sample_t sample_rate = 44100.0)
    {
        set_sample_rate(sample_rate);

        ramp_ = 0.0;
    }

    bool set_sample_rate(const fp_sample_t sample_rate)
    {
        if (sample_rate > 0.0)
        {
            inv_sample_rate_ = 1.0 / sample_rate;
            half_sample_rate_ = 0.5 * sample_rate;

            return set_frequency(frequency_);
        }

        return false;
    }

    inline bool set_frequency(const fp_sample_t &frequency)
    {
        if (frequency > 0.0)
        {
            frequency_ = frequency;
            step_ = frequency_ * inv_sample_rate_;

            return true;
        }
        
        return false;
    }

    void reset(const fp_sample_t value = 0.0)
    {
        ramp_ = value;
    }

    inline fp_sample_t run()
    {
        ramp_ += step_;

        while (ramp_ > 1.0)
        {
            ramp_ -= 1.0;
        }

        return sinf(ramp_ * 2.0 * M_PI);
    }

    private:
    fp_sample_t ramp_;
    fp_sample_t step_;
    fp_sample_t half_sample_rate_;
    fp_sample_t inv_sample_rate_;
    fp_sample_t frequency_;
};

#endif // SINOSC_HPP_
