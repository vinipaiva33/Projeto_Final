# Projeto: Máquina de Conversões com RP2040 e Display OLED

## Descrição Geral

Este projeto implementa uma **Máquina de Conversões** utilizando a placa **BitDogLab com o microcontrolador RP2040**. A interface do sistema é composta por um **display OLED SSD1306**, um **joystick analógico**, botões físicos e um **buzzer** para feedback sonoro. O código foi desenvolvido utilizando **Pico SDK** no **VS Code** e é capaz de realizar conversões entre múltiplas unidades de medida.

## Objetivos do Projeto

- **Fornecer uma interface interativa para conversão de unidades** de temperatura, comprimento, massa, área, tempo, volume e dados digitais.
- **Navegação intuitiva por meio de um joystick analógico**, permitindo a seleção de opções no menu.
- **Botões físicos para confirmar e retroceder ações**, proporcionando uma experiência de usuário mais fluida.
- **Indicação sonora e visual para cada ação do usuário**, utilizando um buzzer e LEDs RGB para feedback imediato.
- **Exibição clara das informações no display OLED SSD1306**, garantindo legibilidade e precisão nos resultados apresentados.

## Componentes Utilizados

- **Microcontrolador**: RP2040 (BitDogLab)
- **Display OLED SSD1306**: Comunicação via I2C (GPIO 14 - SDA, GPIO 15 - SCL)
- **Joystick Analógico**: Leitura via ADC (GPIO 26 - Eixo X, GPIO 27 - Eixo Y)
- **Buzzer**: Emissão de tons específicos para diferentes interações (GPIO 10)
- **LEDs RGB**:
  - **LED Azul (GPIO 12)**: Indica que o sistema está em funcionamento.
  - **LED Vermelho (GPIO 13)**: Acende durante a entrada de valores.
  - **LED Verde (GPIO 11)**: Indica a finalização da conversão e exibição do resultado.
- **Botões**:
  - **Botão A (GPIO 5)**: Confirma seleção/avanço no menu.
  - **Botão B (GPIO 6)**: Retorna à etapa anterior no menu.

## Funcionalidades Implementadas

### 1. **Navegação por Joystick**
- O joystick permite deslocamento pelo menu e seleção das opções desejadas.
- Movimentos no **eixo X** alteram o tipo de conversão selecionado.
- Movimentos no **eixo Y** percorrem as opções dentro de cada categoria.

### 2. **Confirmação e Retorno com Botões Físicos**
- O **Botão A (GPIO 5)** é utilizado para confirmar seleções e avançar entre as telas do sistema.
- O **Botão B (GPIO 6)** permite o retorno ao menu anterior.
- Cada ação de botão é acompanhada por um **feedback sonoro** com diferentes frequências emitidas pelo buzzer.

### 3. **Indicação Visual e Sonora das Ações**
- O **LED Azul** permanece **sempre aceso** enquanto o sistema está em execução.
- Durante a entrada de valores para conversão, o **LED Vermelho** acende.
- Quando o resultado da conversão é exibido, o **LED Verde** é acionado.
- O **buzzer emite tons específicos** ao pressionar botões e concluir ações.

### 4. **Conversões de Unidades**
O sistema suporta conversões entre diferentes unidades nas seguintes categorias:
- **Temperatura**: Celsius, Kelvin, Fahrenheit.
- **Comprimento**: Quilômetro, metro, centímetro, milímetro, micrômetro.
- **Massa**: Tonelada, quilograma, grama, miligrama.
- **Área**: Metro quadrado, centímetro quadrado, milímetro quadrado, hectare.
- **Tempo**: Segundos, minutos, horas, dias, semanas.
- **Volume**: Litros, mililitros, centímetros cúbicos, metros cúbicos.
- **Dados Digitais**: Bits, kilobytes, megabytes, gigabytes, terabytes.

## Como Executar o Projeto

### 1. **Configuração do Ambiente**
- Instale o **VS Code** e configure o **Pico SDK**.
- Conecte a placa **BitDogLab (RP2040)** ao computador via USB.

### 2. **Compilação e Execução**
- Compile o código-fonte e gere um arquivo `.uf2`.
- Transfira o arquivo para a placa **BitDogLab**.
- Utilize um **Serial Monitor** para depuração, se necessário.

### 3. **Testes e Depuração**
- Movimente o **joystick** para navegar entre as opções de conversão.
- Pressione **Botão A** para confirmar seleções.
- Pressione **Botão B** para retornar ao menu anterior.
- Observe as alterações nos LEDs e no display OLED conforme as ações são realizadas.
- Ouça os sinais sonoros emitidos pelo **buzzer** ao interagir com o sistema.

## Estrutura do Código-Fonte

- `main.c`: Código principal contendo a lógica de navegação e conversão.
- `ssd1306.h / ssd1306.c`: Biblioteca para controle do display OLED SSD1306.
- `conversoes.h / conversoes.c`: Implementação das funções matemáticas de conversão.
- `font.h`: Arquivo da fonte do display.
- `README.md`: Documentação detalhada do projeto.

## Vídeo de Demonstração

https://drive.google.com/file/d/1KWQH45EO5_TZXgoK61r35ncPMWF0gAh1/view?usp=drivesdk

## Considerações Finais

Este projeto demonstra **o poder do RP2040** em aplicações interativas, combinando **hardware e software para uma experiência fluida** de conversão de unidades. A interface intuitiva e os efeitos visuais e sonoros tornam a experiência do usuário **mais acessível e responsiva**. Caso deseje expandir as funcionalidades, novas unidades de conversão podem ser adicionadas facilmente, mantendo a modularidade do código.

**Desenvolvido por:** Vinicius Paiva de Oliveira

