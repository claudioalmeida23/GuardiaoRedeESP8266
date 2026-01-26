🛡️ Guardião de Rede – Stealth (ESP8266)

Autor: Claudio Almeida
Plataforma: ESP8266
Licença: MIT

O Guardião de Rede é um sistema de detecção de intrusão (IDS) de baixo custo, desenvolvido para a plataforma ESP8266. Ele foi criado com um objetivo claro: proteger usuários comuns contra ataques de redes falsas (Evil Twin).

Muitas vezes, atacantes criam redes Wi-Fi com o mesmo nome da sua para roubar senhas. O Guardião monitora o ambiente 24h por dia e avisa você fisicamente (via som e luz) se uma rede "clone" aparecer.

✨ Principais Funcionalidades
Detecção de Evil Twin: Identifica redes clonadas comparando o endereço físico (BSSID) do roteador real com os sinais encontrados no ar.

Modo Stealth (Furtivo): O dispositivo opera de forma silenciosa, realizando varreduras aleatórias para não ser detectado pelo invasor.

Blindagem Reativa: Caso um ataque seja detectado, o dispositivo "tranca" suas próprias credenciais na memória para evitar extração de dados.

Registro Forense: Salva o endereço MAC do invasor permanentemente na memória EEPROM para análise posterior.

Alerta Físico: Sistema de Beeps e LED para avisar o usuário imediatamente, sem a necessidade de telas ou computadores.

🚀  1. Como Funciona?
O Guardião não confia apenas no nome da rede (SSID), mas sim na "impressão digital" do seu roteador (BSSID).

Configuração: Na primeira vez que ligar, ele cria uma rede chamada Guardiao_Config. Você conecta, digita o nome e a senha da sua rede.

Aprendizado: Ele se conecta ao seu Wi-Fi e "anota" quem é o roteador legítimo.

Vigilância: Ele entra em modo sentinela. Se alguém subir uma rede com o mesmo nome, mas o "RG" (MAC) for diferente, o alarme toca!

┌──────────────────────────┐
│ ESP cria AP               │
│ SSID: Guardiao_Config     │
│ Senha: admin12345         │
└───────────┬──────────────┘
            ▼
┌──────────────────────────┐
│ Interface Web             │
│ - SSID                    │
│ - Senha                   │
└───────────┬──────────────┘
            ▼
┌──────────────────────────┐
│ Testa conexão Wi-Fi       │
└───────────┬──────────────┘
            ▼
┌──────────────────────────┐
│ Conectou?                 │
└──────┬──────────┬────────┘
       │ NÃO       │ SIM
       ▼           ▼
┌────────────┐  ┌──────────────────────────┐
│ Mostra erro │  │ Salva SSID, senha e BSSID │
└────────────┘  │ Ofusca dados (XOR)        │
                │ Reinicia ESP               │
                └──────────────────────────┘


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


🛠️ Materiais Necessários
1x ESP8266 (NodeMCU ou Wemos D1 Mini)

1x Buzzer Ativo (Pino D8 / GPIO 14)

1x LED (Embutido no pino D4 / GPIO 2)

Cabo USB para alimentação

💻 Instalação
Abra o código na Arduino IDE.

Instale as bibliotecas padrão do ESP8266.

Carregue o código para o seu dispositivo.

Abra o Monitor Serial (115200 baud) para acompanhar os primeiros logs.

📜 Licença e Ética
Este projeto é distribuído sob a licença MIT. Ele foi criado estritamente para fins defensivos e educacionais. O autor não se responsabiliza pelo uso indevido da ferramenta.

Dica de Segurança: Mantenha seu Guardião escondido próximo ao seu roteador. Se ele apitar, desligue seu Wi-Fi e verifique se há dispositivos estranhos por perto.




