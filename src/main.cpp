#include <mbed.h>
#include "lvgl.h"
#include <threadLvgl.h>
#include "AnalogIn.h"
#include <cstdio>
#include "DigitalOut.h"

ThreadLvgl threadLvgl(30);
AnalogIn turbiditySensor(A0); // Capteur de turbidité connecté à la broche A0
lv_obj_t *chart;
lv_chart_series_t *ser1;
bool isPaused = false;
lv_obj_t *label;
lv_obj_t *label_state;
lv_obj_t *bar; // Nouvel objet pour la barre
lv_obj_t *led1;
lv_obj_t *led2;

void update_turbidity_value() {
    while (1) {
        if (!isPaused) {
            // Lire la valeur du capteur de turbidité
            float turbidityValue = turbiditySensor.read();

            // Convertir la valeur en plage de 0 à 100
            int turbidity = turbidityValue * 100;

            // Ajouter la valeur au graphique
            lv_chart_set_next_value(chart, ser1, turbidity);

            // Mettre à jour le texte avec la valeur du graphique
            char value_str[10];
            snprintf(value_str, sizeof(value_str), "%d", turbidity);
            lv_label_set_text(label, value_str);

            // Mettre à jour l'état en fonction de la valeur du capteur
            const char *state;
            if (turbidity >= 0 && turbidity <= 25) {
                state = "L'eau est très sale";
            } else if (turbidity > 25 && turbidity <= 50) {
                state = "L'eau est sale";
            } else if (turbidity > 50 && turbidity <= 75) {
                state = "L'eau est légèrement sale";
            } else if (turbidity > 75 && turbidity <= 100) {
                state = "L'eau est propre";
            } else {
                state = "État inconnu";
            }

            lv_label_set_text(label_state, state);

            // Mettre à jour la valeur de la barre
            lv_bar_set_value(bar, turbidity, LV_ANIM_ON);

            // Allumer ou éteindre la LED2 en fonction de la valeur de turbidité
            if (turbidity > 50) {
                lv_led_on(led2);
            } else {
                lv_led_off(led2);
            }

            // Allumer ou éteindre la LED1 en fonction de la valeur de turbidité
            if (turbidity < 50) {
                lv_led_on(led1);
            } else {
                lv_led_off(led1);
            }

        }
        ThisThread::sleep_for(500ms); // Attendre un court instant avant la prochaine lecture
    }
}

static void btn_pause_event_cb(lv_event_t *e) {
    if (e->code == LV_EVENT_CLICKED) {
        isPaused = true;
    }
}

static void btn_play_event_cb(lv_event_t *e) {
    if (e->code == LV_EVENT_CLICKED) {
        isPaused = false;
    }
}

void toggle_leds() {
    lv_led_toggle(led1);
    lv_led_toggle(led2);
}


int main() {
    threadLvgl.lock();

    // Initialiser LVGL
    lv_init();

    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_bg_color(&style, lv_palette_main(LV_PALETTE_BLUE_GREY));

    lv_obj_t *obj = lv_obj_create(lv_scr_act());
    lv_obj_add_style(obj, &style, 0);

    lv_obj_set_size(obj, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));

    // Créer un graphique
    chart = lv_chart_create(lv_scr_act());
    lv_obj_set_size(chart, 200, 150);
    lv_obj_align(chart, LV_ALIGN_CENTER, 0, -25);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);

    // Ajouter une série de données au graphique
    ser1 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);

    // Ajouter les axes X et Y avec des numéros
    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, 10, 5, 6, 5, true, 30);

    // Créer le bouton Pause
    lv_obj_t *btn_pause = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn_pause, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_align(btn_pause, LV_ALIGN_LEFT_MID, 0, 50);
    lv_obj_add_event_cb(btn_pause, btn_pause_event_cb, LV_EVENT_CLICKED, NULL);

    // Créer le label pour le bouton Pause
    lv_obj_t *label_pause = lv_label_create(btn_pause);
    lv_label_set_text(label_pause, "Pause");

    // Créer le bouton Relancer
    lv_obj_t *btn_play = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn_play, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_align(btn_play, LV_ALIGN_LEFT_MID, 0, -50);
    lv_obj_add_event_cb(btn_play, btn_play_event_cb, LV_EVENT_CLICKED, NULL);

    // Créer le label pour le bouton Relancer
    lv_obj_t *label_play = lv_label_create(btn_play);
    lv_label_set_text(label_play, "Relancer");

    // Créer le texte pour afficher la valeur du graphique
    label = lv_label_create(lv_scr_act());
    lv_obj_align(label, LV_ALIGN_CENTER, 50, 75);
    lv_label_set_text(label, "");

    // Créer le texte pour afficher "taux de turbidité"
    lv_obj_t *label_title = lv_label_create(lv_scr_act());
    lv_label_set_text(label_title, "Taux de turbidité: ");
    lv_obj_align(label_title, LV_ALIGN_CENTER, -30, 75);

    // Créer le texte pour afficher l'état du liquide
    label_state = lv_label_create(lv_scr_act());
    lv_obj_align(label_state, LV_ALIGN_CENTER, 0, 100);
    lv_label_set_text(label_state, "");

    // Créer la barre
    bar = lv_bar_create(lv_scr_act());
    lv_obj_set_size(bar, 20, 200);
    lv_obj_center(bar);
    lv_bar_set_range(bar, 0, 100); // Définir la plage de la barre
    lv_obj_align(bar, LV_ALIGN_CENTER, 115, 0);

    // Créer les LEDs
    led1 = lv_led_create(lv_scr_act());
    lv_obj_align(led1, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
    lv_led_off(led1);
    lv_led_set_color(led1, lv_palette_main(LV_PALETTE_RED));


    led2 = lv_led_create(lv_scr_act());
    lv_obj_align(led2, LV_ALIGN_BOTTOM_RIGHT, -50, -10);
    lv_led_off(led2);
    lv_led_set_color(led2, lv_palette_main(LV_PALETTE_BLUE));


    threadLvgl.unlock();

    printf("Démarrage du programme...\n");
    Thread updateThread;
    updateThread.start(update_turbidity_value);

     // Créer un fragment vide pour afficher une page noire au lancement
    lv_obj_t *fragment = lv_obj_create(lv_scr_act());
    lv_obj_set_size(fragment, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
    lv_obj_set_style_bg_opa(fragment, LV_OPA_COVER, 0);

    // Créer le label "Bienvenue"
    lv_obj_t *label_welcome = lv_label_create(fragment);
    lv_label_set_text(label_welcome, "Bienvenue");

    // Créer un style pour la police du texte
    static lv_style_t style_welcome;
    lv_style_init(&style_welcome);
    lv_obj_add_style(label_welcome, &style_welcome, 0);

    lv_style_set_text_font(&style_welcome, &lv_font_montserrat_22);  // Set a larger font

    lv_obj_align(label_welcome, LV_ALIGN_CENTER, 0, 0);

    // Afficher la page noire pendant 5 secondes
    ThisThread::sleep_for(5s);

    // Supprimer le fragment vide
    lv_obj_del(fragment);   

    // Créer une boucle d'événements LVGL
    while (1) {
        ThisThread::sleep_for(10ms);
    }
}