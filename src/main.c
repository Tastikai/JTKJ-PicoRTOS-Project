#include <stdio.h>
#include <string.h>
#include <pico/stdlib.h>
#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include "tkjhat/sdk.h"


#define STACKSIZE 1024
#define LOPETUS '#'

//globaalit muuttujat
static TaskHandle_t sensor_task_handle = NULL;
static QueueHandle_t merkki_jono = NULL;


//prototyypit
static void sensor_task(void *arg);
static void button_task(void * arg);
static void print_task(void *arg);


int main(){

    stdio_init_all();
    init_hat_sdk();
    init_ICM42670();
    ICM42670_start_with_default_values();
    sleep_ms(100);
    init_sw1();
    init_sw2();



    BaseType_t result = xTaskCreate(
        sensor_task,
        "LUE_DATA",
        STACKSIZE,
        NULL,
        2,
        &sensor_task_handle
    );

    if(result != pdPASS){
        printf("Taskia ei voitu luoda\n");
        return 1;
    }

    printf("valiaikatietoja\n");

    vTaskStartScheduler();

    printf("Jokin meni pieleen\n");
    return 1;

}

/*ohjeet tulostamiseen: USB-piuha ylöspäin, näytön adapteri itseään päin. 
Näin saadaan seuraavia tuloksia, vaaka: ax=-1.00 ay=0.00 az=0.06
.................................pysty: ax=0.00 ay=-1.00 az=0.01

Ja jos loogisesti ajattelisi, niin laitetaan vaaka VIIVAKSI ja pysty PISTEEKSI

AZ arvoon voidaan laittaa toleranssiksi < 0.1 ja ax sekä ay vaikka +- 0.05*/

static void sensor_task(void *arg){         //luetaan sensoridata
    (void)arg;


    float ax, ay, az;                       //Nämä olivat aluksi globaaleja, mutta mmuutettu paikallisiksi. Globaalit kahvat suorittavat
    float gx, gy, gz;                       //kommunikoinnin taskien välillä
    float t;    

    while(1){
        
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); //odotetaan, että nappia painetaan
        vTaskDelay(pdMS_TO_TICKS(200));          // pieni odotus 
        int r = ICM42670_read_sensor_data(&ax, &ay, &az, &gx, &gy, &gz, &t); //tämä sen takia, jotta tarkasminen onnistuu helposti if-lauseella

        if(r < 0){
            printf("Lukeminen epäonnistui");
            continue;
        }


        //printf("ax=%.2f ay=%.2f az=%.2f\n", ax, ay, az);        //tarkistusta varten, poistetaan lopullisesta koodista      
        //printf("onnistui\n");
        char c = 0;
        if(ax < 1.05f && ax > 0.95f && ay > -0.05f && ay < 0.05f && az > -0.1f && az < 0.1f){                   //Tämä taski tulostaa pisteitä tai viivoja riippuen anturin asennosta
            c = "-";                                                                                       //sama taski siis kerää datan sekä antaa tulosteen. Toisella taskilla muodostetaan viesti joka lähetetään           
        } else if (ay > -1.05f && ay < -0.95f && ax > -0.05f && ax < 0.05f && az > -0.1f && az < 0.1f) {
            c = ".";
        }

        if(c != 0){
            xQueueSend(merkki_jono, &c, 0);
        }else{
            printf("Ei tunnistettu: ax= %.2f, ay= %.2f, az= %.2f", ax, ay, az);
        }

        vTaskDelay(pdMS_TO_TICKS(5000));

    }
}

static void button_task(void * arg){
    (void)arg;

    bool sw1_ed = false;
    bool sw2_ed = false;

    while(1){
        bool sw1_pressed = gpio_get(SW1_PIN);
        bool sw2_pressed = gpio_get(SW2_PIN);

        if(sw1_pressed && !sw1_ed){
            xTaskNotifyGive(sensor_task_handle);
        }
        if(sw2_pressed && !sw2_ed){
            char c = LOPETUS;
            xQueueSend(merkki_jono, &c, 0);
        }

        vTaskDelay(pdMS_TO_TICKS(30));
    }
}

static void print_task(void *arg){
    (void)arg;
    
    while(1){

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}