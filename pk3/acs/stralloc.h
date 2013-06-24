global int 22:strings[];
global int 23:stringIndex[];

function int addString(int string)
{
    int strSize = StrLen(string) + 1;  // gotta remember the null byte at the end

    int index = 0; int ret; int i = 0; int j; int c;
    
    int wIndex = findFreeIndex(strSize);

    for (i = 0; i < strSize; i++)
    {
        j = i + wIndex;

        strings[j] = GetChar(string, i);
    }

    strings[j + i] = 0;
    
    addIndex(wIndex);
    return wIndex;
}

function int addCleanString(int string)
{
    int strSize = StrLen(string) + 1;

    int index = 0; int i = 0; int j = 0; int k = 0; int c; int ignoreNext;
    
    int wIndex = findFreeIndex(strSize);

    for (i = 0; i < strSize; i++)
    {
        c = GetChar(string, i);

        if ( ( ((c > 8) && (c < 14)) || ((c > 31) && (c < 127)) || ((c > 160) && (c < 173)) ) && !ignoreNext)
        {
            j = (k++) + wIndex;
            strings[j] = c;
        }
        else if (c == 28 && !ignoreNext) { ignoreNext = 1; }
        else { ignoreNext = 0; }
    }

    strings[j + 1] = 0;
    
    addIndex(wIndex);
    return wIndex;
}

function int addLowerString(int string)
{
    int strSize = StrLen(string) + 1;

    int index = 0; int i = 0; int j = 0; int k = 0; int c; int ignoreNext;
    
    int wIndex = findFreeIndex(strSize);

    for (i = 0; i < strSize; i++)
    {
        c = GetChar(string, i);

        if ( ( ((c > 8) && (c < 14)) || ((c > 31) && (c < 127)) || ((c > 160) && (c < 173)) ) && !ignoreNext)
        {
            j = (k++) + wIndex;

            if ((c > 64) && (c < 91))
            {
                strings[j] = c + 32;
            }
            else
            {
                strings[j] = c;
            }
        }
        else if (c == 28 && !ignoreNext)
        {
            ignoreNext = 1;
        }
        else
        {
            ignoreNext = 0;
        }
    }

    strings[j + 1] = 0;
    
    addIndex(wIndex);
    return wIndex;
}

function int getString(int index)
{
    int ret = "";
    int i = 0; int j; int c;
    
    if (getIndex(index) == -1)
    {
        return StrParam(s:"ERR");
    }

    while (1)
    {
        j = (i++) + index;
        c = strings[j];
        if (c == 0) { break; }

        ret = StrParam(s:ret, c:c);
    }
    
    return ret;
}

function void freeString(int index)
{
    int i = 0; int j; int c;
    
    if (getIndex(index) == -1)
    {
        return;
    }

    while (1)
    {
        j = (i++) + index;
        c = strings[j];

        if (c == 0) { break; }

        strings[j] = 0;
    }

    freeIndex(index);
}

function int stringLength(int index)
{
    int i = 0; int j; int c;
    
    if (getIndex(index) == -1) { return -1; }

    while (1)
    {
        j = (i++) + index;
        c = strings[j];
        if (c == 0) { break; }
    }
    
    return i;
}

function int reallocString(int index, int string)
{
    freeString(index);
    return addString(string);
}

function int reallocCleanString(int index, int string)
{
    freeString(index);
    return addCleanString(string);
}

function int reallocLowerString(int index, int string)
{
    freeString(index);
    return addLowerString(string);
}



function int findFreeIndex(int size)
{
    int c; int i = -1; int index = 0; int ret;

    while (1)
    {
        c = strings[index];
        
        if (c == 0)
        {
            if (i == 0) { ret = index; }
            i++;
        }
        else { i = -1; }

        if (i >= (size - 1)) { break; }

        index++;
    }

    return ret;
}

function int addIndex(int index)
{
    int i = 0;
    while (1)
    {
        if (stringIndex[i] == (index + 1)) { break; }
        else if (stringIndex[i] == 0)
        {
            stringIndex[i] = (index + 1);
            break;
        }
        i++;
    }

    return i;
}

function int getIndex(int index)
{
    int i = 0;

    while (1)
    {
        if (stringIndex[i] == (index + 1)) { break; }
        else if (stringIndex[i] == 0) { return -1; }

        i++;
    }

    return i;
}

function void freeIndex(int index)
{
    int curIndex = getIndex(index);
    
    if (curIndex == -1)
    {
        return;
    }

    int i = curIndex + 1;
    int j = i;

    stringIndex[curIndex] = 0;

    while (1)
    {
        if (stringIndex[i] == 0)
        {
            break;
        }
        j = i;
        i++;
    }
    
    stringIndex[curIndex] = stringIndex[j];
    stringIndex[j] = 0;
}
