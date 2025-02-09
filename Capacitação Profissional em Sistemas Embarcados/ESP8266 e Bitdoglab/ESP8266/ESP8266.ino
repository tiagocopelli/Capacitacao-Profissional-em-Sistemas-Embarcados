#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>

// Configuração dos sensores
#define DHTPIN 12      // Pino digital 12 para o DHT11
#define DHTTYPE DHT11  // Tipo do sensor DHT11
#define MQ135PIN A0    // Pino analógico do MQ135

DHT dht(DHTPIN, DHTTYPE);

// Configuração do Wi-Fi
const char* ssid = "COPELLI-2G";         // Nome da rede wifi
const char* password = "copelli1";       // Senha da rede wifi 

const char* serverUrl = "http://192.168.0.38:3000/api/submitData";  // IP do servidor local

WiFiClient client;
HTTPClient http;

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    Serial.print("Conectando ao Wi-Fi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\n Conectado ao Wi-Fi!");
    
    dht.begin();
}

float readMQ135() {
    int sensorValue = analogRead(MQ135PIN);  // Leitura do valor analógico do MQ-135
    // Mapeamento do valor do MQ-135 para estimativa de concentração de gases
    return sensorValue;
}

float getGasConcentration(int mq135Value) {
    // Mapeamento fictício de valores para cada gás
    float alcool = mq135Value * 0.1;   // Fictício, apenas para teste
    float benzeno = mq135Value * 0.2;  
    float co2 = mq135Value * 0.05;     
    float fumaca = mq135Value * 0.15;  

    // Retorna os valores dos gases
    Serial.print("Álcool: "); Serial.println(alcool);
    Serial.print("Benzeno: "); Serial.println(benzeno);
    Serial.print("CO2: "); Serial.println(co2);
    Serial.print("Fumaça: "); Serial.println(fumaca);

    return mq135Value;  // Retorna o valor do MQ135 para qualidade do ar
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        // Leitura dos sensores
        float temperatura = dht.readTemperature();
        float umidade = dht.readHumidity();
        int qualidadeAr = readMQ135();  // Leitura do MQ135

        // Verifica se a leitura do DHT11 foi bem-sucedida
        if (isnan(temperatura) || isnan(umidade)) {
            Serial.println("Erro ao ler o sensor DHT11!");
            return;
        }

        // Imprime os dados no Serial Monitor
        Serial.print("Temperatura: ");
        Serial.print(temperatura);
        Serial.println(" °C");

        Serial.print("Umidade: ");
        Serial.print(umidade);
        Serial.println(" %");

        Serial.print("Qualidade do ar (MQ135): ");
        Serial.println(qualidadeAr);

        // Analisando os gases detectados
        float alcool = getGasConcentration(qualidadeAr);
        float benzeno = alcool * 2; 
        float co2 = alcool * 0.5;  
        float fumaca = alcool * 0.8;  

        // Enviando os dados para o servidor local
        http.begin(client, serverUrl);
        http.addHeader("Content-Type", "application/json");

        // Geração do payload com o Node ID e os dados de sensores
        String nodeID = WiFi.macAddress();  // Usando o MAC address como ID
        String payload = "{\"id\": \"" + nodeID + 
                         "\", \"temperatura\": " + String(temperatura) + 
                         ", \"umidade\": " + String(umidade) + 
                         ", \"qualidadeAr\": " + String(qualidadeAr) + 
                         ", \"alcool\": " + String(alcool) + 
                         ", \"benzeno\": " + String(benzeno) + 
                         ", \"CO2\": " + String(co2) + 
                         ", \"fumaca\": " + String(fumaca) + "}";

        int httpResponseCode = http.POST(payload);
        if (httpResponseCode > 0) {
            Serial.print("Dados enviados com sucesso! Resposta do servidor: ");
            Serial.println(httpResponseCode);
        } else {
            Serial.print("Falha ao enviar os dados. Código de erro: ");
            Serial.println(httpResponseCode);
        }

        http.end();
    } else {
        Serial.println("Wi-Fi desconectado! Tentando reconectar...");
        WiFi.begin(ssid, password);
    }

    delay(60000);  // Aguarda 1 minuto antes de fazer nova leitura
}
