/*
 * File:   charToInt.c
 * Author: Erik Sarkinen
 *
 * Created on September 21, 2021, 7:13 PM
 */
#include "main.h"
//d3 = +/-, d2 is the hundredths place, d1 is the tenths place, and d0 is ones place
int charToInt(char d3, char d2, char d1, char d0)
{
    int integer = 0, negative = 0;
    switch(d3) {
        case '+' :
          break;
        case '-' :
          negative = 1;
          break;
        default : 
          integer = 0;
          break;
    }
    switch(d2) {
        case '0' :
          integer += 0;
          break;
        case '1' :
          integer += 100;
          break;
        case '2' :
          integer += 200;
          break;
        case '3' :
          integer += 300;
          break;
        case '4' :
          integer += 400;
          break;
        case '5' :
          integer += 500;
          break;
        case '6' :
          integer += 600;
          break;
        case '7' :
          integer += 700;
          break;
        case '8' :
          integer += 800;
          break;
        case '9' :
          integer += 900;
          break;
        default : 
          integer += 0;
          break;
    }
    switch(d1) {
        case '0' :
          integer += 0;
          break;
        case '1' :
          integer += 10;
          break;
        case '2' :
          integer += 20;
          break;
        case '3' :
          integer += 30;
          break;
        case '4' :
          integer += 40;
          break;
        case '5' :
          integer += 50;
          break;
        case '6' :
          integer += 60;
          break;
        case '7' :
          integer += 70;
          break;
        case '8' :
          integer += 80;
          break;
        case '9' :
          integer += 90;
          break;
        default : 
          integer += 0;
          break;
    }
    switch(d0) {
        case '0' :
          integer += 0;
          break;
        case '1' :
          integer += 1;
          break;
        case '2' :
          integer += 2;
          break;
        case '3' :
          integer += 3;
          break;
        case '4' :
          integer += 4;
          break;
        case '5' :
          integer += 5;
          break;
        case '6' :
          integer += 6;
          break;
        case '7' :
          integer += 7;
          break;
        case '8' :
          integer += 8;
          break;
        case '9' :
          integer += 9;
          break;
        default : 
          integer += 0;
          break;
    }
    if(negative)
    {
        integer *= -1;
        negative = 0;
    }
    return(integer);
}

