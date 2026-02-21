#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/TestRegistry.h"

int main(int ac, char **av)
{
    return CommandLineTestRunner::RunAllTests(ac, av);
}