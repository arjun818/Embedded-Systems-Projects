#include <Arduino.h>


void BSP_display(String str){
    Serial.println(str);
}

void BSP_exit(void){
    Serial.println("Bye!!");
}