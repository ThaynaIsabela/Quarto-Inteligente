#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

#define DHTTYPE DHT11

#define led_azul 9
#define led_vermelho 10
#define led_verde 11
#define pir 8
#define DHTPIN A1
#define ldr A3

int led_status = 0; // 0 = Desligado, 1 = Ligado
int led_r = 0; // Se brilho especifico no vermelho
int led_g = 0; // Se brilho especifico no no verde;
int led_b = 0; // Se brilho especifico no azul;
int led_efeito = 1;  // 0 = cor fixa
                     // 1 = efeito 1
int pir_estado_atual = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  pinMode(led_azul, OUTPUT);
  pinMode(led_vermelho, OUTPUT);
  pinMode(led_verde, OUTPUT);
  pinMode(pir, INPUT);
  pinMode(ldr, INPUT);
  analogWrite(led_azul, 0);
  analogWrite(led_vermelho, 0);
  analogWrite(led_verde, 0);

  lcd.begin();
  lcd.backlight();
  dht.begin();
}

void loop() {
  // Verifica se tem algum comando na serial
  if (Serial.available()) { // Verifica se o Node-red mandou algo
    String frase = Serial.readString();
    frase.trim();

    String dispositivo = frase.substring(0, frase.indexOf(":"));
    if (dispositivo == "LED") {
      if (frase == "LED:ligar") {
        led_status = 1;
        Serial.println("LED:status(1)");
      } else if (frase == "LED:desligar") {
        led_status = 0;
        Serial.println("LED:status(0)");
      } else if (frase == "LED:efeito1") {
        led_efeito = 1;
        led_status = 1;
        Serial.println("LED:status(1)");
        Serial.println("LED:efeito(1)");
      } else { // LED:mudarcor
        led_status = 1;
        led_efeito = 0;
        Serial.println("LED:status(1)");
        Serial.println("LED:efeito(0)");
        String cores = frase.substring(frase.indexOf("(", 0) + 1, frase.indexOf(")", 0)); // 0,0,0
        String vermelho = cores.substring(0, cores.indexOf(",", 0));                      // 01234
        int primeiraVirgula = cores.indexOf(",", 0);
        int segundaVirgula = cores.indexOf(",", primeiraVirgula + 1);
        String verde = cores.substring(primeiraVirgula + 1, segundaVirgula);
        String azul = cores.substring(segundaVirgula + 1);
        Serial.print("DEBUG:");
        Serial.print(vermelho);
        Serial.print(":");
        Serial.print(verde);
        Serial.print(":");
        Serial.println(azul);
        led_r = atoi(vermelho.c_str());
        led_g = atoi(verde.c_str());
        led_b = atoi(azul.c_str());
      }
    } else if (dispositivo == "LDR") {
      Serial.print("LDR:luminosidade(");
      Serial.print(analogRead(ldr));
      Serial.println(")");
    } else if (dispositivo == "DHT") {
      Serial.println("DEBUG:Entrou no DHT");
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      if (isnan(h) || isnan(t)) {
        Serial.println(F("ERRO:Nao consegui ler o DHT11!"));
      } else {
        char tmpbuff[30];
        dtostrf(t, 4, 6, tmpbuff);
        Serial.print("DHT:temperatura(");
        Serial.print(tmpbuff);
        Serial.print(",");
        dtostrf(h, 4, 6, tmpbuff);
        Serial.print(tmpbuff);
        Serial.println(")");
      }
    } else if (dispositivo == "DISPLAY") {
      String mensagem = frase.substring(frase.indexOf(":") + 1);
      lcd.clear();
      lcd.print(mensagem);
      Serial.println("DEBUG:Entrou no DISPLAY");
    } else {
      Serial.println("ERRO:Não conheço o dispositivo");
    }
  }

  // Blocos de comando do PIR
  pir_controle();
  
  if (led_status == 1) {
    if (led_efeito == 0) {
      analogWrite(led_vermelho, led_r);
      analogWrite(led_verde, led_g);
      analogWrite(led_azul, led_b);
      delay(100);
    } else if (led_efeito == 1) {
      efeito1();
    }
  } else {
    analogWrite(led_vermelho, 0);
    analogWrite(led_verde, 0);
    analogWrite(led_azul, 0);
    delay(100);
  }

}

void pir_controle() {
  // Blocos de comando do PIR
  int pir_agora = digitalRead(pir);
  if (pir_estado_atual != pir_agora) {
    Serial.print("PIR:temmovimento(");
    Serial.print(pir_agora);
    Serial.println(")");
    pir_estado_atual = pir_agora;
  }
}

void efeito1() {
  int vermelho, verde, azul;
  for(vermelho = 0; vermelho <= 255 && led_efeito == 1; vermelho++)
  {
    analogWrite(led_vermelho, vermelho);
    delay(10);
    pir_controle();
  }
  for(azul = 255; azul >= 0  && led_efeito == 1; azul--)
  {
    analogWrite(led_azul, azul);
    delay(10);
    pir_controle();
  }
  for(verde = 0; verde <= 255  && led_efeito == 1; verde++)
  {
    analogWrite(led_verde, verde);
    delay(10);
    pir_controle();
  }
  for(vermelho = 255; vermelho >= 0  && led_efeito == 1; vermelho--)
  {
    analogWrite(led_vermelho, vermelho);
    delay(10);
    pir_controle();
  }
  for(azul = 0; azul <= 255  && led_efeito == 1; azul++)
  {
    analogWrite(led_azul, azul);
    delay(10);
    pir_controle();
  }
  for(verde = 255; verde >= 0  && led_efeito == 1; verde--)
  {
    analogWrite(led_verde, verde);
    delay(10);
    pir_controle();
  }  
}
