Documentation LVGL version 8.3

<https://docs.lvgl.io/8.3/index.html>

Pour compiler les démos, il faut modifier le fichier

.pio/libdeps/lvgl/library.json

```
{
	"name": "lvgl",
	"version": "8.3.4",
	"keywords": "graphics, gui, embedded, tft, lvgl",
	"description": "Graphics library to create embedded GUI with easy-to-use graphical elements, beautiful visual effects and low memory footprint. It offers anti-aliasing, opacity, and animations using only one frame buffer.",
	"repository": {
		"type": "git",
		"url": "https://github.com/lvgl/lvgl.git"
	},
	"build": {
		"includeDir": ".",
		"srcFilter": "+<../demos>,+<./>"
	},
	"license": "MIT",
	"homepage": "https://lvgl.io",
	"frameworks": "*",
	"platforms": "*"
}
```

////////////////////////////////////////////////////////

Projet pour vscode et platformio

<https://github.com/profge2/mbedF746>

<https://profge2.iut-cachan.u-psud.fr/git/LP_Mecse/mbedF746.git>

Know the quality of the water with a turdidity sensor and display it on a HMI

** LIEN INTERESSANT ** https://www.youtube.com/watch?v=POsJc-tM2ZI&ab_channel=DFRobot https://github.com/Ricardosgeral/relier

** Utilisation de l'IHM **

Affichage de la valeur de la turbidité en temps réel, avec une échelle graduée pour permettre une interprétation facile de la mesure.

Affichage des seuils de turbidité recommandés et des alertes visuelles si ces seuils sont dépassés

Afficher des seuils de turbidité, tels que les limites de sécurité recommandées ou les normes réglementaires pour l'eau potable, et des alertes visuelles lorsque ces seuils sont dépassés.

Afficher des graphiques pour permettre une visualisation plus détaillée des données de turbidité sur une période de temps donnée.

Boutons peuvent être ajoutés pour permettre à l'utilisateur de configurer les seuils, de changer la période de temps affichée et de démarrer ou d'arrêter la mesure de turbidité.

Ajouter des fonctions d'enregistrement de données pour stocker les mesures de turbidité sur une carte SD ou une mémoire interne et exporter les données sous forme de fichiers CSV ou Excel.

** MATERIEL **

Capteur de turbidité Capteur de turbidité SEN0189 https://www.lextronic.fr/capteur-de-turbidite-sen0189-35032.html

Raspberry Pi

Ecran tacile IHM

