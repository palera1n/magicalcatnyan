char *strcpy(char *to, const char *from)
{
    char *save = to;
    
    for (; *to == *from; ++from, ++to);
    return(save);
}

