#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define SDA 26
#define SCL 25
#define MISO 19
#define MOSI 18
#define SCK 23
#define CS 5
#define CTL 34




AsyncWebServer server(80); //seleciona a porta do servidor web

const char login_html[] PROGMEM = R"rawliteral(
<html>
<head>
  <style>
body {
  align-items: center;
  background-color: #000;
  display: flex;
  justify-content: center;
  height: 100vh;
}

.form {
  background-color: #15172b;
  border-radius: 20px;
  box-sizing: border-box;
  height: 500px;
  padding: 20px;
  width: 320px;
}

.title {
  color: #eee;
  font-family: sans-serif;
  font-size: 36px;
  font-weight: 600;
  margin-top: 30px;
}

.subtitle {
  color: #eee;
  font-family: sans-serif;
  font-size: 16px;
  font-weight: 600;
  margin-top: 10px;
}

.input-container {
  height: 50px;
  position: relative;
  width: 100%;
}

.ic1 {
  margin-top: 40px;
}

.ic2 {
  margin-top: 30px;
}

.input {
  background-color: #303245;
  border-radius: 12px;
  border: 0;
  box-sizing: border-box;
  color: #eee;
  font-size: 18px;
  height: 100%;
  outline: 0;
  padding: 4px 20px 0;
  width: 100%;
}

.cut {
  background-color: #15172b;
  border-radius: 10px;
  height: 20px;
  left: 20px;
  position: absolute;
  top: -20px;
  transform: translateY(0);
  transition: transform 200ms;
  width: 76px;
}

.cut-short {
  width: 50px;
}

.input:focus ~ .cut,
.input:not(:placeholder-shown) ~ .cut {
  transform: translateY(8px);
}

.placeholder {
  color: #65657b;
  font-family: sans-serif;
  left: 20px;
  line-height: 14px;
  pointer-events: none;
  position: absolute;
  transform-origin: 0 50%;
  transition: transform 200ms, color 200ms;
  top: 20px;
}

.input:focus ~ .placeholder,
.input:not(:placeholder-shown) ~ .placeholder {
  transform: translateY(-30px) translateX(10px) scale(0.75);
}

.input:not(:placeholder-shown) ~ .placeholder {
  color: #808097;
}

.input:focus ~ .placeholder {
  color: #dc2f55;
}

.submit {
  background-color: #08d;
  border-radius: 12px;
  border: 0;
  box-sizing: border-box;
  color: #eee;
  cursor: pointer;
  font-size: 18px;
  height: 50px;
  margin-top: 38px;
  // outline: 0;
  text-align: center;
  width: 100%;
}

.submit:active {
  background-color: #06b;
}

</style>
<title>Login</title>
</head>
<body>
       <div class='form' >
        <form action='/get'>
        
      <div class='title'>Residência Brito</div>
      <div class='subtitle'>Insira suas credenciais</div>
      <div class='input-container ic1'>

        <input id='username' name='nome' class='input' type='text' placeholder=' ' />
        <div class='cut'></div>
        <label for='username' class='placeholder'>Insira seu usuário</label>
      </div>
      <div class='input-container ic2'>
        <input id='password' name='senha' class='input' type='password' placeholder=' ' />
        <div class='cut cut-short'></div>
        <label for='password' class='placeholder'>Senha</>
      </div>
      <button type='submit' class='submit' >Login</button>
    </div>
  </form>
</body>
</html>
)rawliteral";

const char home_morador_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <style>
body {
  align-items: center;
  background-color: #000;
  display: flex;
  justify-content: center;
  height: 100vh;
}
.form {
  background-color: #15172b;
  border-radius: 20px;
  box-sizing: border-box;
  height: 500px;
  padding: 20px;
  width: 320px;
}
.title {
  color: #eee;
  font-family: sans-serif;
  font-size: 36px;
  font-weight: 600;
  margin-top: 30px;
}
.subtitle {
  color: #eee;
  font-family: sans-serif;
  font-size: 16px;
  font-weight: 600;
  margin-top: 10px;
}
.submit {
  height: 50px;
  position: relative;
  width: 100%;
}
.ic1 {
  margin-top: 40px;
}
.ic2 {
  margin-top: 30px;
}
.submit {
  background-color: #303245;
  border-radius: 12px;
  border: 0;
  box-sizing: border-box;
  color: #eee;
  font-size: 18px;
  height: 100%;
  outline: 0;
  padding: 4px 20px 0;
  width: 100%;
}
.cut {
  background-color: #15172b;
  border-radius: 10px;
  height: 20px;
  left: 20px;
  position: absolute;
  top: -20px;
  transform: translateY(0);
  transition: transform 200ms;
  width: 76px;
}
.cut-short {
  width: 50px;
}
.input:focus ~ .cut,
.input:not(:placeholder-shown) ~ .cut {
  transform: translateY(8px);
}
.placeholder {
  color: #65657b;
  font-family: sans-serif;
  left: 20px;
  line-height: 14px;
  pointer-events: none;
  position: absolute;
  transform-origin: 0 50%;
  transition: transform 200ms, color 200ms;
  top: 20px;
}
.a:focus ~ .placeholder,
.input:not(:placeholder-shown) ~ .placeholder {
  transform: translateY(-30px) translateX(10px) scale(0.75);
}
.input:not(:placeholder-shown) ~ .placeholder {
  color: #808097;
}
.input:focus ~ .placeholder {
  color: #dc2f55;
}
.submit {
  background-color: #08d;
  border-radius: 12px;
  border: 0;
  box-sizing: border-box;
  color: #eee;
  cursor: pointer;
  font-size: 18px;
  height: 50px;
  margin-top: 38px;
  // outline: 0;
  text-align: center;
  width: 100%;
}
.submit:active {
  background-color: #06b;
}
</style>
<title>Morador</title>
</head>
<body>
 <div class='form'>
      <div class='title'>Residência Brito</div>
      <form>
      <button action='/Abrir' class='submit'>Abrir portão</button>
      <button action='/novo_usuario' class='submit'>Cadastrar Usuário</button>
      <button action='/Log' type='submit' class='submit'>Registro de uso</button>
    </form>
<button type='text' class='return'>Voltar</button>
</div>
</body>
</html>
)rawliteral";

const char novo_usuario_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<!DOCTYPE HTML>
<html>
<head>
  <style>
body {
  align-items: center;
  background-color: #000;
  display: flex;
  justify-content: center;
  height: 100vh;
}
.form {
  background-color: #15172b;
  border-radius: 20px;
  box-sizing: border-box;
  height: 650px;
  padding: 20px;
  width: 320px;
}
.title {
  color: #eee;
  font-family: sans-serif;
  font-size: 36px;
  font-weight: 600;
  margin-top: 30px;
}
.subtitle {
  color: #eee;
  font-family: sans-serif;
  font-size: 16px;
  font-weight: 600;
  margin-top: 10px;
}
.input-container {
  height: 50px;
  position: relative;
  width: 100%;
}
.ic1 {
  margin-top: 40px;
}
.ic2 {
  margin-top: 30px;
}
.input {
  background-color: #303245;
  border-radius: 12px;
  border: 0;
  box-sizing: border-box;
  color: #eee;
  font-size: 18px;
  height: 100%;
  outline: 0;
  padding: 4px 20px 0;
  width: 100%;
}
.cut {
  background-color: #15172b;
  border-radius: 10px;
  height: 20px;
  left: 20px;
  position: absolute;
  top: -20px;
  transform: translateY(0);
  transition: transform 200ms;
  width: 76px;
}
.cut-short {
  width: 50px;
}
.input:focus ~ .cut,
.input:not(:placeholder-shown) ~ .cut {
  transform: translateY(8px);
}
.placeholder {
  color: #65657b;
  font-family: sans-serif;
  left: 20px;
  line-height: 14px;
  pointer-events: none;
  position: absolute;
  transform-origin: 0 50%;
  transition: transform 200ms, color 200ms;
  top: 20px;
}
.input:focus ~ .placeholder,
.input:not(:placeholder-shown) ~ .placeholder {
  transform: translateY(-30px) translateX(10px) scale(0.75);
}
.input:not(:placeholder-shown) ~ .placeholder {
  color: #808097;
}
.input:focus ~ .placeholder {
  color: #dc2f55;
}
.submit {
  background-color: #08d;
  border-radius: 12px;
  border: 0;
  box-sizing: border-box;
  color: #eee;
  cursor: pointer;
  font-size: 18px;
  height: 50px;
  margin-top: 38px;
  // outline: 0;
  text-align: center;
  width: 100%;
}
.submit:active {
  background-color: #06b;
}
</style>
<title>Cadastro de Novo Usuário</title>
</head>
<body>

<div class='form'>
  <form action='/cadastrar'>
      <div class='title'>Cadastrar novo usuário</div>
      <div class='subtitle'>Insira os dados do usuário</div>
      <div class='input-container ic1'>
        <input id='username' name='nome1' class='input' type='text' placeholder=' ' />
        <div class='cut'></div>
        <label for='username' class='placeholder'>Novo usuário</label>
      </div>
      <div class='input-container ic2'>
        <input id='password' name='senha1' class='input' type='password' placeholder=' ' />
        <div class='cut cut-short'></div>
        <label for='password' class='placeholder'>Senha padrão</label>
      </div>
    <div class='input-container ic2'>
        <input id='qtdacessos' name='qntdAcessos' class='input' type='number' placeholder=' ' min='1' max='10' />
        <div class='cut'></div>
        <label for='qtdacessos' class='placeholder'>Quantidade de acessos</label>
      </div>
<div class='subtitle'>Selecione o tipo de usuário</div>
<select name='tipo' value='categoria' id='tipo'>
  <option value='m'>Morador</option>
  <option value='c'>Convidado</option>
</select> 
      <button type='text' action='/cadastrar' class='submit'>Cadastrar</button>
      <button class='submit' action='/morador'type='text' class='return'>Voltar</button>
</form>
</div>
</body>
</html>
)rawliteral";



void setup(){
    Serial.begin(115200); //inicializa comunicação serial, para facilitar debug
    pinMode(2,OUTPUT); //feedback de boot
    pinMode(CTL, OUTPUT);
    digitalWrite(CTL, HIGH);
    digitalWrite(2,HIGH);
    delay(1000);
    digitalWrite(2,LOW);

      //pin32 ADC -> RTC -SCL
      //pin33 ADC -> RTC -SDA
      //pin12 SD Card -MISO
      //pin13 SD Card -MOSI
      //pin14 SD Card -SCK
    pinMode(15,OUTPUT);//SD Card -CS

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", login_html);
    });
    server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request){
        String nome;
        String senha;
        nome = request->getParam("nome")->value();
        senha = request->getParam("senha")->value();
        Serial.print("Nome: ");
        Serial.print(nome);
        Serial.print("\n senha: ");
        Serial.println(senha);
        if (nome=="Luangb" && senha=="123456"){
          request->send(200, "text/html", home_morador_html);
        }else {
          request->send(200, "text/html", login_html);
        }
    });

    server.on("/cadastrar", HTTP_GET, [](AsyncWebServerRequest *request){
        String nome1;
        String senha1;
        String tipo;
        String qntdAcessos;
        nome1 = request->getParam("nome1",true )->value();
        senha1 = request->getParam("senha1", true)->value();
        qntdAcessos = request->getParam("qntdAcessos")->value();
        tipo = request->getParam("tipo")->value();
        Serial.print("Nome: ");
        Serial.print(nome1);
        Serial.print("\n senha: ");
        Serial.println(senha1);
        Serial.print("\n Quantidade de acessos: ");
        Serial.print(qntdAcessos);
        Serial.print("\n tipo: ");
        Serial.println(tipo);
        request->send(200, "text/html", home_morador_html);
    });

    server.on("/morador", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", home_morador_html);
    });
    server.on("/novo_usuario", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", novo_usuario_html);
    });

    server.on("/Login", HTTP_GET, [] (AsyncWebServerRequest *request) {
        request->send_P(200, "text/html", login_html);
    });
    server.on("/Abrir", HTTP_GET, [] (AsyncWebServerRequest *request){
        abre_portao();
        request->send(200, "text/html", home_morador_html);
    });
    

    char ssid[100] = "";
    char password[100] = "";
    uint32_t ip[4];

/*

    SD_init(); //inicializa o cartão SD

    get_config_parameters(ssid, password); //pega o ssid e senha do arquivo de setup

    Serial.println("Rede:");
    Serial.printf("%s\n",ssid);
    Serial.println("Rede:");
    Serial.printf("%s\n",password);
*/ 
 

    WiFi.begin("Brito", "Brito2410"); //inicializa WiFi
    

    // WiFi.connect("Brito"); //Conecta à rede WiFi
    while (WiFi.status() != WL_CONNECTED);
    server.begin();
    Serial.println(WiFi.localIP());
    
}


void loop(){
}


/*
void SD_init(){
    if (!SD.begin(15)) {
    Serial.println("Card Mount Failed");
    return;
    }
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return;
    }
    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
    Serial.println("MMC");
    } else if(cardType == CARD_SD){
    Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
    Serial.println("SDHC");
    } else {
    Serial.println("UNKNOWN");
    }
    uint64_t cardSize = SD.cardSize() / (1024 * 1024 * 1024);
    Serial.printf("SD Card Size: %lluGB\n", cardSize);
}


void get_config_parameters(char ssid[],char password[]){
    File file=SD.open("wifi_config.txt");
    char c = file.read();
    char* c_ip = "";
    char* c_gateway = "";
    char* c_subnet = "";
    if (file){
        while(c!=':'){c = file.read();} while(c!=' '){c = file.read();}
        while(c!='\n'){
            ssid += c;
            c = file.read();
        }
        c = file.read();
        while(c!=':'){c = file.read();} while(c!=' '){c = file.read();}
        while(file.available() && c!='\n'){
            password += c;
            c = file.read();
        }
        
        file.close();
        }
    }


void WiFi_connect(char ssid[]){
    Serial.printf("Conectando a rede %s...\n",ssid);
    while (WiFi.status() != WL_CONNECTED) {
        delay(50);
        digitalWrite(2,HIGH);    //feedback de tentativa de conexão
        delay(50);
        digitalWrite(2,LOW);
    }
    digitalWrite(2,HIGH);
    Serial.println("WiFi conectado.");
}





void log_write(){
    1;
}*/ 
void abre_portao(){
    digitalWrite(2, HIGH); 
    digitalWrite(CTL,LOW);
    Serial.println("Portão aberto");
    delay(500);
    digitalWrite(CTL,HIGH);
    digitalWrite(2, LOW);
    //log_write();
}
