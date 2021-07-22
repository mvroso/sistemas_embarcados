#include "ihm.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

#include "rtc.h"
#include "remote_control.h"
#include "memory_card.h"
#include "ihm.h"

#define SDA 26
#define SCL 25
#define MISO 19
#define MOSI 18
#define SCK 23
#define CS 5
#define CTL 34
#define PORTA 80


// Eventos:

#define NE 0  // Nenhum Evento
#define E1 1  // Morador pressionou botão “Entrar” (solicitou Login  com dados corretos preenchidos)
#define E2 2  // Convidado pressionou botão “Entrar” (solicitou Login  com dados corretos preenchidos)
#define E3 3  // Usuário pressionou botão “Sair”
#define E4 4  // Usuário pressionou botão “Abrir Portão”
#define E5 5  // Morador pressionou botão “Registro de Uso (Log)”
#define E6 6  // Morador pressionou “Cadastrar Novo Usuário”
#define E7 7  // Morador pressionou botão “Voltar”
#define E8 8  // Morador pressionou o botão “OK” na página de cadastro de novo usuário após 
              //preencher com dados válidos todos os campos requeridos


// Ações:

#define NA 0   // Nenhuma Ação
#define A1 1   // Exibir Página Principal do Morador  e Registrar Login no arquivo log.txt
#define A2 2   // Exibir Página Principal do Convidado  e Registrar Login no arquivo log.txt
#define A3 3   // Exibir Página Inicial (Página de Login) 
#define A4 4   // Acionar Controle para Abrir Portão e Registrar Abertura de Portão no log.txt
#define A5 5  // Envio do arquivo log.txt para download 
#define A6 6  // Exibir Página de Cadastro de Novo Usuário
#define A7 7  // Exibir Página Principal do Morador
#define A8 8  // Exibir Página Principal do Morador e Escrever os dados do novo usuário cadastrado no arquivo config.txt 


// Estados:

#define S0 0   // Estado Idle
#define S1 1   // Estado Morador Idle (Exibe Página Principal do Morador)
#define S2 2   // Estado Convidado Idle (Exibe Página Principal do Convidado)
#define S3 3   // Estado Cadastramento Idle (Exibe Página para Cadastro de Novo Usuário)

// Dimensões dos Arrays que representam a Matriz:

#define NUM_ESTADOS 4
#define NUM_EVENTOS 8



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

    

int proximo_estado_matrizTransicaoEstados[NUM_ESTADOS][NUM_ESTADOS];
int acao_matrizTransicaoEstados[NUM_ESTADOS][NUM_ESTADOS];

// Inicialização da Máquina de Estados:

void iniciaMaquinaEstados() {
    int i;
    int j;

    // Preenchimento inicial dos Arrays 
    for (i=0; i < NUM_ESTADOS; i++) {
        for (j=0; j < NUM_EVENTOS; j++) {
        acao_matrizTransicaoEstados[i][j] = NA;
        proximo_estado_matrizTransicaoEstados[i][j] = i;
        }
    }


    // Somente para os pares (Estado Atual / Evento) para os quais
    // efetivamente há mudança de estado e/ou ação associados é  
    // necessário atribuir manualmente o próximo estado e/ou a ação,
    // os demais elementos permanecem os mesmos do preenchimento inicial.
    proximo_estado_matrizTransicaoEstados[S0][E1] = S1;
    acao_matrizTransicaoEstados[S0][E1] = A1;

    proximo_estado_matrizTransicaoEstados[S0][E2] = S2;
    acao_matrizTransicaoEstados[S0][E2] = A2;

    proximo_estado_matrizTransicaoEstados[S1][E3] = S0;
    acao_matrizTransicaoEstados[S1][E3] = A3;

    proximo_estado_matrizTransicaoEstados[S1][E4] = S1;  // desnecessário
    acao_matrizTransicaoEstados[S1][E4] = A4;

    proximo_estado_matrizTransicaoEstados[S1][E5] = S1;  // desnecessário
    acao_matrizTransicaoEstados[S1][E5] = A5;

    proximo_estado_matrizTransicaoEstados[S1][E6] = S3;
    acao_matrizTransicaoEstados[S1][E6] = A6;

    proximo_estado_matrizTransicaoEstados[S2][E3] = S0;
    acao_matrizTransicaoEstados[S2][E3] = A3;

    proximo_estado_matrizTransicaoEstados[S2][E4] = S2;  // desnecessário
    acao_matrizTransicaoEstados[S2][E4] = A4;

    proximo_estado_matrizTransicaoEstados[S3][E7] = S1;
    acao_matrizTransicaoEstados[S3][E7] = A7;

    proximo_estado_matrizTransicaoEstados[S3][E8] = S1;
    acao_matrizTransicaoEstados[S3][E8] = A8;
}

void setup(){

    Serial.begin(115200); //inicializa comunicação serial, para facilitar debug

    //pin32 ADC -> RTC -SCL
    //pin33 ADC -> RTC -SDA
    //pin12 SD Card -MISO
    //pin13 SD Card -MOSI
    //pin14 SD Card -SCK

    Rtc relogio;
    RemoteControl controle_remoto;
    MemoryCard cartaoSD;
    Ihm webserver;

    iniciaMaquinaEstados();

    int pin_relogio[2];
    pin_relogio[0] = SDA;
    pin_relogio[1] = SCL;
    relogio.setup(pin_relogio);

    controle_remoto.setup(CTL);

    cartaoSD.setup(CS);

    webserver.setup(PORTA);





    AsyncWebServer server(80); //seleciona a porta do servidor web: porta 80

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
        request->send(200, "text/html", home_morador_html);
    });

    server.on("/cadastrar", HTTP_GET, [](AsyncWebServerRequest *request){
        String nome1;
        String senha1;
        String tipo;
        String qntdAcessos;
        nome1 = request->getParam("nome")->value();
        senha1 = request->getParam("senha")->value();
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
        // abre_portao();
        request->send(200, "text/html", home_morador_html);
    });
    

    char ssid[100] = "";
    char password[100] = "";
    uint32_t ip[4];

/*
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
