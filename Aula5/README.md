# Lab 5 - Controle de Periféricos com PWM e ESP32-C3

Este repositório contém os códigos desenvolvidos para o experimento de controle de intensidade de LED e posição de servomotor usando PWM no ESP32-C3, com interação por meio de uma interface web local.


### RF1 - Controle de brilho do LED

O sistema deve permitir alterar o brilho do LED externo por meio da interface web.

**Teste realizado:** alterar o slider de brilho entre 0%, 50% e 100%.

**Resultado esperado:** o LED deve variar sua intensidade luminosa conforme o valor selecionado.

**Vídeo de demonstração:**


---

### RF2 - Controle da frequência do PWM do LED

O sistema deve permitir alterar a frequência do sinal PWM usado no LED.

**Teste realizado:** alterar a frequência entre diferentes valores, como 50 Hz, 1000 Hz e 5000 Hz.

**Resultado esperado:** o sistema deve atualizar a frequência do PWM sem travar, mantendo o LED funcionando.

**Vídeo de demonstração:**

---

### RF3 - Controle de posição do servomotor

O sistema deve permitir alterar a posição do servomotor pela interface web.

**Teste realizado:** mover o slider do servo para 0°, 90° e 180°.

**Resultado esperado:** o servomotor deve se mover para posições próximas aos ângulos definidos.

**Vídeo de demonstração:**


---

### RNF1 - Usabilidade da interface web

A interface deve ser simples e acessível por navegador, sem necessidade de aplicativo adicional.

**Teste realizado:** conectar um computador ou celular à rede Wi-Fi criada pelo ESP32 e acessar a página de controle.

**Resultado esperado:** a página deve abrir corretamente e apresentar os controles de LED, frequência e servo.

**Vídeo de demonstração:**

---

### RNF2 - Desempenho

O sistema deve responder aos comandos da interface em tempo adequado, sem atraso perceptível durante o uso normal.

**Teste realizado:** alterar os sliders e observar o tempo de resposta do LED e do servo.

**Resultado esperado:** os periféricos devem responder logo após a alteração dos controles.

**Vídeo de demonstração:**


---

### RNF3 - Confiabilidade

O sistema deve permanecer funcionando durante testes repetidos, sem reiniciar ou travar.

**Teste realizado:** manter o sistema ligado por alguns minutos, alterando repetidamente brilho, frequência e posição do servo.

**Resultado esperado:** o ESP32 deve continuar respondendo às requisições e controlando os periféricos.

**Vídeo de demonstração:**

