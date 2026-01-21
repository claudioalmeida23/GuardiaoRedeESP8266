🛡️ Guardião de Rede – ESP8266

O Guardião de Rede é um projeto de monitoramento passivo de segurança Wi-Fi desenvolvido para o ESP8266.
Ele atua como um sistema de alerta que avisa o usuário quando ocorre uma situação suspeita na rede, como a possível presença de um Evil Twin (ponto de acesso falso usando o mesmo nome da rede legítima).

⚠️ Este projeto não protege ativamente a rede.
Ele não bloqueia dispositivos, não reage a ataques e não interfere no tráfego.
Seu papel é avisar, deixando a decisão e a ação nas mãos do usuário.

🎯 Objetivo do Projeto

O Guardião de Rede foi criado para:

Monitorar continuamente a rede Wi-Fi conectada

Detectar indícios simples de ataques do tipo Evil Twin

Alertar o usuário de forma sonora e visual

Funcionar em hardware limitado, de forma estável e confiável

Ele foi pensado para ambientes domésticos e pequenos escritórios, como um alarme de rede.

🧠 Como o Guardião Funciona

O ESP8266 se conecta a uma rede Wi-Fi configurada pelo usuário.

Ele identifica o SSID e o BSSID (MAC do ponto de acesso legítimo).

Durante o funcionamento, o sistema monitora a rede.

Se for detectado:

O mesmo SSID

Com um BSSID diferente

O sistema interpreta isso como um possível Evil Twin e dispara um alerta.

📌 O Guardião não confirma se o ataque é real.
Ele apenas detecta a condição suspeita e avisa.

🚨 Sistema de Alertas

Quando uma anomalia é detectada, o Guardião responde com:

🔴 Alerta Visual

LED onboard:

Piscando → funcionamento normal

Aceso fixo → alerta ativo

🔊 Alerta Sonoro

Buzzer:

Emite sinal sonoro contínuo ou intermitente

Funciona como aviso imediato local

🖥️ Logs

Mensagens exibidas via Serial Monitor

Informações úteis para diagnóstico e testes

⚙️ Funcionalidades

Conexão automática à rede Wi-Fi salva na EEPROM

Modo de configuração via Access Point próprio

Detecção básica de Evil Twin (SSID igual, BSSID diferente)

Alertas sonoros e visuais em tempo real

Persistência de configurações após reinicialização

Funcionamento contínuo e simples

🌐 Modo Configuração

No primeiro uso ou caso falhe a conexão Wi-Fi:

O ESP8266 cria um ponto de acesso:

SSID: Guardiao_Config

Senha: admin12345

Uma página web simples permite:

Informar SSID da rede

Informar senha

Após salvar, o dispositivo reinicia automaticamente

🔌 Hardware Utilizado

ESP8266 (NodeMCU, Wemos D1 Mini, similares)

LED onboard (GPIO2)

Buzzer simples (GPIO14 / D5)

📌 Nenhum hardware adicional complexo é necessário.

❌ O Que Este Projeto NÃO Faz

Para evitar interpretações incorretas, o Guardião não:

Bloqueia invasores

Desconecta dispositivos da rede

Executa ataques ou contra-ataques

Analisa pacotes de dados

Substitui firewall, roteador ou IDS profissional

Ele é um sistema de alerta, não um sistema de defesa ativa.

⚡ Instalação

Instale o Arduino IDE com suporte ao ESP8266

Abra o arquivo GuardiaoRede.ino

Conecte o ESP8266 ao computador

Selecione a placa e a porta corretas

Compile e faça o upload

No primeiro boot, conecte-se ao Wi-Fi Guardiao_Config

Configure sua rede

O sistema inicia o monitoramento automaticamente

🧩 Estrutura do Projeto

src/GuardiaoRede.ino → Código principal

README.md → Documentação

LICENSE → Licença do projeto

📜 Licença

Este projeto é distribuído sob a Licença MIT.
Você é livre para usar, modificar e distribuir, desde que mantenha os créditos ao autor.

Veja o arquivo LICENSE para mais detalhes.

👤 Autor

Claudio Almeida
Criador do projeto Guardião de Rede – ESP8266


(Seja Ético)
