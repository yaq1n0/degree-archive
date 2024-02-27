#include <cstdio>

int main(int argc, char *argv[])
{
    const char *msg="y";
    if(argc>1){
        msg=argv[1];
    }

    while(1){
        puts(msg);
    }
}