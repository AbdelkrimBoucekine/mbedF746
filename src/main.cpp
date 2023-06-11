#include <mbed.h>
#include "lvgl.h"
#include <threadLvgl.h>
#include "AnalogIn.h"
#include <cstdio>


ThreadLvgl threadLvgl(30);
AnalogIn turbiditySensor(A0); // Capteur de turbidité connecté à la broche A0
lv_obj_t *chart;
lv_chart_series_t *ser1;

void update_turbidity_value() {
    while (1) {
        // Lire la valeur du capteur de turbidité
        float turbidityValue = turbiditySensor.read();

        // Convertir la valeur en plage de 0 à 100
        int turbidity = turbidityValue * 100;

        // Ajouter la valeur au graphique
        lv_chart_set_next_value(chart, ser1, turbidity);

        ThisThread::sleep_for(500ms); // Attendre un court instant avant la prochaine lecture
    }
}

int main() {
    threadLvgl.lock();

    // Initialiser LVGL
    lv_init();

    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_bg_color(&style, lv_palette_main(LV_PALETTE_GREY));

    lv_obj_t *obj = lv_obj_create(lv_scr_act());
    lv_obj_add_style(obj, &style, 0);

    lv_obj_set_size(obj, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL)); 

    // Créer un graphique
    chart = lv_chart_create(lv_scr_act());
    lv_obj_set_size(chart, 200, 150);
    lv_obj_align(chart, LV_ALIGN_CENTER, 0, 0);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);

    // Ajouter une série de données au graphique
    ser1 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);

    // Ajouter les axes X et Y avec des numéros
    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_X, 10, 5, 6, 5, true, 30);
    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, 10, 5, 6, 5, true, 30);
    
 
    /* Activer le scrolling dans le graphique
    lv_chart_set_drag(chart, true);
    lv_chart_set_drag_dir(chart, LV_DIR_HOR | LV_DIR_VER); */

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
            printf("%d L'eau est légèrement sale\n", turbidity);
        } else if (turbidity > 75 && turbidity <= 100) {
            printf("%d L'eau est propre\n", turbidity);
        }

        ThisThread::sleep_for(2s); // Attendre quelques secondes avant la prochaine lecture
    }
}
