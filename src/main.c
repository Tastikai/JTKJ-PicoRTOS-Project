#include <stdio.h>
#include <string.h>
#include <pico/stdlib.h>
#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include "tkjhat/sdk.h"


#define STACKSIZE 512
//globaalit muuttujat
float ax, ay, az;
char buf[32];

//prototyypit
static void sensor_task(void *arg);


int main(){

    stdio_init_all();
    init_hat_sdk();
    init_display();
    init_ICM42670();
    init_ICM42670();
    ICM42670_start_with_default_values();
    sleep_ms(100);
    init_sw1();


    TaskHandle_t myTaskHandle = NULL;

    BaseType_t result = xTaskCreate(
        sensor_task,
        "LUE_DATA",
        STACKSIZE,
        NULL,
        2,
        &myTaskHandle
    );

    if(result != pdPASS){
        printf("Taskia ei voitu luoda");
        return 1;
    }

    printf("valiaikatietoja");

    vTaskStartScheduler();

    printf("Jokin meni pieleen");
    return 1;

}

/*ohjeet tulostamiseen: USB-piuha ylöspäin, näytön adapteri itseään päin. 
Näin saadaan seuraavia tuloksia, vaaka: ax=-1.00 ay=0.00 az=0.06
.................................pysty: ax=0.00 ay=-1.00 az=0.01

Ja jos loogisesti ajattelisi, niin laitetaan vaaka VIIVAKSI ja pysty PISTEEKSI

AZ arvoon voidaan laittaa toleranssiksi < 0.1 ja ax sekä ay vaikka +- 0.05*/

static void sensor_task(void *arg){         //luetaan sensoridata
    (void)arg;



    float gx, gy, gz;
    float t;    

    while(1){
        ICM42670_read_sensor_data(
        &ax, &ay, &az,
        &gx, &gy, &gz,
        &t
    );

        if(ICM42670_read_sensor_data < 0){
            printf("Lukeminen epäonnistui");
        }

        //printf("ax=%.2f ay=%.2f az=%.2f\n", ax, ay, az);              
        //printf("onnistui\n");

        if(ax < 1.05f && ax > 0.95f && ay > -0.05f && ay < 0.05f && az > -0.1f && az < 0.1f){                   //Tämä taski tulostaa pisteitä tai viivoja riippuen anturin asennosta
            printf("-\n");                                                                                       //sa,a taski siis kerää datan sekä antaa tulosteen. Toisella taskilla muodostetaan viesti joka lähetetään           
        } else if (ay > -1.05f && ay < -0.95f && ax > -0.05f && ax < 0.05f && az > -0.1f && az < 0.1f) {
            printf(".\n");
        } 

        vTaskDelay(pdMS_TO_TICKS(5000));

    }
}

static void print_task(void *arg){
    (void)arg;
    
    while(1){

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}