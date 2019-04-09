#pragma once

#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <array>

#include "crde_exception.h"

namespace crde
{
    namespace utils
    {
        /**
         * @brief The credits_tc struct
         * It is a default structure to represent a classic timecode
         */
        struct credits_tc
        {
            /** Frame per seconds **/
            int fps;

            /**< Numbers of the first image of the credits **/
            std::vector<int> starts;

            /**< Numbers of the last image of the credits **/
            std::vector<int> ends;

            /** Name of the videos **/
            std::vector<std::string> video_names;
        };

        template <typename T>
        /**
         * @brief more_less
         * Simple function for recreating a moreless operator with a threshold
         * @param v1
         * Value to compare number 1
         * @param v2
         * Value to com
         * @param threshold
         * @return
         */
        inline bool more_less(const T v1, const T v2, const T threshold)
        {
            if((v1 <= v2 + threshold)
                    && (v1 >= v2 - threshold))
                return true;   

            return false;
        }

        template <typename T>
        void exponential_smoothing(std::vector<T>* datas, const double alpha)
        {
            std::size_t data_size = datas->size();
            T* data_data = datas->data();

            T old_value = *datas->begin();

            for(std::size_t i=0; i<data_size; i++)
            {
                data_data[i] = alpha * data_data[i] + (1-alpha) * old_value;
                old_value=data_data[i];
            }
        }

        template <typename T>
        void apply_threshold(std::vector<T>* datas, const T threshold)
        {
            std::size_t data_size = datas->size();
            T* data_data = datas->data();

            for(std::size_t i=1; i<data_size; i++)
            {
                if(more_less(data_data[i],data_data[i-1],threshold))
                    data_data[i]=data_data[i-1];
            }
        }

        template <typename T>
        void denoise(std::vector<T>* datas,
                     const std::size_t noise_max_size,
                     const T threshold)
        {
            std::size_t data_size = datas->size();
            T* data_data = datas->data();

            for(std::size_t i=0; i<data_size-noise_max_size+1; i++)
            {
//                std::size_t count = 0;
//                while(!more_less(data_data[i], data_data[i+count+1],threshold))
//                {
//                    count++;
//                    if(i+count+1 >= data_size)
//                        break;
//                }

//                if(count<=noise_max_size)
//                {
//                    for(std::size_t j=0; j<count; j++)
//                        data_data[i+1+j] = data_data[i];
//                }

//                count = 0;
            }
        }

        /**
         * @brief operator <<
         * Print credits_tc properly
         * @param os
         * Buffer for printing purpose
         * @param timecodes
         * Object to print
         * @return
         * Buffer modified
         */
        std::ostream& operator<<(std::ostream& os,
                                 const utils::credits_tc& timecodes);
    }
}
