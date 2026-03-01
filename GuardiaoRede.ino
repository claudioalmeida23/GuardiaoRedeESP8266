/*
   ██████╗  █████╗  █████╗  ███████╗
  ██╔════╝ ██╔══██╗██╔══██╗ ██╔════╝
  ██║      ███████║███████║ ███████╗
  ██║      ██╔══██║██╔══██║ ╚════██║
  ╚██████╗ ██║  ██║██║  ██║ ███████║
   ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝ ╚══════╝

  🛡️ Guardião de Rede – Stealth
  Criador: Claudio Almeida
  Plataforma: ESP8266
  Licença: MIT
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

/* ===== PINOS ===== */
#define LED_ALERTA 2          // LED onboard (ativo em LOW)
#define EEPROM_SIZE 512

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
unsigned long intervaloScan = 60000;
bool estadoLed = false;

/* ===== XOR ===== */
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

/* ===== EEPROM ===== */
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

/* ===== AP CONFIG ===== */
void iniciarAPConfig() {
  modoConfig = true;

  WiFi.disconnect(true);
  delay(500);
  WiFi.mode(WIFI_AP);
  WiFi.softAP("Guardiao_Config", "admin12345");

  logar("MODO CONFIG ATIVO");
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
      "<h3>Config salva</h3><p>Reiniciando...</p>");
    delay(1500);
    ESP.restart();
  });

  server.begin();
}

/* ===== EVIL TWIN ===== */
void detectarEvilTwin() {
  int n = WiFi.scanNetworks(false, true);

  for (int i = 0; i < n; i++) {
    if (WiFi.SSID(i) == String(ssid)) {
      if (memcmp(WiFi.BSSID(i), bssidLegitimo, 6) != 0) {
        if (!evilDetectado) {
          evilDetectado = true;
          digitalWrite(LED_ALERTA, LOW);
          logar("EVIL TWIN DETECTADO");
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
  digitalWrite(LED_ALERTA, HIGH);

  logar("BOOT");

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

  logar("CONECTADO");
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
  }

  if (WiFi.status() == WL_CONNECTED && millis() - ultimoScan >= intervaloScan) {
    ultimoScan = millis();
    detectarEvilTwin();

    int tempos[] = {60000, 120000, 180000};
    intervaloScan = tempos[random(0, 3)];
  }
}
