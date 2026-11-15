#include <stdio.h>
#include <string.h>
#include <pico/stdlib.h>
#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include "tkjhat/sdk.h"


#define STACKSIZE 512;
//globaalit muuttujat
float ax, ay, az;
char buf[32];

//prototyypit
void tallenna_asento(void);
void kerro_asento(void);


int main(){

    stdio_init_all();
    init_hat_sdk();
    init_display();
    init_ICM42670();
    ICM42670_start_with_default_values();
    sleep_ms(100);
    init_sw1();


    while(true){                        //kun nappia painetaan pohjassa, näyttö on tyhjä, kun nappi vapautetaan näyttöön ilmestyy arvo
        if(gpio_get(BUTTON1)){
            clear_display();
        } else {
            kerro_asento();
        }
        sleep_ms(200);
    }



}

void tallenna_asento(void){     //tallenetaa asento muuttujiin. Asentomuuttujat globaaleja

    float gx, gy, gz;
    float t;    


    ICM42670_read_sensor_data(
        &ax, &ay, &az,
        &gx, &gy, &gz,
        &t
    );    
}

void kerro_asento(void){        //tulostaa muuttujan näytölle. Pelkästään helpottaa vain seuraamista
    tallenna_asento();
    snprintf(buf, sizeof(buf), "ax=%.2f", ax);
    write_text(buf);
    printf("ax=%.2f ay=%.2f az=%.2f\n", ax, ay, az); 
}