/*
 * main.cpp
 * Hello World for Nintendo DS consoles
 * Created by Nicola Lamacchia, 2020
 */
#include <nds.h>
#include <stdio.h>

int main()
{
    consoleDemoInit();
    printf("\x1b[12;10HHello World");
    for (;;)
        ;
    return 0;
}
