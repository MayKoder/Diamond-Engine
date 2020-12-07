#include "DERect.h"

bool DERect::Contains(DERect b)
{
    if ((b.x + b.w) <= (x + w)
        && (b.x) >= (x)
        && (b.y) >= (y)
        && (b.y + b.h) <= (y + h)
        )
    {
        return true;
    }
    else
    {
        return false;
    }
}
