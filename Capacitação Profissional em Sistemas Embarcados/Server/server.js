import express from 'express';  // Usando import para módulos no formato ES6
import bodyParser from 'body-parser';
import { sendTransactionToIoTeX } from './iotex_sign.js';  // Importa a função do iotex_sign.js

const app = express();
const port = 3000;

// Usando body-parser para garantir que o Express possa ler o corpo das requisições
app.use(bodyParser.json());

// Defina a rota POST para /api/submitData
app.post('/api/submitData', async (req, res) => {
    const {
        id,              // Recebendo o ID da placa
        temperatura,
        umidade,
        qualidadeAr,
        alcool,
        benzeno,
        CO2,
        fumaca
    } = req.body;

    // Verificação de dados
    if (
        id === undefined ||        // Verificando o ID
        temperatura === undefined ||
        umidade === undefined ||
        qualidadeAr === undefined ||
        alcool === undefined ||
        benzeno === undefined ||
        CO2 === undefined ||
        fumaca === undefined
    ) {
        return res.status(400).send('Dados incompletos ou inválidos!');
    }

    console.log(`
        ID da Placa: ${id}
        Temperatura: ${temperatura}°C
        Umidade: ${umidade}%
        Qualidade do ar (MQ135): ${qualidadeAr}
        Álcool: ${alcool}
        Benzeno: ${benzeno}
        CO2: ${CO2}
        Fumaça: ${fumaca}
    `);
    // Preparando os dados para enviar para a IoTeX
    const sensorData = {
        id,                  // Incluindo o ID no pacote de dados
        temperatura,
        umidade,
        qualidadeAr,
        alcool,
        benzeno,
        CO2,
        fumaca
    };
    // Enviar os dados para a IoTeX
    await sendTransactionToIoTeX(sensorData);

    // Respondendo para o ESP8266
    res.status(200).send('Dados recebidos com sucesso e enviados para IoTeX!');
});

// Inicia o servidor na porta 3000
app.listen(port, () => {
    console.log(`Servidor rodando em http://localhost:${port}`);
});

