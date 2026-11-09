#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstring>
#include <iostream>

bool IsSpaceOrNo(char c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f';
}

bool IsLetterOrNumber(char c)
{
    if (c >= 'a' && c <= 'z')
    {
        return true;
    }
    if (c >= 'A' && c <= 'Z')
    {
        return true;
    }
    if (c >= '0' && c <= '9')
    {
        return true;
    }
    return false;
}

struct Options
{
    bool lines = false;
    bool words = false;
    bool alnum = false;
    bool bytes = false;
};

struct Stats
{
    long long lines = 0;
    long long words = 0;
    long long alnum = 0;
    long long bytes = 0;
};

bool IsOptions(char* arg)
{
    return arg[0] == '-' && arg[1] != '\0';
}

bool ParseLongOptions(char* arg, Options& options)
{
    if (strcmp(arg, "--lines") == 0)
    {
        options.lines = true;
    }
    else if (strcmp(arg, "--words") == 0)
    {
        options.words = true;
    }
    else if (strcmp(arg, "--alnum") == 0)
    {
        options.alnum = true;
    }
    else if (strcmp(arg, "--bytes") == 0)
    {
        options.bytes = true;
    }
    else
    {
        return false;
    }
    return true;
}

bool ParseShortOptions(char* arg, Options& options)
{
    for (int j = 1; arg[j] != '\0'; j++)
    {
        if (arg[j] == 'l')
        {
            options.lines = true;
        }
        else if (arg[j] == 'w')
        {
            options.words = true;
        }
        else if (arg[j] == 'a')
        {
            options.alnum = true;
        }
        else if (arg[j] == 'c')
        {
            options.bytes = true;
        }
        else
        {
            return false;
        }
    }
    return true;
}

bool ParseOptions(int argc, char** argv, Options& options)
{
    for (int i = 1; i < argc; i++)
    {
        char* arg = argv[i];
        if (!IsOptions(arg))
        {
            continue;
        }
        bool is_known = false;
        if (arg[1] == '-')
        {
            is_known = ParseLongOptions(arg, options);
        }
        else
        {
            is_known = ParseShortOptions(arg, options);
        }
        if (!is_known)
        {
            std::cerr << "Unknown option: " << arg << '\n';
            return false;
        }
    }
    if (!options.lines && !options.words && !options.alnum && !options.bytes)
    {
        options.lines = true;
        options.words = true;
        options.bytes = true;
    }
    return true;
}

void CountChar(char c, Stats& stats, bool& in_word)
{
    stats.bytes++;
    if (c == '\n')
    {
        stats.lines++;
    }
    if (IsLetterOrNumber(c))
    {
        stats.alnum++;
    }
    if (IsSpaceOrNo(c))
    {
        in_word = false;
    }
    else if (!in_word)
    {
        in_word = true;
        stats.words++;
    }
}

const int bufferSize = 2048;
bool ReadFile(const char* filename, Stats& stats)
{
    FILE* file = fopen(filename, "rb");
    if (file == nullptr)
    {
        std::cerr << "Could not open file: " << filename << '\n';
        return false;
    }
    char buffer[bufferSize];
    size_t readCount;
    bool in_word = false;
    char lastChar = '\0';
    while ((readCount = fread(buffer, sizeof(char), bufferSize, file)) > 0)
    {
        for (size_t i = 0; i < readCount; i++)
        {
            CountChar(buffer[i], stats, in_word);
        }
        lastChar = buffer[readCount - 1];
    }
    if (stats.bytes > 0 && lastChar != '\n')
    {
        stats.lines++;
    }
    fclose(file);
    return true;
}

void PrintStats(Stats& stats, Options& options, const char* filename)
{
    if (options.lines)
    {
        std::cout << stats.lines << ' ';
    }
    if (options.words)
    {
        std::cout << stats.words << ' ';
    }
    if (options.alnum)
    {
        std::cout << stats.alnum << ' ';
    }
    if (options.bytes)
    {
        std::cout << stats.bytes << ' ';
    }
    std::cout << filename << '\n';
}

int main(int argc, char** argv)
{
    Options options;
    int result = 0;
    if (!ParseOptions(argc, argv, options))
    {
        return 1;
    }
    for (int i = 1; i < argc; i++)
    {
        if (IsOptions(argv[i]))
        {
            continue;
        }
        Stats stats;
        if (!ReadFile(argv[i], stats))
        {
            return 1;
            continue;
        }
        PrintStats(stats, options, argv[i]);
    }

    return result;
}
