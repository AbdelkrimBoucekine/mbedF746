#include <mbed.h>
#include "lvgl.h"
#include <threadLvgl.h>
#include "AnalogIn.h"
#include <cstdio>


ThreadLvgl threadLvgl(30);
AnalogIn turbiditySensor(A0); // Capteur de turbidité connecté à la broche A0
lv_obj_t *label;
lv_disp_t* disp;


void update_turbidity_value() {
    while (1) {
        // Lire la valeur du capteur de turbidité
        float turbidityValue = turbiditySensor.read();

        // Convertir la valeur en plage de 0 à 100
        int turbidity = turbidityValue * 100;

        char buf[10];
        snprintf(buf, sizeof(buf), "%d", turbidity);

        lv_label_set_text(label, buf);

        ThisThread::sleep_for(100ms); // Attendre un court instant avant la prochaine lecture
    }
}

int main() {
    threadLvgl.lock();

    lv_init();

    // Initialiser le pilote d'affichage
    // Ici, vous devez ajouter le code spécifique à votre pilote d'affichage
    // Exemple : lvgl_driver_init();

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);

    // Ici, vous pouvez configurer des paramètres spécifiques à l'affichage, tels que la résolution, l'orientation, etc.
    // Exemple : disp_drv.hor_res = ...
    
    lv_disp_drv_register(&disp_drv);

lv_theme_t *th = lv_theme_default_init(disp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_CYAN), false, &lv_font_montserrat_14);
lv_disp_set_theme(disp, th); /*Assign the theme to the display*/

    lv_obj_t *scr = lv_obj_create(NULL);
    lv_scr_load(scr);

    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_text(label, "Valeur du capteur :");
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 10, 10);

    lv_obj_t *value_label = lv_label_create(scr);
    lv_label_set_text(value_label, "");
    lv_obj_align(value_label, LV_ALIGN_TOP_LEFT, 10, 40);

    threadLvgl.unlock();

    printf("Démarrage du programme...\n");

    Thread updateThread;
    updateThread.start(update_turbidity_value);

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
