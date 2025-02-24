#ifndef CONVERSOES_H
#define CONVERSOES_H

// Metro para outras unidades
float metros_para_centimetros(float valor) { return valor * 100; }
float metros_para_milimetros(float valor) { return valor * 1000; }
float metros_para_micrometros(float valor) { return valor * 1000000; }
float metros_para_quilometros(float valor) { return valor / 1000; }

// Centímetro para outras unidades
float centimetros_para_metros(float valor) { return valor / 100; }
float centimetros_para_milimetros(float valor) { return valor * 10; }
float centimetros_para_micrometros(float valor) { return valor * 10000; }
float centimetros_para_quilometros(float valor) { return valor / 100000; }

// Milímetro para outras unidades
float milimetros_para_metros(float valor) { return valor / 1000; }
float milimetros_para_centimetros(float valor) { return valor / 10; }
float milimetros_para_micrometros(float valor) { return valor * 1000; }
float milimetros_para_quilometros(float valor) { return valor / 1000000; }

// Micrômetro para outras unidades
float micrometros_para_metros(float valor) { return valor / 1000000; }
float micrometros_para_centimetros(float valor) { return valor / 10000; }
float micrometros_para_milimetros(float valor) { return valor / 1000; }
float micrometros_para_quilometros(float valor) { return valor / 1000000000; }

// Quilômetro para outras unidades
float quilometros_para_metros(float valor) { return valor * 1000; }
float quilometros_para_centimetros(float valor) { return valor * 100000; }
float quilometros_para_milimetros(float valor) { return valor * 1000000; }
float quilometros_para_micrometros(float valor) { return valor * 1000000000; }


// 🔹 Funções de Conversão para Massa

// 🔹 Conversões envolvendo Quilograma (kg)
float quilograma_para_grama(float valor) { return valor * 1000; }
float quilograma_para_miligrama(float valor) { return valor * 1000000; }
float quilograma_para_tonelada(float valor) { return valor / 1000; }

// 🔹 Conversões envolvendo Grama (g)
float grama_para_miligrama(float valor) { return valor * 1000; }
float grama_para_tonelada(float valor) { return valor / 1000000; }
float grama_para_quilograma(float valor) { return valor / 1000; }


// 🔹 Conversões envolvendo Miligrama (mg)
float miligrama_para_tonelada(float valor) { return valor / 1000000000; }
float miligrama_para_quilograma(float valor) { return valor / 1000000; }
float miligrama_para_grama(float valor) { return valor / 1000; }

// 🔹 Conversões envolvendo tonelada (t)
float tonelada_para_miligrama(float valor) { return valor * 1000000000; }
float tonelada_para_grama(float valor) { return valor * 1000000; }
float tonelada_para_quilograma(float valor) { return valor * 1000; }

// Funções de conversão para Área

float metros_quadrados_para_centimetros_quadrados(float valor) { return valor * 10000; }
float metros_quadrados_para_milimetros_quadrados(float valor) { return valor * 1000000; }
float metros_quadrados_para_hectares(float valor) { return valor / 10000; }
float metros_quadrados_para_quilometros_quadrados(float valor) { return valor / 1000000; }

float centimetros_quadrados_para_metros_quadrados(float valor) { return valor / 10000; }
float centimetros_quadrados_para_milimetros_quadrados(float valor) { return valor * 100; }
float centimetros_quadrados_para_hectares(float valor) { return valor / 100000000; }
float centimetros_quadrados_para_quilometros_quadrados(float valor) { return valor / 10000000000; }

float milimetros_quadrados_para_metros_quadrados(float valor) { return valor / 1000000; }
float milimetros_quadrados_para_centimetros_quadrados(float valor) { return valor / 100; }
float milimetros_quadrados_para_hectares(float valor) { return valor / 10000000000; }
float milimetros_quadrados_para_quilometros_quadrados(float valor) { return valor / 1000000000000; }

float hectares_para_metros_quadrados(float valor) { return valor * 10000; }
float hectares_para_centimetros_quadrados(float valor) { return valor * 100000000; }
float hectares_para_milimetros_quadrados(float valor) { return valor * 10000000000; }
float hectares_para_quilometros_quadrados(float valor) { return valor / 100; }

// Funções de conversão para Tempo

float segundos_para_minutos(float valor) { return valor / 60; }
float segundos_para_horas(float valor) { return valor / 3600; }
float segundos_para_dias(float valor) { return valor / 86400; }
float segundos_para_semanas(float valor) { return valor / 604800; }

float minutos_para_segundos(float valor) { return valor * 60; }
float minutos_para_horas(float valor) { return valor / 60; }
float minutos_para_dias(float valor) { return valor / 1440; }
float minutos_para_semanas(float valor) { return valor / 10080; }

float horas_para_segundos(float valor) { return valor * 3600; }
float horas_para_minutos(float valor) { return valor * 60; }
float horas_para_dias(float valor) { return valor / 24; }
float horas_para_semanas(float valor) { return valor / 168; }

float dias_para_segundos(float valor) { return valor * 86400; }
float dias_para_minutos(float valor) { return valor * 1440; }
float dias_para_horas(float valor) { return valor * 24; }
float dias_para_semanas(float valor) { return valor / 7; }

float semanas_para_segundos(float valor) { return valor * 604800; }
float semanas_para_minutos(float valor) { return valor * 10080; }
float semanas_para_horas(float valor) { return valor * 168; }
float semanas_para_dias(float valor) { return valor * 7; }

// Funções de conversão para Volume

float litro_para_mililitro(float valor) { return valor * 1000; }
float litro_para_centimetros_cubicos(float valor) { return valor * 1000; }
float litro_para_metros_cubicos(float valor) { return valor / 1000; }

float mililitro_para_litro(float valor) { return valor / 1000; }
float mililitro_para_centimetros_cubicos(float valor) { return valor; }
float mililitro_para_metros_cubicos(float valor) { return valor / 1000000; }

float centimetros_cubicos_para_litros(float valor) { return valor / 1000; }
float centimetros_cubicos_para_mililitros(float valor) { return valor; }
float centimetros_cubicos_para_metros_cubicos(float valor) { return valor / 1000000; }

float metros_cubicos_para_litro(float valor) { return valor * 1000; }
float metros_cubicos_para_mililitro(float valor) { return valor * 1000000; }
float metros_cubicos_para_centimetros_cubicos(float valor) { return valor * 1000000; }

// Funções de conversão para Temperatura

float celsius_para_fahrenheit(float valor) { return (valor * 9.0 / 5.0) + 32.0; }
float celsius_para_kelvin(float valor) { return valor + 273.15; }
float fahrenheit_para_celsius(float valor) { return (valor - 32.0) * 5.0 / 9.0; }
float fahrenheit_para_kelvin(float valor) { 
    return (valor - 32.0) * 5.0 / 9.0 + 273.15; 
}
float kelvin_para_celsius(float valor) { return valor - 273.15; }
float kelvin_para_fahrenheit(float valor) { 
    return (valor - 273.15) * 9.0 / 5.0 + 32.0; 
}

// Funções de conversão para Dados Digitais

float bits_para_kb(float valor) { return valor / 8192.0; }
float bits_para_mb(float valor) { return valor / (8192.0 * 1024); }
float bits_para_gb(float valor) { return valor / (8192.0 * 1024 * 1024); }
float bits_para_tb(float valor) { return valor / (8192.0 * 1024 * 1024 * 1024); }

float kb_para_bits(float valor) { return valor * 8192.0; }
float kb_para_mb(float valor) { return valor / 1024.0; }
float kb_para_gb(float valor) { return valor / (1024.0 * 1024); }
float kb_para_tb(float valor) { return valor / (1024.0 * 1024 * 1024); }

float mb_para_bits(float valor) { return valor * 8192.0 * 1024; }
float mb_para_kb(float valor) { return valor * 1024.0; }
float mb_para_gb(float valor) { return valor / 1024.0; }
float mb_para_tb(float valor) { return valor / (1024.0 * 1024); }

float gb_para_bits(float valor) { return valor * 8192.0 * 1024 * 1024; }
float gb_para_kb(float valor) { return valor * 1024.0 * 1024; }
float gb_para_mb(float valor) { return valor * 1024.0; }
float gb_para_tb(float valor) { return valor / 1024.0; }

float tb_para_bits(float valor) { return valor * 8192.0 * 1024 * 1024 * 1024; }
float tb_para_kb(float valor) { return valor * 1024.0 * 1024 * 1024; }
float tb_para_mb(float valor) { return valor * 1024.0 * 1024; }
float tb_para_gb(float valor) { return valor * 1024.0; }


#endif


