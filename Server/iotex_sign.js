import { ethers } from 'ethers';
import fs from 'fs';

// Função para assinar e enviar a transação para a IoTeX
async function sendTransactionToIoTeX(sensorData) {
    const provider = new ethers.JsonRpcProvider('https://babel-api.testnet.iotex.io');
    const privateKey = 'Ocultada'; // Chave privada ocultada por segurança. Com ela, é possível acessar e realizar transações na carteira IoTeX. Para criar uma carteira, acesse o site da IoTeX e gere um par de chaves (pública e privada), ambas necessárias para este projeto.
    const wallet = new ethers.Wallet(privateKey, provider);

    try {
        console.log('Enviando dados para a IoTeX...');

        const nonce = await provider.getTransactionCount(wallet.address, 'latest');
        const gasPrice = await provider.getFeeData();

        const dataHex = ethers.hexlify(ethers.toUtf8Bytes(JSON.stringify(sensorData)));

        const tx = {
            to: wallet.address,
            value: ethers.parseEther("0"), // Nenhum valor está sendo transferido
            gasLimit: 100000,
            gasPrice: gasPrice.gasPrice,
            data: dataHex,
            nonce: nonce,
            chainId: 4690, // IoTeX Testnet Chain ID
        };

        const signedTx = await wallet.signTransaction(tx);
        const txResponse = await provider.broadcastTransaction(signedTx);
        console.log('Transação enviada para IoTeX! Hash:', txResponse);
    } catch (error) {
        console.error('Erro ao enviar transação para IoTeX:', error);
    }
}

// Exporte a função corretamente
export { sendTransactionToIoTeX };
