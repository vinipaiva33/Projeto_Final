#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include <string.h>
#include <math.h>
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "inc/conversoes.h"

// Definição dos pinos
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define ENDERECO_SSD1306 0x3C

#define BUTTON_SELECT 5  // Botão para seleção
#define BUTTON_BACK 6  // Botão será usado para voltar no menu
#define VRX_PIN 26  // Eixo X do Joystick (ADC0)
#define VRY_PIN 27  // Eixo Y do Joystick (ADC1)

#define DEADZONE 100  // Faixa de tolerância para considerar o joystick centralizado
#define CENTER_VALUE 2048  // Valor central do ADC

// Instância do display OLED
ssd1306_t ssd; 

// Variáveis globais
bool menu_ativo = true;  // Define o menu inicial como ativo
int tipo = 0;       // Tipo de conversão selecionado
int escolha = 0;    // Escolha da unidade inicial
int conversor = 0;  // Unidade para conversão
double numero = 0;   // Valor a ser convertido
bool precisa_atualizar = true;
int op1 = 0, op2 = 0, op3 = 0, op4 = 0;  // Inicializa as opções da conversão
int opcoes_filtradas[4];  // Global para armazenar opções dinâmicas
int total_opcoes_filtradas = 0; // Guarda quantas opções estão disponíveis
int total_opcoes = 0;  // Número total de opções da lista atual

// Sistema de Estados
enum Estados {
    MENU_INICIAL,
    SELECAO_TIPO,
    SELECAO_UNIDADE,
    SELECAO_CONVERSAO,
    ENTRADA_VALOR,
    EXIBIR_RESULTADO // 🔥 Novo estado adicionado
};

enum Estados estado = MENU_INICIAL;  // Começa na tela inicial

// Função para inicializar o I2C e o display OLED
void init_display() {
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init(&ssd, 128, 64, false, ENDERECO_SSD1306, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
}

// Função para inicializar os pinos dos botões e ADC do joystick
void init_peripherals() {
    gpio_init(BUTTON_SELECT);
    gpio_set_dir(BUTTON_SELECT, GPIO_IN);
    gpio_pull_up(BUTTON_SELECT);

    gpio_init(BUTTON_BACK);
    gpio_set_dir(BUTTON_BACK, GPIO_IN);
    gpio_pull_up(BUTTON_BACK);


    adc_init();
    adc_gpio_init(VRX_PIN);
    adc_gpio_init(VRY_PIN);
}

// Função para mapear valores do analógico
int map_value(int value, int fromLow, int fromHigh, int toLow, int toHigh) {
    return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
}

void display_pagina_inicial() {
    ssd1306_fill(&ssd, false);  // Limpa o display
    ssd1306_rect(&ssd, 3, 3, 122, 60, true, false);  // Desenha um retângulo
    ssd1306_draw_string(&ssd, "Maquina de", 25, 20);
    ssd1306_draw_string(&ssd, "Conversoes", 25, 35);
    ssd1306_send_data(&ssd);  // Atualiza o display
}

void display_tipo_conversao() {
    ssd1306_fill(&ssd, false);  // Limpa o display

    switch (tipo) {
        case 0:
            ssd1306_draw_string(&ssd, " Tipo: ", 40, 5);
            ssd1306_draw_string(&ssd, "< Temperatura >", 5, 30);
            break;
        case 1:
            ssd1306_draw_string(&ssd, " Tipo: ", 40, 5);
            ssd1306_draw_string(&ssd, "< Comprimento >", 5, 30);
            break;
        case 2:
            ssd1306_draw_string(&ssd, " Tipo: ", 40, 5);
            ssd1306_draw_string(&ssd, "< Massa >", 30, 30);
            break;
        case 3:
            ssd1306_draw_string(&ssd, " Tipo: ", 40, 5);
            ssd1306_draw_string(&ssd, "< Area >", 30, 30);
            break;
        case 4:
            ssd1306_draw_string(&ssd, " Tipo: ", 40, 5);
            ssd1306_draw_string(&ssd, "< Tempo >", 25, 30);
            break;
        case 5:
            ssd1306_draw_string(&ssd, " Tipo: ", 40, 5);
            ssd1306_draw_string(&ssd, "< Volume >", 25, 30);
            break;
        case 6:
            ssd1306_draw_string(&ssd, " Tipo: ", 40, 5);
            ssd1306_draw_string(&ssd, "< Digital >", 20, 30);
            break;
    }
    
    ssd1306_send_data(&ssd);
}

void atualizar_tipo() {
    adc_select_input(1);  // 🚀 Garante que está lendo apenas VRX (Eixo X)
    uint16_t vrx_value = adc_read();
    
    static uint16_t last_vrx_value = CENTER_VALUE;
    static uint32_t last_move_time = 0;
    
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    
    if (abs(vrx_value - last_vrx_value) > DEADZONE && (current_time - last_move_time > 200)) {
        if (vrx_value > CENTER_VALUE + DEADZONE) {  
            tipo = (tipo + 1) % 7;  // Avança o tipo de conversão e volta ao 0 após 6
        } else if (vrx_value < CENTER_VALUE - DEADZONE) {  
            tipo = (tipo - 1 + 7) % 7;  // Retrocede e volta para 6 se estiver em 0
        }

        last_vrx_value = vrx_value;
        last_move_time = current_time;
    }
}


void display_lista_medidas() {
    if (!precisa_atualizar) return;  // Evita redesenhos desnecessários

    ssd1306_fill(&ssd, false);
    ssd1306_draw_string(&ssd, "QUAL SUA MEDIDA?", 5, 5);

    // 🔥 Determinar as unidades com base no tipo de conversão
    char *opcoes_temp[3] = {"CELSIUS", "KELVIN", "FAHRENHEIT"};
    char *opcoes_comp[5] = {"KILOMETRO", "METRO", "CENTIMETRO", "MILIMETRO", "MICROMETRO"};
    char *opcoes_massa[4] = {"TONELADA", "QUILOGRAMA", "GRAMA", "MILIGRAMA"};
    char *opcoes_area[4] = {"METROS", "CENTIMETROS", "MILIMETROS", "HECTARES"};
    char *opcoes_tempo[5] = {"SEGUNDOS", "MINUTOS", "HORAS", "DIAS", "SEMANAS"};
    char *opcoes_volume[4] = {"LITROS", "MILILITROS", "CM", "M"};
    char *opcoes_digital[5] = {"BITS", "KB", "MB", "GB", "TB"};  // 🔥 Novo

    char **opcoes;
    
    // 🔥 Atualiza opções com base no tipo
    if (tipo == 0) { opcoes = opcoes_temp; total_opcoes = 3; } 
    else if (tipo == 1) { opcoes = opcoes_comp; total_opcoes = 5; } 
    else if (tipo == 2) { opcoes = opcoes_massa; total_opcoes = 4; }
    else if (tipo == 3) { opcoes = opcoes_area; total_opcoes = 4; }
    else if (tipo == 4) { opcoes = opcoes_tempo; total_opcoes = 5; }
    else if (tipo == 5) { opcoes = opcoes_volume; total_opcoes = 4; }
    else { opcoes = opcoes_digital; total_opcoes = 5; }  // ✅ Para Digital

    // 🔥 Lógica de exibição no display
    int inicio_lista = (escolha < 2) ? 0 : (escolha - 1);
    int fim_lista = inicio_lista + 2;
    if (fim_lista >= total_opcoes) {
        inicio_lista = total_opcoes - 3;
        fim_lista = total_opcoes - 1;
    }

    escolha = (escolha + total_opcoes) % total_opcoes;

    int linha = 20;
    for (int i = inicio_lista; i <= fim_lista; i++) {
        char buffer[20];
        if (i == escolha)
            sprintf(buffer, "%s <", opcoes[i]);
        else
            sprintf(buffer, "%s", opcoes[i]);

        ssd1306_draw_string(&ssd, buffer, 30, linha);
        linha += 10;
    }

    ssd1306_send_data(&ssd);
}

void atualizar_escolha() {
    adc_select_input(0);  // Leitura do eixo Y (VRY)
    uint16_t vry_value = adc_read();

    static uint16_t last_vry_value = CENTER_VALUE;
    static uint32_t last_move_time = 0;

    uint32_t current_time = to_ms_since_boot(get_absolute_time());

    if (abs(vry_value - last_vry_value) > DEADZONE && (current_time - last_move_time > 200)) {
        if (vry_value < CENTER_VALUE - DEADZONE) {  
            escolha = (escolha + 1) % total_opcoes;  // 🔄 Agora AUMENTA quando sobe o analógico
        } else if (vry_value > CENTER_VALUE + DEADZONE) {  
            escolha = (escolha - 1 + total_opcoes) % total_opcoes;  // 🔄 Agora DIMINUI quando abaixa o analógico
        }

        last_vry_value = vry_value;
        last_move_time = current_time;
    }
}

void display_lista_conversao() {
    if (!precisa_atualizar) return;
    ssd1306_fill(&ssd, false);
    ssd1306_draw_string(&ssd, "QUAL CONVERSAO?", 5, 5);

    // 🔥 Definir unidades para cada tipo
    char *opcoes_temp[3] = {"CELSIUS", "KELVIN", "FAHRENHEIT"};
    char *opcoes_comp[5] = {"KILOMETRO", "METRO", "CENTIMETRO", "MILIMETRO", "MICROMETRO"};
    char *opcoes_massa[4] = {"TONELADA", "QUILOGRAMA", "GRAMA", "MILIGRAMA"};
    char *opcoes_area[4] = {"METROS", "CENTIMETROS", "MILIMETROS", "HECTARES"};
    char *opcoes_tempo[5] = {"SEGUNDOS", "MINUTOS", "HORAS", "DIAS", "SEMANAS"};
    char *opcoes_volume[4] = {"LITROS", "MILILITROS", "CM", "M"};
    char *opcoes_digital[5] = {"BITS", "KB", "MB", "GB", "TB"};  // 🔥 Novo

    char **opcoes;
    
    if (tipo == 0) { opcoes = opcoes_temp; total_opcoes = 3; } 
    else if (tipo == 1) { opcoes = opcoes_comp; total_opcoes = 5; } 
    else if (tipo == 2) { opcoes = opcoes_massa; total_opcoes = 4; }
    else if (tipo == 3) { opcoes = opcoes_area; total_opcoes = 4; }
    else if (tipo == 4) { opcoes = opcoes_tempo; total_opcoes = 5; }
    else if (tipo == 5) { opcoes = opcoes_volume; total_opcoes = 4; }
    else { opcoes = opcoes_digital; total_opcoes = 5; }  // ✅ Digital

    // 🔥 Filtrar opções removendo a escolhida
    total_opcoes_filtradas = 0;
    for (int i = 0; i < total_opcoes; i++) {
        if (i != escolha) {
            opcoes_filtradas[total_opcoes_filtradas] = i;
            total_opcoes_filtradas++; // Incrementa somente após armazenar
        }
    }

    // 🔍 Debug para verificar a filtragem correta
    printf("DEBUG - Opções Filtradas (Total: %d):\n", total_opcoes_filtradas);
    for (int i = 0; i < total_opcoes_filtradas; i++) {
        printf("   [%d] %s\n", i, opcoes[opcoes_filtradas[i]]);
    }
    printf("DEBUG - Total Opções Filtradas: %d | Conversor Atual: %d\n", total_opcoes_filtradas, conversor);

    int inicio_lista = (conversor < 2) ? 0 : (conversor - 1);
    if (inicio_lista > total_opcoes_filtradas - 3) inicio_lista = total_opcoes_filtradas - 3;
    if (inicio_lista < 0) inicio_lista = 0;

    int linha = 20;
    for (int i = inicio_lista; i < inicio_lista + 3 && i < total_opcoes_filtradas; i++) {
        char buffer[20];
        if (conversor == opcoes_filtradas[i])
            sprintf(buffer, "%s <", opcoes[opcoes_filtradas[i]]);
        else
            sprintf(buffer, "%s", opcoes[opcoes_filtradas[i]]);

        ssd1306_draw_string(&ssd, buffer, 30, linha);
        linha += 10;
    }

    ssd1306_send_data(&ssd);
}


void atualizar_conversor() {
    adc_select_input(0);  // Leitura do eixo Y (VRY)
    uint16_t vry_value = adc_read();

    static uint16_t last_vry_value = CENTER_VALUE;
    static uint32_t last_move_time = 0;

    uint32_t current_time = to_ms_since_boot(get_absolute_time());

    if (abs(vry_value - last_vry_value) > DEADZONE && (current_time - last_move_time > 200)) {
        int index = -1;
        for (int i = 0; i < total_opcoes_filtradas; i++) {
            if (opcoes_filtradas[i] == conversor) {
                index = i;
                break;
            }
        }

        if (vry_value < CENTER_VALUE - DEADZONE) {  
            conversor = (index < total_opcoes_filtradas - 1) ? opcoes_filtradas[index + 1] : opcoes_filtradas[0];
        } 
        else if (vry_value > CENTER_VALUE + DEADZONE) {  
            conversor = (index > 0) ? opcoes_filtradas[index - 1] : opcoes_filtradas[total_opcoes_filtradas - 1];
        }
        

        last_vry_value = vry_value;
        last_move_time = current_time;
    }
}

void display_entrada_valor() {
    ssd1306_fill(&ssd, false);

    // 🔥 Exibir a unidade correta no cabeçalho
    if (tipo == 0) {  // Temperatura
        if (escolha == 0) ssd1306_draw_string(&ssd, "CELSIUS:", 10, 5);
        else if (escolha == 1) ssd1306_draw_string(&ssd, "KELVIN:", 10, 5);
        else ssd1306_draw_string(&ssd, "FAHRENHEIT:", 5, 5);
    } 
    else if (tipo == 1) {  // Comprimento
        char *unidades[5] = {"KILOMETROS:", "METROS:", "CENTIMETROS:", "MILIMETROS:", "MICROMETROS:"};
        ssd1306_draw_string(&ssd, unidades[escolha], 5, 5);
    } 
    else if (tipo == 2) {  // Massa
        char *unidades[4] = {"TONELADAS:", "QUILOGRAMAS:", "GRAMAS:", "MILIGRAMAS:"};
        ssd1306_draw_string(&ssd, unidades[escolha], 5, 5);
    } 
    else if (tipo == 3) {  // Área
        char *unidades[4] = {"METROS:", "CENTIMETROS:", "MILIMETROS:", "HECTARES:"};
        ssd1306_draw_string(&ssd, unidades[escolha], 5, 5);
    } 
    else if (tipo == 4) {  // Tempo
        char *unidades[5] = {"SEGUNDOS:", "MINUTOS:", "HORAS:", "DIAS:", "SEMANAS:"};
        ssd1306_draw_string(&ssd, unidades[escolha], 5, 5);
    } 
    else if (tipo == 5) {  // Volume
        char *unidades[4] = {"LITROS:", "MILILITROS:", "CM:", "M:"};
        ssd1306_draw_string(&ssd, unidades[escolha], 5, 5);
    } 
    else {  // 🔥 Digital
        char *unidades[5] = {"BITS:", "KB:", "MB:", "GB:", "TB:"};
        ssd1306_draw_string(&ssd, unidades[escolha], 5, 5);
    }

    // Exibe o valor centralizado na tela
    char buffer[10];
    sprintf(buffer, "%.1f", numero);
    ssd1306_draw_string(&ssd, buffer, 50, 30);

    ssd1306_send_data(&ssd);
}

void display_resultado_centralizado(float resultado) {
    char buffer[15];
    if (fabs(resultado) < 0.0001 || fabs(resultado) > 99999) {
        sprintf(buffer, "%.2e", resultado);  // Notação científica
    } else {
        sprintf(buffer, "%.3f", resultado);  // Notação decimal normal
    }

    int largura_texto = strlen(buffer) * 6;  // Aproximadamente 6 pixels por caractere
    int x_centralizado = (128 - largura_texto) / 2;  // Centraliza no display de 128px

    ssd1306_fill(&ssd, false);  // Limpa a tela
    ssd1306_draw_string(&ssd, "RESULTADO:", 10, 5);  // Cabeçalho
    ssd1306_draw_string(&ssd, buffer, x_centralizado, 30);  // Exibe número centralizado
    ssd1306_send_data(&ssd);
}

void realizar_conversao() {
    float resultado = 0;

    // 🔥 Depuração antes da conversão
    printf("🚀 Iniciando conversão - Tipo: %d, Escolha: %d, Conversor: %d, Numero: %.2f\n", tipo, escolha, conversor, numero);

    // ✅ CONVERSÕES DE TEMPERATURA (tipo == 0)
    if (tipo == 0) {
        if (escolha == 0 && conversor == 2)  
            resultado = celsius_para_fahrenheit(numero);  
        else if (escolha == 0 && conversor == 1)  
            resultado = celsius_para_kelvin(numero);
        else if (escolha == 1 && conversor == 0)  
            resultado = kelvin_para_celsius(numero);  
        else if (escolha == 1 && conversor == 2)  
            resultado = kelvin_para_fahrenheit(numero);
        else if (escolha == 2 && conversor == 0)  
            resultado = fahrenheit_para_celsius(numero);  
        else if (escolha == 2 && conversor == 1)  
            resultado = fahrenheit_para_kelvin(numero);
        else {
            printf("⚠ ERRO: Nenhuma conversão de temperatura foi chamada!\n");
            return;
        }
    }

    // ✅ CONVERSÕES DE COMPRIMENTO (tipo == 1)
    else if (tipo == 1) {
        if (escolha == 0 && conversor == 1)  
            resultado = quilometros_para_metros(numero);
        else if (escolha == 0 && conversor == 2)  
            resultado = quilometros_para_centimetros(numero);
        else if (escolha == 0 && conversor == 3)  
            resultado = quilometros_para_milimetros(numero);
        else if (escolha == 0 && conversor == 4)  
            resultado = quilometros_para_micrometros(numero);
        else if (escolha == 1 && conversor == 0)  
            resultado = metros_para_quilometros(numero);
        else if (escolha == 1 && conversor == 2)  
            resultado = metros_para_centimetros(numero);
        else if (escolha == 1 && conversor == 3)  
            resultado = metros_para_milimetros(numero);
        else if (escolha == 1 && conversor == 4)  
            resultado = metros_para_micrometros(numero);
        else if (escolha == 2 && conversor == 0)  
            resultado = centimetros_para_quilometros(numero);
        else if (escolha == 2 && conversor == 1)  
            resultado = centimetros_para_metros(numero);
        else if (escolha == 2 && conversor == 3)  
            resultado = centimetros_para_milimetros(numero);
        else if (escolha == 2 && conversor == 4)  
            resultado = centimetros_para_micrometros(numero);
        else if (escolha == 3 && conversor == 0)  
            resultado = milimetros_para_quilometros(numero);
        else if (escolha == 3 && conversor == 1)  
            resultado = milimetros_para_metros(numero);
        else if (escolha == 3 && conversor == 2)  
            resultado = milimetros_para_centimetros(numero);
        else if (escolha == 3 && conversor == 4)  
            resultado = milimetros_para_micrometros(numero);
        else if (escolha == 4 && conversor == 0)  
            resultado = micrometros_para_quilometros(numero);
        else if (escolha == 4 && conversor == 1)  
            resultado = micrometros_para_metros(numero);
        else if (escolha == 4 && conversor == 2)  
            resultado = micrometros_para_centimetros(numero);
        else if (escolha == 4 && conversor == 3)  
            resultado = micrometros_para_milimetros(numero);
        else {
            printf("⚠ ERRO: Nenhuma conversão de comprimento foi chamada!\n");
            return;
        }
    }

    // ✅ CONVERSÕES DE MASSA (tipo == 2)
    else if (tipo == 2) {
        if (escolha == 0 && conversor == 1)  
            resultado = tonelada_para_quilograma(numero);
        else if (escolha == 0 && conversor == 2)  
            resultado = tonelada_para_grama(numero);
        else if (escolha == 0 && conversor == 3)  
            resultado = tonelada_para_miligrama(numero);
        else if (escolha == 1 && conversor == 0)  
            resultado = quilograma_para_tonelada(numero);
        else if (escolha == 1 && conversor == 2)  
            resultado = quilograma_para_grama(numero);
        else if (escolha == 1 && conversor == 3)  
            resultado = quilograma_para_miligrama(numero);
        else if (escolha == 2 && conversor == 0)  
            resultado = grama_para_tonelada(numero);
        else if (escolha == 2 && conversor == 1)  
            resultado = grama_para_quilograma(numero);
        else if (escolha == 2 && conversor == 3)  
            resultado = grama_para_miligrama(numero);
        else if (escolha == 3 && conversor == 0)  
            resultado = miligrama_para_tonelada(numero);
        else if (escolha == 3 && conversor == 1)  
            resultado = miligrama_para_quilograma(numero);
        else if (escolha == 3 && conversor == 2)  
            resultado = miligrama_para_grama(numero);
        else {
            printf("⚠ ERRO: Nenhuma conversão de massa foi chamada!\n");
            return;
        }
    }

            // ✅ CONVERSÕES DE ÁREA (tipo == 3)
    else if (tipo == 3) {
        if (escolha == 0 && conversor == 1)  
            resultado = metros_quadrados_para_centimetros_quadrados(numero);
        else if (escolha == 0 && conversor == 2)  
            resultado = metros_quadrados_para_milimetros_quadrados(numero);
        else if (escolha == 0 && conversor == 3)  
            resultado = metros_quadrados_para_hectares(numero);
        else if (escolha == 1 && conversor == 0)  
            resultado = centimetros_quadrados_para_metros_quadrados(numero);
        else if (escolha == 1 && conversor == 2)  
            resultado = centimetros_quadrados_para_milimetros_quadrados(numero);
        else if (escolha == 1 && conversor == 3)  
            resultado = centimetros_quadrados_para_hectares(numero);
        else if (escolha == 2 && conversor == 0)  
            resultado = milimetros_quadrados_para_metros_quadrados(numero);
        else if (escolha == 2 && conversor == 1)  
            resultado = milimetros_quadrados_para_centimetros_quadrados(numero);
        else if (escolha == 2 && conversor == 3)  
            resultado = milimetros_quadrados_para_hectares(numero);
        else if (escolha == 3 && conversor == 0)  
            resultado = hectares_para_metros_quadrados(numero);
        else if (escolha == 3 && conversor == 1)  
            resultado = hectares_para_centimetros_quadrados(numero);
        else if (escolha == 3 && conversor == 2)  
            resultado = hectares_para_milimetros_quadrados(numero);
        else {
            printf("⚠ ERRO: Nenhuma conversão de área foi chamada!\n");
            return;
        }
    }

    // ✅ CONVERSÕES DE TEMPO (tipo == 4)
    else if (tipo == 4) {
        if (escolha == 0 && conversor == 1)  
            resultado = segundos_para_minutos(numero);
        else if (escolha == 0 && conversor == 2)  
            resultado = segundos_para_horas(numero);
        else if (escolha == 0 && conversor == 3)  
            resultado = segundos_para_dias(numero);
        else if (escolha == 0 && conversor == 4)  
            resultado = segundos_para_semanas(numero);
        else if (escolha == 1 && conversor == 0)  
            resultado = minutos_para_segundos(numero);
        else if (escolha == 1 && conversor == 2)  
            resultado = minutos_para_horas(numero);
        else if (escolha == 1 && conversor == 3)  
            resultado = minutos_para_dias(numero);
        else if (escolha == 1 && conversor == 4)  
            resultado = minutos_para_semanas(numero);
        else if (escolha == 2 && conversor == 0)  
            resultado = horas_para_segundos(numero);
        else if (escolha == 2 && conversor == 1)  
            resultado = horas_para_minutos(numero);
        else if (escolha == 2 && conversor == 3)  
            resultado = horas_para_dias(numero);
        else if (escolha == 2 && conversor == 4)  
            resultado = horas_para_semanas(numero);
        else if (escolha == 3 && conversor == 0)  
            resultado = dias_para_segundos(numero);
        else if (escolha == 3 && conversor == 1)  
            resultado = dias_para_minutos(numero);
        else if (escolha == 3 && conversor == 2)  
            resultado = dias_para_horas(numero);
        else if (escolha == 3 && conversor == 4)  
            resultado = dias_para_semanas(numero);
        else if (escolha == 4 && conversor == 0)  
            resultado = semanas_para_segundos(numero);
        else if (escolha == 4 && conversor == 1)  
            resultado = semanas_para_minutos(numero);
        else if (escolha == 4 && conversor == 2)  
            resultado = semanas_para_horas(numero);
        else if (escolha == 4 && conversor == 3)  
            resultado = semanas_para_dias(numero);
        else {
            printf("⚠ ERRO: Nenhuma conversão de tempo foi chamada!\n");
            return;
        }
    }

    // ✅ CONVERSÕES DE VOLUME (tipo == 5) ✅ ADICIONADO!
    if (tipo == 5) {
        if (escolha == 0 && conversor == 1)  
            resultado = litro_para_mililitro(numero);
        else if (escolha == 0 && conversor == 2)  
            resultado = litro_para_centimetros_cubicos(numero);
        else if (escolha == 0 && conversor == 3)  
            resultado = litro_para_metros_cubicos(numero);
        else if (escolha == 1 && conversor == 0)  
            resultado = mililitro_para_litro(numero);
        else if (escolha == 1 && conversor == 2)  
            resultado = mililitro_para_centimetros_cubicos(numero);
        else if (escolha == 1 && conversor == 3)  
            resultado = mililitro_para_metros_cubicos(numero);
        else if (escolha == 2 && conversor == 0)  
            resultado = centimetros_cubicos_para_litros(numero);
        else if (escolha == 2 && conversor == 1)  
            resultado = centimetros_cubicos_para_mililitros(numero);
        else if (escolha == 2 && conversor == 3)  
            resultado = centimetros_cubicos_para_metros_cubicos(numero);
        else if (escolha == 3 && conversor == 0)  
            resultado = metros_cubicos_para_litro(numero);
        else if (escolha == 3 && conversor == 1)  
            resultado = metros_cubicos_para_mililitro(numero);
        else if (escolha == 3 && conversor == 2)  
            resultado = metros_cubicos_para_centimetros_cubicos(numero);
        else {
            printf("⚠ ERRO: Nenhuma conversão de volume foi chamada!\n");
            return;
        }
    }

        // ✅ CONVERSÕES DE DADOS DIGITAIS (tipo == 6)
    if (tipo == 6) {
        if (escolha == 0 && conversor == 1)  
            resultado = bits_para_kb(numero);
        else if (escolha == 0 && conversor == 2)  
            resultado = bits_para_mb(numero);
        else if (escolha == 0 && conversor == 3)  
            resultado = bits_para_gb(numero);
        else if (escolha == 0 && conversor == 4)  
            resultado = bits_para_tb(numero);
        else if (escolha == 1 && conversor == 0)  
            resultado = kb_para_bits(numero);
        else if (escolha == 1 && conversor == 2)  
            resultado = kb_para_mb(numero);
        else if (escolha == 1 && conversor == 3)  
            resultado = kb_para_gb(numero);
        else if (escolha == 1 && conversor == 4)  
            resultado = kb_para_tb(numero);
        else if (escolha == 2 && conversor == 0)  
            resultado = mb_para_bits(numero);
        else if (escolha == 2 && conversor == 1)  
            resultado = mb_para_kb(numero);
        else if (escolha == 2 && conversor == 3)  
            resultado = mb_para_gb(numero);
        else if (escolha == 2 && conversor == 4)  
            resultado = mb_para_tb(numero);
        else if (escolha == 3 && conversor == 0)  
            resultado = gb_para_bits(numero);
        else if (escolha == 3 && conversor == 1)  
            resultado = gb_para_kb(numero);
        else if (escolha == 3 && conversor == 2)  
            resultado = gb_para_mb(numero);
        else if (escolha == 3 && conversor == 4)  
            resultado = gb_para_tb(numero);
        else if (escolha == 4 && conversor == 0)  
            resultado = tb_para_bits(numero);
        else if (escolha == 4 && conversor == 1)  
            resultado = tb_para_kb(numero);
        else if (escolha == 4 && conversor == 2)  
            resultado = tb_para_mb(numero);
        else if (escolha == 4 && conversor == 3)  
            resultado = tb_para_gb(numero);
        else {
            printf("⚠ ERRO: Nenhuma conversão digital foi chamada!\n");
            return;
        }
    }



    // 🔥 Depuração depois da conversão
    printf("📊 Depois da conversão - Resultado: %.9f\n", resultado);

    // ✅ Exibir o resultado no display
    ssd1306_fill(&ssd, false);

    // ✅ Mostrar o resultado convertido
    display_resultado_centralizado(resultado);

    ssd1306_send_data(&ssd);
}

bool debounce(uint32_t *last_press_time) {
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    if (current_time - *last_press_time > 200) {  // 200ms de intervalo mínimo entre pressões
        *last_press_time = current_time;
        return true;
    }
    return false;
}

void atualizar_numero() {
    adc_select_input(0);  // Leitura do eixo Y (VRY)
    uint16_t vry_value = adc_read();

    static uint16_t last_vry_value = CENTER_VALUE;
    static uint32_t last_move_time = 0;

    uint32_t current_time = to_ms_since_boot(get_absolute_time());

    if (abs(vry_value - last_vry_value) > DEADZONE && (current_time - last_move_time > 200)) {
        if (vry_value > CENTER_VALUE + DEADZONE) {  
            numero++;  // Aumenta o valor
        } else if (vry_value < CENTER_VALUE - DEADZONE) {  
            numero--;  // Diminui o valor (suporta negativos)
        }

        last_vry_value = vry_value;
        last_move_time = current_time;
    }
}


void button_callback(uint gpio, uint32_t events) {
    static uint32_t last_interrupt_time = 0;
    uint32_t current_time = to_ms_since_boot(get_absolute_time());

    // ✅ Debouncing para evitar múltiplas ativações
    if (current_time - last_interrupt_time < 200) {
        return;
    }
    last_interrupt_time = current_time;

    if (gpio == BUTTON_SELECT) {  // ✅ Botão A - Avançar/Selecionar
        if (estado == MENU_INICIAL) {
            estado = SELECAO_TIPO;
            precisa_atualizar = true;
        }
        else if (estado == SELECAO_TIPO) {
            estado = SELECAO_UNIDADE;
            escolha = 0;  // Sempre começa na primeira opção
            precisa_atualizar = true;

            // ✅ Atualiza o total de opções com base no tipo de conversão selecionado
            if (tipo == 0) total_opcoes = 3;  // 🔥 Temperatura: 3 opções
            else if (tipo == 1) total_opcoes = 5;  // 🔥 Comprimento: 5 opções
            else if (tipo == 2) total_opcoes = 4;  // 🔥 Massa: 4 opções
            else if (tipo == 3) total_opcoes = 4;  // 🔥 Área: 4 opções
            else if (tipo == 4) total_opcoes = 5;  // 🔥 Tempo: 5 opções
            else if (tipo == 5) total_opcoes = 4;  // 🔥 Volume: 4 opções
            else if (tipo == 6) total_opcoes = 6;  // ✅ Digital: 6 opções (Bits, Bytes, KB, MB, GB, TB)
        }
        else if (estado == SELECAO_UNIDADE) {
            estado = SELECAO_CONVERSAO;
            conversor = 0;  // Reseta para a primeira opção disponível
            precisa_atualizar = true;

            // ✅ Atualiza o total de opções da conversão
            if (tipo == 0) total_opcoes = 2;  // 🔥 Temperatura: Sempre 2 opções remanescentes
            else if (tipo == 1) total_opcoes = 4;  // 🔥 Comprimento: Sempre 4 opções remanescentes
            else if (tipo == 2) total_opcoes = 3;  // 🔥 Massa: 3 opções remanescentes
            else if (tipo == 3) total_opcoes = 3;  // 🔥 Área: 3 opções remanescentes
            else if (tipo == 4) total_opcoes = 4;  // 🔥 Tempo: 4 opções remanescentes
            else if (tipo == 5) total_opcoes = 3;  // 🔥 Volume: 3 opções remanescentes
            else if (tipo == 6) total_opcoes = 5;  // ✅ Digital: 5 opções remanescentes
        }
        else if (estado == SELECAO_CONVERSAO) {
            estado = ENTRADA_VALOR;
            numero = 0;  // Inicia o número como zero
            precisa_atualizar = true;
        }
        else if (estado == ENTRADA_VALOR) {
            estado = EXIBIR_RESULTADO;
            realizar_conversao();  // Converte e exibe o resultado
            precisa_atualizar = true;
        }
    } 
    else if (gpio == BUTTON_BACK) {  // ✅ Botão B - Voltar
        if (estado == SELECAO_TIPO) {
            estado = MENU_INICIAL;
            precisa_atualizar = true;
        }
        else if (estado == SELECAO_UNIDADE) {
            estado = SELECAO_TIPO;
            precisa_atualizar = true;
        }
        else if (estado == SELECAO_CONVERSAO) {
            // ✅ Definir `total_opcoes` ANTES de voltar para `SELECAO_UNIDADE`
            if (tipo == 0) total_opcoes = 3;  // 🔥 Temperatura: 3 opções
            else if (tipo == 1) total_opcoes = 5;  // 🔥 Comprimento: 5 opções
            else if (tipo == 2) total_opcoes = 4;  // 🔥 Massa: 4 opções
            else if (tipo == 3) total_opcoes = 4;  // 🔥 Área: 4 opções
            else if (tipo == 4) total_opcoes = 5;  // 🔥 Tempo: 5 opções
            else if (tipo == 5) total_opcoes = 4;  // 🔥 Volume: 4 opções
            else if (tipo == 6) total_opcoes = 6;  // ✅ Digital: 6 opções

            estado = SELECAO_UNIDADE;
            precisa_atualizar = true;
        }
        else if (estado == ENTRADA_VALOR) {
            estado = SELECAO_CONVERSAO;
            precisa_atualizar = true;

            // ✅ Atualiza `total_opcoes` da conversão ao voltar
            if (tipo == 0) total_opcoes = 2;  // 🔥 Temperatura: 2 opções remanescentes
            else if (tipo == 1) total_opcoes = 4;  // 🔥 Comprimento: 4 opções remanescentes
            else if (tipo == 2) total_opcoes = 3;  // 🔥 Massa: 3 opções remanescentes
            else if (tipo == 3) total_opcoes = 3;  // 🔥 Área: 3 opções remanescentes
            else if (tipo == 4) total_opcoes = 4;  // 🔥 Tempo: 4 opções remanescentes
            else if (tipo == 5) total_opcoes = 3;  // 🔥 Volume: 3 opções remanescentes
            else if (tipo == 6) total_opcoes = 5;  // ✅ Digital: 5 opções remanescentes
        }
        else if (estado == EXIBIR_RESULTADO) {
            estado = ENTRADA_VALOR;
            precisa_atualizar = true;
        }
    }
}


int main() {
    stdio_init_all();
    init_display();
    init_peripherals();
    display_pagina_inicial();

    gpio_set_irq_enabled_with_callback(BUTTON_SELECT, GPIO_IRQ_EDGE_FALL, true, &button_callback);
    gpio_set_irq_enabled_with_callback(BUTTON_BACK, GPIO_IRQ_EDGE_FALL, true, &button_callback);

    while (1) {
        switch (estado) {
            case MENU_INICIAL:
                display_pagina_inicial();  // 🔥 Agora exibe corretamente a tela inicial!
                break;
            case SELECAO_TIPO:
                atualizar_tipo();
                display_tipo_conversao();
                break;
            case SELECAO_UNIDADE:
                atualizar_escolha();
                display_lista_medidas();
                break;
            case SELECAO_CONVERSAO:
                atualizar_conversor();
                display_lista_conversao();
                break;
            case ENTRADA_VALOR:
                atualizar_numero();
                display_entrada_valor();
                break;
            case EXIBIR_RESULTADO:
                // O resultado já foi calculado, só exibe a tela
                break;
        }
        sleep_ms(100);
    }
    
    
    return 0;
}
