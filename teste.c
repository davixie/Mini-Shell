#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){

    char palavra[50] = "/bin/cat test.txt | /bin/grep 123";
    char *pt;
    
    char token[] = " ";
    
    char special_carac_1[] = ">";
    char special_carac_2[] = "<";
    char special_carac_3[] = "|";

    pt = strtok(palavra, token);
    printf("\naqui temos token: %s\n", pt);
    // if(strcmp(pt, palavra) == 0){
    //     // temos <
    // }
    while(pt){
        printf("token: %s\n", pt);
        if(strcmp(pt, special_carac_1) == 0){
            printf("\n temos carcater especial 1 de %s\n", special_carac_1);
        } else if(strcmp(pt, special_carac_2) == 0){
            printf("\n temos carcater especial 2 de %s\n", special_carac_2);
        }else if(strcmp(pt, special_carac_3) == 0){
            printf("\n temos carcater especial 3 de %s\n", special_carac_3);
        }
        
        pt = strtok(NULL, token);
    }

    return 0;
}