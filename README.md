# 🛡️ Guardião de Rede – ESP8266

O **Guardião de Rede** é um projeto para ESP8266 que monitora sua rede Wi-Fi, detectando **Evil Twins** (pontos de acesso falsos) e alertando com **LED e buzzer**. Inclui modo de configuração via ponto de acesso e logs em tempo real.

---

## ⚙️ Funcionalidades

- Conexão automática a Wi-Fi salvo na EEPROM
- Detecção de **Evil Twin** (mesmo SSID, BSSID diferente)
- Alertas:
  - LED onboard (piscando = ok, aceso = alerta)
  - Buzzer (padrão PC speaker)
- Modo configuração via **AP próprio**:
  - SSID: `Guardiao_Config`
  - Senha: `admin12345`
  - Página web simples para salvar SSID e senha
- Persistência de configuração via EEPROM

---

## 🔌 Hardware

- ESP8266 (NodeMCU, Wemos D1 Mini, etc.)
- LED onboard (GPIO2)
- Buzzer (D5 / GPIO14)

---

## 📝 Código

O arquivo principal está em `src/GuardiaoRede.ino`.

---

## ⚡ Instalação

1. Instale o **Arduino IDE** com suporte ao ESP8266.
2. Abra `GuardiaoRede.ino`.
3. Conecte seu ESP8266 ao computador.
4. Selecione **placa** e **porta** corretas.
5. Compile e faça o upload.
6. Primeiro boot: ESP abre AP `Guardiao_Config` para configurar Wi-Fi.
7. Após salvar, ESP reinicia e entra em modo stealth.

---



## 📜 Licença

MIT License – veja arquivo (LICENSE).

---

## 👤 Autor

Claudio Almeida – Criador do Guardião de Rede
