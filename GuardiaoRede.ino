/*
 🛡️ Guardião de Rede – Stealth + Logs Reais
 Criador: Claudio Almeida
 Plataforma: ESP8266
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

/* ===== PINOS ===== */
#define LED_ALERTA 2        // LED onboard (LOW = ligado)
#define PINO_BEEP 14        // D5 – speaker de PC
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
unsigned long ultimoScan = 0;
bool estadoLed = false;

/* ===== LOG ===== */
void logar(String m) {
  Serial.println("[GUARDIAO] " + m);
}

/* ===== EEPROM ===== */
bool temConfig() {
  EEPROM.begin(EEPROM_SIZE);
  bool ok = EEPROM.read(0) == 1;
  if (ok) {
    EEPROM.get(4, ssid);
    EEPROM.get(36, pass);
  }
  EEPROM.end();
  return ok && strlen(ssid) > 2;
}

void salvarConfig() {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.write(0, 1);          // FLAG CONFIG OK
  EEPROM.put(4, ssid);
  EEPROM.put(36, pass);
  EEPROM.put(80, bssidLegitimo);
  EEPROM.commit();
  EEPROM.end();
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
  logar("Conecte em Guardiao_Config");
  logar("Acesse http://192.168.4.1");

  digitalWrite(LED_ALERTA, LOW); // LED fixo

  server.on("/", []() {
    String h;
    h += "<h2>Guardião de Rede</h2>";
    h += "<form method='POST' action='/salvar'>";
    h += "SSID:<br><input name='s'><br>";
    h += "Senha:<br><input name='p' type='password'><br><br>";
    h += "<input type='submit' value='Salvar'>";
    h += "</form>";
    server.send(200, "text/html", h);
  });

  server.on("/salvar", []() {
    server.arg("s").toCharArray(ssid, 32);
    server.arg("p").toCharArray(pass, 32);

    EEPROM.begin(EEPROM_SIZE);
    EEPROM.write(0, 1);
    EEPROM.put(4, ssid);
    EEPROM.put(36, pass);
    EEPROM.commit();
    EEPROM.end();

    server.send(200, "text/html",
      "<h3>Configuração salva</h3>"
      "<p>O Guardião entrará em modo invisível.</p>");

    delay(1500);
    WiFi.softAPdisconnect(true);
    delay(500);
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
          logar("ALERTA: EVIL TWIN DETECTADO");
          beep(3, 120);
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

  logar("Boot do Guardião iniciado");

  if (!temConfig()) {
    logar("PRIMEIRO BOOT - abrindo AP");
    iniciarAPConfig();
    return;
  }

  logar("Config encontrada - modo fantasma");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  unsigned long t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 15000) {
    delay(500);
  }

  if (WiFi.status() != WL_CONNECTED) {
    logar("Falha Wi-Fi - voltando ao AP");
    iniciarAPConfig();
    return;
  }

  memcpy(bssidLegitimo, WiFi.BSSID(), 6);
  salvarConfig();

  logar("Conectado com sucesso");
}

/* ===== LOOP ===== */
void loop() {

  if (modoConfig) {
    server.handleClient();
    return;
  }

  /* LED piscando = OK */
  if (WiFi.status() == WL_CONNECTED) {
    if (millis() - ultimoPisca > 1000) {
      ultimoPisca = millis();
      estadoLed = !estadoLed;
      digitalWrite(LED_ALERTA, estadoLed ? LOW : HIGH);
    }
    wifiFalha = false;
  }

  /* Queda de rede */
  if (WiFi.status() != WL_CONNECTED && !wifiFalha) {
    wifiFalha = true;
    digitalWrite(LED_ALERTA, LOW);
    logar("ALERTA: CONEXAO WI-FI PERDIDA");
    beep(4);
  }

  /* Evil Twin (1x por minuto) */
  if (WiFi.status() == WL_CONNECTED && millis() - ultimoScan > 60000) {
    ultimoScan = millis();
    detectarEvilTwin();
  }
}
