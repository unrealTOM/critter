//------------------------------------------------------------------------------------//
//                                                                                    //
//               _________        .__  __    __                                       //
//               \_   ___ \_______|__|/  |__/  |_  ___________                        //
//               /    \  \/\_  __ \  \   __\   __\/ __ \_  __ \                       //
//               \     \____|  | \/  ||  |  |  | \  ___/|  | \/                       //
//                \______  /|__|  |__||__|  |__|  \___  >__|                          //
//                       \/                           \/                              //
//                                                                                    //
//    Critter is provided under the MIT License(MIT)                                  //
//    Critter uses portions of other open source software.                            //
//    Please review the LICENSE file for further details.                             //
//                                                                                    //
//    Copyright(c) 2015 Matt Davidson                                                 //
//                                                                                    //
//    Permission is hereby granted, free of charge, to any person obtaining a copy    //
//    of this software and associated documentation files(the "Software"), to deal    //
//    in the Software without restriction, including without limitation the rights    //
//    to use, copy, modify, merge, publish, distribute, sublicense, and / or sell     //
//    copies of the Software, and to permit persons to whom the Software is           //
//    furnished to do so, subject to the following conditions :                       //
//                                                                                    //
//    1. Redistributions of source code must retain the above copyright notice,       //
//    this list of conditions and the following disclaimer.                           //
//    2. Redistributions in binary form must reproduce the above copyright notice,    //
//    this list of conditions and the following disclaimer in the                     //
//    documentation and / or other materials provided with the distribution.          //
//    3. Neither the name of the copyright holder nor the names of its                //
//    contributors may be used to endorse or promote products derived                 //
//    from this software without specific prior written permission.                   //
//                                                                                    //
//    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      //
//    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        //
//    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE      //
//    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          //
//    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   //
//    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN       //
//    THE SOFTWARE.                                                                   //
//                                                                                    //
//------------------------------------------------------------------------------------//
/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2011 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#ifndef _Bitwise_H__
#define _Bitwise_H__

#include <CtrPlatform.h>

namespace Ctr
{
    /** \addtogroup Core
    *  @{
    */
    /** \addtogroup Math
    *  @{
    */

    /** Class for manipulating bit patterns.
    */
    class Bitwise {
    public:
        /** Returns the most significant bit set in a value.
        */
        static FORCEINLINE unsigned int mostSignificantBitSet(unsigned int value)
        {
            unsigned int result = 0;
            while (value != 0) {
                ++result;
                value >>= 1;
            }
            return result-1;
        }
        /** Returns the closest power-of-two number greater or equal to value.
            @note 0 and 1 are powers of two, so 
                firstPO2From(0)==0 and firstPO2From(1)==1.
        */
        static FORCEINLINE uint32_t firstPO2From(uint32_t n)
        {
            --n;            
            n |= n >> 16;
            n |= n >> 8;
            n |= n >> 4;
            n |= n >> 2;
            n |= n >> 1;
            ++n;
            return n;
        }
        /** Determines whether the number is power-of-two or not.
            @note 0 and 1 are tread as power of two.
        */
        template<typename T>
        static FORCEINLINE bool isPO2(T n)
        {
            return (n & (n-1)) == 0;
        }
        /** Returns the number of bits a pattern must be shifted right by to
            remove right-hand zeros.
        */
        template<typename T>
        static FORCEINLINE unsigned int getBitShift(T mask)
        {
            if (mask == 0)
                return 0;

            unsigned int result = 0;
            while ((mask & 1) == 0) {
                ++result;
                mask >>= 1;
            }
            return result;
        }

        /** Takes a value with a given src bit mask, and produces another
            value with a desired bit mask.
            @remarks
                This routine is useful for colour conversion.
        */
        template<typename SrcT, typename DestT>
        static inline DestT convertBitPattern(SrcT srcValue, SrcT srcBitMask, DestT destBitMask)
        {
            // Mask off irrelevant source value bits (if any)
            srcValue = srcValue & srcBitMask;

            // Shift source down to bottom of DWORD
            const unsigned int srcBitShift = getBitShift(srcBitMask);
            srcValue >>= srcBitShift;

            // Get max value possible in source from srcMask
            const SrcT srcMax = srcBitMask >> srcBitShift;

            // Get max available in dest
            const unsigned int destBitShift = getBitShift(destBitMask);
            const DestT destMax = destBitMask >> destBitShift;

            // Scale source value into destination, and shift back
            DestT destValue = (srcValue * destMax) / srcMax;
            return (destValue << destBitShift);
        }

        /**
         * Convert N bit colour channel value to P bits. It fills P bits with the
         * bit pattern repeated. (this is /((1<<n)-1) in fixed point)
         */
        static inline unsigned int fixedToFixed(uint32_t value, unsigned int n, unsigned int p) 
        {
            if(n > p) 
            {
                // Less bits required than available; this is easy
                value >>= n-p;
            } 
            else if(n < p)
            {
                // More bits required than are there, do the fill
                // Use old fashioned division, probably better than a loop
                if(value == 0)
                        value = 0;
                else if(value == (static_cast<unsigned int>(1)<<n)-1)
                        value = (1<<p)-1;
                else    value = value*(1<<p)/((1<<n)-1);
            }
            return value;    
        }

        /**
         * Convert floating point colour channel value between 0.0 and 1.0 (otherwise clamped) 
         * to integer of a certain number of bits. Works for any value of bits between 0 and 31.
         */
        static inline unsigned int floatToFixed(const float value, const unsigned int bits)
        {
            if(value <= 0.0f) return 0;
            else if (value >= 1.0f) return (1<<bits)-1;
            else return (unsigned int)(value * (1<<bits));     
        }

        /**
         * Fixed point to float
         */
        static inline float fixedToFloat(unsigned value, unsigned int bits)
        {
            return (float)value/(float)((1<<bits)-1);
        }

        /**
         * Write a n*8 bits integer value to memory in native endian.
         */
        static inline void intWrite(void *dest, const int n, const unsigned int value)
        {
            switch(n) {
                case 1:
                    ((uint8_t*)dest)[0] = (uint8_t)value;
                    break;
                case 2:
                    ((uint16_t*)dest)[0] = (uint16_t)value;
                    break;
                case 3:
                    ((uint8_t*)dest)[2] = (uint8_t)((value >> 16) & 0xFF);
                    ((uint8_t*)dest)[1] = (uint8_t)((value >> 8) & 0xFF);
                    ((uint8_t*)dest)[0] = (uint8_t)(value & 0xFF);
                    break;
                case 4:
                    ((uint32_t*)dest)[0] = (uint32_t)value;                
                    break;                
            }        
        }
        /**
         * Read a n*8 bits integer value to memory in native endian.
         */
        static inline unsigned int intRead(const void *src, int n) {
            switch(n) {
                case 1:
                    return ((uint8_t*)src)[0];
                case 2:
                    return ((uint16_t*)src)[0];
                case 3:
                    return ((uint32_t)((uint8_t*)src)[0])|
                            ((uint32_t)((uint8_t*)src)[1]<<8)|
                            ((uint32_t)((uint8_t*)src)[2]<<16);
                case 4:
                    return ((uint32_t*)src)[0];
            } 
            return 0; // ?
        }

        /** Convert a float32 to a float16 (NV_half_float)
             Courtesy of OpenEXR
        */
        static inline uint16_t floatToHalf(float i)
        {
            union { float f; uint32_t i; } v;
            v.f = i;
            return floatToHalfI(v.i);
        }
        /** Converts float in uint32_t format to a a half in uint16_t format
        */
        static inline uint16_t floatToHalfI(uint32_t i)
        {
            register int s =  (i >> 16) & 0x00008000;
            register int e = ((i >> 23) & 0x000000ff) - (127 - 15);
            register int m =   i        & 0x007fffff;
        
            if (e <= 0)
            {
                if (e < -10)
                {
                    return 0;
                }
                m = (m | 0x00800000) >> (1 - e);
        
                return static_cast<uint16_t>(s | (m >> 13));
            }
            else if (e == 0xff - (127 - 15))
            {
                if (m == 0) // Inf
                {
                    return static_cast<uint16_t>(s | 0x7c00);
                } 
                else    // NAN
                {
                    m >>= 13;
                    return static_cast<uint16_t>(s | 0x7c00 | m | (m == 0));
                }
            }
            else
            {
                if (e > 30) // Overflow
                {
                    return static_cast<uint16_t>(s | 0x7c00);
                }
        
                return static_cast<uint16_t>(s | (e << 10) | (m >> 13));
            }
        }
        
        /**
         * Convert a float16 (NV_half_float) to a float32
         * Courtesy of OpenEXR
         */
        static inline float halfToFloat(uint16_t y)
        {
            union { float f; uint32_t i; } v;
            v.i = halfToFloatI(y);
            return v.f;
        }
        /** Converts a half in uint16_t format to a float
             in uint32_t format
         */
        static inline uint32_t halfToFloatI(uint16_t y)
        {
            register int s = (y >> 15) & 0x00000001;
            register int e = (y >> 10) & 0x0000001f;
            register int m =  y        & 0x000003ff;
        
            if (e == 0)
            {
                if (m == 0) // Plus or minus zero
                {
                    return s << 31;
                }
                else // Denormalized number -- renormalize it
                {
                    while (!(m & 0x00000400))
                    {
                        m <<= 1;
                        e -=  1;
                    }
        
                    e += 1;
                    m &= ~0x00000400;
                }
            }
            else if (e == 31)
            {
                if (m == 0) // Inf
                {
                    return (s << 31) | 0x7f800000;
                }
                else // NaN
                {
                    return (s << 31) | 0x7f800000 | (m << 13);
                }
            }
        
            e = e + (127 - 15);
            m = m << 13;
        
            return (s << 31) | (e << 23) | m;
        }
         

    };
    /** @} */
    /** @} */

}

#endif
