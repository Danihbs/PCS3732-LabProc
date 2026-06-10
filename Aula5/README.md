# Lab 5 - Controle de Periféricos com PWM e ESP32-C3

Este repositório contém os códigos desenvolvidos para o experimento de controle de intensidade de LED e posição de servomotor usando PWM no ESP32-C3, com interação por meio de uma interface web local.

###


### RF1 - Controle de brilho do LED

O sistema deve permitir alterar o brilho do LED externo por meio da interface web.

**Teste realizado:** alterar o slider de brilho entre 0%, 50% e 100%.

**Resultado esperado:** o LED deve variar sua intensidade luminosa conforme o valor selecionado.

**Vídeo de demonstração:**

https://github.com/user-attachments/assets/fb1608d3-0694-40a2-b62d-bf0f78d3b7e1

https://github.com/user-attachments/assets/88b5dc72-42de-4962-81ab-a619fea1bfdd

https://github.com/user-attachments/assets/88d1e91d-a318-460f-9e6d-8ec2ae3fbd9c

---

### RF2 - Controle da frequência do PWM do LED

O sistema deve permitir alterar a frequência do sinal PWM usado no LED.

**Teste realizado:** alterar a frequência entre diferentes valores, como 50 Hz, 1000 Hz e 5000 Hz.

**Resultado esperado:** o sistema deve atualizar a frequência do PWM sem travar, mantendo o LED funcionando.

**Vídeo de demonstração:**

https://github.com/user-attachments/assets/fc9b0b19-1ecd-4cd7-9128-3ebc9658f19e

---

### RF3 - Controle de posição do servomotor

O sistema deve permitir alterar a posição do servomotor pela interface web.

**Teste realizado:** mover o slider do servo para 0° e 90°

**Resultado esperado:** o servomotor deve se mover para posições próximas aos ângulos definidos.

**Vídeo de demonstração:**

https://github.com/user-attachments/assets/f0b1d99f-68a1-4082-b282-956b18943844

https://github.com/user-attachments/assets/0c6af831-9e14-49b6-8ea9-542ba8789e96

---
### RF4 - Implementação integrada

O sistema deve permitir alterar a posição do servomotor além da intensidade e frequência do led pela mesma interface web.

**Teste realizado:** mover o slider do servo para 0°, 90° e 180°, alterar a frequência entre diferentes valores, como 50 Hz, 1000 Hz e 5000 Hz e alterar o slider de brilho entre 0%, 50% e 100%.

**Resultado esperado:** o servomotor deve se mover para posições próximas aos ângulos definidos e o led deve alterar seu brilho e frequência conforme os valroes especificados

**Vídeo de demonstração:**

https://github.com/user-attachments/assets/ee7ff6c4-e970-4345-afbc-c64548f385c8

---

### RNF1 - Usabilidade da interface web

A interface deve ser simples e acessível por navegador, sem necessidade de aplicativo adicional.

**Teste realizado:** conectar um computador ou celular à rede Wi-Fi criada pelo ESP32 e acessar a página de controle.

**Resultado esperado:** a página deve abrir corretamente e apresentar os controles de LED, frequência e servo.

**Demonstrado nos vídeos anteriores**

---

### RNF2 - Desempenho

O sistema deve responder aos comandos da interface em tempo adequado, sem atraso perceptível durante o uso normal.

**Teste realizado:** alterar os sliders e observar o tempo de resposta do LED e do servo.

**Resultado esperado:** os periféricos devem responder logo após a alteração dos controles.

**Demonstrado nos vídeos anteriores**


---

### RNF3 - Confiabilidade

O sistema deve permanecer funcionando durante testes repetidos, sem reiniciar ou travar.

**Teste realizado:** manter o sistema ligado por alguns minutos, alterando repetidamente brilho, frequência e posição do servo.

**Resultado esperado:** o ESP32 deve continuar respondendo às requisições e controlando os periféricos.

**Demonstrado nos vídeos anteriores**

