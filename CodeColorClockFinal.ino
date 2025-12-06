#include <Adafruit_NeoPixel.h>
#include <IRremote.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "RTClib.h"
#include <WiFi.h>
#include <WebServer.h>

#define IR_RECEIVE_PIN 15
#define LED_PIN        33

// --- CONFIG LEDS ---
// Matrice centrale
#define MATRIX_LEDS   554      // nombre de LEDs de ta matrice
// Contour
#define CONTOUR_LEDS  78       // nombre de LEDs du contour
// Total
#define NUM_LEDS      (MATRIX_LEDS + CONTOUR_LEDS)

// Index de début du contour dans la bande complète
const int contourStartIndex = MATRIX_LEDS;

// Facteur "théorique" max pour le contour
const float CONTOUR_FACTOR       = 999.0f;
// Couleur de la LED d'heure en mode Ninja (bleu)
const int   NINJA_HOUR_R         = 0;
const int   NINJA_HOUR_G         = 77;
const int   NINJA_HOUR_B         = 153;
// Facteur de surbrillance pour la LED d'heure en mode Ninja (non utilisé direct)
const float NINJA_CONTOUR_FACTOR = 1.0f;

RTC_DS3231 rtc;

const char *ssid     = "Bbox-E0150BBA";
const char *password = "iE4xkMPhdb94PU626F";

WebServer server(80);

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Luminosité logique (en %)
int matrixBrightnessPercent  = 4;   // matrice : 4 % par défaut
int contourBrightnessPercent = 22;  // contour (LED heure) : 75 % du max par défaut

// --------- MATRICE 34 LIGNES ---------
const int nbRows = 34;
const int rowLengths[34] = {
    7, 9, 11, 13, 15, 15, 15,
    17, 17, 17,
    19, 19, 19,
    21, 21, 21, 21, 21, 21, 21, 21,
    19, 19, 19,
    17, 17, 17,
    15, 15, 15,
    13, 11, 9, 7
};

const int ledMatrix[34][34] = {
    {0, 1, 2, 3, 4, 5, 6},
    {7, 8, 9, 10, 11, 12, 13, 14, 15},
    {16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26},
    {27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39},
    {40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54},

    {55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69},
    {70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84},
    {85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101},
    {102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118},
    {119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135},

    {136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154},
    {155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173},
    {174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192},

    {193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213},
    {214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234},
    {235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255},
    {256, 257, 258, 259, 260, 261, 262, 263, 264, 265, 266, 267, 268, 269, 270, 271, 272, 273, 274, 275, 276},
    {277, 278, 279, 280, 281, 282, 283, 284, 285, 286, 287, 288, 289, 290, 291, 292, 293, 294, 295, 296, 297},
    {298, 299, 300, 301, 302, 303, 304, 305, 306, 307, 308, 309, 310, 311, 312, 313, 314, 315, 316, 317, 318},
    {319, 320, 321, 322, 323, 324, 325, 326, 327, 328, 329, 330, 331, 332, 333, 334, 335, 336, 337, 338, 339},
    {340, 341, 342, 343, 344, 345, 346, 347, 348, 349, 350, 351, 352, 353, 354, 355, 356, 357, 358, 359, 360},

    {361, 362, 363, 364, 365, 366, 367, 368, 369, 370, 371, 372, 373, 374, 375, 376, 377, 378, 379},
    {380, 381, 382, 383, 384, 385, 386, 387, 388, 389, 390, 391, 392, 393, 394, 395, 396, 397, 398},
    {399, 400, 401, 402, 403, 404, 405, 406, 407, 408, 409, 410, 411, 412, 413, 414, 415, 416, 417},

    {418, 419, 420, 421, 422, 423, 424, 425, 426, 427, 428, 429, 430, 431, 432, 433, 434},
    {435, 436, 437, 438, 439, 440, 441, 442, 443, 444, 445, 446, 447, 448, 449, 450, 451},
    {452, 453, 454, 455, 456, 457, 458, 459, 460, 461, 462, 463, 464, 465, 466, 467, 468},

    {469, 470, 471, 472, 473, 474, 475, 476, 477, 478, 479, 480, 481, 482, 483},
    {484, 485, 486, 487, 488, 489, 490, 491, 492, 493, 494, 495, 496, 497, 498},
    {499, 500, 501, 502, 503, 504, 505, 506, 507, 508, 509, 510, 511, 512, 513},

    {514, 515, 516, 517, 518, 519, 520, 521, 522, 523, 524, 525, 526},
    {527, 528, 529, 530, 531, 532, 533, 534, 535, 536, 537},
    {538, 539, 540, 541, 542, 543, 544, 545, 546},
    {547, 548, 549, 550, 551, 552, 553}
};

int IRState = 0;

const int playbackPin = 4; // GPIO relié au PL du module son (optionnel)

// === Compte à rebours ===
bool countdownActive = false;
unsigned long countdownStartMillis = 0;
unsigned long countdownDurationMillis = 0; // en millisecondes

// Flash final du compte à rebours
bool countdownFlashing = false;
unsigned long countdownFlashStartMillis = 0;
const unsigned long COUNTDOWN_FLASH_DURATION = 4000; // ms

// Animation hypnotique non bloquante
bool hypnoticActive = false;
unsigned long hypnoticStartMillis = 0;
const unsigned long HYPNOTIC_DURATION = 30000; // ms

// Couleurs heure / ninja / etc.
int r1 = 0;
int g1 = 77;
int b1 = 153;
int r2 = 217;
int g2 = 242;
int b2 = 0;

// Couleurs personnalisées pour "Change Color"
int customColor1[3] = {0, 77, 153};
int customColor2[3] = {217, 242, 0};

uint8_t RGB_array[24][2][3] = {
    {{217, 242, 0},   {0, 77, 153}},
    {{255, 81, 208},  {217, 242, 0}},
    {{0, 147, 47},    {255, 81, 208}},
    {{153, 153, 153}, {0, 147, 47}},
    {{255, 182, 0},   {153, 153, 153}},
    {{76, 0, 153},    {255, 182, 0}},
    {{242, 121, 0},   {76, 0, 153}},
    {{153, 153, 153}, {242, 121, 0}},
    {{0, 102, 26},    {153, 153, 153}},
    {{38, 0, 153},    {0, 102, 26}},
    {{178, 0, 178},   {38, 0, 153}},
    {{0, 77, 153},    {178, 0, 178}},
    {{248, 157, 255}, {0, 77, 153}},
    {{217, 242, 0},   {248, 157, 255}},
    {{0, 147, 47},    {217, 242, 0}},
    {{57, 166, 229},  {0, 147, 47}},
    {{255, 151, 35},  {57, 166, 229}},
    {{76, 0, 153},    {255, 151, 35}},
    {{242, 121, 0},   {76, 0, 153}},
    {{153, 153, 153}, {242, 121, 0}},
    {{0, 102, 26},    {153, 153, 153}},
    {{38, 0, 153},    {0, 102, 26}},
    {{86, 124, 158},  {38, 0, 153}},
    {{0, 77, 153},    {86, 124, 158}}
};


// Pour arc-en-ciel continu
int rainbowOffset = 0;

volatile bool stopAnimation = false;

// ====================================================================
// GESTION LUMINOSITÉ MATRICE
// ====================================================================
inline float getMatrixBrightnessFactor() {
    float f = (float)matrixBrightnessPercent / 100.0f;
    if (f < 0.02f) f = 0.02f;  // jamais totalement éteint
    if (f > 1.0f)  f = 1.0f;
    return f;
}

inline uint8_t applyMatrixBrightness(uint8_t c) {
    float f = getMatrixBrightnessFactor();
    int v = (int)(c * f + 0.5f);
    if (v < 0)   v = 0;
    if (v > 255) v = 255;
    return (uint8_t)v;
}

inline uint32_t makeMatrixColor(uint8_t r, uint8_t g, uint8_t b) {
    uint8_t rr = applyMatrixBrightness(r);
    uint8_t gg = applyMatrixBrightness(g);
    uint8_t bb = applyMatrixBrightness(b);
    return strip.Color(rr, gg, bb);
}

// Prototype parseur de couleur
bool parseHexColor(const String &hex, int &r, int &g, int &b);

// ====================================================================
// PAGE HTML
// ====================================================================
String getHtmlContent()
{
    String html = "<!DOCTYPE html>";
    html += "<html lang=\"fr\">";
    html += "<head>";
    html += "<meta charset=\"UTF-8\">";
    html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
    html += "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css\">";
    html += "<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.5.1/jquery.min.js\"></script>";
    html += "<script src=\"https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.16.0/umd/popper.min.js\"></script>";
    html += "<script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/js/bootstrap.min.js\"></script>";
    html += "<style>";
    html += "body { padding-bottom: 40px; }";
    html += ".pastel-btn { background-color: #AEC6CF; border-color: #AEC6CF; }";
    html += ".last-btn { background-color: #FFD1DC; border-color: #FFD1DC; }";
    html += ".palette-btn { border:none; padding:10px; border-radius:12px; width:100%; margin-bottom:10px; }";
    html += ".palette-colors { display:flex; height:30px; border-radius:8px; overflow:hidden; }";
    html += ".palette-color-block { flex:1; }";
    html += ".palette-label { font-size:0.9rem; margin-top:4px; }";
    html += "#customColorsSection { display:none; margin-top:10px; }";
    html += "</style>";
    html += "<script>";
    html += "var clickedButton = null;";  // IMPORTANT : var (et pas let) pour être global

    // Envoi normal du formulaire (pour les boutons Modes, luminosité, etc.)
    html += "function sendForm(event) {";
    html += "  event.preventDefault();";
    html += "  let form = event.target;";
    html += "  let formData = new FormData(form);";
    html += "  if (clickedButton) {";
    html += "    formData.append(clickedButton.name, clickedButton.value);";
    html += "    clickedButton = null;";
    html += "  }";
    html += "  fetch('/function', { method: 'POST', body: formData })";
    html += "    .then(response => response.text())";
    html += "    .then(data => console.log(data));";
    html += "}";

    // Sélection d'un thème + envoi immédiat en mode Change Color (function = 5)
    html += "function selectTheme(c1, c2){";
    html += "  document.getElementById('color1').value = c1;";
    html += "  document.getElementById('color2').value = c2;";
    html += "  let c1Input = document.getElementById('custom_color1');";
    html += "  let c2Input = document.getElementById('custom_color2');";
    html += "  if(c1Input) c1Input.value = c1;";
    html += "  if(c2Input) c2Input.value = c2;";
    html += "  let form = document.querySelector('form');";
    html += "  let formData = new FormData(form);";
    html += "  formData.append('function', '5');";
    html += "  fetch('/function', { method: 'POST', body: formData })";
    html += "    .then(response => response.text())";
    html += "    .then(data => console.log('Change Color direct:', data));";
    html += "}";

    html += "function updateFromCustom(){";
    html += "  let c1 = document.getElementById('custom_color1').value;";
    html += "  let c2 = document.getElementById('custom_color2').value;";
    html += "  document.getElementById('color1').value = c1;";
    html += "  document.getElementById('color2').value = c2;";
    html += "}";

    html += "function toggleCustomColors(){";
    html += "  let section = document.getElementById('customColorsSection');";
    html += "  if(section.style.display === 'none' || section.style.display === ''){";
    html += "    section.style.display = 'block';";
    html += "  } else {";
    html += "    section.style.display = 'none';";
    html += "  }";
    html += "}";
    html += "</script>";
    html += "</head>";
    html += "<body>";
    html += "<div class=\"container\">";
    html += "<h1 class=\"text-center mt-4 mb-3\">Magic Clock</h1>";

    html += "<form method=\"POST\" onsubmit=\"sendForm(event);\">";

    // ---------- MODES ----------
    html += "<hr>";
    html += "<h3 class=\"mb-3\">Modes</h3>";
    html += "<div class=\"row text-center\">";

    const int numButtons = 5;
    int funcValues[numButtons] = {1, 2, 4, 5, 10};
    String labels[numButtons] = {"Clock", "Animation", "Ninja", "Change Color", "Off"};

    for (int i = 0; i < numButtons; i++)
    {
        html += "<div class=\"col-6 col-md-4 my-2\">";
        String extraClass = (funcValues[i] == 10) ? " last-btn" : "";
        html += "<button class=\"btn pastel-btn" + extraClass + " w-100\" ";
        html += "name=\"function\" value=\"" + String(funcValues[i]) + "\" ";
        html += "onclick=\"clickedButton=this;\">";
        html += labels[i];
        html += "</button>";
        html += "</div>";
    }

    html += "</div>";

    // Hidden pour les couleurs (utilisés par Change Color)
    html += "<input type=\"hidden\" id=\"color1\" name=\"color1\" value=\"#004D99\">";
    html += "<input type=\"hidden\" id=\"color2\" name=\"color2\" value=\"#D9F200\">";

    // ---------- LUMINOSITÉS ----------
    html += "<hr>";
    html += "<h3 class=\"mb-3\">Luminosité</h3>";

    // Matrice
    html += "<div class=\"form-group\">";
    html += "  <label for=\"matrix_brightness\">Luminosité de la matrice :</label>";
    html += "  <input type=\"range\" class=\"custom-range\" id=\"matrix_brightness\" name=\"matrix_brightness\" ";
    html += "         min=\"1\" max=\"100\" value=\"" + String(matrixBrightnessPercent) + "\" ";
    html += "         oninput=\"document.getElementById('matrixBrightnessValue').innerText=this.value + '%';\">";
    html += "  <small>Valeur : <span id=\"matrixBrightnessValue\">" + String(matrixBrightnessPercent) + "%</span></small>";
    html += "</div>";

    // Contour
    html += "<div class=\"form-group mt-3\">";
    html += "  <label for=\"contour_brightness\">Luminosité de la LED du contour (heure) :</label>";
    html += "  <input type=\"range\" class=\"custom-range\" id=\"contour_brightness\" name=\"contour_brightness\" ";
    html += "         min=\"1\" max=\"100\" value=\"" + String(contourBrightnessPercent) + "\" ";
    html += "         oninput=\"document.getElementById('contourBrightnessValue').innerText=this.value + '%';\">";
    html += "  <small>Valeur : <span id=\"contourBrightnessValue\">" + String(contourBrightnessPercent) + "%</span></small>";
    html += "</div>";

    html += "<button type=\"submit\" class=\"btn btn-secondary w-100 mb-3\" ";
    html += "        name=\"function\" value=\"20\" onclick=\"clickedButton=this;\">";
    html += "Appliquer les réglages de luminosité";
    html += "</button>";

    // ---------- COMPTE À REBOURS ----------
    html += "<hr>";
    html += "<h3 class=\"mb-3\">Compte à rebours</h3>";
    html += "<div class=\"form-group\">";
    html += "<label for=\"cd_minutes\">Durée (minutes et secondes) :</label>";
    html += "<div class=\"form-row\">";
    html += "  <div class=\"col\">";
    html += "    <input type=\"number\" class=\"form-control\" id=\"cd_minutes\" name=\"cd_minutes\" min=\"0\" placeholder=\"Minutes\">";
    html += "  </div>";
    html += "  <div class=\"col\">";
    html += "    <input type=\"number\" class=\"form-control\" id=\"cd_seconds\" name=\"cd_seconds\" min=\"0\" max=\"59\" placeholder=\"Secondes\">";
    html += "  </div>";
    html += "</div>";
    html += "</div>";
    html += "<button type=\"submit\" class=\"btn btn-primary w-100 mb-4\" name=\"function\" value=\"12\" onclick=\"clickedButton=this;\">";
    html += "Lancer le compte à rebours";
    html += "</button>";


// ---------- COULEURS (24 thèmes) ----------
html += "<hr>";
html += "<h3 class=\"mb-3\">Couleurs (par heure)</h3>";
html += "<div class=\"row\">";

// H0 : {217,242,0} , {0,77,153}
html += "<div class=\"col-6\">";
html += "<button type=\"button\" class=\"palette-btn\" onclick=\"selectTheme('#D9F200','#004D99')\">";
html += "<div class=\"palette-colors\">";
html += "<div class=\"palette-color-block\" style=\"background:#D9F200;\"></div>";
html += "<div class=\"palette-color-block\" style=\"background:#004D99;\"></div>";
html += "</div><div class=\"palette-label text-center\">H0</div></button></div>";

// H1 : {255,81,208} , {217,242,0}
html += "<div class=\"col-6\">";
html += "<button type=\"button\" class=\"palette-btn\" onclick=\"selectTheme('#FF51D0','#D9F200')\">";
html += "<div class=\"palette-colors\">";
html += "<div class=\"palette-color-block\" style=\"background:#FF51D0;\"></div>";
html += "<div class=\"palette-color-block\" style=\"background:#D9F200;\"></div>";
html += "</div><div class=\"palette-label text-center\">H1</div></button></div>";

// H2 : {0,147,47} , {255,81,208}
html += "<div class=\"col-6\">";
html += "<button type=\"button\" class=\"palette-btn\" onclick=\"selectTheme('#00932F','#FF51D0')\">";
html += "<div class=\"palette-colors\">";
html += "<div class=\"palette-color-block\" style=\"background:#00932F;\"></div>";
html += "<div class=\"palette-color-block\" style=\"background:#FF51D0;\"></div>";
html += "</div><div class=\"palette-label text-center\">H2</div></button></div>";

// H3 : {153,153,153} , {0,147,47}
html += "<div class=\"col-6\">";
html += "<button type=\"button\" class=\"palette-btn\" onclick=\"selectTheme('#999999','#00932F')\">";
html += "<div class=\"palette-colors\">";
html += "<div class=\"palette-color-block\" style=\"background:#999999;\"></div>";
html += "<div class=\"palette-color-block\" style=\"background:#00932F;\"></div>";
html += "</div><div class=\"palette-label text-center\">H3</div></button></div>";

// H4 : {255,182,0} , {153,153,153}
html += "<div class=\"col-6\">";
html += "<button type=\"button\" class=\"palette-btn\" onclick=\"selectTheme('#FFB600','#999999')\">";
html += "<div class=\"palette-colors\">";
html += "<div class=\"palette-color-block\" style=\"background:#FFB600;\"></div>";
html += "<div class=\"palette-color-block\" style=\"background:#999999;\"></div>";
html += "</div><div class=\"palette-label text-center\">H4</div></button></div>";

// H5 : {76,0,153} , {255,182,0}
html += "<div class=\"col-6\">";
html += "<button type=\"button\" class=\"palette-btn\" onclick=\"selectTheme('#4C0099','#FFB600')\">";
html += "<div class=\"palette-colors\">";
html += "<div class=\"palette-color-block\" style=\"background:#4C0099;\"></div>";
html += "<div class=\"palette-color-block\" style=\"background:#FFB600;\"></div>";
html += "</div><div class=\"palette-label text-center\">H5</div></button></div>";

// H6 : {242,121,0} , {76,0,153}
html += "<div class=\"col-6\">";
html += "<button type=\"button\" class=\"palette-btn\" onclick=\"selectTheme('#F27900','#4C0099')\">";
html += "<div class=\"palette-colors\">";
html += "<div class=\"palette-color-block\" style=\"background:#F27900;\"></div>";
html += "<div class=\"palette-color-block\" style=\"background:#4C0099;\"></div>";
html += "</div><div class=\"palette-label text-center\">H6</div></button></div>";

// H7 : {153,153,153} , {242,121,0}
html += "<div class=\"col-6\">";
html += "<button type=\"button\" class=\"palette-btn\" onclick=\"selectTheme('#999999','#F27900')\">";
html += "<div class=\"palette-colors\">";
html += "<div class=\"palette-color-block\" style=\"background:#999999;\"></div>";
html += "<div class=\"palette-color-block\" style=\"background:#F27900;\"></div>";
html += "</div><div class=\"palette-label text-center\">H7</div></button></div>";

// H8 : {0,102,26} , {153,153,153}
html += "<div class=\"col-6\">";
html += "<button type=\"button\" class=\"palette-btn\" onclick=\"selectTheme('#00661A','#999999')\">";
html += "<div class=\"palette-colors\">";
html += "<div class=\"palette-color-block\" style=\"background:#00661A;\"></div>";
html += "<div class=\"palette-color-block\" style=\"background:#999999;\"></div>";
html += "</div><div class=\"palette-label text-center\">H8</div></button></div>";

// H9 : {38,0,153} , {0,102,26}
html += "<div class=\"col-6\">";
html += "<button type=\"button\" class=\"palette-btn\" onclick=\"selectTheme('#260099','#00661A')\">";
html += "<div class=\"palette-colors\">";
html += "<div class=\"palette-color-block\" style=\"background:#260099;\"></div>";
html += "<div class=\"palette-color-block\" style=\"background:#00661A;\"></div>";
html += "</div><div class=\"palette-label text-center\">H9</div></button></div>";

// H10 : {178,0,178} , {38,0,153}
html += "<div class=\"col-6\">";
html += "<button type=\"button\" class=\"palette-btn\" onclick=\"selectTheme('#B200B2','#260099')\">";
html += "<div class=\"palette-colors\">";
html += "<div class=\"palette-color-block\" style=\"background:#B200B2;\"></div>";
html += "<div class=\"palette-color-block\" style=\"background:#260099;\"></div>";
html += "</div><div class=\"palette-label text-center\">H10</div></button></div>";

// H11 : {0,77,153} , {178,0,178}
html += "<div class=\"col-6\">";
html += "<button type=\"button\" class=\"palette-btn\" onclick=\"selectTheme('#004D99','#B200B2')\">";
html += "<div class=\"palette-colors\">";
html += "<div class=\"palette-color-block\" style=\"background:#004D99;\"></div>";
html += "<div class=\"palette-color-block\" style=\"background:#B200B2;\"></div>";
html += "</div><div class=\"palette-label text-center\">H11</div></button></div>";

// H12 : {248,157,255} , {0,77,153}
html += "<div class=\"col-6\">";
html += "<button type=\"button\" class=\"palette-btn\" onclick=\"selectTheme('#F89DFF','#004D99')\">";
html += "<div class=\"palette-colors\">";
html += "<div class=\"palette-color-block\" style=\"background:#F89DFF;\"></div>";
html += "<div class=\"palette-color-block\" style=\"background:#004D99;\"></div>";
html += "</div><div class=\"palette-label text-center\">H12</div></button></div>";

// H13 : {217,242,0} , {248,157,255}
html += "<div class=\"col-6\">";
html += "<button type=\"button\" class=\"palette-btn\" onclick=\"selectTheme('#D9F200','#F89DFF')\">";
html += "<div class=\"palette-colors\">";
html += "<div class=\"palette-color-block\" style=\"background:#D9F200;\"></div>";
html += "<div class=\"palette-color-block\" style=\"background:#F89DFF;\"></div>";
html += "</div><div class=\"palette-label text-center\">H13</div></button></div>";

// H14 : {0,147,47} , {217,242,0}
html += "<div class=\"col-6\">";
html += "<button type=\"button\" class=\"palette-btn\" onclick=\"selectTheme('#00932F','#D9F200')\">";
html += "<div class=\"palette-colors\">";
html += "<div class=\"palette-color-block\" style=\"background:#00932F;\"></div>";
html += "<div class=\"palette-color-block\" style=\"background:#D9F200;\"></div>";
html += "</div><div class=\"palette-label text-center\">H14</div></button></div>";

// H15 : {57,166,229} , {0,147,47}
html += "<div class=\"col-6\">";
html += "<button type=\"button\" class=\"palette-btn\" onclick=\"selectTheme('#39A6E5','#00932F')\">";
html += "<div class=\"palette-colors\">";
html += "<div class=\"palette-color-block\" style=\"background:#39A6E5;\"></div>";
html += "<div class=\"palette-color-block\" style=\"background:#00932F;\"></div>";
html += "</div><div class=\"palette-label text-center\">H15</div></button></div>";

// H16 : {255,151,35} , {57,166,229}
html += "<div class=\"col-6\">";
html += "<button type=\"button\" class=\"palette-btn\" onclick=\"selectTheme('#FF9723','#39A6E5')\">";
html += "<div class=\"palette-colors\">";
html += "<div class=\"palette-color-block\" style=\"background:#FF9723;\"></div>";
html += "<div class=\"palette-color-block\" style=\"background:#39A6E5;\"></div>";
html += "</div><div class=\"palette-label text-center\">H16</div></button></div>";

// H17 : {76,0,153} , {255,151,35}
html += "<div class=\"col-6\">";
html += "<button type=\"button\" class=\"palette-btn\" onclick=\"selectTheme('#4C0099','#FF9723')\">";
html += "<div class=\"palette-colors\">";
html += "<div class=\"palette-color-block\" style=\"background:#4C0099;\"></div>";
html += "<div class=\"palette-color-block\" style=\"background:#FF9723;\"></div>";
html += "</div><div class=\"palette-label text-center\">H17</div></button></div>";

// H18 : {242,121,0} , {76,0,153}
html += "<div class=\"col-6\">";
html += "<button type=\"button\" class=\"palette-btn\" onclick=\"selectTheme('#F27900','#4C0099')\">";
html += "<div class=\"palette-colors\">";
html += "<div class=\"palette-color-block\" style=\"background:#F27900;\"></div>";
html += "<div class=\"palette-color-block\" style=\"background:#4C0099;\"></div>";
html += "</div><div class=\"palette-label text-center\">H18</div></button></div>";

// H19 : {153,153,153} , {242,121,0}
html += "<div class=\"col-6\">";
html += "<button type=\"button\" class=\"palette-btn\" onclick=\"selectTheme('#999999','#F27900')\">";
html += "<div class=\"palette-colors\">";
html += "<div class=\"palette-color-block\" style=\"background:#999999;\"></div>";
html += "<div class=\"palette-color-block\" style=\"background:#F27900;\"></div>";
html += "</div><div class=\"palette-label text-center\">H19</div></button></div>";

// H20 : {0,102,26} , {153,153,153}
html += "<div class=\"col-6\">";
html += "<button type=\"button\" class=\"palette-btn\" onclick=\"selectTheme('#00661A','#999999')\">";
html += "<div class=\"palette-colors\">";
html += "<div class=\"palette-color-block\" style=\"background:#00661A;\"></div>";
html += "<div class=\"palette-color-block\" style=\"background:#999999;\"></div>";
html += "</div><div class=\"palette-label text-center\">H20</div></button></div>";

// H21 : {38,0,153} , {0,102,26}
html += "<div class=\"col-6\">";
html += "<button type=\"button\" class=\"palette-btn\" onclick=\"selectTheme('#260099','#00661A')\">";
html += "<div class=\"palette-colors\">";
html += "<div class=\"palette-color-block\" style=\"background:#260099;\"></div>";
html += "<div class=\"palette-color-block\" style=\"background:#00661A;\"></div>";
html += "</div><div class=\"palette-label text-center\">H21</div></button></div>";

// H22 : {86,124,158} , {38,0,153}
html += "<div class=\"col-6\">";
html += "<button type=\"button\" class=\"palette-btn\" onclick=\"selectTheme('#567C9E','#260099')\">";
html += "<div class=\"palette-colors\">";
html += "<div class=\"palette-color-block\" style=\"background:#567C9E;\"></div>";
html += "<div class=\"palette-color-block\" style=\"background:#260099;\"></div>";
html += "</div><div class=\"palette-label text-center\">H22</div></button></div>";

// H23 : {0,77,153} , {86,124,158}
html += "<div class=\"col-6\">";
html += "<button type=\"button\" class=\"palette-btn\" onclick=\"selectTheme('#004D99','#567C9E')\">";
html += "<div class=\"palette-colors\">";
html += "<div class=\"palette-color-block\" style=\"background:#004D99;\"></div>";
html += "<div class=\"palette-color-block\" style=\"background:#567C9E;\"></div>";
html += "</div><div class=\"palette-label text-center\">H23</div></button></div>";

html += "</div>"; // fin row


    // ---------- COULEURS PERSONNALISÉES ----------
    html += "<div class=\"text-center mt-2\">";
    html += "<button type=\"button\" class=\"btn btn-outline-secondary btn-sm\" onclick=\"toggleCustomColors()\">";
    html += "Couleurs personnalisées";
    html += "</button>";
    html += "</div>";

    html += "<div id=\"customColorsSection\" class=\"mt-3\">";
    html += "  <div class=\"form-row\">";
    html += "    <div class=\"col\">";
    html += "      <label for=\"custom_color1\">Couleur 1 :</label>";
    html += "      <input type=\"color\" class=\"form-control\" id=\"custom_color1\" value=\"#004D99\" onchange=\"updateFromCustom()\">";
    html += "    </div>";
    html += "    <div class=\"col\">";
    html += "      <label for=\"custom_color2\">Couleur 2 :</label>";
    html += "      <input type=\"color\" class=\"form-control\" id=\"custom_color2\" value=\"#D9F200\" onchange=\"updateFromCustom()\">";
    html += "    </div>";
    html += "  </div>";
    html += "</div>";

    // ---------- RÉGLAGE DE L'HEURE RTC ----------
    html += "<hr>";
    html += "<h3 class=\"mb-3\">Régler l'heure de l'horloge</h3>";
    html += "<div class=\"form-group\">";
    html += "<label for=\"set_hh\">Heure (HH:MM) :</label>";
    html += "<div class=\"form-row\">";
    html += "  <div class=\"col\">";
    html += "    <input type=\"number\" class=\"form-control\" id=\"set_hh\" name=\"set_hh\" min=\"0\" max=\"23\" placeholder=\"Heures\">";
    html += "  </div>";
    html += "  <div class=\"col\">";
    html += "    <input type=\"number\" class=\"form-control\" id=\"set_mm\" name=\"set_mm\" min=\"0\" max=\"59\" placeholder=\"Minutes\">";
    html += "  </div>";
    html += "</div>";
    html += "</div>";
    html += "<button type=\"submit\" class=\"btn btn-info w-100 mb-4\" name=\"function\" value=\"30\" onclick=\"clickedButton=this;\">";
    html += "Régler l'heure exacte";
    html += "</button>";

    html += "</form>";
    html += "</div>";
    html += "</body>";
    html += "</html>";

    return html;
}

// ====================================================================
// SETUP
// ====================================================================
void setup()
{
    Serial.begin(9600);
    Serial.println("Initialize System");
    Wire.begin();

    IrReceiver.begin(IR_RECEIVE_PIN);

    strip.begin();
    strip.setBrightness(255);   // on laisse à 255, on gère tout en logiciel
    strip.clear();
    strip.show();

    if (!rtc.begin())
    {
        Serial.println("Couldn't find RTC");
    }

    if (rtc.lostPower())
    {
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }

    IRState = 1;

    pinMode(playbackPin, OUTPUT);
    digitalWrite(playbackPin, HIGH); // état repos

    WiFi.begin(ssid, password);
    Serial.print("Attente de connexion ...");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(100);
    }

    Serial.println("\n");
    Serial.println("Connexion etablie !");
    Serial.print("Adresse IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();
    Serial.print("Accédez à la page web ici : http://");
    Serial.print(WiFi.localIP());
    Serial.println("/");

    server.on("/", []()
              { server.send(200, "text/html", getHtmlContent()); });

    server.on("/function", HTTP_POST, []()
              {
                  int function = server.arg("function").toInt();
                  Serial.print("Received function: ");
                  Serial.println(function);

                  // Compte à rebours (durée)
                  if (function == 12)
                  {
                      int cdMin = server.arg("cd_minutes").toInt();
                      int cdSec = server.arg("cd_seconds").toInt();
                      if (cdMin < 0) cdMin = 0;
                      if (cdSec < 0) cdSec = 0;
                      if (cdSec > 59) cdSec = 59;

                      unsigned long totalSeconds = (unsigned long)cdMin * 60UL + (unsigned long)cdSec;
                      if (totalSeconds == 0) totalSeconds = 1;

                      countdownDurationMillis = totalSeconds * 1000UL;
                      countdownStartMillis = millis();
                      countdownActive = true;
                      countdownFlashing = false;

                      Serial.print("Compte à rebours lancé pour ");
                      Serial.print(cdMin);
                      Serial.print(" min ");
                      Serial.print(cdSec);
                      Serial.println(" s");
                  }

                  // Couleurs perso pour Change Color
                  if (function == 5)
                  {
                      String c1 = server.arg("color1");
                      String c2 = server.arg("color2");
                      int r, g, b;

                      if (c1.length() > 0 && parseHexColor(c1, r, g, b))
                      {
                          customColor1[0] = r;
                          customColor1[1] = g;
                          customColor1[2] = b;
                          Serial.print("Color1 = ");
                          Serial.println(c1);
                      }
                      if (c2.length() > 0 && parseHexColor(c2, r, g, b))
                      {
                          customColor2[0] = r;
                          customColor2[1] = g;
                          customColor2[2] = b;
                          Serial.print("Color2 = ");
                          Serial.println(c2);
                      }
                  }

                  // Réglage de la luminosité (sliders)
                  if (function == 20)
                  {
                      // MATRICE
                      if (server.hasArg("matrix_brightness"))
                      {
                          String mStr = server.arg("matrix_brightness");
                          int m = mStr.toInt();
                          if (m < 1)   m = 1;
                          if (m > 100) m = 100;
                          matrixBrightnessPercent = m;
                          Serial.print("Matrix brightness = ");
                          Serial.print(matrixBrightnessPercent);
                          Serial.println("%");
                      }

                      // CONTOUR
                      if (server.hasArg("contour_brightness"))
                      {
                          String cStr = server.arg("contour_brightness");
                          int c = cStr.toInt();
                          if (c < 1)   c = 1;
                          if (c > 100) c = 100;
                          contourBrightnessPercent = c;
                          Serial.print("Contour brightness = ");
                          Serial.print(contourBrightnessPercent);
                          Serial.println("%");
                      }

                      // Redessiner le mode courant
                      stateChoice();
                  }

                  // Réglage de l'heure RTC
                  if (function == 30)
                  {
                      String sh = server.arg("set_hh");
                      String sm = server.arg("set_mm");
                      int hh = sh.toInt();
                      int mm = sm.toInt();

                      if (hh < 0)  hh = 0;
                      if (hh > 23) hh = 23;
                      if (mm < 0)  mm = 0;
                      if (mm > 59) mm = 59;

                      DateTime nowRtc = rtc.now();
                      rtc.adjust(DateTime(nowRtc.year(), nowRtc.month(), nowRtc.day(), hh, mm, 0));

                      Serial.print("Heure réglée via web à : ");
                      Serial.print(hh);
                      Serial.print(":");
                      Serial.println(mm);
                  }

                  handleFunction(function);
                  server.send(200, "text/html", "Received function: " + String(function));
              });

    server.begin();
}

// ====================================================================
// LOOP
// ====================================================================
void loop()
{
    if (IrReceiver.decode())
    {
        IRChoice();
        IrReceiver.resume();
    }

    server.handleClient();

    // Tant que le compte à rebours est actif ou en flash, on force le mode countdown
    if (countdownActive || countdownFlashing)
    {
        IRState = 6;
    }

    stateChoice();

    DateTime now = rtc.now();
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
}

// ====================================================================
// UTILITAIRES COULEURS / EFFETS
// ====================================================================
bool parseHexColor(const String &hex, int &r, int &g, int &b)
{
    if (hex.length() != 7 || hex[0] != '#') return false;
    r = (int)strtol(hex.substring(1, 3).c_str(), NULL, 16);
    g = (int)strtol(hex.substring(3, 5).c_str(), NULL, 16);
    b = (int)strtol(hex.substring(5, 7).c_str(), NULL, 16);
    return true;
}

// Change Color : LED d'heure sur le contour boostée (slider contour)
void colorchange()
{
    DateTime now = rtc.now();
    int limite = (now.minute() * nbRows) / 60; // 0–34
    strip.clear();

    // --- 1) MATRICE avec customColor1 / customColor2 ---
    for (int row = 0; row < nbRows; row++)
    {
        for (int col = 0; col < rowLengths[row]; col++)
        {
            int ledIndex = ledMatrix[row][col];
            if (ledIndex < 0 || ledIndex >= MATRIX_LEDS) continue;

            if (row < limite)
            {
                strip.setPixelColor(
                    ledIndex,
                    makeMatrixColor(customColor1[0], customColor1[1], customColor1[2])
                );
            }
            else
            {
                strip.setPixelColor(
                    ledIndex,
                    makeMatrixColor(customColor2[0], customColor2[1], customColor2[2])
                );
            }
        }
    }

    // --- 2) CONTOUR : LED de l'heure en version boostée de customColor1 ---
    int hour24 = now.hour();        // 0..23
    int hour12 = hour24 % 12;       // 0..11
    int hourIdx;

    if (hour12 == 0)
        hourIdx = 12;               // 0h / 12h -> 12
    else
        hourIdx = hour12;           // 1..11

    int markerIndex = hourIdx;

    int contourLedIndex = contourStartIndex + 2 + markerIndex * 6;

    if (contourLedIndex >= contourStartIndex + CONTOUR_LEDS)
    {
        contourLedIndex -= 12 * 6;
    }

    if (contourLedIndex >= contourStartIndex &&
        contourLedIndex < contourStartIndex + CONTOUR_LEDS)
    {
        int baseR = customColor1[0];
        int baseG = customColor1[1];
        int baseB = customColor1[2];

        int maxBase = max(baseR, max(baseG, baseB));
        if (maxBase == 0) maxBase = 1;

        float maxFactorNoClip = 255.0f / (float)maxBase;

        // Luminosité du contour contrôlée par le slider (0–100 %)
        float userFactor = (float)contourBrightnessPercent / 100.0f;
        if (userFactor < 0.0f) userFactor = 0.0f;
        if (userFactor > 1.0f) userFactor = 1.0f;

        float useFactor = maxFactorNoClip * userFactor;

        int cR = (int)(baseR * useFactor + 0.5f);
        int cG = (int)(baseG * useFactor + 0.5f);
        int cB = (int)(baseB * useFactor + 0.5f);

        if (cR > 255) cR = 255;
        if (cG > 255) cG = 255;
        if (cB > 255) cB = 255;

        strip.setPixelColor(contourLedIndex, strip.Color((uint8_t)cR, (uint8_t)cG, (uint8_t)cB));
    }

    strip.show();
}

void randomcolor()
{
    strip.clear();
    for (int pixel = 0; pixel < NUM_LEDS; pixel++)
    {
        int red = random(0, 255);
        int green = random(0, 255);
        int blue = random(0, 255);
        strip.setPixelColor(pixel, makeMatrixColor(red, green, blue));
    }
    strip.show();
}

void increaseBrightness()
{
    matrixBrightnessPercent += 5;
    if (matrixBrightnessPercent > 100) matrixBrightnessPercent = 100;
    Serial.print("Matrix brightness IR + -> ");
    Serial.print(matrixBrightnessPercent);
    Serial.println("%");
    stateChoice();
}

void decreaseBrightness()
{
    matrixBrightnessPercent -= 5;
    if (matrixBrightnessPercent < 1) matrixBrightnessPercent = 1;
    Serial.print("Matrix brightness IR - -> ");
    Serial.print(matrixBrightnessPercent);
    Serial.println("%");
    stateChoice();
}

// Wheel pour la matrice (avec luminosité)
uint32_t WheelMatrix(byte WheelPos)
{
    WheelPos = 255 - WheelPos;
    uint8_t r, g, b;
    if (WheelPos < 85)
    {
        r = 255 - WheelPos * 3;
        g = 0;
        b = WheelPos * 3;
    }
    else if (WheelPos < 170)
    {
        WheelPos -= 85;
        r = 0;
        g = WheelPos * 3;
        b = 255 - WheelPos * 3;
    }
    else
    {
        WheelPos -= 170;
        r = WheelPos * 3;
        g = 255 - WheelPos * 3;
        b = 0;
    }
    return makeMatrixColor(r, g, b);
}

void rainbow()
{
    for (int pixel = 0; pixel < NUM_LEDS; pixel++)
    {
        strip.setPixelColor(pixel, WheelMatrix(((pixel * 256 / 128)) & 255));
    }
    strip.show();
}

void rainbowContinuous()
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        byte pos = (i + rainbowOffset) & 255;
        strip.setPixelColor(i, WheelMatrix(pos));
    }
    strip.show();
    rainbowOffset = (rainbowOffset + 1) & 255;
}

// WheelBis pour animation hypnotique (utilise setBrightness)
uint32_t WheelBis(byte WheelPos, int pos)
{
    WheelPos = 255 - WheelPos;
    if (WheelPos < 85)
    {
        return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    if (WheelPos < 170)
    {
        WheelPos -= 85;
        return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    WheelPos -= 170;
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

// ====================================================================
// CHOIX D’ÉTAT
// ====================================================================
void stateChoice()
{
    // Si on N'EST PAS en mode animation hypnotique (IRState != 8),
    // on s'assure que la luminosité globale de la bande est à fond.
    if (IRState != 8)
    {
        strip.setBrightness(255);
    }

    if (IRState == 1)
    {
        twoColors();
    }
    else if (IRState == 2)
    {
        // chronomètre (non utilisé directement)
    }
    else if (IRState == 3)
    {
        ninjaClock();
    }
    else if (IRState == 4)
    {
        turnoff();
    }
    else if (IRState == 5)
    {
        rainbowContinuous();
    }
    else if (IRState == 6)
    {
        if (countdownFlashing)
            countdownFlashStep();
        else
            countdownModeStep();
    }
    else if (IRState == 7)
    {
        colorchange();
    }
    else if (IRState == 8)
    {
        verticalRainbowAnimationStep();
    }
}

void turnoff()
{
    countdownActive   = false;
    countdownFlashing = false;
    hypnoticActive    = false;

    strip.clear();
    strip.show();
    IRState = 0;
}

void clearState()
{
    IRState = 0;
}

// ====================================================================
// IR
// ====================================================================
void IRChoice()
{
    if (IrReceiver.decodedIRData.decodedRawData == 0xBA45FF00) // 1
    {
        countdownActive   = false;
        countdownFlashing = false;
        hypnoticActive    = false;

        IRState = 5;
        rainbowOffset = 0;
    }
    else if (IrReceiver.decodedIRData.decodedRawData == 0xB946FF00) // 2
    {
        countdownActive   = false;
        countdownFlashing = false;
        hypnoticActive    = false;

        randomcolor();
        clearState();
    }
    else if (IrReceiver.decodedIRData.decodedRawData == 0xB847FF00) // 3
    {
        turnoff();
    }
    else if (IrReceiver.decodedIRData.decodedRawData == 0xBB44FF00) // 4
    {
        countdownActive   = false;
        countdownFlashing = false;
        hypnoticActive    = false;

        IRState = 1; // Clock
    }
    else if (IrReceiver.decodedIRData.decodedRawData == 0xBF40FF00) // 5
    {
        countdownActive   = false;
        countdownFlashing = false;
        hypnoticActive    = false;

        twoColors();
        clearState();
    }
    else if (IrReceiver.decodedIRData.decodedRawData == 0xBC43FF00) // 6
    {
        countdownActive   = false;
        countdownFlashing = false;
        hypnoticActive    = false;

        rainbow();
        clearState();
    }
    else if (IrReceiver.decodedIRData.decodedRawData == 0xF807FF00) // 7
    {
        countdownActive   = false;
        countdownFlashing = false;
        hypnoticActive    = false;

        colorchange();
        clearState();
    }
    else if (IrReceiver.decodedIRData.decodedRawData == 0xEA15FF00) // 8
    {
        countdownActive   = false;
        countdownFlashing = false;

        IRState = 8;       // animation hypnotique
        hypnoticActive = false;
    }
    else if (IrReceiver.decodedIRData.decodedRawData == 0xF609FF00) // 9
    {
        countdownActive   = false;
        countdownFlashing = false;
        hypnoticActive    = false;

        verticalRainbow();
        clearState();
    }
    else if (IrReceiver.decodedIRData.decodedRawData == 0xF20DFF00) // #
    {
        countdownActive   = false;
        countdownFlashing = false;
        hypnoticActive    = false;

        colorchange();
        clearState();
    }
    else if (IrReceiver.decodedIRData.decodedRawData == 0xE916FF00) // *
    {
        countdownActive   = false;
        countdownFlashing = false;
        hypnoticActive    = false;

        colorchange();
        clearState();
    }
    else if (IrReceiver.decodedIRData.decodedRawData == 0xF708FF00) // gauche
    {
        countdownActive   = false;
        countdownFlashing = false;
        hypnoticActive    = false;

        colorchange();
        clearState();
    }
    else if (IrReceiver.decodedIRData.decodedRawData == 0xA55AFF00) // droite
    {
        countdownActive   = false;
        countdownFlashing = false;
        hypnoticActive    = false;

        colorchange();
        clearState();
    }
    else if (IrReceiver.decodedIRData.decodedRawData == 0xE718FF00) // haut
    {
        increaseBrightness();
    }
    else if (IrReceiver.decodedIRData.decodedRawData == 0xAD52FF00) // bas
    {
        decreaseBrightness();
    }
    else if (IrReceiver.decodedIRData.decodedRawData == 0xE31CFF00) // OK
    {
        countdownActive   = false;
        countdownFlashing = false;
        hypnoticActive    = false;

        colorchange();
        clearState();
    }
}

// ====================================================================
// MODES BASÉS SUR LA MATRICE
// ====================================================================

// Horloge : deux couleurs selon les minutes + LED d’heure sur le contour
void twoColors()
{
    DateTime now = rtc.now();
    int limite = (now.minute() * nbRows) / 60;   // 0–34 environ
    strip.clear();

    // --- Couleurs en fonction de l'heure ---
    int colorTest = now.hour() % 24;

    int color1[3] = {
        RGB_array[colorTest][0][0],
        RGB_array[colorTest][0][1],
        RGB_array[colorTest][0][2]
    };
    int color2[3] = {
        RGB_array[colorTest][1][0],
        RGB_array[colorTest][1][1],
        RGB_array[colorTest][1][2]
    };

    // --- 1) MATRICE CENTRALE : remplissage en 2 couleurs suivant les minutes ---
    for (int row = 0; row < nbRows; row++)
    {
        for (int col = 0; col < rowLengths[row]; col++)
        {
            int ledIndex = ledMatrix[row][col];
            if (ledIndex < 0 || ledIndex >= MATRIX_LEDS) continue;

            if (row < limite)
            {
                strip.setPixelColor(
                    ledIndex,
                    makeMatrixColor(color1[0], color1[1], color1[2])
                );
            }
            else
            {
                strip.setPixelColor(
                    ledIndex,
                    makeMatrixColor(color2[0], color2[1], color2[2])
                );
            }
        }
    }

    // --- 2) CONTOUR : LED pour l’heure, boostée selon contourBrightnessPercent ---

    int hour24 = now.hour();        // 0..23
    int hour12 = hour24 % 12;       // 0..11
    int hourIdx;

    if (hour12 == 0)
        hourIdx = 12;               // 0h / 12h -> 12
    else
        hourIdx = hour12;           // 1..11

    int markerIndex = hourIdx;

    int contourLedIndex = contourStartIndex + 2 + markerIndex * 6;

    if (contourLedIndex >= contourStartIndex + CONTOUR_LEDS)
    {
        contourLedIndex -= 12 * 6;
    }

    if (contourLedIndex >= contourStartIndex &&
        contourLedIndex < contourStartIndex + CONTOUR_LEDS)
    {
        int baseR = color1[0];
        int baseG = color1[1];
        int baseB = color1[2];

        int maxBase = max(baseR, max(baseG, baseB));
        if (maxBase == 0) maxBase = 1;

        float maxFactorNoClip = 255.0f / (float)maxBase;

        float userFactor = (float)contourBrightnessPercent / 100.0f;
        if (userFactor < 0.0f) userFactor = 0.0f;
        if (userFactor > 1.0f) userFactor = 1.0f;

        float useFactor = maxFactorNoClip * userFactor;

        int cR = (int)(baseR * useFactor + 0.5f);
        int cG = (int)(baseG * useFactor + 0.5f);
        int cB = (int)(baseB * useFactor + 0.5f);

        if (cR > 255) cR = 255;
        if (cG > 255) cG = 255;
        if (cB > 255) cB = 255;

        strip.setPixelColor(contourLedIndex, strip.Color((uint8_t)cR, (uint8_t)cG, (uint8_t)cB));
    }

    strip.show();
}

// ARC-EN-CIEL PAR LIGNES VERTICALES (matrice)
void verticalRainbow()
{
    for (int row = 0; row < nbRows; row++)
    {
        byte WheelPosBase = (row * 256) / nbRows;

        for (int col = 0; col < rowLengths[row]; col++)
        {
            int ledIndex = ledMatrix[row][col];
            if (ledIndex < 0 || ledIndex >= MATRIX_LEDS) continue;

            uint32_t color = WheelMatrix(WheelPosBase);
            strip.setPixelColor(ledIndex, color);
        }
    }

    strip.show();
}

// Animation hypnotique non bloquante
void verticalRainbowAnimationStep()
{
    if (!hypnoticActive)
    {
        hypnoticActive = true;
        hypnoticStartMillis = millis();
    }

    unsigned long tAll = millis() - hypnoticStartMillis;

    // Au bout de 30 s : on stoppe l'animation et on revient sur twoColors()
    if (tAll >= HYPNOTIC_DURATION)
    {
        hypnoticActive = false;

        strip.setBrightness(255);
        strip.clear();
        strip.show();

        IRState = 1;      // état "Clock"
        twoColors();      // on redessine tout de suite l'horloge
        return;
    }

    // Animation plus lente
    float phase = tAll * 0.004f;

    // Respiration de luminosité douce
    float breathe = (sin(tAll * 0.0007f) + 1.0f) * 0.5f;
    uint8_t breatheBrightness = 5 + (uint8_t)(breathe * 15.0f); // ≈ 5–20

    strip.setBrightness(breatheBrightness);

    for (int row = 0; row < nbRows; row++)
    {
        uint8_t basePos = (uint8_t)(row * 8 + (int)phase);

        for (int col = 0; col < rowLengths[row]; col++)
        {
            int ledIndex = ledMatrix[row][col];
            if (ledIndex < 0 || ledIndex >= MATRIX_LEDS) continue;

            uint8_t pos = basePos + col * 2;
            uint32_t color = WheelBis(pos, row);

            strip.setPixelColor(ledIndex, color);
        }
    }

    strip.show();
}

// NINJA CLOCK : ligne des minutes + LED d'heure sur le contour (bleu)
void ninjaClock()
{
    DateTime now = rtc.now();

    // --------- 1) MATRICE : ligne des minutes ----------
    int row = (now.minute() * nbRows) / 60; // 0–33
    if (row < 0) row = 0;
    if (row >= nbRows) row = nbRows - 1;

    strip.clear();

    for (int col = 0; col < rowLengths[row]; col++)
    {
        int ledIndex = ledMatrix[row][col];
        if (ledIndex >= 0 && ledIndex < MATRIX_LEDS)
        {
            strip.setPixelColor(ledIndex, makeMatrixColor(r1, g1, b1));
        }
    }

    // --------- 2) CONTOUR : LED de l'heure en bleu, FIXE à 40 % du max possible ----------

    int hour24 = now.hour();        // 0..23
    int hour12 = hour24 % 12;       // 0..11
    int hourIdx;

    if (hour12 == 0)
        hourIdx = 12;               // 0h / 12h -> 12
    else
        hourIdx = hour12;           // 1..11

    int markerIndex = hourIdx;

    int contourLedIndex = contourStartIndex + 2 + markerIndex * 6;

    if (contourLedIndex >= contourStartIndex + CONTOUR_LEDS)
    {
        contourLedIndex -= 12 * 6;
    }

    if (contourLedIndex >= contourStartIndex &&
        contourLedIndex < contourStartIndex + CONTOUR_LEDS)
    {
        int baseR = NINJA_HOUR_R;
        int baseG = NINJA_HOUR_G;
        int baseB = NINJA_HOUR_B;

        int maxBase = max(baseR, max(baseG, baseB));
        if (maxBase == 0) maxBase = 1;

        float maxFactorNoClip = 255.0f / (float)maxBase;

        // Contour bloqué à 40 % du max possible, indépendamment du slider
        float userFactor = 0.10f;
        if (userFactor < 0.0f) userFactor = 0.0f;
        if (userFactor > 1.0f) userFactor = 1.0f;

        float useFactor = maxFactorNoClip * userFactor;

        int cR = (int)(baseR * useFactor + 0.5f);
        int cG = (int)(baseG * useFactor + 0.5f);
        int cB = (int)(baseB * useFactor + 0.5f);

        if (cR > 255) cR = 255;
        if (cG > 255) cG = 255;
        if (cB > 255) cB = 255;

        strip.setPixelColor(contourLedIndex, strip.Color((uint8_t)cR, (uint8_t)cG, (uint8_t)cB));
    }

    strip.show();
}

// MODE COMPTE A REBOURS : remplissage progressif (non bloquant)
void countdownModeStep()
{
    if (!countdownActive || countdownDurationMillis == 0)
    {
        return;
    }

    unsigned long nowMs = millis();
    unsigned long elapsed = nowMs - countdownStartMillis;

    float ratio = (float)elapsed / (float)countdownDurationMillis;
    if (ratio >= 1.0f)
    {
        ratio = 1.0f;
        countdownActive = false;
        countdownFlashing = true;
        countdownFlashStartMillis = millis();
    }

    int rowsToFill = (int)(ratio * nbRows + 0.5f);

    DateTime now = rtc.now();
    int h = now.hour() % 24;
    int colorFill[3] = {
        RGB_array[h][0][0],
        RGB_array[h][0][1],
        RGB_array[h][0][2]
    };

    strip.clear();

    for (int row = 0; row < nbRows; row++)
    {
        for (int col = 0; col < rowLengths[row]; col++)
        {
            int ledIndex = ledMatrix[row][col];
            if (ledIndex < 0 || ledIndex >= MATRIX_LEDS) continue;

            if (row < rowsToFill)
            {
                strip.setPixelColor(
                    ledIndex,
                    makeMatrixColor(colorFill[0], colorFill[1], colorFill[2])
                );
            }
            else
            {
                strip.setPixelColor(ledIndex, strip.Color(0, 0, 0));
            }
        }
    }

    strip.show();
}

// FLASH FINAL : rouge ↔ orange, non bloquant
// À la fin => retour au mode Clock (twoColors)
void countdownFlashStep()
{
    unsigned long t = millis() - countdownFlashStartMillis;

    if (t >= COUNTDOWN_FLASH_DURATION)
    {
        countdownFlashing = false;
        countdownActive   = false;

        strip.setBrightness(255);
        strip.clear();
        strip.show();

        IRState = 1;
        return;
    }

    bool orangePhase = ((t / 200) % 2 == 0);
    uint8_t r = orangePhase ? 255 : 255;
    uint8_t g = orangePhase ? 80  : 0;
    uint8_t b = orangePhase ? 0   : 0;

    uint32_t col = makeMatrixColor(r, g, b);

    for (int i = 0; i < NUM_LEDS; i++)
    {
        strip.setPixelColor(i, col);
    }
    strip.show();
}

// ====================================================================
// FONCTIONS GÉNÉRALES
// ====================================================================
void handleFunction(int function)
{
    stopAnimation = true;
    delay(2);

    Serial.print("Function requested: ");
    Serial.println(function);

    switch (function)
    {
        case 1: // Clock (twoColors)
            countdownActive   = false;
            countdownFlashing = false;
            hypnoticActive    = false;
            IRState = 1;
            Serial.println("Mode : Clock");
            break;

        case 2: // Animation Hypnotique (non bloquante)
            countdownActive   = false;
            countdownFlashing = false;
            IRState = 8;
            hypnoticActive = false;
            Serial.println("Mode : Animation Hypnotique");
            break;

        case 3: // Chronomètre (non utilisé ici)
            IRState = 2;
            Serial.println("Mode : chronometre");
            break;

        case 4: // Ninja
            countdownActive   = false;
            countdownFlashing = false;
            hypnoticActive    = false;
            IRState = 3;
            Serial.println("Mode : Ninja Clock");
            break;

        case 5: // Change Color
            countdownActive   = false;
            countdownFlashing = false;
            hypnoticActive    = false;
            Serial.println("Mode : Change Color");
            IRState = 7;
            break;

        case 10: // Off
            Serial.println("Mode : OFF");
            turnoff();
            break;

        case 12: // Compte à rebours (durée déjà réglée)
            Serial.println("Mode : Compte à rebours");
            IRState = 6;
            break;

        case 20: // Luminosité (gérée dans server.on)
            Serial.println("Réglage de la luminosité (web)");
            break;

        case 30: // Réglage de l'heure (géré dans server.on)
            Serial.println("Heure RTC réglée (web)");
            break;

        default:
            Serial.println("Function inconnue !");
            break;
    }
}

void triggerPlayback()
{
    digitalWrite(playbackPin, LOW);
    delay(100);
    digitalWrite(playbackPin, HIGH);
}
