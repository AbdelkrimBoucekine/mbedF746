#include <mbed.h>
#include <threadLvgl.h>
#include <AnalogIn.h>

ThreadLvgl threadLvgl(30);
AnalogIn turbiditySensor(A0); // Capteur de turbidité connecté à la broche A0

int main() {
    threadLvgl.lock();

    //lv_demo_widgets();

    threadLvgl.unlock();

    printf("Démarrage du programme...\n");

    while (1) {
        // Lire la valeur du capteur de turbidité
        float turbidityValue = turbiditySensor.read();

        // Convertir la valeur en plage de 0 à 100
        int turbidity = turbidityValue * 100;

        // Afficher l'état de l'eau en fonction de la valeur de turbidité
        if (turbidity >= 0 && turbidity <= 25) {
            printf("%d L'eau est très sale\n", turbidity);
        } else if (turbidity > 25 && turbidity <= 50) {
            printf("%d L'eau est sale\n", turbidity);
        } else if (turbidity > 50 && turbidity <= 75) {
            printf("%d L'eau est trouble\n", turbidity);
        } else if (turbidity > 75 && turbidity <= 100) {
            printf("%d L'eau est propre\n", turbidity);
        }

        ThisThread::sleep_for(100ms); //Attendre un court instant avant la prochaine lecture
    }
}