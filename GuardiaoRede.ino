secundario

/*
   ██████╗  █████╗  █████╗  ███████╗
  ██╔════╝ ██╔══██╗██╔══██╗ ██╔════╝
  ██║      ███████║███████║ ███████╗
  ██║      ██╔══██║██╔══██║ ╚════██║
  ╚██████╗ ██║  ██║██║  ██║ ███████║
   ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝ ╚══════╝

  🛡️ Guardião de Rede  
  Criador: Claudio Almeida.CAAS
  Licença: MIT
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

#define LED_ALERTA 2
#define EEPROM_SIZE 512

ESP8266WebServer server(80);

const char* BOT_TOKEN = "SEU_TOKEN_AQUI";
const char* CHAT_ID   = "SEU_CHAT_ID_AQUI";

unsigned long ultimoTelegram = 0;
#define TELEGRAM_COOLDOWN 60000

char ssid[32] = "";
char pass[32] = "";
uint8_t bssidLegitimo[6] = {0};

bool evilDetectado = false;
unsigned long ultimoScan = 0;
unsigned long ultimoPisca = 0;
unsigned long alertaDesde = 0;  // NOVO: tempo do alerta
#define RESET_ALERTA 300000  // Reset após 5min


void obf(char* b, int l) {
  for (int i = 0; i < l; i++) {
    if (!b[i]) break;
    b[i] ^= 0x5A;
  }
}

void logar(const char* m) {
  Serial.print("[Guardiao de Rede-CAAS] ");
  Serial.println(m);
}

void enviarTelegram(String msg) {
  if (millis() - ultimoTelegram < TELEGRAM_COOLDOWN) {
    Serial.println("⏳ Cooldown...");
    return;
  }
  ultimoTelegram = millis();

  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient https;
  String url = "https://api.telegram.org/bot" + String(BOT_TOKEN) +
               "/sendMessage?chat_id=" + String(CHAT_ID) + "&text=" + msg;

  if (https.begin(client, url)) {
    int httpCode = https.GET();
    Serial.print("📲 HTTP: ");
    Serial.println(httpCode);
    https.end();
  }
}

bool temConfig() {
  EEPROM.begin(EEPROM_SIZE);
  if (EEPROM.read(0) != 0xCA) {
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

void salvarConfig() {
  char s[32], p[32];
  strcpy(s, ssid);
  strcpy(p, pass);
  obf(s, 32);
  obf(p, 32);

  EEPROM.begin(EEPROM_SIZE);
  EEPROM.write(0, 0xCA);
  EEPROM.put(4, s);
  EEPROM.put(36, p);
  EEPROM.put(80, bssidLegitimo);
  EEPROM.commit();
  EEPROM.end();
  logar("💾 Salva");
}


void iniciarAPConfig() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP("Guardiao de Rede", "");
  logar("🔥 AP Guardiao de Rede");

  server.on("/", []() {
    String html = R"(
<!DOCTYPE html>
<html>
<head>
<title>Guardião de Rede</title>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">

<style>
body{
  font-family:Arial;
  background: radial-gradient(circle at top, #001100, #000);
  color:#00ff88;
  padding:20px;
  text-align:center;
}

/* Container central */
.box{
  max-width:400px;
  margin:auto;
  padding:20px;
  border:1px solid #00ff88;
  border-radius:10px;
  box-shadow:0 0 15px #00ff88;
  background:#000;
}

/* Títulos */
h1{
  margin-bottom:5px;
}

h2{
  font-size:14px;
  color:#00aa55;
  margin-bottom:20px;
}

/* Inputs */
input{
  width:100%;
  padding:14px;
  margin:8px 0;
  font-size:16px;
  border:1px solid #00ff88;
  border-radius:5px;
  background:#001a00;
  color:#00ff88;
}

/* Botão */
button{
  width:100%;
  padding:15px;
  margin-top:10px;
  font-size:18px;
  background:#00ff88;
  color:black;
  border:none;
  border-radius:5px;
  font-weight:bold;
  cursor:pointer;
  transition:0.2s;
}

button:hover{
  background:#00cc66;
  box-shadow:0 0 10px #00ff88;
}

/* Rodapé */
.footer{
  margin-top:20px;
  font-size:12px;
  color:#555;
}

/* Link */
a{
  color:#00ff88;
  text-decoration:none;
}
</style>

</head>

<body>

<div class="box">

<h1>🛡️ Guardião de Rede</h1>
<h2>Claudio Almeida • CAAS</h2>

<form method="POST" action="/config">
<input name="ssid" placeholder="📡 SSID da Rede" required>
<input name="pass" type="password" placeholder="🔑 Senha WiFi" required>
<button>🔒 PROTEGER REDE</button>
</form>

<div class="footer">
<p>Guardião CAAS | 192.168.4.1</p>
<p><a href="https://github.com/claudioalmeida23/GuardiaoRedeESP8266">🔗 GitHub</a></p>
</div>

</div>

</body>
</html>
    )";
    server.send(200, "text/html", html);
  });

  server.on("/config", HTTP_POST, []() {
    server.arg("ssid").toCharArray(ssid, 32);
    server.arg("pass").toCharArray(pass, 32);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
    unsigned long t0 = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - t0 < 20000) delay(500);

    if (WiFi.status() == WL_CONNECTED) {
      memcpy(bssidLegitimo, WiFi.BSSID(), 6);
      salvarConfig();
      server.send(200, "text/html", 
        "<h1 style='color:lime'>✅ Guardiao de Rede ATIVO!<br>Criador:Claudio Almeida.CAAS</h1>");
      delay(3000);
      ESP.restart();
    } else {
      server.send(200, "text/html", 
        "<h1 style='color:red'>❌ WiFi falhou</h1><a href='/'>Tentar</a>");
    }
  });
  server.begin();
}


void evilCallback(int n) {
  bool cloneAtivo = false;
  
  for (int i = 0; i < n; i++) {
    if (WiFi.SSID(i) == String(ssid)) {
      if (memcmp(WiFi.BSSID(i), bssidLegitimo, 6) != 0) {
        cloneAtivo = true;
        break;
      }
    }
  }

  
  if (cloneAtivo && !evilDetectado) {
    evilDetectado = true;
    alertaDesde = millis();
    logar("🚨 EVIL TWIN!");
    digitalWrite(LED_ALERTA, LOW);
    
    String alerta = "🚨 EVIL TWIN!\nSSID: " + String(ssid) + 
                   "\nIP: " + WiFi.localIP().toString() + 
                   "\nGuardiao de Rede - Criador Claudio Almeida>CAAS";
    enviarTelegram(alerta);
  }
  // Reset automático após 5min
  else if (!cloneAtivo && evilDetectado && 
           (millis() - alertaDesde > RESET_ALERTA)) {
    evilDetectado = false;
    logar("✅ Clone sumiu - Normal");
  }

  WiFi.scanDelete();
}


void setup() {
  Serial.begin(115200);
  pinMode(LED_ALERTA, OUTPUT);
  digitalWrite(LED_ALERTA, HIGH);
  logar("BOOT Guardiao de Rede");

  if (!temConfig()) {
    iniciarAPConfig();
    return;
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  unsigned long t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 20000) {
    delay(500);
  }

  if (WiFi.status() != WL_CONNECTED) {
    logar("❌ WiFi ➜ Config");
    iniciarAPConfig();
    return;
  }

  logar("✅ Guardiao de Rede ATIVO");

  server.on("/", []() {
    String status = "Guardiao de Rede - Criador Claudio Almeida>CAAS\n";
    status += "IP: " + WiFi.localIP().toString() + "\n";
    status += "Evil: " + String(evilDetectado ? "SIM" : "NAO") + "\n";
    status += "Prox scan: " + String((ultimoScan - millis()) / 1000) + "s";
    server.send(200, "text/plain", status);
  });

  server.begin();
}


void loop() {
  server.handleClient();

  /* LED INTELIGENTE */
  unsigned long intervaloLed = evilDetectado ? 250 : 2000;
  if (millis() - ultimoPisca > intervaloLed) {
    ultimoPisca = millis();
    digitalWrite(LED_ALERTA, !digitalRead(LED_ALERTA));
  }

  /* SCAN ALEATÓRIO  */
  if (millis() - ultimoScan > 60000) {
    ultimoScan = millis() + random(60000, 180000);  // 1-3min EXATO
    WiFi.scanNetworksAsync(evilCallback, true);
    logar("🔍 Scan...");
  }

  delay(50);
}
