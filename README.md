# 🛡️ Guardião de Rede – Stealth (ESP8266)

**Autor:** Claudio Almeida  
**Plataforma:** ESP8266  
**Licença:** MIT  

O **Guardião de Rede – Stealth** é um sistema de detecção de intrusão (IDS) de baixo custo, desenvolvido para a plataforma **ESP8266**.  
Ele foi criado com um objetivo claro: **proteger usuários comuns contra ataques de redes Wi-Fi falsas (Evil Twin)**.

Atacantes frequentemente criam redes Wi-Fi com o mesmo nome (SSID) da sua para enganar usuários e roubar senhas.  
O Guardião monitora o ambiente continuamente e **avisa fisicamente (LED e buzzer)** caso uma rede clonada seja detectada.

---

## ✨ Principais Funcionalidades

### 🔍 Detecção de Evil Twin
Identifica redes clonadas comparando:
- **SSID** (nome da rede)
- **BSSID** (MAC real do roteador)

Se o nome for igual e o BSSID diferente, o sistema sinaliza possível ataque.

### 🕵️ Modo Stealth (Furtivo)
- Opera de forma **passiva**
- Apenas escuta o ambiente
- Não interfere em redes
- Evita padrões previsíveis de varredura

### 🛡️ Blindagem Reativa
- Ao detectar uma anomalia, reforça a proteção dos dados salvos
- Impede sobrescritas acidentais na memória

### 📁 Registro Forense
- Armazena o **BSSID suspeito** na EEPROM
- Permite análise posterior via Serial Monitor

### 🚨 Alerta Físico
- **LED onboard** indica status do sistema
- **Buzzer ativo** alerta imediatamente o usuário
- Não depende de telas, apps ou servidores

---

## 🚀 Como Funciona

### 1️⃣ Configuração Inicial
Na primeira inicialização:
- O ESP8266 cria uma rede Wi-Fi chamada **Guardiao_Config**
- O usuário conecta e informa:
  - SSID da rede legítima
  - Senha do Wi-Fi

### 2️⃣ Aprendizado
- O dispositivo conecta ao Wi-Fi informado
- Registra o **BSSID legítimo** do roteador

### 3️⃣ Vigilância
- Entra em modo sentinela
- Executa varreduras passivas periódicas
- Se surgir um SSID igual com BSSID diferente:
  - Ativa alerta
  - Registra o invasor
  - Mantém vigilância contínua

---

## 💾 Armazenamento em EEPROM

Utiliza **EEPROM interna (512 bytes)** para persistência de dados.

### Dados armazenados:
- Flag de configuração válida
- SSID
- Senha
- BSSID legítimo
- Flag de invasão detectada
- BSSID suspeito

### 🔐 Ofuscação
- SSID e senha são ofuscados usando **XOR simples**
- ⚠️ **Não é criptografia forte**
- Serve apenas para dificultar leitura direta

---

## ❌ O QUE ESTE PROJETO NÃO FAZ

Este ponto é essencial para evitar interpretações erradas.

### ❌ NÃO ataca redes
- Não desassocia clientes
- Não interfere em roteadores
- Não envia pacotes maliciosos

### ❌ NÃO invade redes
- Não quebra senhas
- Não executa força bruta
- Não captura handshakes

### ❌ NÃO é um sistema profissional
- Não substitui WIDS/WIPS corporativos
- Não é firewall
- Não é solução comercial

### ❌ NÃO usa criptografia forte
- Sem AES, RSA, TLS ou similares
- Não protege contra acesso físico ao chip

### ❌ NÃO garante que todo SSID duplicado seja ataque
- Redes legítimas podem compartilhar SSID
- A detecção é **heurística simples (SSID + BSSID)**

### ❌ NÃO envia dados externos
- Sem internet
- Sem servidores
- Logs apenas via Serial Monitor

---

## 🕵️ Detecção de Evil Twin (Passiva)

O sistema executa varreduras periódicas e verifica:

- Mesmo SSID configurado
- BSSID diferente do roteador legítimo

### Ao detectar:
- Salva o BSSID suspeito na EEPROM
- Ativa LED de alerta
- Emite sinal sonoro
- Mantém estado de alerta

---

## ⏱️ Intervalo de Varredura Aleatório

Para evitar padrões previsíveis, o tempo entre scans é aleatório:

- 1 minuto  
- 2 minutos  
- 3 minutos  

O intervalo é sorteado automaticamente a cada ciclo.

---

## 🚨 Alertas Visuais e Sonoros

### LED (D4 / GPIO2)
- Pisca lentamente: funcionamento normal
- Aceso fixo: alerta ou falha

### Buzzer (D8 / GPIO15)
- Beeps curtos: falha de conexão
- Beeps longos: possível Evil Twin detectado

---

## 🛠️ Materiais Necessários

- 1× ESP8266 (NodeMCU ou Wemos D1 Mini)
- 1× Buzzer ativo (D8 / GPIO15)
- LED onboard (D4 / GPIO2)
- Cabo USB para alimentação

---

## 💻 Instalação

1. Abra o código no **Arduino IDE**
2. Instale as bibliotecas padrão do ESP8266
3. Compile e grave no dispositivo
4. Abra o **Serial Monitor (115200 baud)**
5. Realize a configuração inicial

---

## 📜 Licença e Ética

Este projeto é distribuído sob a **Licença MIT**.

Foi desenvolvido **exclusivamente para fins defensivos e educacionais**.  
O autor **não se responsabiliza por uso indevido**.

### ⚠️ Dica de Segurança
Mantenha o Guardião escondido próximo ao seu roteador.  
Se o alerta disparar:
- Desligue o Wi-Fi
- Verifique redes suspeitas
- Reconfigure seu roteador se necessário
  

Antes de explorar uma falha, pense em quem está do outro lado.
