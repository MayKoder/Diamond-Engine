using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections.Generic;

namespace DiamondEngine 
{
    public sealed class Counter
    {
        public enum CounterTypes
        {
            NONE,
            BOKATAN_RES,
            WRECKER_RES,
            ENEMY_BANTHA,
            ENEMY_STORMTROOP,
            MAX,
        }

        public int place;
        public int amount;
        public CounterTypes type;

        public Counter()
        {
            place = 0;
            amount = 0;
            type = CounterTypes.NONE;
        }
        public Counter(int _place, int _amount, CounterTypes ty)
        {
            place = _place;
            amount = _amount;
            type = ty;
        }

        public static Dictionary<CounterTypes, Counter> GameCounters = new Dictionary<CounterTypes, Counter>();
        public static void SumToCounterType(CounterTypes type)
        {
            if (GameCounters.ContainsKey(type))
            {
                GameCounters[type].amount += 1;
            }
            else 
            {
                Counter aux = new Counter(GameCounters.Count + 1, 1, type);
                GameCounters.Add(type, aux);
            }
        }

        public static void ResetCounters()
        {
            GameCounters.Clear();
        }
    }

}
