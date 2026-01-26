/*
  /$$$$$$                                          /$$      
 /$$__  $$                                        | $$      
| $$  \ $$ /$$$$$$$   /$$$$$$   /$$$$$$   /$$$$$$ | $$   /$$
| $$$$$$$$| $$__  $$ |____  $$ /$$__  $$ /$$__  $$| $$  /$$/
| $$__  $$| $$  \ $$  /$$$$$$$| $$  \__/| $$  \ $$| $$$$$$/ 
| $$  | $$| $$  | $$ /$$__  $$| $$      | $$  | $$| $$_  $$ 
| $$  | $$| $$  | $$|  $$$$$$$| $$      |  $$$$$$/| $$ \  $$
|__/  |__/|__/  |__/ \_______/|__/       \______/ |__/  \__/

 🛡️ Guardião de Rede – Stealth
 Criador: Claudio Almeida
 Plataforma: ESP8266
 GitHub: https://github.com/claudioalmeida23
 Licença: MIT
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

/* ===== PINOS ===== */
#define LED_ALERTA 2
#define PINO_BEEP 14
#define EEPROM_SIZE 512

/* ===== EEPROM MAP ===== */
#define EEPROM_FLAG_INVASOR   90
#define EEPROM_BSSID_INVASOR  91  // 6 bytes

ESP8266WebServer server(80);

/* ===== VARIÁVEIS ===== */
char ssid[32] = "";
char pass[32] = "";
uint8_t bssidLegitimo[6];

bool modoConfig = false;
bool wifiFalha = false;
bool evilDetectado = false;

unsigned long ultimoPisca = 0;
unsigned long ultimoScan  = 0;
unsigned long intervaloScan = 60000; // será randomizado
bool estadoLed = false;

/* ===== XOR SIMPLES ===== */
void obf(char* b, int l) {
  for (int i = 0; i < l; i++) {
    if (!b[i]) break;
    b[i] ^= 0x5A;
  }
}

/* ===== LOG ===== */
void logar(const char* m) {
  Serial.print("[GUARDIAO] ");
  Serial.println(m);
}

/* ===== EEPROM CONFIG ===== */
bool temConfig() {
  EEPROM.begin(EEPROM_SIZE);
  if (EEPROM.read(0) != 1) {
    EEPROM.end();
    return false;
  }

  EEPROM.get(4, ssid);
  EEPROM.get(36, pass);
  EEPROM.get(80, bssidLegitimo);
  EEPROM.end();

  obf(ssid, 32);
  obf(pass, 32);

  return strlen(ssid) > 2;
}

void salvarConfig(bool aplicarXor) {
  char s[32], p[32];
  strcpy(s, ssid);
  strcpy(p, pass);

  if (aplicarXor) {
    obf(s, 32);
    obf(p, 32);
  }

  EEPROM.begin(EEPROM_SIZE);
  EEPROM.write(0, 1);
  EEPROM.put(4, s);
  EEPROM.put(36, p);
  EEPROM.put(80, bssidLegitimo);
  EEPROM.commit();
  EEPROM.end();
}

/* ===== INVASOR ===== */
void salvarInvasor(uint8_t *bssid) {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.write(EEPROM_FLAG_INVASOR, 1);
  EEPROM.put(EEPROM_BSSID_INVASOR, bssid);
  EEPROM.commit();
  EEPROM.end();

  Serial.printf(
    "[GUARDIAO] INVASOR REGISTRADO: %02X:%02X:%02X:%02X:%02X:%02X\n",
    bssid[0], bssid[1], bssid[2],
    bssid[3], bssid[4], bssid[5]
  );
}

bool invasorJaRegistrado(uint8_t *bssid) {
  uint8_t salvo[6];
  EEPROM.begin(EEPROM_SIZE);
  bool existe = EEPROM.read(EEPROM_FLAG_INVASOR) == 1;
  if (existe) EEPROM.get(EEPROM_BSSID_INVASOR, salvo);
  EEPROM.end();
  return existe && memcmp(bssid, salvo, 6) == 0;
}

/* ===== BEEP ===== */
void beep(int n, int t = 150) {
  for (int i = 0; i < n; i++) {
    digitalWrite(PINO_BEEP, HIGH);
    delay(t);
    digitalWrite(PINO_BEEP, LOW);
    delay(t);
  }
}

/* ===== AP CONFIG ===== */
void iniciarAPConfig() {
  modoConfig = true;

  WiFi.disconnect(true);
  delay(500);
  WiFi.mode(WIFI_AP);
  WiFi.softAP("Guardiao_Config", "admin12345");

  logar("MODO CONFIGURACAO ATIVO");
  digitalWrite(LED_ALERTA, LOW);

  server.on("/", []() {
    server.send(200, "text/html",
      "<h2>Guardiao de Rede</h2>"
      "<form method='POST' action='/salvar'>"
      "SSID:<br><input name='s'><br>"
      "Senha:<br><input name='p' type='password'><br><br>"
      "<input type='submit' value='Salvar'>"
      "</form>");
  });

  server.on("/salvar", []() {
    server.arg("s").toCharArray(ssid, 32);
    server.arg("p").toCharArray(pass, 32);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);

    unsigned long t0 = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - t0 < 15000) {
      delay(500);
    }

    if (WiFi.status() != WL_CONNECTED) {
      server.send(200, "text/html", "<h3>Falha ao conectar</h3>");
      return;
    }

    memcpy(bssidLegitimo, WiFi.BSSID(), 6);
    salvarConfig(true);

    server.send(200, "text/html",
      "<h3>Configuração salva</h3><p>Reiniciando...</p>");
    delay(1500);
    ESP.restart();
  });

  server.begin();
}

/* ===== BLINDAGEM ===== */
void blindarEEPROM() {
  char s[32], p[32];

  EEPROM.begin(EEPROM_SIZE);
  EEPROM.get(4, s);
  EEPROM.get(36, p);
  EEPROM.end();

  obf(s, 32); obf(p, 32);
  obf(s, 32); obf(p, 32);

  EEPROM.begin(EEPROM_SIZE);
  EEPROM.put(4, s);
  EEPROM.put(36, p);
  EEPROM.commit();
  EEPROM.end();

  logar("EEPROM REBLINDADA");
}

/* ===== EVIL TWIN ===== */
void detectarEvilTwin() {
  int n = WiFi.scanNetworks(false, true);

  for (int i = 0; i < n; i++) {
    if (WiFi.SSID(i) == String(ssid)) {
      if (memcmp(WiFi.BSSID(i), bssidLegitimo, 6) != 0) {

        uint8_t *bssidInvasor = WiFi.BSSID(i);

        Serial.printf(
          "[GUARDIAO] EVIL TWIN | BSSID %02X:%02X:%02X:%02X:%02X:%02X | CH %d | RSSI %d\n",
          bssidInvasor[0], bssidInvasor[1], bssidInvasor[2],
          bssidInvasor[3], bssidInvasor[4], bssidInvasor[5],
          WiFi.channel(i), WiFi.RSSI(i)
        );

        if (!invasorJaRegistrado(bssidInvasor)) {
          salvarInvasor(bssidInvasor);
        }

        if (!evilDetectado) {
          evilDetectado = true;
          digitalWrite(LED_ALERTA, LOW);
          logar("EVIL TWIN DETECTADO");
          beep(3, 120);
          blindarEEPROM();
        }
      }
    }
  }
  WiFi.scanDelete();
}

/* ===== SETUP ===== */
void setup() {
  Serial.begin(115200);

  pinMode(LED_ALERTA, OUTPUT);
  pinMode(PINO_BEEP, OUTPUT);
  digitalWrite(LED_ALERTA, HIGH);
  digitalWrite(PINO_BEEP, LOW);

  logar("BOOT DO GUARDIAO");

  if (!temConfig()) {
    iniciarAPConfig();
    return;
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  unsigned long t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 15000) {
    delay(500);
  }

  if (WiFi.status() != WL_CONNECTED) {
    iniciarAPConfig();
    return;
  }

  randomSeed(ESP.getCycleCount());

  int tempos[] = {60000, 120000, 180000};
  intervaloScan = tempos[random(0, 3)];

  Serial.printf("[GUARDIAO] SCAN ALEATORIO ARMADO: %lu ms\n", intervaloScan);

  logar("CONECTADO COM SUCESSO");
}

/* ===== LOOP ===== */
void loop() {

  if (modoConfig) {
    server.handleClient();
    return;
  }

  if (WiFi.status() == WL_CONNECTED) {
    if (millis() - ultimoPisca > 1000) {
      ultimoPisca = millis();
      estadoLed = !estadoLed;
      digitalWrite(LED_ALERTA, estadoLed ? LOW : HIGH);
    }
    wifiFalha = false;
  }

  if (WiFi.status() != WL_CONNECTED && !wifiFalha) {
    wifiFalha = true;
    digitalWrite(LED_ALERTA, LOW);
    logar("WIFI PERDIDO");
    beep(4);
  }

  if (WiFi.status() == WL_CONNECTED && millis() - ultimoScan >= intervaloScan) {
    ultimoScan = millis();

    Serial.printf("[GUARDIAO] INICIANDO SCAN (%lu ms)\n", intervaloScan);
    detectarEvilTwin();

    int tempos[] = {60000, 120000, 180000};
    intervaloScan = tempos[random(0, 3)];

    Serial.printf("[GUARDIAO] PROXIMO SCAN EM %lu ms\n", intervaloScan);
  }
}
