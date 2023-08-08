#include<stdio.h>
#include<stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include<pthread.h>

// Static Global Variables, not accessible from outside file
static bool ignitionSwitch = false;
static int coolantTemperature = 0; 

// Mutex's
pthread_mutex_t ignition_mutex;
pthread_mutex_t temperature_mutex;

// Macro's
#define MAX_SYSTEM_TEMPERATURE_CELSIUS  80
#define IGNITION_POLLING_PERIOD_SECONDS 1
#define TEMPERATURE_POLLING_PERIOD_SECONDS 1
#define CONTROL_LOOP_PERIOD_SECONDS 2

// Enum
enum ComponentState{
    OFF,
    ON
};

// Function Declarations
int getCoolantTemperature();
bool getIgnitionStatus();


/*----------------------------------------------------------------Thread Functions----------------------------------------------------------------*/
void *PollIgnition(void* rank) {
    for(;;) {
        // poll ignition status
        pthread_mutex_lock(&ignition_mutex);
        ignitionSwitch = getIgnitionStatus();
        pthread_mutex_unlock(&ignition_mutex);

        // achieve desired polling frequency
        sleep(IGNITION_POLLING_PERIOD_SECONDS); // **ASSUMES Linux implementation with seconds argument**

    }

    return NULL;
}


void *PollTemperature(void* rank) {
    for(;;) {
        // poll temperature sensor
        pthread_mutex_lock(&temperature_mutex);
        coolantTemperature = getCoolantTemperature();
        pthread_mutex_unlock(&temperature_mutex);

        // achieve desired polling frequency
        sleep(TEMPERATURE_POLLING_PERIOD_SECONDS); // **ASSUMES Linux implementation with seconds argument**

    }

    return NULL;
}


void *ControlCoolingSystem(void* rank) {
    int waterPump = (enum ComponentState) OFF;
    int radiatorFan = (enum ComponentState) OFF;

    for(;;) {
        pthread_mutex_lock(&temperature_mutex);
        pthread_mutex_lock(&ignition_mutex);

        if (ignitionSwitch) {
            waterPump = (enum ComponentState) ON;
            printf("Ignition is ON, Water pump is ON,  ");

            if (coolantTemperature >= MAX_SYSTEM_TEMPERATURE_CELSIUS) {
                radiatorFan = (enum ComponentState) ON;    
                printf("Temperature is %dC, Radiator fan is ON \n", coolantTemperature);

            } else {
                radiatorFan = (enum ComponentState) OFF;    
                printf("Temperature is %dC, Radiator fan is OFF \n", coolantTemperature);

            }

        } else {
            waterPump = (enum ComponentState) OFF;
            radiatorFan = (enum ComponentState) OFF;
            printf("Ignition is OFF, Water pump is OFF,  Temperature is %dC, Radiator fan is OFF \n", coolantTemperature);

        }

        pthread_mutex_unlock(&temperature_mutex);
        pthread_mutex_unlock(&ignition_mutex);

        /*----------------------------------------------------------------Temporary code for ending simulation----------------------------------------------------------------*/
        pthread_mutex_lock(&temperature_mutex);
        bool simulationFinished = false;

        if (coolantTemperature > 100) {
            printf("Temperature has reached critical levels, end simulation");
            simulationFinished = true;

        }

        pthread_mutex_unlock(&temperature_mutex);
        if (simulationFinished) {break;}
        /*----------------------------------------------------------------Temporary code for ending simulation----------------------------------------------------------------*/

        // achieve desired polling frequency
        sleep(CONTROL_LOOP_PERIOD_SECONDS); // **ASSUMES Linux implementation with seconds argument**

    }

    return NULL;
}


/*----------------------------------------------------------------Polling Functions----------------------------------------------------------------*/
int getCoolantTemperature() {
    // "fake" implementation just to get a incrementing value for temperature for the simulation
    static int temp = 0;
    temp+=5;

    return temp;

}


bool getIgnitionStatus() {
    // "fake" implementation to get values for the simulation
    static int test_ignition = 0;

    if (test_ignition<10) {
        test_ignition++;
        return false;

    } else {
        return true;

    }

}


int main(int argc, char* argv[]) {
    long       thread;  /* Use long in case of a 64-bit system */
    pthread_t* thread_handles; 
    int num_threads = 3;
    
    // Allocate dynamic memory
    thread_handles = malloc(num_threads*sizeof(pthread_t)); 

    // Initialize Mutex's
    pthread_mutex_init(&ignition_mutex, NULL);
    pthread_mutex_init(&temperature_mutex, NULL);

    // Create threads
    pthread_create(&thread_handles[0], NULL, PollIgnition, (void*) thread);  
    pthread_create(&thread_handles[1], NULL, PollTemperature, (void*) thread);  
    pthread_create(&thread_handles[2], NULL, ControlCoolingSystem, (void*) thread);  

    // Finalize threads
    pthread_join(thread_handles[0], NULL); 
    pthread_join(thread_handles[1], NULL); 
    pthread_join(thread_handles[2], NULL); 

    // Destroy Mutex's
    pthread_mutex_destroy(&ignition_mutex);
    pthread_mutex_destroy(&temperature_mutex);

    // Free dynamic memory
    free(thread_handles);

    return 0;

}

