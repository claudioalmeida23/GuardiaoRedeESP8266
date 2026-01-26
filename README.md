🛡️ Guardião de Rede – Stealth (ESP8266)

Autor: Claudio Almeida
Plataforma: ESP8266
Licença: MIT

O Guardião de Rede – Stealth é um firmware experimental para ESP8266 projetado para monitoramento passivo de redes Wi-Fi, com foco na detecção de possíveis ataques do tipo Evil Twin, utilizando comparação de SSID + BSSID.

✅ O QUE ESTE CÓDIGO FAZ
🔐 1. Configuração inicial via Access Point

Cria um AP local chamado Guardiao_Config caso não exista configuração salva.

Interface web simples para:

Inserir SSID

Inserir senha

Após conectar com sucesso:

Salva SSID e senha na EEPROM

Registra o BSSID legítimo do ponto de acesso conectado

💾 2. Armazenamento em EEPROM

Usa EEPROM interna (512 bytes) para persistência de dados.

Armazena:

Flag de configuração válida

SSID

Senha

BSSID legítimo

Flag de invasor detectado

BSSID do possível invasor

SSID e senha são ofuscados com XOR simples antes de serem gravados.

⚠️ A ofuscação NÃO é criptografia forte, apenas dificulta leitura direta.

🕵️ 3. Detecção de Evil Twin (Passiva)

Executa scans periódicos de redes Wi-Fi.

Se encontrar uma rede com:

Mesmo SSID configurado

BSSID diferente do legítimo

O sistema considera como possível Evil Twin.

Ações ao detectar:

Registra o BSSID suspeito na EEPROM

Aciona LED de alerta

Emite sinais sonoros (buzzer)

Executa uma reblindagem da EEPROM

⏱️ 4. Scans com intervalo aleatório

O tempo entre scans é randomizado:

60s

120s

180s

Isso evita padrões previsíveis de monitoramento.

🚨 5. Alertas visuais e sonoros

LED onboard indica:

Conectado: pisca lento

Alerta ou falha: LED aceso contínuo

Buzzer emite:

Alertas de falha de Wi-Fi

Detecção de possível Evil Twin

🧠 6. Reblindagem da EEPROM

Ao detectar um Evil Twin:

Os dados salvos são lidos

Re-ofuscados

Gravados novamente

Objetivo: dificultar leitura direta após eventos suspeitos.

❌ O QUE ESTE CÓDIGO NÃO FAZ

Este ponto é essencial para evitar interpretações erradas.

❌ NÃO bloqueia ataques

O ESP8266 não interfere, não desassocia clientes, não desliga redes.

Atua apenas como sensor de monitoramento.

❌ NÃO invade redes

Não quebra senhas

Não força conexões

Não executa brute-force

Não captura handshakes

❌ NÃO é um IDS/IPS profissional

Não substitui:

Firewalls

Sistemas corporativos de segurança

WIDS/WIPS comerciais

É um projeto educacional e experimental.

❌ NÃO usa criptografia forte

A proteção da EEPROM é apenas ofuscação XOR

Não há AES, RSA, TLS ou similares

Não protege contra acesso físico ao chip

❌ NÃO garante que todo SSID duplicado seja ataque

Redes com mesmo SSID podem existir legitimamente

A detecção é baseada em heurística simples (SSID + BSSID)

❌ NÃO registra logs externos

Não envia dados para servidores

Não possui dashboard remoto

Logs são apenas via Serial Monitor

