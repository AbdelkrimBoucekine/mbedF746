#include <mbed.h>
#include "lvgl.h"
#include <threadLvgl.h>
#include "AnalogIn.h"
#include "DigitalOut.h"
#include <ctime>

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
Ticker ticker;

std::string get_current_time() {
    std::time_t now = std::time(nullptr);
    std::tm* local_time = std::localtime(&now);

    char time_str[9];
    std::strftime(time_str, sizeof(time_str), "%H:%M:%S", local_time);

    return std::string(time_str);
}

void update_time() {
    // Mettre à jour le temps ici
}

void update_turbidity_value() {
        const char* state = "État inconnu"; // Valeur par défaut
    while (1) {
        if (!isPaused) {
            // Lire la valeur du capteur de turbidité
            float turbidityValue = turbiditySensor.read();

            // Inverser la valeur en soustrayant la valeur lue de 1
            float invertedValue = 1 - turbidityValue;

            // Convertir la valeur en plage de 0 à 100
            int turbidity = invertedValue * 100;

            // Ajouter la valeur au graphique
            lv_chart_set_next_value(chart, ser1, turbidity);

            // Mettre à jour le texte avec la valeur du graphique
            char value_str[10];
            snprintf(value_str, sizeof(value_str), "%d", turbidity);
            lv_label_set_text(label, value_str);

            // Mettre à jour l'état en fonction de la valeur du capteur
            if (turbidity >= 0 && turbidity <= 25) {
                state = "L'eau est propre";
            } else if (turbidity > 25 && turbidity <= 50) {
                state = "L'eau est legerement propre";
            } else if (turbidity > 50 && turbidity <= 75) {
                state = "L'eau est legerement sale";
            } else if (turbidity > 75 && turbidity <= 100) {
                state = "L'eau est tres sale";
            }

            lv_label_set_text(label_state, state);

            // Mettre à jour la valeur de la barre
            lv_bar_set_value(bar, turbidity, LV_ANIM_ON);

            // Allumer ou éteindre la LED2 en fonction de la valeur de turbidité
            if (turbidity > 50) {
                lv_led_on(led1);
            } else {
                lv_led_off(led1);
            }

            // Allumer ou éteindre la LED1 en fonction de la valeur de turbidité
            if (turbidity < 50) {
                lv_led_on(led2);
            } else {
                lv_led_off(led2);
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

void reset_chart() {
    lv_chart_hide_series(chart, ser1, true); // Réinitialiser la série du graphique en supprimant toutes les données
    lv_chart_refresh(chart); // Rafraîchir le graphique
}

// Définition de la fonction event_cb
static void event_cb(lv_event_t *e) {
    // Traitement des événements du message box
}

// Définir la fonction lv_example_msgbox_1()
void lv_example_msgbox_1(void) {
    // Créer un style personnalisé pour le message box
    static lv_style_t style_msgbox;
    lv_style_init(&style_msgbox);
    lv_style_set_bg_color(&style_msgbox, lv_color_make(0x00, 0x80, 0xC0)); // Couleur de fond
    lv_style_set_text_color(&style_msgbox, lv_color_make(0xFF, 0xFF, 0xFF)); // Couleur du texte

    // ...
    static const char * btns[] = {""};
    lv_obj_t * mbox1 = lv_msgbox_create(NULL, "Bienvenue dans le dispositif d'analyse de turbidite", "Quitter cette page pour commencer.", btns, true);
    lv_obj_add_event_cb(mbox1, event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_center(mbox1);
    lv_obj_set_size(mbox1, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));

    // Appliquer le style personnalisé au message box
    lv_obj_add_style(mbox1, &style_msgbox, 0);
}

int main() {
    threadLvgl.lock();

    // Initialiser LVGL
    lv_init();

    // Définition de deux styles pour les objets graphiques
    static lv_style_t style, style_indic;
    lv_style_init(&style);
    lv_style_set_bg_color(&style, lv_palette_main(LV_PALETTE_BLUE_GREY)); // Définir la couleur de fond du style

    // Création d'un objet graphique
    lv_obj_t *obj = lv_obj_create(lv_scr_act()); // Créer un nouvel objet sur l'écran actuel
    lv_obj_add_style(obj, &style, 0); // Ajouter le style défini au nouvel objet

    // Définir la taille de l'objet en fonction de la résolution de l'écran
    lv_obj_set_size(obj, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));


    // Créer un graphique
    chart = lv_chart_create(lv_scr_act());
    lv_obj_set_size(chart, 200, 150);
    lv_obj_align(chart, LV_ALIGN_CENTER, 30, -30);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);

    // Ajouter une série de données au graphique
    ser1 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);

    // Ajouter les axes X et Y avec des numéros
    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, 10, 5, 6, 5, true, 30);

    // Créer le bouton Pause
    lv_obj_t *btn_pause = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn_pause, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_align(btn_pause, LV_ALIGN_BOTTOM_LEFT, 10, -5);
    lv_obj_add_event_cb(btn_pause, btn_pause_event_cb, LV_EVENT_CLICKED, NULL);

    // Créer le label pour le bouton Pause
    lv_obj_t *label_pause = lv_label_create(btn_pause);
    lv_label_set_text(label_pause, "Pause");

    // Créer le bouton Relancer
    lv_obj_t *btn_play = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn_play, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_align(btn_play, LV_ALIGN_BOTTOM_LEFT, 10, -50);
    lv_obj_add_event_cb(btn_play, btn_play_event_cb, LV_EVENT_CLICKED, NULL);

    // Créer le label pour le bouton Reprendre
    lv_obj_t *label_play = lv_label_create(btn_play);
    lv_label_set_text(label_play, "Reprendre");

    // Créer le texte pour afficher la valeur du graphique
    label = lv_label_create(lv_scr_act());
    lv_obj_align(label, LV_ALIGN_CENTER, 50, 75);
    lv_label_set_text(label, "");

    // Créer le texte pour afficher "taux de turbidité"
    lv_obj_t *label_title = lv_label_create(lv_scr_act());
    lv_label_set_text(label_title, "Taux de turbidite: ");
    lv_obj_align(label_title, LV_ALIGN_CENTER, -30, 75);

    // Créer le texte pour afficher l'état du liquide
    label_state = lv_label_create(lv_scr_act());
    lv_obj_align(label_state, LV_ALIGN_CENTER, 0, 100);
    lv_label_set_text(label_state, "");

    // Créer la barre
    bar = lv_bar_create(lv_scr_act());
    lv_obj_add_style(bar, &style_indic, LV_PART_INDICATOR);
    lv_obj_set_size(bar, 20, 200);
    lv_obj_center(bar);
    lv_bar_set_range(bar, 0, 100); // Définir la plage de la barre
    lv_obj_align(bar, LV_ALIGN_CENTER, 150, 0);

    lv_style_set_bg_opa(&style_indic, LV_OPA_COVER);
    lv_style_set_bg_color(&style_indic, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_bg_grad_color(&style_indic, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_bg_grad_dir(&style_indic, LV_GRAD_DIR_VER);

    // Créer les LEDs
    led1 = lv_led_create(lv_scr_act());
    lv_obj_align(led1, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
    lv_led_off(led1);
    lv_led_set_color(led1, lv_palette_main(LV_PALETTE_RED));

    led2 = lv_led_create(lv_scr_act());
    lv_obj_align(led2, LV_ALIGN_BOTTOM_RIGHT, -50, -10);
    lv_led_off(led2);
    lv_led_set_color(led2, lv_palette_main(LV_PALETTE_BLUE));

    lv_obj_t *btn_reset = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn_reset, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_align(btn_reset, LV_ALIGN_BOTTOM_LEFT, 10, -95);
    lv_obj_add_event_cb(btn_reset, [](lv_event_t *e) {
        if (e->code == LV_EVENT_CLICKED) {
            reset_chart(); // Utiliser la fonction reset_chart() pour réinitialiser le graphique
        }
    }, LV_EVENT_CLICKED, NULL);  

    lv_obj_t *label_reset = lv_label_create(btn_reset);
    lv_label_set_text(label_reset, "Reinitialiser");

    threadLvgl.unlock();           

    printf("Démarrage du programme...\n");
    Thread updateThread;
    updateThread.start(update_turbidity_value);

    // Créer le texte pour afficher l'heure
    lv_obj_t *label_time = lv_label_create(lv_scr_act());
    lv_obj_align(label_time, LV_ALIGN_TOP_RIGHT, -10, 10);
    lv_label_set_text(label_time, get_current_time().c_str());  

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

    // Appliquer le style au label "Bienvenue"
    lv_obj_add_style(label_welcome, &style_welcome, 0);

    // Centrer le label "Bienvenue" sur l'écran
    lv_obj_align(label_welcome, LV_ALIGN_CENTER, 0, 0);

    // Afficher le fragment pendant 3 secondes
    ThisThread::sleep_for(3000ms);

    // Supprimer le fragment après l'affichage
    lv_obj_del(fragment);

    // Appeler lv_example_msgbox_1() pour afficher la boîte de dialogue
    lv_example_msgbox_1();

    // Créer une fonction de mise à jour récurrente toutes les secondes
    ticker.attach(update_time, 1s);

    while (1) {
        threadLvgl.lock();
        lv_label_set_text(label_time, get_current_time().c_str());
        threadLvgl.unlock();
        ThisThread::sleep_for(100ms);
    }
}