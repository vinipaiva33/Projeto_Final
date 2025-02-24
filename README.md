# Projeto: Máquina de Conversões com RP2040

## Descrição Geral

Este projeto implementa uma **Máquina de Conversões** utilizando a placa **BitDogLab** com microcontrolador **RP2040**. O sistema permite a conversão entre diversas unidades de medida, proporcionando uma interface interativa por meio de um **display OLED SSD1306**, um **joystick analógico** e botões físicos. O projeto também inclui **efeitos sonoros e visuais** com um **buzzer** e um **LED RGB** para fornecer feedback ao usuário.

## Objetivos

- **Facilitar a conversão entre diferentes unidades** de temperatura, comprimento, massa, área, tempo, volume e dados digitais.
- **Fornecer uma interface intuitiva** para seleção de tipos e unidades de conversão.
- **Implementar feedback visual e sonoro** utilizando um LED RGB e um buzzer.

## Componentes Utilizados

- **Microcontrolador**: RP2040 (Placa BitDogLab)
- **Display OLED SSD1306**: Interface via I2C (GPIO 14 - SDA, GPIO 15 - SCL)
- **Joystick Analógico**: Leitura via ADC (GPIO 26 - Eixo X, GPIO 27 - Eixo Y)
- **Botões**:
  - **Botão A** (GPIO 5): Confirma a seleção
  - **Botão B** (GPIO 6): Retorna ao menu anterior
- **Buzzer**: Indica confirmação de seleção e erros
- **LED RGB**: Indica estados do sistema

## Funcionalidades Implementadas

### 1. Navegação por Menu

- O **joystick** é utilizado para navegar entre os menus e selecionar opções.
- O **Botão A** confirma a seleção.
- O **Botão B** permite retornar ao menu anterior.

### 2. Seleção de Tipo de Conversão

- **Temperatura** (Celsius, Kelvin, Fahrenheit)
- **Comprimento** (km, m, cm, mm, µm)
- **Massa** (toneladas, kg, g, mg)
- **Área** (m², cm², mm², hectares)
- **Tempo** (segundos, minutos, horas, dias, semanas)
- **Volume** (litros, mililitros, cm³, m³)
- **Dados Digitais** (bits, KB, MB, GB, TB)

### 3. Conversão de Unidades

- O usuário escolhe uma unidade inicial e uma unidade destino.
- O valor é inserido via joystick.
- A conversão é realizada automaticamente e exibida no display.

### 4. Feedback Sonoro e Visual

- **Buzzer**:
  - Som curto ao confirmar seleção
  - Som prolongado para erros
- **LED RGB**:
  - Verde: Operação bem-sucedida
  - Vermelho: Erro na entrada
  - Azul: Sistema aguardando interação

## Como Executar o Projeto

### 1. Configuração do Ambiente

- Instale o **VS Code** e configure o **Pico SDK**.
- Conecte a placa **BitDogLab** ao computador via USB.

### 2. Compilação e Execução

- Compile o código e gere um arquivo `.uf2`.
- Transfira o arquivo para a placa **BitDogLab**.
- Utilize o **Serial Monitor** do VS Code para monitorar mensagens do programa.

### 3. Testes e Depuração

- Navegue pelo menu e selecione um tipo de conversão.
- Escolha as unidades e insira um valor.
- Verifique a resposta no display.
- Observe o feedback sonoro e visual.

## Estrutura do Projeto

- `Maquina_de_conversoes.c` Código principal do projeto.
- `ssd1306.h / ssd1306.c`: Biblioteca para controle do display OLED SSD1306.
- `conversoes.h / conversoes.c`: Implementação das funções de conversão.
- `font.h`: Arquivo da fonte do display.
- `README.md`: Documentação do projeto.

## Espaço para Vídeo de Demonstração

[ESPAÇO PARA VIDEO]

