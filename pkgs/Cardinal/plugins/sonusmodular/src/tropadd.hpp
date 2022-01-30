/******************************************************************************
 * Copyright 2017-2019 Valerio Orlandini / Sonus Dept. <sonusdept@gmail.com>
 * Algorithm trop(x[n]) copyright 2018 Cristiano Bocci and Giorgio Sancristoforo
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


#ifndef TROPADD_H_
#define TROPADD_H_

#include <algorithm>
#include <iostream>

#include "sinosc.hpp"

enum HarmonicModes
{
    ODD,
    EVEN,
    FULL,
    MODES
};

template <class fp_sample_t>
class TropAdd
{
public:
    TropAdd(const fp_sample_t &sample_rate = 44100.0)
    {
        if (sample_rate < 0.0)
        {
            sample_rate_ = 44100.0;

            std::cerr << "TropAdd::sample_rate_ must be positive. Set to 44100.0\n";
        }

        for (int o = 0; o < 5; o++)
        {
            oscillators_[o].set_frequency(sample_rate);
            trop_vca_[o] = 0.0;
        }

        sample_rate_ = sample_rate;
        dc_offset_ = 0.0;
        mode_ = FULL;
        trop_beat_ = {0.0, 0.0, 0.0, 0.0, 0.0};

        set_frequency(440.0, {0.0, 0.0, 0.0, 0.0, 0.0});
    }

    bool set_frequency(const fp_sample_t &frequency, const std::vector<float> &trop_beat)
    {
        if (frequency > 0.0)
        {
            bool result = true;
            fp_sample_t freq = 0.0;

            frequency_ = frequency;
            trop_beat_ = trop_beat;

            int mult = 1;
            int add = 0;

            if (mode_ != FULL)
            {
                mult = 2;
            }

            if (mode_ != EVEN)
            {
                add = 1;
            }

            freq = frequency + trop_beat_[0];
            result &= oscillators_[0].set_frequency(freq);

            for (int o = 1; o < 5; o++)
            {
                freq = ((frequency) * (fp_sample_t)((o * mult) + add)) + trop_beat_[o];
                result &= oscillators_[o].set_frequency(freq);
            }

            return result;
        }

        return false;
    }

    bool set_mode(const HarmonicModes &mode)
    {
        if (mode == mode_)
        {
            return true;
        }

        if (mode < MODES)
        {
            mode_ = mode;
            return set_frequency(frequency_, trop_beat_);
        }

        return false;
    }

    void set_offset(fp_sample_t offset)
    {
        dc_offset_ = offset;
    }

    fp_sample_t get_frequency()
    {
        return frequency_;
    }

    bool set_vca(const fp_sample_t &value, const int &osc)
    {
        if ((value >= -1.0 && value <= 1.0) && (osc >= 0 && osc <= 4))
        {
            trop_vca_[osc] = value;

            return true;
        }

        return false;
    }

    inline fp_sample_t run()
    {
        fp_sample_t osc_out[5];

        /* Run oscillators */
        for (int o = 0; o < 5; o++)
        {
            osc_out[o] = oscillators_[o].run() + trop_vca_[o];
        }

        /* Mixer */
        return std::min({osc_out[0], osc_out[1], osc_out[2], osc_out[3], osc_out[4]}) + dc_offset_;
    }

private:
    fp_sample_t sample_rate_;
    fp_sample_t frequency_;
    SinOsc<fp_sample_t> oscillators_[5];
    fp_sample_t trop_vca_[5];
    std::vector<float> trop_beat_;
    fp_sample_t dc_offset_;
    HarmonicModes mode_;
};

#endif // TROPADD_H_
