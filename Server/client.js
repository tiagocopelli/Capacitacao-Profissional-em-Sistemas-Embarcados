import fetch from 'node-fetch';  // Usando import no lugar de require

async function sendData() {
  const data = {
    id: 'Placa_01',           // Adicionando o ID da placa
    temperatura: 30.80,       // Alterado para 'temperatura'
    umidade: 57.00,           // Alterado para 'umidade'
    qualidadeAr: 825,         // Alterado para 'qualidadeAr'
    alcool: 64.20,
    benzeno: 128.40,          // Alterado para 'benzeno'
    CO2: 32.10,
    fumaca: 96.30             // Alterado para 'fumaca'
  };

  try {
    const response = await fetch('http://192.168.0.38:3000/api/submitData', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify(data),
    });

    if (!response.ok) {
      // Se a resposta não for 200 OK
      const errorData = await response.text();
      console.error('Erro ao enviar dados:', errorData);
    } else {
      const result = await response.json();
      console.log('Dados enviados com sucesso!', result);
    }
  } catch (error) {
    console.error('Erro ao enviar dados:', error);
  }
}

sendData();
