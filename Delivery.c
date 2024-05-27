#include <pthread.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>  
#include <time.h>
#include "constants.h" 

int available_phones=Ntel;
int available_cook = Ncook;
int available_oven = Noven;
int available_deliverer=Ndeliver;

pthread_mutex_t phone_mutex, mutexCook, mutexOven, mutexDeliverer; 
pthread_cond_t phone_cond, condCook, condOven, condDeliverer;

int profit=0;
int Sm=0;
int Sp=0;
int Ss=0;
int successful_orders=0;
int failed_orders=0;
int sum_total_time=0;
int max_time=0; 
int sum_cold=0;
int max_cold=0;

void* phone_operator(void *threadId) {
    int *tid = (int *)threadId;
    unsigned int seed = time(NULL) ^ *tid;
    //printf("%d\n", *tid);
    struct timespec start_time, start_time_cold, end_time_prepare, end_time_deliver;
    clock_gettime(CLOCK_REALTIME, &start_time);

    pthread_mutex_lock(&phone_mutex);

    while (available_phones == 0) {
        pthread_cond_wait(&phone_cond, &phone_mutex);
    }

    // At this point, this customer is next in line
    available_phones--;
    int order_pizzas = Norderlow + rand_r(&seed) % (Norderhigh - Norderlow + 1);

    pthread_mutex_unlock(&phone_mutex);
    
    int arr[order_pizzas];
    for (int i = 0; i < order_pizzas; i++) {
        float rand_prob = (float)rand_r(&seed) / RAND_MAX;
        if (rand_prob < Pm) {
            arr[i]=1;
            Sm++;
        } else if (rand_prob < Pm + Pp) {
            arr[i]=2;
            Sp++;
        } else {
            arr[i]=3;
            Ss++;
        }
    }

    int payment_time = Tpaymentlow + rand_r(&seed) % (Tpaymenthigh - Tpaymentlow + 1);
    sleep(payment_time);

    float payment_prob = (float)rand_r(&seed) / RAND_MAX;
    if (payment_prob < Pfail) {
        failed_orders++;
        printf("Η παραγγελία με αριθμό %d απέτυχε.\n", *tid);
        pthread_mutex_lock(&phone_mutex);
        available_phones++;
        pthread_cond_signal(&phone_cond);
        pthread_mutex_unlock(&phone_mutex);

        return NULL;
    } else {
        successful_orders++;
        printf("Η παραγγελία με αριθμό %d καταχωρήθηκε.\n", *tid);
        for (int i = 0; i < order_pizzas; i++) {
            if (arr[i]==1) {
                profit+=Cm;
            } else if (arr[i]==2) {
                profit+=Cp;
            } else {
                profit+=Cs;
            }
        }
    }


    pthread_mutex_lock(&mutexCook);
    while (available_cook == 0) {
        //printf("No Cook Available\n");
        pthread_cond_wait(&condCook, &mutexCook);
    }
    pthread_mutex_lock(&phone_mutex);
    available_phones++;
    pthread_cond_signal(&phone_cond);
    pthread_mutex_unlock(&phone_mutex);
    available_cook--;
    pthread_mutex_unlock(&mutexCook);

    sleep(Tprep * order_pizzas);
    //printf("Finished preparation for order %d \n", *tid);

    pthread_mutex_lock(&mutexOven);
    while (available_oven < order_pizzas) {
        //printf("Not enough ovens available\n");
        pthread_cond_wait(&condOven, &mutexOven);
    }
    available_oven -= order_pizzas;
    pthread_mutex_unlock(&mutexOven);

    sleep(Tbake); 

    //printf("Pizzas ready\n");
    clock_gettime(CLOCK_REALTIME, &start_time_cold);

    pthread_mutex_lock(&mutexOven);
    available_oven += order_pizzas;
    pthread_cond_signal(&condOven);
    pthread_mutex_unlock(&mutexOven);
    
    pthread_mutex_lock(&mutexDeliverer);
    while (available_deliverer == 0) {
        printf("No Deliverer Available\n");
        pthread_cond_wait(&condDeliverer, &mutexDeliverer);
    }
    available_deliverer--;
    pthread_mutex_unlock(&mutexDeliverer);

    pthread_mutex_lock(&mutexCook);
    available_cook++;
    pthread_cond_signal(&condCook);
    pthread_mutex_unlock(&mutexCook);

    clock_gettime(CLOCK_REALTIME, &end_time_prepare);
    int elapsed_time_prepare = (end_time_prepare.tv_sec - start_time.tv_sec) + 
                          (end_time_prepare.tv_nsec - start_time.tv_nsec) / 1e9;

    printf("Η παραγγελία με αριθμό %d ετοιμάστηκε σε %d λεπτά.\n", *tid, elapsed_time_prepare );
    
    sleep(Tpack*order_pizzas);

    int waiting=Tdellow + rand_r(&seed) % (Tdelhigh - Tdellow + 1);
    sleep(waiting);
    clock_gettime(CLOCK_REALTIME, &end_time_deliver);
    int elapsed_time_delivery = (end_time_deliver.tv_sec - start_time.tv_sec) + 
                      (end_time_deliver.tv_nsec - start_time.tv_nsec) / 1e9;
    printf("Η παραγγελία με αριθμό %d παραδόθηκε σε %d λεπτά.\n", *tid, elapsed_time_delivery );
    sum_total_time+=elapsed_time_delivery;
    if (max_time<elapsed_time_delivery) {
        max_time=elapsed_time_delivery;
    }

    int elapsed_time_cold = (end_time_deliver.tv_sec - start_time_cold.tv_sec) + 
                      (end_time_deliver.tv_nsec - start_time_cold.tv_nsec) / 1e9;
    sum_cold+=elapsed_time_cold;
    if (max_cold<elapsed_time_cold) {
        max_cold=elapsed_time_cold;
    }
    
    available_deliverer +=1;
    pthread_cond_signal(&condDeliverer);
    pthread_mutex_unlock(&mutexDeliverer);

    pthread_mutex_lock(&mutexOven);
    available_oven += order_pizzas;
    pthread_cond_signal(&condOven);
    pthread_mutex_unlock(&mutexOven);

    return NULL;
}

int main(int argc, char *argv[]) { 
  int Ncust = 50; 
  pthread_t threads[Ncust]; 
  int threadIds[Ncust];
  int threadTime[Ncust]; 

  unsigned int seed = time(NULL);
  pthread_mutex_init(&mutexCook, NULL);
  pthread_cond_init(&condCook, NULL);
  
  pthread_mutex_init(&mutexOven, NULL);
  pthread_cond_init(&condOven, NULL); 
  
  pthread_mutex_init(&phone_mutex, NULL);
  pthread_cond_init(&phone_cond, NULL);
    
   for (int i = 0; i < Ncust; i++) {
        threadIds[i] = i + 1;
        if (i == 0) {
            threadTime[i] = 0;
        } else {
            threadTime[i] = threadTime[i - 1] + Torderlow + rand_r(&seed) % (Torderhigh - Torderlow + 1);
        }

        //printf("Main: creating thread %d\n", threadIds[i]);
        pthread_create(&threads[i], NULL, phone_operator, &threadIds[i]);
        sleep(threadTime[i] - (i > 0 ? threadTime[i - 1] : 0));
   }
   // Join threads 
   for (int i = 0; i < Ncust; i++) { 
      pthread_join(threads[i], NULL); 
   } 
   
   pthread_mutex_destroy(&phone_mutex); 
   pthread_cond_destroy(&phone_cond); 
   pthread_mutex_destroy(&mutexCook);
   pthread_cond_destroy(&condCook);
   pthread_mutex_destroy(&mutexOven);
   pthread_cond_destroy(&condOven);

printf("Όλες οι παραγγελίες έχουν ολοκληρωθεί.\n");
    printf("Συνολικά έσοδα: %d\n", profit);
    printf("Συνολικές πίτσες Margarita: %d\n", Sm);
    printf("Συνολικές πίτσες Peperoni: %d\n", Sp);
    printf("Συνολικές πίτσες Special: %d\n", Ss);
    printf("Πλήθος πετυχημένων παραγγελειών: %d\n", successful_orders);
    printf("Πλήθος αποτυχημένων παραγγελειών: %d\n", failed_orders); 

    if (successful_orders>0) {
        double average_time= sum_total_time/successful_orders;
        printf("Ο μέσος χρόνος εξυπηρέτησης των πελατών είναι: %.2f \n", average_time); 
        printf("Ο μέγιστος χρόνος εξυπηρέτησης των πελατών είναι: %d\n", max_time);

        double average_cold_time= sum_cold/successful_orders;
        printf("Ο μέσος χρόνος κρυώματος της παραγγελιας είναι: %.2f \n", average_cold_time); 
        printf("Ο μέγιστος χρόνος κρυώματος της παραγγελιας είναι: %d\n", max_cold);
    }
return 0; 
}  
